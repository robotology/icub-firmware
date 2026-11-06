/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_NET_THELOGGER_H_
#define __EMBOT_NET_THELOGGER_H_

#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_net_eth.h"
#include <memory>

#if 0

theLogger: the lines of embot::core::print() sent over UDP to an IP:port, and a few text commands to control it from the
same port. the simplest host is netcat, which talks to the board and shows what it receives in the same terminal:

    ncat -u 10.0.1.99 9999      (Windows, ncat comes w/ nmap; Linux / WSL2: nc -u 10.0.1.99 9999)
    <Enter>                     an empty line (or hello): the board sends the log to this terminal
    off<Enter>                  [theLogger] ok: off
    status<Enter>               [theLogger] status: dest 10.0.1.104:54321, off, deco on, commands on, lines 12, ...

the board sends to the port the host has written from, so it works also behind a NAT (e.g. WSL2 w/out mirrored networking).

the lines
- puts() never blocks and never sends: it copies the line into a buffer in RAM and wakes up the thread of the logger,
  which runs at low priority and sends the lines w/ its own socket, many lines in a frame. if the buffer is full, the line
  is lost and counted (Stats::droppedlines)
- a print() is always one line: the '\r' and '\n' at its end are removed (puts() adds its own '\n'), those inside it
  become spaces. a line longer than a frame is cut and its last character becomes '~'
- initialise() redirects embot::core::print() to puts() (w/ embot::core::setprint()). if Config::alsoprevious is true,
  print() also keeps going to the previous output (e.g. ITM / SWO)
- the socket is opened by the thread of the logger, which starts after the init thread: so initialise() can be called
  early in the init thread, also before embot::net::eth::theIPservice::initialise(), and the lines printed from then on
  are kept in the buffer until the socket is open (as long as they fit). if the socket cannot be opened, the thread
  tries again every second
- set(destination), set(Activation) and set(Decoration) change where the lines go, whether they are taken at all and
  whether they carry a prefix, at any time and from any thread. Activation::OFF: puts() discards the lines (the buffer
  keeps what it already has)
- Decoration::ON: each line begins w/ [<IP of the board> @ <time>], e.g.  [10.0.1.99 @ S12:m345:u678] text
  w/ the time of embot::core::now() when puts() takes the line, in the compact form of embot::core::TimeFormatter
- the thread of the logger does not log itself: a print() done inside it (e.g. by theIPservice when it sends a frame of
  the logger) goes only to the previous output, if any. it avoids a loop
- puts() must not be called from an ISR (the buffer is protected by a mutex)

the commands (Config::commands = Commands::ON)
- one command per UDP frame, at most maxcommandsize bytes. case, spaces at the ends and the final '\r' '\n' do not matter

    (empty) or hello    the lines go to the sender of the command, from now on
    on / off            Activation. off also discards the lines not yet sent
    deco on / deco off  Decoration
    dest <ip>:<port>    the lines go to <ip>:<port>, e.g. dest 10.0.1.104:9999
    status              one line w/ the destination, the modes and the counters
    help                the list of the commands, one line each:  [theLogger] help: off: the lines are discarded, ...

- every command has a reply, sent to its sender (not to the destination), also when the logger is OFF:

    [theLogger] ok: off
    [theLogger] error: unknown command "of"

  the replies do not go through puts(): they have no decoration, they are not counted in Stats and not printed on the
  previous output. a print() must not begin w/ [theLogger], so that the host can tell the two apart
- whoever reaches the port can change the destination: fine in a lab network

call initialise() once from the init thread. the logger attaches a socket to embot::net::eth::theIPservice (one of
theIPservice::Config::numberofattachablesockets).

#endif

namespace embot::net {

    class theLogger
    {
    public:
        static theLogger& getInstance();

        enum class Activation : uint8_t { OFF = 0, ON = 1 };
        enum class Decoration : uint8_t { OFF = 0, ON = 1 };
        enum class Commands : uint8_t { OFF = 0, ON = 1 };

        // the size of the buffer of the lines (allocated by initialise())
        static constexpr size_t buffercapacity {4096};
        // the max size of a command: a bigger frame is dropped by the socket
        static constexpr size_t maxcommandsize {64};

        struct Config
        {
            embot::net::eth::SocketAddress destination {};                  // where the lines go. port 0: nowhere yet, they stay in the buffer
            embot::net::eth::IPaddress board {};                            // the IP of the board, shown by the decoration
            embot::net::eth::Port localport {9999};                         // the port of the socket of the board
            Activation activation {Activation::ON};                         // at start
            Decoration decoration {Decoration::OFF};                        // at start
            Commands commands {Commands::ON};                               // ON: the socket accepts the text commands
            bool alsoprevious {true};                                       // print() also goes to the output it had before
            uint16_t maxframesize {1024};                                   // bytes of lines in a UDP frame. a line longer than this is cut
            embot::os::Thread::Props thread {embot::os::Priority::low10, 2*1024};

            constexpr Config() = default;
            constexpr explicit Config(const embot::net::eth::SocketAddress &d, const embot::net::eth::IPaddress &b, embot::net::eth::Port lp,
                                      Activation a, Decoration de, Commands c, bool ap, uint16_t mfs, const embot::os::Thread::Props &t)
                : destination(d), board(b), localport(lp), activation(a), decoration(de), commands(c), alsoprevious(ap), maxframesize(mfs), thread(t) {}

            constexpr bool isvalid() const { return (maxframesize >= 64) && (maxframesize <= 1472) && thread.isvalid(); }
        };

        struct Stats
        {
            uint32_t lines {0};             // lines accepted in the buffer
            uint32_t frames {0};            // frames of lines given to the socket
            uint32_t droppedlines {0};      // lines lost: buffer full, or printed by the thread of the logger itself

            constexpr Stats() = default;
            constexpr explicit Stats(uint32_t l, uint32_t f, uint32_t d) : lines(l), frames(f), droppedlines(d) {}
        };

        // what the logger is doing: a coherent snapshot, taken w/ a single lock
        struct Status
        {
            embot::net::eth::SocketAddress destination {};
            Activation activation {Activation::OFF};
            Decoration decoration {Decoration::OFF};
            Commands commands {Commands::OFF};
            Stats stats {};

            constexpr Status() = default;
            constexpr explicit Status(const embot::net::eth::SocketAddress &d, Activation a, Decoration de, Commands c, const Stats &s)
                : destination(d), activation(a), decoration(de), commands(c), stats(s) {}
        };

        // it creates the thread and the socket of the logger and redirects embot::core::print(). false if the Config is
        // not valid or if already initialised
        bool initialise(const Config &config);
        bool initialised() const;

        // it copies the line into the buffer (w/ the decoration, if ON, and a '\n' at its end) and wakes up the thread. it
        // never blocks. false if the logger is OFF or the buffer has no room for the line
        bool puts(const char *str);

        // where the lines go, from now on. the thread resolves the new address (ARP) before it sends to it
        bool set(const embot::net::eth::SocketAddress &destination);
        // ON: puts() takes the lines. OFF: puts() discards them
        bool set(Activation activation);
        // ON: the lines from now on begin w/ [<IP of the board> @ <time>]
        bool set(Decoration decoration);

        // all of it at once
        Status status() const;
        // one value at a time
        embot::net::eth::SocketAddress destination() const;
        Activation activation() const;
        Decoration decoration() const;
        Stats stats() const;

    private:
        theLogger();
        ~theLogger();

    public:
        theLogger(const theLogger&) = delete;
        theLogger& operator=(const theLogger&) = delete;
        theLogger(theLogger&&) = delete;
        theLogger& operator=(theLogger&&) = delete;

        struct Impl;
    private:
        std::unique_ptr<Impl> pImpl;
    };

} // namespace embot::net


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

