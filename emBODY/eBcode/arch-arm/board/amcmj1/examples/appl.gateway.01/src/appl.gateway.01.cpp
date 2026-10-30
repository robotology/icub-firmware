


/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

the amcmj1.appl.gateway: TETHER for diagnostics and accessory operations, and the link w/ the Speedgoat.

- tGATEWAY is the thread that owns the CAN (PX3Q, CubeMars) and the three sockets of the Speedgoat link:
    commands 5001   the Speedgoat sends a frame at every step (10 ms now, 1 ms later), also w/o commands: it is the clock of the
                    MIT. the thread wakes up at every frame, sends the MIT to the CubeMars and replies on the state socket
    rpc      5002   enable / disable of the CubeMars (run<OPcubeENABLE>, run<OPcubeDISABLE>) and of the PX3Q (run<OPpaxENABLE>,
             run<OPpaxDISABLE>); pids, setup and control mode of the macromotor (run<OPmotorPIDS>, <OPmotorSETUP>,
             <OPmotorMODE>); the EXTFAULT of the Speedgoat, held here (run<OPgwFAULT, 1 / 0>). served only if there is a frame
    state    5003   one frame per command frame: CubeMars, PX3Q torque, macromotor, counters, position sensor (amcmj1_gwlink.h)
- the macromotor is driven by amc.mot on the CM7: amcmj1_motdriver.h keeps what the Speedgoat asks and will talk to amc.mot
  over the ICC (to do). until then it plays amc.mot w/ a simple model (AMCMJ1_MOTDRIVER_EMULATED)
- the position sensor of the joint (an SPI encoder, embot::hw::encoder) is read every 1 ms by amcmj1_posdriver.h, w/ a 1 ms
  timer (evtPOStick). it goes in the state frame (IDposSTATE). a placeholder: emulated until the encoder is wired
  (AMCMJ1_POSDRIVER_EMULATED)
- the rules of the three sockets (parsing, limits, gate) are in amcmj1_gwlink.h/.cpp, tested on a PC in test/host

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
#include "amcmj1_motdriver.h"           // the macromotor: amc.mot on the CM7 (ICC to do, emulated for now)
#include "amcmj1_posdriver.h"           // the position sensor of the joint: SPI encoder every 1 ms (emulated for now)
#include "dictionary.amcmj1.h"         // the Dictionary: amcmj1::dict::dictionary and the handles amcmj1::vars::vXxx
#include "amcmj1_gateway.h"             // the events of the gateway thread and how the others signal it
#include "amcmj1_gwlink.h"              // the rules of the commands, rpc and state sockets of the Speedgoat link
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

    // the Speedgoat link (amcmj1_gwlink.h)
    constexpr uint16_t portCMD {5001};          // commands: the Speedgoat sends the MIT frames here
    constexpr uint16_t portGWRPC {5002};        // rpc of the gateway: enable / disable. it replies to the sender
    constexpr uint16_t portSTATE {5003};        // state: the board sends from here...
    constexpr uint16_t portHOSTSTATE {5003};    // ...to this port of the sender of the commands

} // namespace amcmj1::net

// ---- amcmj1::pax (PAXcfg, PAXstatus, PAXvalue, State, Command) is in amcmj1_paxdriver.h: shared by the rpc
//      operations, the gateway thread and the driver


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
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"


void startIPservice();

void onIdle(embot::os::Thread * /*t*/, void * /*idleparam*/)
{
    static uint32_t i = 0;
    i++;
}

void startGATEWAY();

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
    
    startGATEWAY();

    embot::core::print("quitting the INIT thread. Normal scheduling starts");
}


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig
    {
        embot::core::time1millisec, initcfg, idlecfg, onOSerror,
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
    5,                                      // numberofattachablesockets: 2 of theTETHERservice + 3 of tGATEWAY (MEMP_NUM_UDP_PCB is 6)
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

// 10 ms: the slow clock of tGATEWAY. it checks the watchdog and the acks of the PX3Q, and it publishes the shared variables. the MIT
// does not wait for it: it goes to the CAN at the arrival of the command frame. cube::CUBEcfg::repeatms (servo) has this granularity
constexpr embot::core::relTime tickperiod = 10*embot::core::time1millisec;

// 1 ms: the clock of the reads of the position sensor (amcmj1_posdriver.h). a read is non-blocking: this only starts it
constexpr embot::core::relTime posperiod = embot::core::time1millisec;

namespace amcmj1::gateway {

    // it is the only data of the thread that is not a shared variable. no constructor: it is constant-initialised
    struct Context
    {
        embot::net::eth::Socket *sockCMD {nullptr};
        embot::net::eth::Socket *sockRPC {nullptr};
        embot::net::eth::Socket *sockSTATE {nullptr};
        embot::net::eth::Packet *rxCMD {nullptr};
        embot::net::eth::Packet *rxRPC {nullptr};
        embot::net::eth::Packet *tx {nullptr};              // the one that the thread sends: they are used one at a time
        embot::net::eth::IPaddress speedgoat {0, 0, 0, 0};  // the sender of the last command frame: where the state goes

        // what the state frame tells, kept by the thread
        cube::CUBEstatus cubestatus {};
        cube::CUBEstate cubestate {};
        PAXtorque paxtorque {};

        GWload load {};
        uint32_t lastwakes {0};
        uint32_t lastcmdframes {0};
        uint32_t cmdframes {0};
        uint32_t ticks {0};
        uint32_t lastseqpublished {0};
        bool seqpublished {false};
    };

    Context ctx {};

    constexpr size_t packetcapacity {1500};

    void alertCMD(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtCMDrx); }
    void alertRPC(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtRPCrx); }

    // a socket of the Speedgoat link. the callbacks run in the IP service thread: they only wake tGATEWAY
    embot::net::eth::Socket *opensocket(embot::os::Thread *t, uint16_t port, void (*onrx)(void *),
                                        const embot::net::eth::Socket::Pipe &in, const embot::net::eth::Socket::Pipe &out,
                                        embot::net::eth::Socket::TXpriority txp)
    {
        const embot::core::Callback cbrx {onrx, t};
        const embot::net::eth::Socket::Properties props
        {
            port, embot::net::eth::IPany,
            { (nullptr != onrx) ? cbrx : embot::core::Callback {}, {}, {} },
            txp
        };
        embot::net::eth::Socket *s {new embot::net::eth::Socket(embot::net::eth::Socket::Config(in, out))};
        if(false == s->open(props))
        {
            embot::core::print("tGATEWAY: a socket of the Speedgoat link did not open");
        }
        return s;
    }

} // namespace amcmj1::gateway


void eventbasedthread_startup(embot::os::Thread *t, void * /*param*/)
{    
    using namespace amcmj1;
    using embot::net::eth::Socket;

    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(gateway::evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);

    // the position sensor: its own 1 ms timer
    if(false == amcmj1::posdriver::init())
    {
        embot::core::print("tGATEWAY: the position sensor does not initialise: IDposSTATE is not valid");
    }
    embot::os::Timer *tmrpos = new embot::os::Timer;
    embot::os::Timer::Config cfgpos{posperiod, embot::os::Action(embot::os::EventToThread(gateway::evtPOStick, t)), embot::os::Timer::Mode::forever, 0};
    tmrpos->start(cfgpos);

    gateway::sharedvars = &embot::app::eth::theTETHERservice::getInstance().sharedvariables();

    // the three sockets of the Speedgoat link. the commands are fresh data: a few packets and the oldest is lost if the thread is late
    gateway::ctx.rxCMD = new embot::net::eth::Packet(gateway::packetcapacity);
    gateway::ctx.rxRPC = new embot::net::eth::Packet(gateway::packetcapacity);
    gateway::ctx.tx = new embot::net::eth::Packet(gateway::packetcapacity);
    const Socket::Pipe tiny {Socket::Policy::dropnewest, 1, 64};
    const Socket::Pipe fresh {Socket::Policy::dropoldest, 4, gateway::packetcapacity};
    gateway::ctx.sockCMD = gateway::opensocket(t, net::portCMD, gateway::alertCMD, fresh, tiny, Socket::TXpriority::normal);
    gateway::ctx.sockRPC = gateway::opensocket(t, net::portGWRPC, gateway::alertRPC, Socket::Pipe {Socket::Policy::dropnewest, 2, 256},
                                               Socket::Pipe {Socket::Policy::dropnewest, 1, 256}, Socket::TXpriority::normal);
    gateway::ctx.sockSTATE = gateway::opensocket(t, net::portSTATE, nullptr, tiny, Socket::Pipe {Socket::Policy::dropoldest, 4, 256},
                                                 Socket::TXpriority::high);

    // the CAN of the PX3Q and of the CubeMars: its rx IRQ only wakes this thread, which drains the frames w/ canbus::rx()
    const embot::core::Callback onCANrx { [](void *p) { reinterpret_cast<embot::os::Thread*>(p)->setEvent(amcmj1::gateway::evtCANrx); }, t };
    if((false == amcmj1::canbus::open(onCANrx)) || (false == amcmj1::paxdriver::open()) || (false == amcmj1::cubedriver::open()))
    {
        embot::core::print("tGATEWAY: the CAN is not available: OPpaxINIT and OPcubeINIT will fail");
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


// ---- the enable gate. no CAN frame goes out here: the MIT of the command frames does, once the motor is running

// OPcubeENABLE: init w/ IDcubeCFG if needed, then start. true if the motor is running
bool enablecube(embot::app::eth::SharedVariables &sv)
{
    using namespace amcmj1;
    if(true == gateway::fault())
    {
        return false;                               // EXTFAULT (OPgwFAULT): refused until it is cleared
    }
    if(cube::State::running == cubedriver::state())
    {
        return true;                                // already enabled
    }
    if(cube::State::error == cubedriver::state())
    {
        cubedriver::deinit();                       // the CAN refused a frame: start again from off
    }
    if(cube::State::off == cubedriver::state())
    {
        cube::CUBEcfg cfg {};
        sv.read(vars::vCubeCfg, cfg);
        const bool ok {cubedriver::init(cfg)};
        reportcube(sv, cube::Command::init, ok);
        if(false == ok)
        {
            return false;
        }
    }
    const bool ok {cubedriver::start()};
    reportcube(sv, cube::Command::start, ok);
    return ok;
}

// OPcubeDISABLE: the disable packet. true also if the motor was not running
bool disablecube(embot::app::eth::SharedVariables &sv)
{
    using namespace amcmj1;
    if(cube::State::running != cubedriver::state())
    {
        return true;
    }
    const bool ok {cubedriver::stop()};
    reportcube(sv, cube::Command::stop, ok);
    return ok;
}


// OPpaxENABLE: init w/ IDpaxCFG if needed, then start. the PX3Q answers to its commands w/ an ack that arrives later (evtCANrx):
// this only begins the sequence init -> start, which continuepax() completes when the init is acknowledged. the ack of the rpc means
// "accepted": the stream shows in IDpaxTORQUE (counter). false if the PX3Q refuses the first command or is in a state that needs more
bool paxwanted {false};                             // the sequence is in progress: when the PX3Q is initialised, start it

bool enablepax(embot::app::eth::SharedVariables &sv)
{
    using namespace amcmj1;
    paxwanted = true;
    if(pax::State::error == paxdriver::state())
    {
        report(sv, pax::Command::deinit, paxdriver::deinit());     // a failed command: start again from off
    }
    switch(paxdriver::state())
    {
        case pax::State::running:
        {
            paxwanted = false;                      // already enabled
            return true;
        }
        case pax::State::busy:
        {
            return true;                            // a command is in progress: continuepax() goes on
        }
        case pax::State::initialised:
        {
            const bool ok {paxdriver::start()};
            report(sv, pax::Command::start, ok);
            paxwanted = ok;                         // running when the ack arrives
            return ok;
        }
        case pax::State::off:
        {
            pax::PAXcfg cfg {};
            sv.read(vars::vPaxCfg, cfg);
            const bool ok {paxdriver::init(cfg)};
            report(sv, pax::Command::init, ok);
            paxwanted = ok;                         // then continuepax() starts it
            return ok;
        }
        default:
        {
            paxwanted = false;
            return false;                           // still in error: see IDpaxSTATUS
        }
    }
}

// OPpaxDISABLE: the stream stops. true also if the PX3Q was not running
bool disablepax(embot::app::eth::SharedVariables &sv)
{
    using namespace amcmj1;
    paxwanted = false;                              // an enable in progress ends at initialised
    if(pax::State::running != paxdriver::state())
    {
        return true;
    }
    const bool ok {paxdriver::stop()};
    report(sv, pax::Command::stop, ok);
    return ok;
}

// call it after the CAN rx and at every tick: the second step of OPpaxENABLE
void continuepax(embot::app::eth::SharedVariables &sv)
{
    using namespace amcmj1;
    if(false == paxwanted)
    {
        return;
    }
    switch(paxdriver::state())
    {
        case pax::State::initialised:
        {
            paxwanted = false;
            report(sv, pax::Command::start, paxdriver::start());
        } break;
        case pax::State::busy:
        {
        } break;
        default:
        {
            paxwanted = false;                      // running, or the init failed (error): the sequence is over
        } break;
    }
}


// ---- the Speedgoat link

// one state frame to the sender of the last command frame
void sendstate()
{
    using namespace amcmj1;
    gateway::Context &c {gateway::ctx};
    if((nullptr == c.sockSTATE) || (0 == c.speedgoat.v))
    {
        return;
    }
    // the status that changes w/ the commands is in the shared variable: the driver knows the state and the watchdogs at once
    c.cubestatus.state = cubedriver::state();
    c.cubestatus.watchdogs = cubedriver::stats().watchdogs;

    uint8_t frame[gateway::statesize];
    size_t size {0};
    if(true == gateway::buildstate(frame, sizeof(frame), size, embot::core::now(), c.cubestatus, c.cubestate, c.paxtorque, motdriver::state(),
                                     posdriver::state()))
    {
        c.tx->load({c.speedgoat, net::portHOSTSTATE}, size, frame);
        c.sockSTATE->transmit(*c.tx);
    }
}

// the commands: all the frames that are waiting, one by one. the MIT goes to the CAN as it arrives
void handlecommands()
{
    using namespace amcmj1;
    gateway::Context &c {gateway::ctx};
    while((nullptr != c.sockCMD) && (true == c.sockCMD->receive(*c.rxCMD)))
    {
        c.cmdframes++;
        c.speedgoat = c.rxCMD->address().addr;
        if(gateway::Frame::bad != gateway::oncommands(c.rxCMD->data(), c.rxCMD->size(), embot::core::now()))
        {
            sendstate();
        }
    }
}

// the rpc of the gateway: enable / disable. only if there is a frame
void handlerpc(embot::app::eth::SharedVariables &sv)
{
    using namespace amcmj1;
    gateway::Context &c {gateway::ctx};
    while((nullptr != c.sockRPC) && (true == c.sockRPC->receive(*c.rxRPC)))
    {
        const gateway::Rpc r {gateway::onrpc(c.rxRPC->data(), c.rxRPC->size())};
        bool done {false};
        const char *why {nullptr};
        switch(r.request)
        {
            case gateway::Request::enable:
            {
                done = enablecube(sv);
                why = gateway::fault() ? "EXTFAULT: cleared only by OPgwFAULT 0" : "cube not enabled: see IDcubeSTATUS";
            } break;
            case gateway::Request::disable: { done = disablecube(sv); why = "cube not disabled"; } break;
            case gateway::Request::paxenable:  { done = enablepax(sv); why = "px3q not enabled: see IDpaxSTATUS"; } break;
            case gateway::Request::paxdisable: { done = disablepax(sv); why = "px3q not disabled"; } break;
            case gateway::Request::motorpids:
            {
                const motdriver::Result res {motdriver::setpids(r.pids)};
                done = (motdriver::Result::ok == res); why = motdriver::why(res);
            } break;
            case gateway::Request::motorsetup:
            {
                const motdriver::Result res {motdriver::setsetup(r.setup)};
                done = (motdriver::Result::ok == res); why = motdriver::why(res);
            } break;
            case gateway::Request::motormode:
            {
                if((true == gateway::fault()) && (mot::ControlMode::idle != r.mode))
                {
                    why = "EXTFAULT: only idle until OPgwFAULT 0";
                    break;
                }
                const motdriver::Result res {motdriver::setmode(r.mode, embot::core::now())};
                done = (motdriver::Result::ok == res); why = motdriver::why(res);
            } break;
            case gateway::Request::gwfault:
            {
                // set: the gateway stops both motors at once and holds it. clear: they can be used again (nothing restarts)
                gateway::setfault(r.fault);
                if(true == r.fault)
                {
                    disablecube(sv);
                    motdriver::setmode(mot::ControlMode::idle, embot::core::now());
                }
                done = true;
            } break;
            case gateway::Request::badargument: { why = "wrong size of the argument"; } break;
            default:                        { why = "not served on this socket"; } break;
        }
        uint8_t frame[64];
        size_t size {0};
        if(true == gateway::buildreply(frame, sizeof(frame), size, embot::core::now(), r, done, why))
        {
            c.tx->load(c.rxRPC->address(), size, frame);
            c.sockRPC->transmit(*c.tx);
        }
    }
}


void eventbasedthread_onevent(embot::os::Thread * /*t*/, embot::os::EventMask eventmask, void * /*param*/)
{
    using embot::core::binary::mask::check;
    using namespace amcmj1;

    if((0 == eventmask) || (nullptr == gateway::sharedvars))
    {   // timeout, or not started yet
        return;
    }
    embot::app::eth::SharedVariables &sv {*gateway::sharedvars};    // taken in the startup, see before
    gateway::Context &ctx {gateway::ctx};
    const embot::core::Time t0 {embot::core::now()};
    ctx.load.wakes++;

    // the position sensor: take the last read and start the next one (non-blocking), before the state frame is built
    if(check(eventmask, gateway::evtPOStick))
    {
        posdriver::tick(t0);
    }

    // the commands first, one at a time thanks to the priorities. they only send CAN frames: the outcome arrives
    // later w/ the ack (evtCANrx) or w/ its timeout (evtTick), and publishstate() shows it
    if(check(eventmask, gateway::evtPAXinit))
    {
        pax::PAXcfg cfg {};
        sv.read(vars::vPaxCfg, cfg);                          // the whole struct, coherent
        report(sv, pax::Command::init, paxdriver::init(cfg));
    }
    
    if(check(eventmask, gateway::evtPAXstart))  
    { 
        report(sv, pax::Command::start, paxdriver::start()); 
    }
    
    if(check(eventmask, gateway::evtPAXstop))
    {
        report(sv, pax::Command::stop, paxdriver::stop());
    }

    if(check(eventmask, gateway::evtPAXdeinit))
    {
        report(sv, pax::Command::deinit, paxdriver::deinit());
    }

    if(check(eventmask, gateway::evtPAXenable))
    {
        enablepax(sv);
    }

    if(check(eventmask, gateway::evtPAXdisable))
    {
        disablepax(sv);
    }

    if(check(eventmask, gateway::evtCUBEinit))
    {
        cube::CUBEcfg cfg {};
        sv.read(vars::vCubeCfg, cfg);
        reportcube(sv, cube::Command::init, cubedriver::init(cfg));
    }

    if(check(eventmask, gateway::evtCUBEstart))
    {
        reportcube(sv, cube::Command::start, cubedriver::start());
    }

    if(check(eventmask, gateway::evtCUBEstop))
    {
        reportcube(sv, cube::Command::stop, cubedriver::stop());
    }

    if(check(eventmask, gateway::evtCUBEdeinit))
    {
        reportcube(sv, cube::Command::deinit, cubedriver::deinit());
    }

    if(check(eventmask, gateway::evtCUBEenable))
    {
        enablecube(sv);
    }

    if(check(eventmask, gateway::evtCUBEdisable))
    {
        disablecube(sv);
    }

    if(check(eventmask, gateway::evtCUBElimits))
    {
        gateway::GWlimits l {};
        sv.read(vars::vCubeLimits, l);
        gateway::setlimits(l);
    }

    if(check(eventmask, gateway::evtRPCrx))
    {
        handlerpc(sv);
    }

    if(check(eventmask, gateway::evtCANrx))
    {
        // the frames of the bus go to the PX3Q and to the CubeMars. every drain publishes the latest reading of each: the
        // stream of TETHER samples it anyway, the state frame takes it from the copies of the thread
        canbus::rx();
        continuepax(sv);                            // the PX3Q may have acknowledged the init of an OPpaxENABLE

        pax::PAXvalue value {};
        if(true == paxdriver::take(value))
        {
            sv.write(vars::vPaxValue, value, tether::Origin::local);
            ctx.paxtorque = gateway::PAXtorque {};
            ctx.paxtorque.torque[0] = value.torque[0];
            ctx.paxtorque.torque[1] = value.torque[1];
            ctx.paxtorque.torque[2] = value.torque[2];
            ctx.paxtorque.counter = value.counter;
        }

        cube::CUBEstate cs {};
        if(true == cubedriver::take(cs))
        {
            sv.write(vars::vCubeState, cs, tether::Origin::local);
            ctx.cubestate = cs;
        }
    }

    // the Speedgoat: every frame is a MIT to the CubeMars and a state frame back. after the CAN, so that the state is the latest
    if(check(eventmask, gateway::evtCMDrx))
    {
        handlecommands();
    }

    if(check(eventmask, gateway::evtTick))
    {
        const embot::core::Time now {embot::core::now()};

        // the timeout of the acks of the PX3Q
        paxdriver::tick(now);
        continuepax(sv);

        // a servo setpoint written w/ OPcubeSERVO (the bench): the driver sends it every cfg.repeatms. the MIT does not wait for the tick
        if(cube::State::running == cubedriver::state())
        {
            cube::CUBEservo servo {};
            sv.read(vars::vCubeServo, servo);
            cubedriver::update(servo, now);
        }
        // the watchdog of both: no command frame (or servo write) for cfg.watchdogms -> disable
        cubedriver::tick(now);

        // the macromotor: no setpoint for mot::watchdog -> idle. w/ the emulation, the model goes on
        motdriver::tick(now);

        // the copies for TETHER: the last MIT accepted and the counters of the link. they change at the rate of the frames,
        // so they are written at the rate of the tick
        cube::CUBEmit lastmit {};
        if(true == gateway::takelastmit(lastmit))
        {
            sv.write(vars::vCubeMIT, lastmit, tether::Origin::local);
        }
        sv.write(vars::vGwStats, gateway::stats(), tether::Origin::local);

        // the copies of the macromotor for TETHER
        sv.write(vars::vMotorState, motdriver::state(), tether::Origin::local);
        sv.write(vars::vMotorPids, motdriver::pids(), tether::Origin::local);
        sv.write(vars::vMotorSetup, motdriver::setup(), tether::Origin::local);
        sv.write(vars::vMotorTarget, motdriver::lasttarget(), tether::Origin::local);
        sv.write(vars::vMotorStatus, motdriver::status(), tether::Origin::local);

        // the copy of the position sensor for TETHER
        sv.write(vars::vPosState, posdriver::state(), tether::Origin::local);

        // the status of the CubeMars for the state frame
        sv.read(vars::vCubeStatus, ctx.cubestatus);

        // the load, once per second
        if(++ctx.ticks >= (1000 / (tickperiod / embot::core::time1millisec)))
        {
            ctx.ticks = 0;
            ctx.load.wakespersecond = ctx.load.wakes - ctx.lastwakes;
            ctx.lastwakes = ctx.load.wakes;
            ctx.load.cmdframes = ctx.cmdframes - ctx.lastcmdframes;
            ctx.lastcmdframes = ctx.cmdframes;
            ctx.load.maxcanframes = canbus::stats().maxframes;
            sv.write(vars::vGwLoad, ctx.load, tether::Origin::local);
        }

        // the test sine on dummyUINT32: lock-free
        sv.write(vars::vDummyUINT32, signals::sine(now), tether::Origin::local);

        // ... the control
    }

    publishstate(sv);
    publishcube(sv);

    const embot::core::Time dt {embot::core::now() - t0};
    if(dt > ctx.load.maxloopus)
    {
        ctx.load.maxloopus = static_cast<uint32_t>(dt);
    }
}

void tGATEWAY(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

void startGATEWAY()
{    
    embot::os::EventThread::Config configEV { 
        9*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent,
        "tGATEWAY"
    };
               

    amcmj1::gateway::thread = new embot::os::EventThread;          
    amcmj1::gateway::thread->start(configEV, tGATEWAY);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

