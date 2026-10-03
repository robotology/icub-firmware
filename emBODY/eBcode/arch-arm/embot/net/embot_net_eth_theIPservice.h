
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_NET_ETH_THEIPSERVICE_H_
#define EMBOT_NET_ETH_THEIPSERVICE_H_


#include "embot_core.h"
#include "embot_os_common.h"
#include "embot_net_eth.h"
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"

#include <memory>

namespace embot::net::eth {



#if 0
 Class `embot::net::eth::theIPservice`
 It owns an embot::os::EventThread (the "IP service thread") which is the only one that touches the IP stack (lwIP).
 The priority of this thread MUST be higher than the one of any thread which owns a Socket.

 Commands (attach, detach, resolve) are blocking: the caller hands the command to the IP service thread and waits
 for its completion, up to a timeout. A command which times out before the IP service thread has started it is cancelled. Only one command at a time is in flight: concurrent callers are serialised by a mutex
 (with priority inheritance). A command cannot be issued by the IP service thread itself (it would deadlock):
 in such a case the call returns false.

 Events of the IP service thread: RX (a frame has arrived: the stack processes the input), TX (a socket has something to send: 
 the sockets are scanned), CMD (a command is ready). At a timeout (no event for Config::tickperiod) the thread processes the input too.
 The sockets are served in this order: the ones with Socket::TXpriority::high first (all their packets), then one packet 
 per normal socket in round robin, and again from the high ones.
 The tick of the stack is generated inside the same thread, which wakes up at least every Config::tickperiod.
#endif

    class theIPservice
    {
    public:

        static theIPservice& getInstance();

        // non-copyable, non-moveable: there is exactly one instance, reachable only via getInstance()
        theIPservice(const theIPservice&) = delete;
        theIPservice& operator=(const theIPservice&) = delete;
        theIPservice(theIPservice&&) = delete;
        theIPservice& operator=(theIPservice&&) = delete;

    public:

        struct Config
        {
            embot::net::eth::IPconfig ipconfig {};
            uint8_t numberofattachablesockets {4};             // the max number of sockets that one can attach to the IP service
            embot::os::Priority priority {embot::os::Priority::system50};   // of the IP service thread. higher than the owners of sockets
            uint16_t stacksize {2048};
            embot::core::relTime tickperiod {10*embot::core::time1millisec};  // period of lwip::sys::tick()
            uint16_t maxsizeofpacket {1500};   // size of the scratch packets used for rx and tx. >= maxsizeofpacket of the pipes of all sockets
            constexpr Config(const embot::net::eth::IPconfig &ipc, const uint8_t nas, embot::os::Priority pr, uint16_t ss, 
                             embot::core::relTime tp = 10*embot::core::time1millisec, uint16_t mps = 1500)
                : ipconfig(ipc), numberofattachablesockets(nas), priority(pr), stacksize(ss), tickperiod(tp), maxsizeofpacket(mps) {}            
            Config() = default;
            bool isvalid() const
            {
                return ((0 == stacksize) || (0 == tickperiod) || (0 == numberofattachablesockets) || (0 == maxsizeofpacket) ||
                        (false == embot::os::priority::isSystem(priority))) ? false : true;
            }
        };

        // it starts the IP service thread, which initialises the stack at its startup. it can be called only once.
        bool initialise(const Config &config);

        // blocking: it sends the ARP request for @remoteaddress and waits until it is resolved or until @tout expires.
        // tout = 0: only one attempt, no wait.
        bool resolve(const embot::net::eth::IPaddress &remoteaddress, embot::core::relTime tout);

    private:
        // for use only by embot::net::eth::Socket
        friend class embot::net::eth::Socket;

        // blocking, but they give up after @tout: in such a case the command is cancelled, so the IP service thread
        // never executes it later. reltimeWaitForever = no timeout.
        bool attach(embot::net::eth::Socket &socket, embot::core::relTime tout);
        bool detach(embot::net::eth::Socket &socket, embot::core::relTime tout);

        enum class Event { RX = 0, TX = 1, CMD = 2 };
        bool alert(Event ev);

    private:
        theIPservice();
        ~theIPservice(); // nobody can accidentally delete the singleton

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace embot::net::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

