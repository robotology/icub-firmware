
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_NET_ETH_SOCKET_H_
#define EMBOT_NET_ETH_SOCKET_H_


#include "embot_core.h"
#include "embot_net_eth.h"
#include "embot_net_eth_Packet.h"

#include <memory>


namespace embot::net::eth {
    
    class theIPservice; // forward declatarion
    
#if 0
 Class `embot::net::eth::Socket`
 It is a UDP socket which always transmits and receives.
 It is meant to be used by ONE owner thread, which calls open(), close(), connect(), transmit(), 
 receive(), input() and setaction(). The IP service thread (which must have a priority higher than 
 the one of any owner thread) only calls the private addRXfifo(), remTXfifo(), txsent() and fail().

 Errors and statistics.
 Every anomaly is counted (see Stats) and remembered in Stats::lasterror. The anomalies which happen asynchronously 
 w.r.t. the owner thread (rx pipe full, rx packet too big, no memory to send, stack failed to send) also call 
 Action::onerr, which is executed in the IP service thread: as for onrx and ontx it must only alert 
 the owner (e.g. by sending an event), not do any real work. The callback does not carry the error: 
 the owner reads it with stats().lasterror. Anomalies which are detected inside a call of the owner 
 (e.g. transmit() on a full pipe) are only counted, because the call itself returns false.
 
 TX priority.
 Properties::txpriority tells the IP service in which order it sends the packets of the attached sockets:
 first it drains the pipes of the HIGH sockets, then it sends one packet per NORMAL socket (round robin) and 
 then it checks again the HIGH sockets. It affects only the transmission: the reception is served in 
 the order frames come from the network.
 
 Timeouts.
 open() and close() are blocking calls, but they give up after @tout. When they give up the command is
 cancelled: it is guaranteed that it will not be executed later by the IP service thread.
#endif
    
    class Socket
    {
    public:

        // what to do when a packet must be put into a pipe which is full:
        // dropnewest: the new packet is lost and the pipe is unchanged. 
        // dropoldest: the oldest packet of the pipe is lost and the new one is stored (good for fresh data like sensor streams).
        enum class Policy : uint8_t { dropnewest = 0, dropoldest = 1 };

        struct Pipe
        {
            Policy   policy {Policy::dropnewest};
            uint8_t  maxpackets {1};
            uint16_t maxsizeofpacket {1500};
            
            // the input pipe counts its packets with a semaphore of maxpackets+1 tokens, whose limit is 255
            static constexpr uint8_t maxpacketslimit {254};
            bool isvalid() const { return (maxpackets >= 1) && (maxpackets <= maxpacketslimit) && (maxsizeofpacket >= 1); }
            
            // default: 1 packet of up to 1500 bytes, dropnewest
            constexpr Pipe() = default;
            // explicit: the user must say everything. a Pipe of 4 bytes
            constexpr Pipe(Policy p, uint8_t mp, uint16_t msp) : policy(p), maxpackets(mp), maxsizeofpacket(msp) {}
        };

        struct Config
        {
            Pipe inputpipe {};
            Pipe outputpipe {};
            
            Config() = default;
            // explicit: both pipes must be specified
            Config(const Pipe &in, const Pipe &out) : inputpipe(in), outputpipe(out) {}
            bool isvalid() const { return inputpipe.isvalid() && outputpipe.isvalid(); }
        };
        
        // the callbacks are executed in the IP service thread: they must be short and only alert the owner thread. 
        // a Callback can carry a param, e.g. the Socket*, to tell which socket has called it.
        struct Action
        {
            embot::core::Callback onrx {};  
            embot::core::Callback ontx {};      // a packet has been accepted by the IP stack (not necessarily already on the wire)
            embot::core::Callback onerr {};     // asynchronous errors only. the owner reads the error w/ stats().lasterror
            
            Action() = default;
            // explicit: every callback must be specified (use {} for none)
            Action(const embot::core::Callback &rx, const embot::core::Callback &tx, const embot::core::Callback &err)
                : onrx(rx), ontx(tx), onerr(err) {}
        };
        
        enum class TXpriority : uint8_t { normal = 0, high = 1 };
        
        struct Properties
        {
            embot::net::eth::Port localport {0};
            embot::net::eth::IPaddress bindingaddress {embot::net::eth::IPany};
            Action action {};
            TXpriority txpriority {TXpriority::normal};
            
            Properties() = default;
            // explicit: everything must be specified
            Properties(embot::net::eth::Port lp, const embot::net::eth::IPaddress &ba, const Action &a, TXpriority tp)
                : localport(lp), bindingaddress(ba), action(a), txpriority(tp) {}
        };
        
        enum class Error : uint8_t 
        { 
            none = 0, 
            rxpipefull = 1,     // a received packet was lost because the input pipe was full (or it replaced the oldest one)
            rxtoobig = 2,       // a received packet was lost because it is bigger than Pipe::maxsizeofpacket
            txnotopen = 3,      // transmit() on a socket which is not open
            txpipefull = 4,     // transmit(): the output pipe was full (a packet was lost)
            txtoobig = 5,       // transmit(): the packet is bigger than Pipe::maxsizeofpacket
            txnomemory = 6,     // the IP stack had no buffer for a packet: it was lost
            txsendfailed = 7,   // the IP stack refused to send the packet: it was lost
            txnoaddress = 8     // transmit(): the packet has no valid destination (Packet::address().isvalid() is false)
        };
        static constexpr size_t numberoferrors {9};     // number of values of Error, none included
        
        // a snapshot, taken by value: the counters keep running while it is read
        struct Stats
        {
            uint32_t rxok {0};          // packets put in the input pipe
            uint32_t txok {0};          // packets given to the IP stack
            uint32_t rxpipefull {0};
            uint32_t rxtoobig {0};
            uint32_t txnotopen {0};
            uint32_t txpipefull {0};
            uint32_t txtoobig {0};
            uint32_t txnomemory {0};
            uint32_t txsendfailed {0};
            uint32_t txnoaddress {0};
            Error lasterror {Error::none};
        };


        static constexpr embot::core::relTime defaulttimeout {3*embot::core::time1second};

        Socket(const Config &config);
        // an open Socket must be closed by its owner thread before it is destroyed: the destructor calls close() 
        // (without timeout) but it works only in a thread which is allowed to do it (see theIPservice). 
        // otherwise theIPservice would keep a pointer to a destroyed Socket.
        ~Socket();   

        // blocking: the call returns when ::theIPservice has attached the socket, or false after @tout.
        // it also returns false if Config is not valid (see Config::isvalid()).
        // on a timeout the request is cancelled and the socket stays closed. 
        bool open(const Properties& props, embot::core::relTime tout = defaulttimeout);
        // blocking: the call returns when ::theIPservice has detached the socket, or false after @tout.
        // on a timeout the request is cancelled and the socket stays open.
        bool close(embot::core::relTime tout = defaulttimeout);
        // blocking: it asks ::theIPservice to resolve the MAC address of the remote host (ARP)
        bool connect(const embot::net::eth::IPaddress &remoteaddress, embot::core::relTime tout = defaulttimeout);
        
        // it copies the packet into the output pipe and alerts ::theIPservice, which sends it as soon as possible.
        // returns false if the socket is not open, if the packet has not a valid destination address, 
        // if the pipe is full or if the packet is bigger than Config::outputpipe.maxsizeofpacket. It never blocks (but for the very short lock of the pipe).
        bool transmit(const embot::net::eth::Packet &packet); 
        
        // number of packets in the input pipe
        size_t input() const;
        
        // it moves a packet from the input pipe into @packet. 
        // tout = 0 makes the call non-blocking. returns false if no packet is available within tout 
        // or if @packet.capacity() is smaller than the received packet (in such a case the packet is dropped).
        bool receive(embot::net::eth::Packet &packet, embot::core::relTime tout = 0);

        const Config & config() const;
        // a copy: the properties given to open() but for the action, which is the current one (see setaction()). 
        // it is thread-safe w.r.t. setaction() and the IP service thread
        Properties properties() const;

        // read-only snapshot of the counters
        Stats stats() const;
        // snapshot of the counters, which are also reset to zero (lasterror = none)
        Stats take();

        // it changes the actions of the socket (open or not). It is safe w.r.t. the IP service thread.
        void setaction(const Action &action);
        
    private:
        // for use only by embot::net::eth::theIPservice 
        friend class embot::net::eth::theIPservice;
    
        // it adds to the input pipe a packet received by the IP network, releases the token which counts 
        // the received packets and calls Properties::Action::onrx().
        // returns false if the pipe is full or if the packet is too big: the packet is dropped.
        bool addRXfifo(const embot::net::eth::Packet &packet); 

        // it removes a packet from the output pipe so that it can be sent to the IP network. 
        // returns false if the pipe is empty.
        bool remTXfifo(embot::net::eth::Packet &packet); 

        // the IP service tells that the last packet of remTXfifo() was accepted by the IP stack (lwip::udp::send() returned true):
        // it counts it and calls Properties::Action::ontx().
        void txsent();
        
        // the IP service tells that something went wrong: it counts the error and calls Properties::Action::onerr()
        void fail(Error e);

    public:
        // neither copyable nor moveable: a Socket owns its pipes and, while it is open, theIPservice holds a pointer to it
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
        Socket(Socket&&) = delete;
        Socket& operator=(Socket&&) = delete;
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
} // namespace embot::net::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
