


/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

this file tests theTETHERservice: TETHER, the link to the amcmj1.appl.gateway for diagnostics and accessory operations

- the Dictionary (variables and operations) is constexpr and checked at compile time by TETHER_VALIDATE()
- theTETHERservice owns the rpc socket (6666), the stream socket (7777), their threads and the timer of the stream
- the host does not need a table kept by hand: tether.py reads the Dictionary from the board (browse) and writes
  dictionary.toml by itself. so the names below are the ones the host uses

#endif


#include "embot_app_eth_TETHER.h"
#include "embot_app_eth_theTETHERservice.h"
#include "embot_core.h"
#include "embot_core_binary.h"          
#include "embot_os_Thread.h"
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "amcmj1_paxdriver.h"           // the driver of the PX3Q: protocol only, the CAN is owned by tCTRL (here). w/ PAXcfg and PAXstatus
#include "dictionary.amcmj1.h"         // the Dictionary: amcmj1::dict::dictionary and the handles amcmj1::vars::vXxx
#include "amcmj1_ctrl.h"                // the events of the ctrl thread and how the others signal it
#include "embot_hw_led.h"
#include "embot_app_theLEDmanager.h"
#include <cmath>
#include <cstddef>
#include <string>

namespace tether = embot::app::eth::tether;
using embot::app::eth::theTETHERservice;


// ---- the network

namespace amcmj1::net {

    constexpr embot::net::eth::IPaddress localaddress {10, 0, 1, 99};
    constexpr embot::net::eth::IPaddress hostaddress {10, 0, 1, 104};

    constexpr uint16_t portRPC {6666};          // the rpc socket of the board. it replies to the sender
    constexpr uint16_t portSTREAM {7777};       // the stream socket of the board. it accepts only ping
    constexpr uint16_t portHOSTSTREAM {7777};   // where the host receives the stream (tether.py str)

} // namespace amcmj1::net

// ---- amcmj1::pax (PAXcfg, PAXstatus, PAXvalue, State, Command) is in amcmj1_paxdriver.h: shared by the rpc
//      operations, the ctrl thread and the driver


// ---- the Dictionary and theTETHERservice

// the version of the application: the one of the Signature, as 4 bytes. TETHER only carries it to the host (MGversion)
constexpr uint32_t applicationversion {0x01000000};

constexpr theTETHERservice::Config tetherconfig
{
    amcmj1::dict::dictionary,
    applicationversion,
    theTETHERservice::Network
    {
        amcmj1::net::portRPC, amcmj1::net::portSTREAM, amcmj1::net::portHOSTSTREAM,
        true, amcmj1::net::hostaddress // force the ARP to the host at start
    },
    theTETHERservice::Stream
    {
        100*embot::core::time1millisec,                             // set<MGstreamSTART, 0>: the default period
        1*embot::core::time1millisec, 10*embot::core::time1second,  // set<MGstreamSTART, ms>: ms in [min=1, max=10000]        
        10*embot::core::time1second                                 // keepalive: tether.py str pings every 2 s
    },
    embot::os::Thread::Props {embot::os::Priority::abovenorm37, 6 * 1024},     // rpc
    embot::os::Thread::Props {embot::os::Priority::abovenorm36, 6 * 1024}      // stream
};


// -------------------------------------------------------------------------------------------------------------------


#include "embot_hw_dualcore.h"
#include "stm32hal.h"

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include "embot_hw_bsp_config.h"

#include "embot_net_eth.h"
#include "embot_net_eth_theIPservice.h"


void startIPservice();

void onIdle(embot::os::Thread * /*t*/, void * /*idleparam*/)
{
    static uint32_t i = 0;
    i++;
}

void startCTRL();

void initSystem(embot::os::Thread * /*t*/, void * /*initparam*/)
{
    embot::os::theTimerManager::getInstance().start({});
    embot::os::theCallbackManager::getInstance().start({});

    static const std::initializer_list<embot::hw::LED> allleds =
    {
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three
    };

    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();

    theleds.init(allleds);
    for(const auto &l : allleds)
    {
        theleds.get(l).on();
        theleds.get(l).off();
    }

    // theTETHERservice needs theIPservice
    startIPservice();

    if(false == theTETHERservice::getInstance().initialise(tetherconfig))
    {
        embot::core::print("theTETHERservice::initialise() failed");
    }
    
    startCTRL();

    embot::core::print("quitting the INIT thread. Normal scheduling starts");
}


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::core::Time tickperiod {embot::core::time1millisec}; //{500*embot::core::time1microsec}; // {embot::core::time1millisec}; 
    constexpr embot::os::Config osconfig
    {
        tickperiod, initcfg, idlecfg, onOSerror,
        embot::hw::FLASHpartitionID::eapplication01
    };

    bool iamthemaster = embot::hw::dualcore::ismaster();

    if(true == iamthemaster)
    {
        constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};
        embot::hw::dualcore::config(dualcoreconfig);
    }

    embot::os::init(osconfig);
    embot::os::start();
}


// ---- theIPservice

constexpr embot::net::eth::IPconfig ipconfig
{
    {0x70, 0x9A, 0x0B, 0x00, 0x00, 0x00},   // mac address
    amcmj1::net::localaddress,              // ip address
    {255, 255, 255, 0},                     // netmask
    {10, 0, 1, 104}                         // gateway
};

constexpr embot::net::eth::theIPservice::Config ipSERcfg
{
    ipconfig,
    4,                                      // numberofattachablesockets
    embot::os::Priority::system50,          // above the threads of theTETHERservice
    4*1024,
    10*embot::core::time1millisec,
    1500
};

void startIPservice()
{
    // it starts lwip, the related ETH hw, it manages ping and accepts the sockets
    embot::net::eth::theIPservice::getInstance().initialise(ipSERcfg);
}


// --------------------

constexpr embot::core::relTime tickperiod = 100*embot::core::time1millisec;

#include "embot_hw_can.h"
#include "theCANdispatcher.h"

std::vector<embot::hw::can::Frame> _tmpoutCANframes;

void alertonrxcanframe(void *arg)
{
    embot::os::EventThread* evthr = reinterpret_cast<embot::os::EventThread*>(arg);
    if(nullptr != evthr)
    {
        evthr->setEvent(amcmj1::ctrl::evtRXcan);
    }
}

constexpr embot::hw::CAN bus {embot::hw::CAN::one};

// the CAN is owned by tCTRL: it inits it, receives the frames (evtRXcan) and transmits the ones in _tmpoutCANframes.
// false if the CAN is not available on this core (see EMBOT_ENABLE_hw_can in the bsp)
bool can_start(embot::os::Thread *t)
{
    if(false == embot::hw::can::supported(bus))
    {
        return false;
    }

    // init the can ...
    embot::hw::can::Config canconfig {};
    canconfig.rxcapacity = 32;
    canconfig.txcapacity = 32;
    canconfig.onrxframe = embot::core::Callback(alertonrxcanframe, t); 
    if(embot::hw::resOK != embot::hw::can::init(bus, canconfig))
    {
        return false;
    }
            
    // pre-allocate output vector of can frame
    
    _tmpoutCANframes.reserve(16);
        
    // and ok, enable can
    return (embot::hw::resOK == embot::hw::can::enable(bus));
}

// every protocol on the bus, in the order in which they look at a frame. for now only the PX3Q, later the CubeMars.
// the dispatcher is created in eventbasedthread_startup(): the only static is this pointer, so no static object w/ a ctor
// or a dtor (nothing at startup, no guard, no atexit). there: canparsers->add(*amcmj1::paxdriver::parser()); canparsers->add(*cubemars::parser());
embot::app::can::theCANdispatcher<1> *canparsers {nullptr};

void parse_can(embot::os::Thread * /*t*/, embot::os::EventMask /*eventmask*/, void * /*param*/, const embot::hw::can::Frame &rxframe, std::vector<embot::hw::can::Frame> & /*outputframes*/)
{
    // the parser of the protocol that owns the frame handles it: an ack advances the sequence of commands of the PX3Q,
    // a torque is copied into the memory of its driver. the frames to send (e.g. the next command after an ack) are
    // pushed by the driver into _tmpoutCANframes, which is the vector given to paxdriver::bind(): outputframes
    // is that same vector. a frame of nobody is counted by the dispatcher: canparsers->stats().unknown
    if(nullptr != canparsers)
    {
        canparsers->process(rxframe);
    }
}

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{
    if(true == can_start(t))
    {
        // the driver of the PX3Q does not own the CAN: the frames it has to send go into _tmpoutCANframes
        amcmj1::paxdriver::bind(_tmpoutCANframes);
        canparsers = new embot::app::can::theCANdispatcher<1>();
        canparsers->add(*amcmj1::paxdriver::parser());
    }
    else
    {
        embot::core::print("tCTRL: the CAN is not available: the commands of the PX3Q will be refused");
    }
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(amcmj1::ctrl::evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    amcmj1::ctrl::sharedvars = &embot::app::eth::theTETHERservice::getInstance().sharedvariables();
}



namespace amcmj1::signals {

    constexpr embot::core::relTime sineperiod {3 * embot::core::time1second};
    constexpr float sineoffset {1000.0f};       // dummyUINT32 is unsigned: the sine oscillates in [0, 2000]
    constexpr float sineamplitude {1000.0f};
    constexpr float twopi {6.28318530718f};

    // the value of the sine at time t. the phase uses t modulo the period in integer microseconds, so it stays
    // precise also after days of uptime (a float of t in seconds would lose resolution)
    uint32_t sine(embot::core::Time t)
    {
        const float phase {static_cast<float>(t % sineperiod) / static_cast<float>(sineperiod)};   // [0, 1)
        const float v {sineoffset + sineamplitude * std::sin(twopi * phase)};
        return static_cast<uint32_t>(std::lround(v));
    }

} // namespace amcmj1::signals



// it updates the status in one critical section: a command accepted by the driver counts in commands, a refused
// one in errors. the state is written only by publishstate()
void report(embot::app::eth::SharedVariables &sv, amcmj1::pax::Command c, bool accepted)
{
    sv.update(amcmj1::vars::vPaxStatus, [c, accepted](amcmj1::pax::PAXstatus &st)
    {
        st.lastcommand = c;
        if(accepted) { st.commands++; } else { st.errors++; }
    }, tether::Origin::local);
}

// the state changes also w/out a command: when the ack arrives (busy -> target) or when it does not (busy -> error)
void publishstate(embot::app::eth::SharedVariables &sv)
{
    static amcmj1::pax::State last {amcmj1::pax::State::off};
    const amcmj1::pax::State s {amcmj1::paxdriver::state()};
    if(s == last)
    {
        return;
    }
    last = s;
    sv.update(amcmj1::vars::vPaxStatus, [s](amcmj1::pax::PAXstatus &st)
    {
        st.state = s;
        if(amcmj1::pax::State::error == s) { st.errors++; }
    }, tether::Origin::local);
}

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    using embot::core::binary::mask::check;
    using namespace amcmj1;

    if((0 == eventmask) || (nullptr == ctrl::sharedvars))
    {   // timeout, or not started yet
        return;
    }
    embot::app::eth::SharedVariables &sv {*ctrl::sharedvars};    // taken in the startup, see before
    
    if(check(eventmask, ctrl::evtRXcan))  
    {        
        // attempt to get all messages in one shot
        std::uint8_t insideRXQ = embot::hw::can::inputqueuesize(bus);
        std::uint8_t remainingINrx = 0;
        embot::hw::can::Frame hwframe {};
        for(uint8_t i=0; i<insideRXQ; i++)
        {
            remainingINrx = 0;            
            if(embot::hw::resOK == embot::hw::can::get(bus, hwframe, remainingINrx))
            {
                // parse hwframe: acks and torques of the PX3Q. any frame to send is pushed into _tmpoutCANframes
                parse_can(t, eventmask, param, hwframe, _tmpoutCANframes);                                
            }
        }
        // if any arrives since we called embot::hw::can::inputqueuesize(bus) ...
        if(remainingINrx > 0)
        {
            t->setEvent(ctrl::evtRXcan);                 
        }

        // the stream of the PX3Q: the parser has copied the torques into the memory of the driver. publish only the
        // latest reading: the stream of TETHER samples it anyway
        pax::PAXvalue value {};
        if(true == paxdriver::take(value))
        {
            sv.write(vars::vPaxValue, value, tether::Origin::local);
        }
    }

    // the commands, one at a time thanks to the priorities. they only push CAN frames into _tmpoutCANframes: the outcome
    // arrives later w/ the ack (evtRXcan) or w/ its timeout (evtTick), and publishstate() shows it
    if(check(eventmask, ctrl::evtPAXinit))
    {
        pax::PAXcfg cfg {};
        sv.read(vars::vPaxCfg, cfg);                          // the whole struct, coherent
        report(sv, pax::Command::init, paxdriver::init(cfg));
    }
    
    if(check(eventmask, ctrl::evtPAXstart))  
    { 
        report(sv, pax::Command::start, paxdriver::start()); 
    }
    
    if(check(eventmask, ctrl::evtPAXstop))
    {
        report(sv, pax::Command::stop, paxdriver::stop());
    }

    if(check(eventmask, ctrl::evtPAXdeinit))
    {
        report(sv, pax::Command::deinit, paxdriver::deinit());
    }

    if(check(eventmask, ctrl::evtTick))
    {
        const embot::core::Time now {embot::core::now()};

        // the timeout of the acks of the PX3Q
        paxdriver::tick(now);

        // the test sine on dummyUINT32: lock-free
        sv.write(vars::vDummyUINT32, signals::sine(now), tether::Origin::local);

        // ... the control
    }

    // the state of the PX3Q changes with a command, an ack or a timeout
    publishstate(sv);
    
    // evaluate can tx
    size_t num = _tmpoutCANframes.size();
    if(num > 0)
    {        
        for(size_t i=0; i<num; i++)
        {
            embot::hw::can::put(bus, _tmpoutCANframes[i]);
        }
        
        embot::hw::can::transmit(bus);         
    }  
    
    // and now we can clear the msgs to be trasmitted
    _tmpoutCANframes.clear();
    
}

void tCTRL(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

void startCTRL()
{    
    embot::os::EventThread::Config configEV { 
        9*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent,
        "tCTRL"
    };
               

    amcmj1::ctrl::thread = new embot::os::EventThread;          
    amcmj1::ctrl::thread->start(configEV, tCTRL);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

