
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theTETHERservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core_binary.h"
#include "embot_os_Thread.h"
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"
#include "embot_net_eth_theIPservice.h"
#include "embot_app_eth_theRPCmanager.h"
#include "embot_app_eth_theSTRmanager.h"

// --------------------------------------------------------------------------------------------------------------------
// - impl
// --------------------------------------------------------------------------------------------------------------------


void tTETHERstr(void *p)
{
    reinterpret_cast<embot::os::Thread *>(p)->run();
}

void tTETHERrpc(void *p)
{
    reinterpret_cast<embot::os::Thread *>(p)->run();
}

namespace {

    using embot::app::eth::rop::theRPCmanager;
    using embot::app::eth::rop::theSTRmanager;

    constexpr embot::os::Event evtRX {embot::core::binary::mask::pos2mask<embot::os::Event>(0)};       // a frame has arrived
    constexpr embot::os::Event evtTICK {embot::core::binary::mask::pos2mask<embot::os::Event>(1)};     // time to stream
    constexpr embot::os::Event evtPERIOD {embot::core::binary::mask::pos2mask<embot::os::Event>(2)};   // the period has changed
    constexpr embot::os::Event evtERROR {embot::core::binary::mask::pos2mask<embot::os::Event>(3)};    // a socket error

    // the timeout of the threads: a safety net that re-checks the period of the stream
    constexpr embot::core::relTime threadtimeout {100 * embot::core::time1millisec};

    // the same capacity of the default pipes of a Socket
    constexpr size_t packetcapacity {1500};

    void alertRX(void *p)
    {
        reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtRX);
    }

    void alertERROR(void *p)
    {
        reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtERROR);
    }
}


namespace embot::app::eth {

    struct theTETHERservice::Impl
    {
        Config config {};
        SharedVariablesLUT *dict {nullptr};

        // rpc
        embot::os::EventThread *thrRPC {nullptr};
        embot::net::eth::Socket *sockRPC {nullptr};
        embot::net::eth::Packet rxRPC {packetcapacity};
        embot::net::eth::Packet txRPC {packetcapacity};

        // stream
        embot::os::EventThread *thrSTR {nullptr};
        embot::net::eth::Socket *sockSTR {nullptr};
        embot::net::eth::Packet rxSTR {packetcapacity};
        embot::net::eth::Packet txSTR {packetcapacity};
        embot::os::Timer *timer {nullptr};
        embot::os::Timer::Config timerconfig {0, {}, embot::os::Timer::Mode::forever};
        embot::core::relTime armedperiod {0};       // the period of the running timer, 0 if stopped
        bool initialised {false};
        SharedVariablesLUT empty {nullptr, 0};      // returned by sharedvariables() before initialise(): every access fails

        Impl() = default;

        embot::net::eth::Socket *opensocket(embot::os::Thread *t, embot::net::eth::Port port);
        void rearm();
    };


    // ---- the sockets

    embot::net::eth::Socket *theTETHERservice::Impl::opensocket(embot::os::Thread *t, embot::net::eth::Port port)
    {
        const embot::core::Callback onrx {alertRX, t};
        const embot::core::Callback onerror {alertERROR, t};
        const embot::net::eth::Socket::Properties props
        {
            port, embot::net::eth::IPany,                   // it listens on port and accepts from any IP address
            {
                onrx,                                       // a frame has arrived
                {},                                         // a frame has been delivered: not needed
                onerror                                     // any error
            },
            embot::net::eth::Socket::TXpriority::normal
        };

        // the default pipes: 1 packet of 1500 bytes in tx and in rx
        embot::net::eth::Socket *s {new embot::net::eth::Socket({})};
        s->open(props);
        if(config.network.arp)
        {   // it forces the ARP resolution of the host, so that the first reply is not lost
            s->connect(config.network.arphost);
        }
        return s;
    }


    // ---- the rpc thread

    void rpcstartup(embot::os::Thread *t, void *param)
    {
        theTETHERservice::Impl *impl {static_cast<theTETHERservice::Impl *>(param)};
        impl->sockRPC = impl->opensocket(t, impl->config.network.rpcport);
    }

    void rpconevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {
        theTETHERservice::Impl *impl {static_cast<theTETHERservice::Impl *>(param)};

        if(true == embot::core::binary::mask::check(eventmask, evtRX))
        {
            if(true == impl->sockRPC->receive(impl->rxRPC))
            {
                theRPCmanager &rpc {theRPCmanager::getInstance()};
                const embot::net::eth::IPaddress from {impl->rxRPC.address().addr};
                rpc.parse(impl->rxRPC.data(), impl->rxRPC.size(), from);

                const void *payload {nullptr};
                size_t size {0};
                if(true == rpc.get(payload, size))
                {   // reply to the sender: IP and port
                    impl->txRPC.load(impl->rxRPC.address(), size, payload);
                    impl->sockRPC->transmit(impl->txRPC);
                }
            }

            if(impl->sockRPC->input() > 0)
            {   // one frame per event: the others at the next one
                t->setEvent(evtRX);
            }
        }
    }


    // ---- the stream thread

    // it keeps the timer aligned w/ the period asked by the host
    void theTETHERservice::Impl::rearm()
    {
        const embot::core::relTime period {theSTRmanager::getInstance().period()};
        if(period == armedperiod)
        {
            return;
        }
        if(0 != armedperiod)
        {
            timer->stop();
        }
        if(0 != period)
        {
            timerconfig.countdown = period;
            timer->start(timerconfig);
        }
        armedperiod = period;
    }

    // called by theSTRmanager when the period changes, in the thread that changes it (rpc, or stream for the keepalive)
    void onperiodchange(void *param)
    {
        theTETHERservice::Impl *impl {static_cast<theTETHERservice::Impl *>(param)};
        if(nullptr != impl->thrSTR)
        {
            impl->thrSTR->setEvent(evtPERIOD);
        }
    }

    void strstartup(embot::os::Thread *t, void *param)
    {
        theTETHERservice::Impl *impl {static_cast<theTETHERservice::Impl *>(param)};
        impl->sockSTR = impl->opensocket(t, impl->config.network.streamport);
        impl->timer = new embot::os::Timer;
        impl->timerconfig.onexpiry = embot::os::Action(embot::os::EventToThread(evtTICK, t));
        impl->timerconfig.countdown = 0;
        impl->armedperiod = 0;
    }

    void stronevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {
        theTETHERservice::Impl *impl {static_cast<theTETHERservice::Impl *>(param)};
        theSTRmanager &str {theSTRmanager::getInstance()};

        if((0 == eventmask) || (true == embot::core::binary::mask::check(eventmask, evtPERIOD)))
        {   // a new period, or the timeout of the thread as a safety net
            impl->rearm();
        }

        if(true == embot::core::binary::mask::check(eventmask, evtRX))
        {
            if(true == impl->sockSTR->receive(impl->rxSTR))
            {   // only ping is accepted: its ack goes back to the sender
                const void *reply {nullptr};
                size_t replysize {0};
                const embot::net::eth::IPaddress from {impl->rxSTR.address().addr};
                if(true == str.onrx(impl->rxSTR.data(), impl->rxSTR.size(), from, reply, replysize))
                {
                    impl->txSTR.load(impl->rxSTR.address(), replysize, reply);
                    impl->sockSTR->transmit(impl->txSTR);
                }
            }

            if(impl->sockSTR->input() > 0)
            {
                t->setEvent(evtRX);
            }
        }

        if(true == embot::core::binary::mask::check(eventmask, evtTICK))
        {
            const void *payload {nullptr};
            size_t size {0};
            theSTRmanager::Destination dst {};
            if(true == str.get(payload, size, dst))
            {
                impl->txSTR.load({dst.ip, dst.port}, size, payload);
                impl->sockSTR->transmit(impl->txSTR);
            }
            else if(false == str.running())
            {   // stopped meanwhile, e.g. by the keepalive
                impl->rearm();
            }
        }
    }


    // ---- theTETHERservice

    theTETHERservice &theTETHERservice::getInstance()
    {
        static theTETHERservice *p = new theTETHERservice();
        return *p;
    }

    theTETHERservice::theTETHERservice()
    {
        pImpl = std::make_unique<Impl>();
    }

    theTETHERservice::~theTETHERservice() { }

    bool theTETHERservice::initialised() const
    {
        return pImpl->initialised;
    }

    SharedVariables &theTETHERservice::sharedvariables()
    {
        // before initialise(): an empty table, never initialised, so every access fails. it lives in Impl: a static
        // object w/ a destructor here would register it w/ __aeabi_atexit
        return (true == pImpl->initialised) ? *pImpl->dict : pImpl->empty;
    }

    bool theTETHERservice::initialise(const Config &config)
    {
        if(true == pImpl->initialised)
        {
            return false;
        }

        if(nullptr == config.dictionary)
        {
            return false;
        }

        Impl *impl {pImpl.get()};
        impl->config = config;

        // the SharedVariables, built from the Dictionary
        const tether::Dictionary &d {*config.dictionary};
        impl->dict = new SharedVariablesLUT {d.variables, d.numberofvariables};
        if(false == impl->dict->initialise())
        {
            return false;
        }

        // the stream: theSTRmanager tells the stream thread when the period changes
        theSTRmanager &str {theSTRmanager::getInstance()};
        const theSTRmanager::Config strconfig
        {
            impl->dict, config.stream.defaultperiod, config.stream.minperiod, config.stream.maxperiod,
            config.network.hostport, config.stream.keepalive, onperiodchange, impl
        };
        if(false == str.initialise(strconfig))
        {
            return false;
        }

        // the rpc
        theRPCmanager &rpc {theRPCmanager::getInstance()};
        const theRPCmanager::Config rpcconfig
        {
            impl->dict, &str, d.operations, d.numberofoperations, nullptr, nullptr, config.applicationversion
        };
        if(false == rpc.initialise(rpcconfig))
        {
            return false;
        }

        // the threads. the stream one first, so that it exists when the rpc thread changes the period
        const embot::os::EventThread::Config strthread
        {
            config.streamthread.stacksize, config.streamthread.priority,
            strstartup, impl, threadtimeout, stronevent, "tTETHERstr"
        };
        impl->thrSTR = new embot::os::EventThread;
        impl->thrSTR->start(strthread, tTETHERstr);

        const embot::os::EventThread::Config rpcthread
        {
            config.rpcthread.stacksize, config.rpcthread.priority,
            rpcstartup, impl, threadtimeout, rpconevent, "tTETHERrpc"
        };
        impl->thrRPC = new embot::os::EventThread;
        impl->thrRPC->start(rpcthread, tTETHERrpc);

        impl->initialised = true;
        return true;
    }

} // namespace embot::app::eth

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
