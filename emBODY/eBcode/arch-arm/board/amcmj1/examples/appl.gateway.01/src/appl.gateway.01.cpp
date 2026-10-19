


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
#include "amcmj1_canbus.h"              // the CAN bus shared by the drivers
#include "amcmj1_paxdriver.h"           // the driver of the PX3Q over CAN, w/ PAXcfg and PAXstatus
#include "amcmj1_cubedriver.h"         // the driver of the CubeMars over CAN, w/ CUBEcfg, CUBEmit, CUBEstate
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

// 10 ms: it is the period of the setpoint of the CubeMars (cube::CUBEcfg::repeatms has this granularity)
constexpr embot::core::relTime tickperiod = 10*embot::core::time1millisec;

void eventbasedthread_startup(embot::os::Thread *t, void * /*param*/)
{    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(amcmj1::ctrl::evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    amcmj1::ctrl::sharedvars = &embot::app::eth::theTETHERservice::getInstance().sharedvariables();

    // the CAN of the PX3Q and of the CubeMars: its rx IRQ only wakes this thread, which drains the frames w/ canbus::rx()
    const embot::core::Callback onCANrx { [](void *p) { reinterpret_cast<embot::os::Thread*>(p)->setEvent(amcmj1::ctrl::evtCANrx); }, t };
    if((false == amcmj1::canbus::open(onCANrx)) || (false == amcmj1::paxdriver::open()) || (false == amcmj1::cubedriver::open()))
    {
        embot::core::print("tCTRL: the CAN is not available: OPpaxINIT and OPcubeINIT will fail");
    }
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

// the same for the CubeMars: the state changes also when the CAN refuses a frame (-> error), the watchdogs when it stops the motor
void reportcube(embot::app::eth::SharedVariables &sv, amcmj1::cube::Command c, bool accepted)
{
    sv.update(amcmj1::vars::vCubeStatus, [c, accepted](amcmj1::cube::CUBEstatus &st)
    {
        st.lastcommand = c;
        if(accepted) { st.commands++; } else { st.errors++; }
    }, tether::Origin::local);
}

void publishcube(embot::app::eth::SharedVariables &sv)
{
    static amcmj1::cube::State last {amcmj1::cube::State::off};
    static uint32_t lastwd {0};
    const amcmj1::cube::State s {amcmj1::cubedriver::state()};
    const uint32_t wd {amcmj1::cubedriver::stats().watchdogs};
    if((s == last) && (wd == lastwd))
    {
        return;
    }
    const bool toerror {(amcmj1::cube::State::error == s) && (last != s)};
    last = s;
    lastwd = wd;
    sv.update(amcmj1::vars::vCubeStatus, [s, wd, toerror](amcmj1::cube::CUBEstatus &st)
    {
        st.state = s;
        st.watchdogs = wd;
        if(toerror) { st.errors++; }
    }, tether::Origin::local);
}

void eventbasedthread_onevent(embot::os::Thread * /*t*/, embot::os::EventMask eventmask, void * /*param*/)
{
    using embot::core::binary::mask::check;
    using namespace amcmj1;

    if((0 == eventmask) || (nullptr == ctrl::sharedvars))
    {   // timeout, or not started yet
        return;
    }
    embot::app::eth::SharedVariables &sv {*ctrl::sharedvars};    // taken in the startup, see before

    // the commands first, one at a time thanks to the priorities. they only send CAN frames: the outcome arrives
    // later w/ the ack (evtCANrx) or w/ its timeout (evtTick), and publishstate() shows it
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

    if(check(eventmask, ctrl::evtCUBEinit))
    {
        cube::CUBEcfg cfg {};
        sv.read(vars::vCubeCfg, cfg);
        reportcube(sv, cube::Command::init, cubedriver::init(cfg));
    }

    if(check(eventmask, ctrl::evtCUBEstart))
    {
        reportcube(sv, cube::Command::start, cubedriver::start());
    }

    if(check(eventmask, ctrl::evtCUBEstop))
    {
        reportcube(sv, cube::Command::stop, cubedriver::stop());
    }

    if(check(eventmask, ctrl::evtCUBEdeinit))
    {
        reportcube(sv, cube::Command::deinit, cubedriver::deinit());
    }

    if(check(eventmask, ctrl::evtCANrx))
    {
        // the frames of the bus go to the PX3Q and to the CubeMars. publish only the latest reading of each: the stream of TETHER
        // samples it anyway
        canbus::rx();

        pax::PAXvalue value {};
        if(true == paxdriver::take(value))
        {
            sv.write(vars::vPaxValue, value, tether::Origin::local);
        }

        cube::CUBEstate cs {};
        if(true == cubedriver::take(cs))
        {
            sv.write(vars::vCubeState, cs, tether::Origin::local);
        }
    }

    if(check(eventmask, ctrl::evtTick))
    {
        const embot::core::Time now {embot::core::now()};

        // the timeout of the acks of the PX3Q
        paxdriver::tick(now);

        // the CubeMars: the setpoints written by the host. when seq changes the driver sends them at once, then it sends
        // them again every cfg.repeatms. if seq stops changing for cfg.watchdogms the motor is disabled
        if(cube::State::running == cubedriver::state())
        {
            cube::CUBEmit mit {};
            cube::CUBEservo servo {};
            sv.read(vars::vCubeMIT, mit);
            sv.read(vars::vCubeServo, servo);
            cubedriver::update(mit, now);
            cubedriver::update(servo, now);
        }
        cubedriver::tick(now);

        // the test sine on dummyUINT32: lock-free
        sv.write(vars::vDummyUINT32, signals::sine(now), tether::Origin::local);

        // ... the control
    }

    publishstate(sv);
    publishcube(sv);
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

