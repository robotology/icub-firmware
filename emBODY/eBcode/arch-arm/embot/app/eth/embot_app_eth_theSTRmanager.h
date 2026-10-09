
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THESTRMANAGER_H_
#define __EMBOT_APP_ETH_THESTRMANAGER_H_

#include "embot_core.h"
#include "embot_os_rtos.h"
#include "embot_app_eth_ROP.h"
#include "embot_app_eth_SharedVariables.h"
#include <array>
#include <atomic>


namespace embot::app::eth::rop {

    // the manager of the streaming socket.
    // - it transmits frames of sig<> ROPs, one per streamed variable, to the host that started the stream, i.e. to
    //   <IP of the sender of set<MGstreamSTART, ms>>:Config::hostport
    // - on the streaming socket it accepts only ping<>, which it acknowledges to the sender. the rest is discarded
    //
    // threads:
    // - add() / rem() / clear() / list() / start() / stop() / destination(): rpc thread (via theRPCmanager)
    // - get() / onrx(): streaming thread
    // - touch() / running() / periodms() / period(): any thread
    //
    // efficiency: the streamed variables change rarely, while get() may run at a high rate. so get() rebuilds the
    // skeleton of the frame (header + the 4-byte head of every ROP) only after a change of the list. otherwise it
    // takes no mutex of its own: it copies the values into the data fields (one critical section of the
    // SharedVariables for all of them) and writes the timestamp. the slots have the smallest size that holds the
    // biggest streamed variable.
    //
    // the period: start(0) uses Config::defaultperiod, start(ms) w/ ms in [Config::minperiod, Config::maxperiod] uses
    // ms. stop() stops. every change of the period (also the stop of the keepalive) calls Config::onchange, so the
    // streaming thread can re-arm its timer.
    //
    // keepalive: if Config::keepalive > 0 the stream stops by itself when nothing arrives from its host for that
    // time. any valid frame from the host refreshes it: a rpc frame, or a ping on the streaming socket.
    class theSTRmanager
    {
    public:

        static theSTRmanager &getInstance();

        // called when the period changes: start(), stop(), or the keepalive
        using fpOnChange = void (*)(void *param);

        struct Config
        {
            embot::app::eth::SharedVariables *sharedvariables {nullptr};
            embot::core::relTime defaultperiod {100 * embot::core::time1millisec};  // used by start(0)
            embot::core::relTime minperiod {1 * embot::core::time1millisec};
            embot::core::relTime maxperiod {10 * embot::core::time1second};
            embot::net::eth::Port hostport {7777};                   // where the host receives the stream
            embot::core::relTime keepalive {0};         // 0 = the stream never stops by itself
            fpOnChange onchange {nullptr};
            void *onchangeparam {nullptr};

            Config() = default;
            explicit Config(embot::app::eth::SharedVariables *v, embot::core::relTime dp, embot::core::relTime minp,
                            embot::core::relTime maxp, uint16_t hp, embot::core::relTime ka, fpOnChange oc, void *ocp)
                : sharedvariables(v), defaultperiod(dp), minperiod(minp), maxperiod(maxp), hostport(hp), keepalive(ka),
                  onchange(oc), onchangeparam(ocp) {}

            // the periods are whole ms, w/ 1 ms <= minperiod <= defaultperiod <= maxperiod <= 65535 ms
            bool isvalid() const
            {
                constexpr embot::core::relTime ms {embot::core::time1millisec};
                const bool whole {(0 == (defaultperiod % ms)) && (0 == (minperiod % ms)) && (0 == (maxperiod % ms))};
                const bool ordered {(minperiod >= ms) && (minperiod <= defaultperiod) && (defaultperiod <= maxperiod) &&
                                    (maxperiod <= 65535 * ms)};
                return (nullptr != sharedvariables) && (hostport > 0) && whole && ordered;
            }
        };

        struct Destination
        {
            embot::net::eth::IPaddress ip {};
            embot::net::eth::Port port {0};

            Destination() = default;
            explicit Destination(const embot::net::eth::IPaddress &i, embot::net::eth::Port p) : ip(i), port(p) {}
        };

        struct Stats
        {
            uint32_t txframes {0};
            uint32_t readfailures {0};    // frames in which some variable could not be read (its data is the old one)
            uint32_t rebuilds {0};        // times the skeleton of the frame was rebuilt
            uint32_t rxframes {0};        // frames received on the streaming socket
            uint32_t rxpings {0};         // pings acknowledged
            uint32_t rxdiscarded {0};     // wrong frames and ROPs other than ping
            uint32_t keepalivestops {0};  // times the stream stopped because its host was silent

            Stats() = default;
        };

        // call it from the init thread, after the RTOS has started
        bool initialise(const Config &config);

        // ---- rpc thread

        // it adds a variable to the stream. false if the variable is unknown, bigger than ROP::datasize, or
        // if the stream already holds FrameHeader::maxROPs variables. true if already streamed
        bool add(ID id);
        // false if the variable was not streamed
        bool rem(ID id);
        void clear();
        StreamList list() const;

        // it starts (or restarts) the stream toward host:Config::hostport. ms = 0 uses Config::defaultperiod, any other
        // value must be in [Config::minperiod, Config::maxperiod]. false if ms is out of range or host is 0
        bool start(uint16_t ms, const embot::net::eth::IPaddress &host);
        bool stop();
        Destination destination() const;

        // ---- any thread: a valid frame has arrived from IP from. it refreshes the keepalive if from is the host

        void touch(const embot::net::eth::IPaddress &from);

        // ---- any thread

        bool running() const;
        // the period in ms, 0 if stopped (also by the keepalive)
        uint16_t periodms() const;
        // the period the streaming thread must use. 0 if stopped
        embot::core::relTime period() const;

        // ---- streaming thread

        // it gives the frame to transmit and where to send it: one sig<> per streamed variable, w/ values read in a
        // single coherent snapshot of the SharedVariables, and the header stamped w/ embot::core::now().
        // false if stopped (also by the keepalive) or if no variable is streamed. memory valid until the next get()
        bool get(const void *&payload, size_t &size, Destination &destination);

        // call it for every frame received on the streaming socket. if it returns true, transmit reply to the sender.
        // only ping<> is accepted (-> ack<ID, ping>), the rest is discarded. memory valid until the next onrx()
        bool onrx(const void *payload, size_t size, const embot::net::eth::IPaddress &from, const void *&reply, size_t &replysize);

        // ---- anyone

        Stats stats() const;

    private:
        theSTRmanager() = default;
        ~theSTRmanager() = default;

        void rebuild();         // streaming thread
        bool expired();         // streaming thread: true if the keepalive has expired, and then it stops the stream
        void changed() const;   // it calls Config::onchange

    public:
        theSTRmanager(const theSTRmanager &) = delete;
        theSTRmanager &operator=(const theSTRmanager &) = delete;
        theSTRmanager(theSTRmanager &&) = delete;
        theSTRmanager &operator=(theSTRmanager &&) = delete;

    private:
        using Item = embot::app::eth::SharedVariables::Item;

        Config _config {};
        mutable embot::os::rtos::mutex_t *_mtx {nullptr};

        // written by the rpc thread under _mtx, read by the streaming thread. no 64-bit atomics: they are not
        // lock-free on a Cortex-M
        StreamList _list {};
        std::atomic<uint32_t> _version {0};             // bumped at every change of _list
        std::atomic<uint16_t> _periodms {0};          // 0 = stopped
        std::atomic<embot::net::eth::IPaddress> _host {embot::net::eth::IPany};     // 4 bytes: lock-free
        std::atomic<uint32_t> _lastseenms {0};          // time of the last frame from the host, in ms

        // streaming thread only: the frame ready to go and the items that point into its data fields
        alignas(8) std::array<uint8_t, maxsizeofframe> _txbuffer {};
        std::array<Item, FrameHeader::maxROPs> _items {};
        uint8_t _count {0};
        uint8_t _slot {ROP::minsizeofROP};
        size_t _txsize {0};
        uint32_t _builtversion {0};
        bool _built {false};
        // streaming thread only: the replies to the pings. an ack w/out data needs the smallest slot
        alignas(8) std::array<uint8_t, sizeof(FrameHeader) + FrameHeader::maxROPs * ROP::minsizeofROP> _pingbuffer {};

        // statistics, readable by anyone
        std::atomic<uint32_t> _txframes {0};
        std::atomic<uint32_t> _readfailures {0};
        std::atomic<uint32_t> _rebuilds {0};
        std::atomic<uint32_t> _rxframes {0};
        std::atomic<uint32_t> _rxpings {0};
        std::atomic<uint32_t> _rxdiscarded {0};
        std::atomic<uint32_t> _keepalivestops {0};
    };

} // namespace embot::app::eth::rop


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
