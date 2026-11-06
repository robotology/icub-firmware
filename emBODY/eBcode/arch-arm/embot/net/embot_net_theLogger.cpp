/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_net_theLogger.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core_binary.h"
#include "embot_os_rtos.h"
#include "embot_os_theScheduler.h"
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <string_view>

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot::net {

    namespace {

        // the output of embot::core::print() before initialise(), and whether print() keeps using it. constant initialised
        constexpr embot::core::fpPrint noprint {nullptr};
        embot::core::fpPrint s_previousprint {noprint};
        bool s_alsoprevious {false};

        // the function given to embot::core::setprint()
        int s_logprint(const std::string &str);

        // ---- the text of the commands, w/out heap

        constexpr bool iswhite(char c) { return (' ' == c) || ('\t' == c) || ('\r' == c) || ('\n' == c); }
        constexpr char lower(char c) { return ((c >= 'A') && (c <= 'Z')) ? static_cast<char>(c - 'A' + 'a') : c; }

        // it copies the n bytes of src into dst as a command: lower case, no spaces at the ends, a single space between
        // two words, a '\0' at the end. it returns the length of the command (0: an empty line)
        size_t normalise(const uint8_t *src, size_t n, char *dst, size_t max)
        {
            size_t length {0};
            bool space {false};                                 // a space is pending between two words
            for(size_t i = 0; (i < n) && (length < (max - 1)); i++)
            {
                const char c {static_cast<char>(src[i])};
                if(iswhite(c))
                {
                    space = (length > 0);                       // the spaces at the beginning are skipped
                    continue;
                }
                if(space)
                {
                    if(length >= (max - 2))
                    {
                        break;                                  // no room for the space and the next char
                    }
                    dst[length++] = ' ';
                }
                space = false;
                dst[length++] = lower(c);
            }
            dst[length] = '\0';
            return length;
        }

        // 10.0.1.104:9999 -> {10.0.1.104, 9999}. false if the text is not a.b.c.d:port w/ a, b, c, d <= 255 and port > 0
        bool parse(std::string_view text, embot::net::eth::SocketAddress &address)
        {
            uint32_t v[5] {0, 0, 0, 0, 0};                      // the four numbers of the IP and the port
            size_t k {0};
            bool digits {false};
            for(const char c : text)
            {
                if((c >= '0') && (c <= '9'))
                {
                    v[k] = 10*v[k] + static_cast<uint32_t>(c - '0');
                    if(v[k] > 65535)
                    {
                        return false;
                    }
                    digits = true;
                }
                else if(digits && ((('.' == c) && (k < 3)) || ((':' == c) && (3 == k))))
                {
                    k++;
                    digits = false;
                }
                else
                {
                    return false;
                }
            }
            if((4 != k) || (false == digits) || (v[0] > 255) || (v[1] > 255) || (v[2] > 255) || (v[3] > 255) || (0 == v[4]))
            {
                return false;
            }
            address = embot::net::eth::SocketAddress
            {
                embot::net::eth::IPaddress {static_cast<uint8_t>(v[0]), static_cast<uint8_t>(v[1]), static_cast<uint8_t>(v[2]), static_cast<uint8_t>(v[3])},
                static_cast<embot::net::eth::Port>(v[4])
            };
            return true;
        }

        // the IP of an IPaddress as four numbers, w/ the first one at the LSB (as embot::net::eth::IPaddress stores it)
        constexpr unsigned byteof(const embot::net::eth::IPaddress &ip, unsigned i) { return (ip.value() >> (8*i)) & 0xff; }

        // 10.0.1.104:54321, or none if the address is not valid. it returns the chars written (w/out the '\0')
        size_t print(char *dst, size_t max, const embot::net::eth::SocketAddress &a)
        {
            const int n {a.isvalid() ? std::snprintf(dst, max, "%u.%u.%u.%u:%u", byteof(a.addr, 0), byteof(a.addr, 1), byteof(a.addr, 2),
                                                     byteof(a.addr, 3), static_cast<unsigned>(a.port))
                                     : std::snprintf(dst, max, "none")};
            return (n < 0) ? 0 : std::min<size_t>(static_cast<size_t>(n), max - 1);
        }

        constexpr const char * onoff(bool on) { return on ? "on" : "off"; }

    } // namespace

    struct theLogger::Impl
    {
        static constexpr embot::os::Event evtDATA {embot::core::binary::mask::pos2mask<embot::os::Event>(0)};    // puts() has added a line
        static constexpr embot::os::Event evtTX {embot::core::binary::mask::pos2mask<embot::os::Event>(1)};      // the socket has sent a frame
        static constexpr embot::os::Event evtDEST {embot::core::binary::mask::pos2mask<embot::os::Event>(2)};    // set(destination) has been called
        static constexpr embot::os::Event evtRX {embot::core::binary::mask::pos2mask<embot::os::Event>(3)};      // a command has arrived

        // the timeout of the thread: it retries to send (a full output pipe) and to open the socket
        static constexpr embot::core::relTime period {100*embot::core::time1millisec};
        static constexpr uint32_t openretry {10};          // periods between two attempts to open the socket: 1 s

        // the replies to the commands
        static constexpr std::string_view replytag {"[theLogger] "};
        static constexpr size_t maxreplysize {192};         // a reply of one line
        // the reply to help: one line per command, all in one frame
        static constexpr std::string_view helplines[]
        {
            "help: hello (or an empty line): the log goes to who sends it",
            "help: on: the lines are taken and sent",
            "help: off: the lines are discarded, also those not yet sent",
            "help: deco on: each line begins w/ [<ip of the board> @ <time>]",
            "help: deco off: the lines w/out decoration",
            "help: dest <ip>:<port>: the log goes to <ip>:<port>, e.g. dest 10.0.1.104:9999",
            "help: status: the destination, the modes and the counters",
            "help: help: this list"
        };

        Config config {};
        bool initialised {false};

        embot::os::rtos::mutex_t *mtx {nullptr};           // it protects the buffer, the destination, the activation, the decoration, the stats

        // the buffer: a ring of bytes. it holds whole lines, each one w/ its '\n'
        std::array<char, buffercapacity> buffer {};
        size_t head {0};                                    // where the next byte is written
        size_t size {0};                                    // the bytes in the buffer

        embot::net::eth::SocketAddress destination {};
        Activation activation {Activation::ON};
        Decoration decoration {Decoration::OFF};
        Stats stats {};
        bool newdestination {false};                        // the thread must resolve it (ARP) before sending to it

        // used only by the thread of the logger
        embot::os::EventThread *thread {nullptr};
        embot::net::eth::Socket *socket {nullptr};
        embot::net::eth::Packet *packet {nullptr};          // a frame of lines
        embot::net::eth::Packet *command {nullptr};         // a received command
        embot::net::eth::Packet *reply {nullptr};           // the reply to a command
        bool replypending {false};                          // the reply is waiting for room in the output pipe
        embot::net::eth::IPaddress resolved {embot::net::eth::IPany};   // the last address resolved w/ ARP
        bool open {false};
        uint32_t ticks {0};

        Impl() = default;

        size_t room() const { return buffercapacity - size; }

        // it copies n bytes at the end of the ring
        void push(const char *data, size_t n)
        {
            for(size_t i = 0; i < n; i++)
            {
                buffer[head] = data[i];
                head = (head + 1) % buffercapacity;
            }
            size += n;
        }

        // it copies the text of a print() at the end of the ring as one line: a '\r' or '\n' becomes a space. if the text
        // has been cut, its last char becomes '~'
        void pushtext(const char *text, size_t n, bool cut)
        {
            for(size_t i = 0; i < n; i++)
            {
                const char c {text[i]};
                buffer[head] = (cut && (i == (n - 1))) ? '~' : ((('\r' == c) || ('\n' == c)) ? ' ' : c);
                head = (head + 1) % buffercapacity;
            }
            size += n;
        }

        // it copies into dst the whole lines that fit in max bytes, from the oldest one. it returns their bytes, 0 if none.
        // it does not remove them: pop() does it, after the frame has been accepted by the socket
        size_t peek(uint8_t *dst, size_t max) const
        {
            const size_t tail {(head + buffercapacity - size) % buffercapacity};
            size_t lines {0};       // the bytes of the whole lines found so far
            for(size_t i = 0; (i < size) && (i < max); i++)
            {
                const char c {buffer[(tail + i) % buffercapacity]};
                dst[i] = static_cast<uint8_t>(c);
                if('\n' == c)
                {
                    lines = i + 1;
                }
            }
            return lines;
        }

        void pop(size_t n)
        {
            size -= n;
        }

        // [10.0.1.99 @ S12:m345:u678] : the compact form of embot::core::TimeFormatter, written w/out heap
        size_t decorate(char *dst, size_t max) const
        {
            const embot::core::TimeFormatter tf {embot::core::now()};
            // the seconds as 32 bits (136 years): no 64-bit conversion needed in snprintf()
            const int n {std::snprintf(dst, max, "[%u.%u.%u.%u @ S%lu:m%u:u%u] ",
                                       byteof(config.board, 0), byteof(config.board, 1), byteof(config.board, 2), byteof(config.board, 3),
                                       static_cast<unsigned long>(tf.to_seconds()), static_cast<unsigned>(tf.m), static_cast<unsigned>(tf.u))};
            return (n < 0) ? 0 : std::min<size_t>(static_cast<size_t>(n), max - 1);
        }

        bool puts(const char *str)
        {
            if((nullptr == str) || (false == initialised))
            {
                return false;
            }

            // the thread of the logger does not log itself (e.g. a print of theIPservice while it sends a frame of the logger)
            if(embot::os::theScheduler::getInstance().scheduled() == thread)
            {
                embot::os::rtos::Lock lock {mtx};
                stats.droppedlines++;
                return false;
            }

            // the decoration is made before the lock, on the stack: it does not need the buffer
            char prefix[48] {0};
            const size_t lengthprefix {(Decoration::ON == decoration) ? decorate(prefix, sizeof(prefix)) : 0};

            // one print() is one line: the '\r' and '\n' at its end go away, puts() adds its own '\n'
            size_t length {std::strlen(str)};
            while((length > 0) && (('\n' == str[length - 1]) || ('\r' == str[length - 1])))
            {
                length--;
            }

            // a line longer than a frame is cut: it must fit a frame w/ its decoration and its '\n'
            const size_t maxlength {config.maxframesize - 1 - lengthprefix};
            const bool cut {length > maxlength};
            if(cut)
            {
                length = maxlength;
            }

            {
                embot::os::rtos::Lock lock {mtx};
                if(Activation::OFF == activation)
                {
                    return false;
                }
                if(room() < (lengthprefix + length + 1))
                {
                    stats.droppedlines++;
                    return false;
                }
                push(prefix, lengthprefix);
                pushtext(str, length, cut);
                push("\n", 1);
                stats.lines++;
            }

            thread->setEvent(evtDATA);
            return true;
        }

        Status status() const
        {
            embot::os::rtos::Lock lock {mtx};
            return Status {destination, activation, decoration, config.commands, stats};
        }

        // ---- the thread

        static void tLOGGER(void *p)
        {
            embot::os::Thread *t {reinterpret_cast<embot::os::Thread *>(p)};
            t->run();
        }

        static void alertTX(void *p)
        {
            reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtTX);
        }

        static void alertRX(void *p)
        {
            reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtRX);
        }

        static void startup(embot::os::Thread *t, void *param)
        {
            Impl *impl {reinterpret_cast<Impl *>(param)};
            const bool commands {Commands::ON == impl->config.commands};
            impl->packet = new embot::net::eth::Packet(impl->config.maxframesize);
            if(commands)
            {
                impl->command = new embot::net::eth::Packet(maxcommandsize);
                impl->reply = new embot::net::eth::Packet(impl->config.maxframesize);   // a frame: the reply to help has many lines
            }
            // the input pipe holds two commands (or the minimum if there are no commands). the output pipe holds two frames
            // of lines and a reply
            impl->socket = new embot::net::eth::Socket(embot::net::eth::Socket::Config
            {
                commands ? embot::net::eth::Socket::Pipe {embot::net::eth::Socket::Policy::dropnewest, 2, maxcommandsize}
                         : embot::net::eth::Socket::Pipe {embot::net::eth::Socket::Policy::dropnewest, 1, 64},
                embot::net::eth::Socket::Pipe {embot::net::eth::Socket::Policy::dropnewest, 3, impl->config.maxframesize}
            });
            impl->openthesocket(t);
        }

        void openthesocket(embot::os::Thread *t)
        {
            const embot::core::Callback onrx {(Commands::ON == config.commands) ? embot::core::Callback {alertRX, t} : embot::core::Callback {}};
            const embot::net::eth::Socket::Properties props
            {
                config.localport, embot::net::eth::IPany,
                embot::net::eth::Socket::Action {onrx, embot::core::Callback {alertTX, t}, embot::core::Callback {}},
                embot::net::eth::Socket::TXpriority::normal
            };
            open = socket->open(props);
            if(open)
            {
                t->setEvent(evtDEST | evtDATA);         // resolve the destination and send what is in the buffer
            }
        }

        static void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
        {
            Impl *impl {reinterpret_cast<Impl *>(param)};

            if(false == impl->open)
            {
                if(++impl->ticks >= openretry)
                {
                    impl->ticks = 0;
                    impl->openthesocket(t);
                }
                return;
            }

            // the commands first, so that their effect (e.g. a new destination) applies to the lines sent below
            if(Commands::ON == impl->config.commands)
            {
                impl->execute();
            }

            if(true == embot::core::binary::mask::check(eventmask, evtDEST))
            {
                impl->resolve();
            }

            // evtDATA, evtTX and the timeout: send as long as there are lines and the socket takes them
            impl->send();
        }

        // ARP, only if the address is not the last one resolved: so that the first frames to it are not lost
        void arp(const embot::net::eth::IPaddress &ip)
        {
            if(ip.value() != resolved.value())
            {
                socket->connect(ip);
                resolved = ip;
            }
        }

        // the ARP of a new destination
        void resolve()
        {
            embot::net::eth::SocketAddress d {};
            {
                embot::os::rtos::Lock lock {mtx};
                if(false == newdestination)
                {
                    return;
                }
                newdestination = false;
                d = destination;
            }
            if(d.isvalid())
            {
                arp(d.addr);
            }
        }

        // the reply waiting for room in the output pipe, if any. false if it is still waiting
        bool flushreply()
        {
            if(replypending)
            {
                replypending = !socket->transmit(*reply);
            }
            return !replypending;
        }

        void send()
        {
            // a reply goes before the lines that come after it
            if(false == flushreply())
            {
                return;
            }

            for(;;)
            {
                size_t n {0};
                embot::net::eth::SocketAddress d {};
                {
                    embot::os::rtos::Lock lock {mtx};
                    d = destination;
                    if((0 == size) || (false == d.isvalid()))
                    {
                        return;                                 // nothing to send, or nowhere: the lines stay in the buffer
                    }
                    n = peek(packet->data(), packet->capacity());
                }

                if(0 == n)
                {
                    return;
                }

                packet->setsize(n);
                packet->setaddress(d);
                if(false == socket->transmit(*packet))
                {
                    return;                                     // the output pipe is full: again at evtTX or at the timeout
                }

                embot::os::rtos::Lock lock {mtx};
                pop(n);
                stats.frames++;
            }
        }

        // ---- the commands

        // it executes the received commands, one at a time: the next one waits in the input pipe until the reply of the
        // previous one is in the output pipe
        void execute()
        {
            while(flushreply() && socket->receive(*command))
            {
                char text[maxcommandsize + 1] {0};
                const size_t n {normalise(command->data(), command->size(), text, sizeof(text))};
                execute(std::string_view {text, n}, command->address());
            }
        }

        void execute(std::string_view cmd, const embot::net::eth::SocketAddress &from)
        {
            constexpr std::string_view dest {"dest "};

            if(cmd.empty() || ("hello" == cmd))
            {
                setdestination(from);
                answerdestination(from);
            }
            else if(("on" == cmd) || ("off" == cmd))
            {
                const bool on {"on" == cmd};
                {
                    embot::os::rtos::Lock lock {mtx};
                    activation = on ? Activation::ON : Activation::OFF;
                    if(false == on)
                    {
                        size = 0;                               // the lines not yet sent are discarded: the reply is the last line
                    }
                }
                answer(from, on ? "ok: on" : "ok: off");
            }
            else if(("deco on" == cmd) || ("deco off" == cmd))
            {
                const bool on {"deco on" == cmd};
                {
                    embot::os::rtos::Lock lock {mtx};
                    decoration = on ? Decoration::ON : Decoration::OFF;
                }
                answer(from, on ? "ok: deco on" : "ok: deco off");
            }
            else if((cmd.size() > dest.size()) && (dest == cmd.substr(0, dest.size())))
            {
                embot::net::eth::SocketAddress a {};
                if(parse(cmd.substr(dest.size()), a))
                {
                    setdestination(a);
                    answerdestination(from);
                }
                else
                {
                    answerunknown(from, "error: bad address", cmd.substr(dest.size()), " (e.g. dest 10.0.1.104:9999)");
                }
            }
            else if("status" == cmd)
            {
                answerstatus(from);
            }
            else if("help" == cmd)
            {
                answerhelp(from);
            }
            else
            {
                answerunknown(from, "error: unknown command", cmd, " (help: the list of the commands)");
            }
        }

        void setdestination(const embot::net::eth::SocketAddress &a)
        {
            embot::os::rtos::Lock lock {mtx};
            destination = a;
            newdestination = true;
        }

        // it puts the reply [theLogger] <text>\n into the output pipe, or keeps it until there is room (see flushreply())
        void answer(const embot::net::eth::SocketAddress &to, std::string_view text)
        {
            char *r {reinterpret_cast<char *>(reply->data())};
            const size_t max {reply->capacity() - 1};           // room for the '\n'
            size_t n {std::min(replytag.size(), max)};
            std::memcpy(r, replytag.data(), n);
            const size_t t {std::min(text.size(), max - n)};
            std::memcpy(r + n, text.data(), t);
            n += t;
            r[n++] = '\n';
            reply->setsize(n);
            reply->setaddress(to);
            arp(to.addr);                                       // the sender has just talked to the board: its ARP is quick
            replypending = true;
            flushreply();
        }

        // one line per command, each one w/ [theLogger] at its beginning, as many as fit the frame
        void answerhelp(const embot::net::eth::SocketAddress &to)
        {
            char *r {reinterpret_cast<char *>(reply->data())};
            size_t n {0};
            for(const std::string_view &line : helplines)
            {
                const size_t length {replytag.size() + line.size() + 1};
                if((n + length) > reply->capacity())
                {
                    break;
                }
                std::memcpy(r + n, replytag.data(), replytag.size());
                std::memcpy(r + n + replytag.size(), line.data(), line.size());
                r[n + length - 1] = '\n';
                n += length;
            }
            reply->setsize(n);
            reply->setaddress(to);
            arp(to.addr);
            replypending = (n > 0);
            flushreply();
        }

        // error: unknown command "of" (help: the commands)
        void answerunknown(const embot::net::eth::SocketAddress &to, const char *what, std::string_view cmd, const char *hint)
        {
            char text[maxreplysize] {0};
            const int n {std::snprintf(text, sizeof(text), "%s \"%.*s\"%s", what, static_cast<int>(cmd.size()), cmd.data(), hint)};
            answer(to, std::string_view {text, (n < 0) ? 0 : std::min<size_t>(static_cast<size_t>(n), sizeof(text) - 1)});
        }

        // ok: dest 10.0.1.104:54321, on, deco off
        void answerdestination(const embot::net::eth::SocketAddress &to)
        {
            const Status s {status()};
            char address[24] {0};
            print(address, sizeof(address), s.destination);
            char text[maxreplysize] {0};
            const int n {std::snprintf(text, sizeof(text), "ok: dest %s, %s, deco %s", address,
                                       onoff(Activation::ON == s.activation), onoff(Decoration::ON == s.decoration))};
            answer(to, std::string_view {text, (n < 0) ? 0 : std::min<size_t>(static_cast<size_t>(n), sizeof(text) - 1)});
        }

        // status: dest 10.0.1.104:54321, on, deco off, commands on, lines 1234, frames 310, dropped 0
        void answerstatus(const embot::net::eth::SocketAddress &to)
        {
            const Status s {status()};
            char address[24] {0};
            print(address, sizeof(address), s.destination);
            char text[maxreplysize] {0};
            const int n {std::snprintf(text, sizeof(text), "status: dest %s, %s, deco %s, commands %s, lines %lu, frames %lu, dropped %lu", address,
                                       onoff(Activation::ON == s.activation), onoff(Decoration::ON == s.decoration), onoff(Commands::ON == s.commands),
                                       static_cast<unsigned long>(s.stats.lines), static_cast<unsigned long>(s.stats.frames),
                                       static_cast<unsigned long>(s.stats.droppedlines))};
            answer(to, std::string_view {text, (n < 0) ? 0 : std::min<size_t>(static_cast<size_t>(n), sizeof(text) - 1)});
        }
    };


    namespace {

        int s_logprint(const std::string &str)
        {
            theLogger::getInstance().puts(str.c_str());
            if(s_alsoprevious && (noprint != s_previousprint))
            {
                return s_previousprint(str);
            }
            return static_cast<int>(str.size());
        }

    } // namespace


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

    theLogger &theLogger::getInstance()
    {
        static theLogger *p = new theLogger();
        return *p;
    }

    theLogger::theLogger()
    {
        pImpl = std::make_unique<Impl>();
    }

    theLogger::~theLogger() { }

    bool theLogger::initialised() const
    {
        return pImpl->initialised;
    }

    bool theLogger::initialise(const Config &config)
    {
        if((true == pImpl->initialised) || (false == config.isvalid()))
        {
            return false;
        }

        Impl *impl {pImpl.get()};
        impl->config = config;
        impl->destination = config.destination;
        impl->activation = config.activation;
        impl->decoration = config.decoration;
        impl->newdestination = config.destination.isvalid();
        impl->mtx = embot::os::rtos::mutex_new();

        const embot::os::EventThread::Config tc
        {
            config.thread.stacksize,
            config.thread.priority,
            Impl::startup,
            impl,
            Impl::period,
            Impl::onevent,
            "tLOGGER"
        };
        impl->thread = new embot::os::EventThread;
        impl->initialised = true;
        impl->thread->start(tc, Impl::tLOGGER);

        // from now on embot::core::print() goes to puts()
        s_alsoprevious = config.alsoprevious;
        s_previousprint = embot::core::setprint(s_logprint);

        return true;
    }

    bool theLogger::puts(const char *str)
    {
        return pImpl->puts(str);
    }

    bool theLogger::set(const embot::net::eth::SocketAddress &destination)
    {
        if(false == pImpl->initialised)
        {
            return false;
        }
        pImpl->setdestination(destination);
        pImpl->thread->setEvent(Impl::evtDEST | Impl::evtDATA);
        return true;
    }

    bool theLogger::set(Activation activation)
    {
        if(false == pImpl->initialised)
        {
            return false;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        pImpl->activation = activation;
        return true;
    }

    bool theLogger::set(Decoration decoration)
    {
        if(false == pImpl->initialised)
        {
            return false;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        pImpl->decoration = decoration;
        return true;
    }

    theLogger::Status theLogger::status() const
    {
        if(false == pImpl->initialised)
        {
            const Config &c {pImpl->config};
            return Status {c.destination, c.activation, c.decoration, c.commands, Stats {}};
        }
        return pImpl->status();
    }

    theLogger::Decoration theLogger::decoration() const
    {
        if(false == pImpl->initialised)
        {
            return pImpl->config.decoration;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        return pImpl->decoration;
    }

    embot::net::eth::SocketAddress theLogger::destination() const
    {
        if(false == pImpl->initialised)
        {
            return pImpl->config.destination;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        return pImpl->destination;
    }

    theLogger::Activation theLogger::activation() const
    {
        if(false == pImpl->initialised)
        {
            return pImpl->config.activation;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        return pImpl->activation;
    }

    theLogger::Stats theLogger::stats() const
    {
        if(false == pImpl->initialised)
        {
            return Stats {};
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        return pImpl->stats;
    }

} // namespace embot::net


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

