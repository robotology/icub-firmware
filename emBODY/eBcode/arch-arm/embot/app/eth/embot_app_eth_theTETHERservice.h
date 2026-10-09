
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THETETHERSERVICE_H_
#define __EMBOT_APP_ETH_THETETHERSERVICE_H_

#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_net_eth.h"
#include "embot_app_eth_TETHER.h"
#include "embot_app_eth_SharedVariables.h"
#include <memory>


// theTETHERservice: TETHER, the link between a host and a board, all in one.
// - the Dictionary of the board: its variables and operations, given as a reference to a static tether::Dictionary (it can be
//   defined in another file: only its address is needed by the constexpr Config). from it the
//   service builds the SharedVariables: the threads of the board use it thread-safely (sharedvariables()), TETHER uses it for
//   the host
// - two UDP sockets attached to embot::net::eth::theIPservice:
//   - rpc (default :6666): ask / set / sig / run / ping and the management (stream, browse, version, stats).
//     it replies to the sender
//   - stream (default :7777): it transmits the stream to <IP of who started it>:Network::hostport, and it accepts
//     only ping, which keeps alive the stream
// - two threads, one per socket. the stream thread runs a timer w/ the period asked by the host
//
// the other threads of the board use dictionary(), better w/ the typed handles tether::Var<T>.
//
// call initialise() once from the init thread, after theIPservice::initialise()

namespace embot::app::eth {

    class theTETHERservice
    {
    public:

        static theTETHERservice &getInstance();

        struct Network
        {
            embot::net::eth::Port rpcport {6666};
            embot::net::eth::Port streamport {7777};
            embot::net::eth::Port hostport {7777};                      // where the host receives the stream
            bool arp {false};                                           // if true, the sockets connect() to arphost
            embot::net::eth::IPaddress arphost {0, 0, 0, 0};            // at start, to force the ARP resolution

            constexpr Network() = default;
            constexpr explicit Network(embot::net::eth::Port rp, embot::net::eth::Port sp, embot::net::eth::Port hp, bool a, embot::net::eth::IPaddress ah)
                : rpcport(rp), streamport(sp), hostport(hp), arp(a), arphost(ah) {}
        };

        struct Stream
        {
            embot::core::relTime defaultperiod {100 * embot::core::time1millisec};    // set<MGstreamSTART, 0>
            embot::core::relTime minperiod {1 * embot::core::time1millisec};          // set<MGstreamSTART, ms>: ms in
            embot::core::relTime maxperiod {10 * embot::core::time1second};           // [minperiod, maxperiod]
            embot::core::relTime keepalive {10 * embot::core::time1second};           // 0 = never stops by itself

            constexpr Stream() = default;
            constexpr explicit Stream(embot::core::relTime dp, embot::core::relTime minp, embot::core::relTime maxp, embot::core::relTime ka)
                : defaultperiod(dp), minperiod(minp), maxperiod(maxp), keepalive(ka) {}
        };

        struct Config
        {
            const tether::Dictionary *dictionary {nullptr};     // the Dictionary of the board: it must be static (it is not copied). nullptr = not given
            uint32_t applicationversion {0};            // the version of the application, from its own source (e.g. its Signature). 0 = not given. MGversion tells it to the host
            Network network {};
            Stream stream {};
            embot::os::Thread::Props rpcthread {embot::os::Priority::abovenorm37, 6*1024};
            embot::os::Thread::Props streamthread {embot::os::Priority::abovenorm36, 6*1024};

            constexpr Config() = default;
            constexpr explicit Config(const tether::Dictionary &d, uint32_t av, const Network &n, const Stream &s, const embot::os::Thread::Props &rt, const embot::os::Thread::Props &st)
                : dictionary(&d), applicationversion(av), network(n), stream(s), rpcthread(rt), streamthread(st) {}
        };

        // it creates the SharedVariables from the Dictionary, the managers, the sockets and the threads. false if the Dictionary or
        // the stream periods are not valid, or if already initialised
        bool initialise(const Config &config);
        bool initialised() const;

        // the SharedVariables, for the other threads of the board. before initialise() it is empty: every access fails
        embot::app::eth::SharedVariables &sharedvariables();

    private:
        theTETHERservice();
        ~theTETHERservice();

    public:
        theTETHERservice(const theTETHERservice &) = delete;
        theTETHERservice &operator=(const theTETHERservice &) = delete;
        theTETHERservice(theTETHERservice &&) = delete;
        theTETHERservice &operator=(theTETHERservice &&) = delete;

        struct Impl;        // opaque: the sockets, the threads and the timer live in the .cpp

    private:
        std::unique_ptr<Impl> pImpl;
    };

} // namespace embot::app::eth


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
