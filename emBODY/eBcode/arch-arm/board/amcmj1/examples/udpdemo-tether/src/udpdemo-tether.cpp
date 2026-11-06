/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

the udpdemo.tether of amcmj1 (CM4, boot master): the UDP echo of udpdemo-embot-net plus TETHER, w/ a didactic Dictionary.

- the init thread starts theLogger (embot::core::print() also over UDP to the host, port 9999), theIPservice (lwIP, the
  ETH and its switch, ping), then theTETHERservice, then the echo thread
- theTETHERservice owns its two sockets and threads:
    rpc      6666   ask / set / sig / run / ping and browse of the Dictionary. it replies to the sender
    stream   7777   it streams the variables chosen by the host to <IP of who started it>:7777, and accepts only ping
- the echo thread (tECHO) owns the socket of the echo port 666: it replies to every frame w/ the same frame, w/ the first
  byte replaced by a marker. it shares its data w/ the host through the Dictionary (dictionary.udpdemo.h):
    IDechoCONFIG    RW  how it replies: marker, enabled, uppercase. the host sets it, the thread reads it at every frame
    IDechoSTATS     RO  its counters. the thread writes them; OPechoRESET clears them
    IDechoLAST      RO  who sent the last frame and when
    IDticks         RW  incremented by the thread every 5 ms (the timeout of its event loop); the host can set it too
  it also owns two variables of the application that are not in the Dictionary, computed every 5 ms:
    wave    a float: a sine. w/ tetherdebugENABLE_wave the Dictionary has a view of it (IDviewof_wave)
    pwm     a struct of three floats protected by a mutex: three sines 120 degrees apart. w/ tetherdebugENABLE_pwm the
            Dictionary has a view of it (IDviewof_pwm)
  the views are copies taken on read w/ echo::readwave() and echo::readpwm()
  and it executes OPechoHELLO: the operation, in the rpc thread of TETHER, posts evtHELLO to it (udpdemo_echo.h)
- the host: tether.py in host/ of this example (see README.md of this example). the echo and the log: e.g. netcat

    echo -n "xyz" | nc -u -w1 10.0.1.99 666         ->  "-yz"
    nc -u 10.0.1.99 9999  + <Enter>                 ->  the lines of embot::core::print() in the same terminal (ncat on Windows)
                                                        then the commands of theLogger: off, on, deco on, status, help

#endif


#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"
#include "embot_hw_dualcore.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_os_Thread.h"

#include "embot_app_theLEDmanager.h"

#include "embot_net_eth.h"
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"
#include "embot_net_eth_theIPservice.h"
#include "embot_net_theLogger.h"

#include "embot_app_eth_TETHER.h"
#include "embot_app_eth_theTETHERservice.h"

#include "dictionary.udpdemo.h"         // the Dictionary: udpdemo::dict::dictionary and the handles udpdemo::vars::vXxx
#include "udpdemo_echo.h"               // the events of the echo thread and how the operations signal it
#include "udpdemo_types.h"

#include <cmath>
#include <cstring>
#include <string>

namespace tether = embot::app::eth::tether;
using embot::app::eth::theTETHERservice;


// ---- the network

namespace udpdemo::net {

    constexpr embot::net::eth::IPaddress localaddress {10, 0, 1, 99};
    constexpr embot::net::eth::IPaddress hostaddress {10, 0, 1, 104};

    constexpr embot::net::eth::Port portRPC {6666};         // TETHER: the rpc socket of the board. it replies to the sender
    constexpr embot::net::eth::Port portSTREAM {7777};      // TETHER: the stream socket of the board. it accepts only ping
    constexpr embot::net::eth::Port portHOSTSTREAM {7777};  // TETHER: where the host receives the stream (tether.py str)
    constexpr embot::net::eth::Port portLOG {9999};         // theLogger: the lines of print() go to host:9999, from board:9999

    constexpr embot::net::eth::IPconfig ipconfig
    {
        {0x70, 0x9A, 0x0B, 0x00, 0x00, 0x00},   // mac address
        localaddress,                           // ip address
        {255, 255, 255, 0},                     // netmask
        hostaddress                             // gateway
    };

    constexpr embot::net::eth::theIPservice::Config ipservice
    {
        ipconfig,
        4,                                      // numberofattachablesockets: 1 echo + 2 TETHER + 1 theLogger (MEMP_NUM_UDP_PCB is 8: lwip.csol.8udp.tx8k)
        embot::os::Priority::system50,          // above all the threads that use sockets
        4*1024,
        10*embot::core::time1millisec,
        1500
    };

    constexpr embot::net::theLogger::Config logger
    {
        embot::net::eth::SocketAddress {hostaddress, portLOG},     // destination at start: the host. the commands hello and dest change it
        localaddress,                                               // the IP shown by the decoration [10.0.1.99 @ time]
        portLOG,                                                    // the port of the board
        embot::net::theLogger::Activation::ON,                      // the commands on / off change it
        embot::net::theLogger::Decoration::ON,                      // the commands deco on / deco off change it
        embot::net::theLogger::Commands::ON,                        // the text commands on port 9999 (see embot_net_theLogger.h)
        true,                                                       // print() also goes to ITM, as before
        1024,                                                       // bytes of lines in a frame
        embot::os::Thread::Props {embot::os::Priority::low10, 2*1024}
    };

} // namespace udpdemo::net


// ---- theTETHERservice

// the version of the application. TETHER only carries it to the host (MGversion)
constexpr uint32_t applicationversion {0x01000000};

constexpr theTETHERservice::Config tetherconfig
{
    udpdemo::dict::dictionary,
    applicationversion,
    theTETHERservice::Network
    {
        udpdemo::net::portRPC, udpdemo::net::portSTREAM, udpdemo::net::portHOSTSTREAM,
        true, udpdemo::net::hostaddress                                 // force the ARP to the host at start
    },
    theTETHERservice::Stream
    {
        100*embot::core::time1millisec,                                 // set<MGstreamSTART, 0>: the default period
        1*embot::core::time1millisec, 10*embot::core::time1second,      // set<MGstreamSTART, ms>: ms in [min=1, max=10000]
        10*embot::core::time1second                                     // keepalive: tether.py str pings every 2 s
    },
    embot::os::Thread::Props {embot::os::Priority::abovenorm37, 6 * 1024},     // rpc
    embot::os::Thread::Props {embot::os::Priority::abovenorm36, 6 * 1024}      // stream
};


// ---- the echo thread

namespace udpdemo::echo {

    void startup(embot::os::Thread *t, void *param);
    void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

    // the socket and the packets: pointers, constant initialised to nullptr (no constructor runs before main()). the
    // objects are created by startup(), in the echo thread
    constexpr size_t packetcapacity {1500};
    embot::net::eth::Socket *socket {nullptr};
    embot::net::eth::Packet *rxP {nullptr};
    embot::net::eth::Packet *txP {nullptr};

    // the variables of the Dictionary that this thread shares w/ the host
    embot::app::eth::SharedVariables &shared() { return theTETHERservice::getInstance().sharedvariables(); }

    // ---- two variables of the application. the echo thread computes them at every timeout of its loop (always, w/ or
    //      w/out TETHER) and could use them for its own purposes. they are not in the Dictionary: w/ the macros
    //      tetherdebugENABLE_wave and tetherdebugENABLE_pwm the Dictionary has a view of each of them, a copy that it takes
    //      w/ readwave() and readpwm() when the host reads it. no header declares them: the other files use the two
    //      functions. they are not in an anonymous namespace so that a debugger finds them by their names,
    //      udpdemo::echo::wave and udpdemo::echo::pwm (uVision: Watch window and Logic Analyzer; VS Code: Watch)

    // wave: a plain float, as in the existing code. this thread writes it and the threads of TETHER read it w/out a lock:
    // an aligned scalar of 4 bytes is written and read w/ one instruction (STR / LDR) on Cortex-M4 / M7, so a reader
    // never sees half of it. it is the hardware that guarantees it, not C++ (formally it is a data race). to have the
    // guarantee of C++, keep a copy in a std::atomic<float> (see docs/howto-add-a-view.md)
    float wave {0.0f};

    float readwave()
    {
        return wave;
    }

    // pwm: a struct of 12 bytes. a reader w/out a lock could take u new and v, w old: a triple that never existed. so
    // this thread writes it w/ pwmmutex taken, and readpwm() copies it w/ pwmmutex taken. the mutex is created by
    // startup() of this thread, before the first write. rule: do not access the SharedVariables of TETHER w/ pwmmutex
    // taken (the view calls readpwm() w/ the mutex of the SharedVariables taken: the opposite order would deadlock)
    PWM pwm {};
    embot::os::rtos::mutex_t *pwmmutex {nullptr};

    PWM readpwm()
    {
        if(nullptr == pwmmutex)
        {
            return pwm;                                 // the echo thread has not started: nobody writes it yet
        }
        embot::os::rtos::Lock lock {pwmmutex};
        return pwm;
    }

    void alertRX(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtRXframe); }
    void alertTX(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtTXframe); }
    void alertER(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtERframe); }

    void startup(embot::os::Thread *t, void *param)
    {
        pwmmutex = embot::os::rtos::mutex_new();
        const embot::net::eth::Socket::Properties props
        {
            port, embot::net::eth::IPany,                   // listens on the echo port and accepts from any IP address
            {
                embot::core::Callback {alertRX, t},         // a frame has arrived
                embot::core::Callback {alertTX, t},         // a frame was given to lwIP
                embot::core::Callback {alertER, t}          // an error
            },
            embot::net::eth::Socket::TXpriority::normal
        };

        // the default size of its pipes: 1 packet of 1500 bytes in rx and in tx, policy dropnewest
        socket = new embot::net::eth::Socket({});
        socket->open(props);
        socket->connect(udpdemo::net::hostaddress);         // force the ARP to the host

        rxP = new embot::net::eth::Packet(packetcapacity);
        txP = new embot::net::eth::Packet(packetcapacity);
    }

    // the reply: the same frame, w/ the first byte replaced by the marker and, if asked, the letters in uppercase
    void prepare(const types::EchoCONFIG &cfg, embot::net::eth::Packet &reply, const embot::net::eth::Packet &request)
    {
        reply.load(request);
        uint8_t *d {reply.data()};
        const size_t s {reply.size()};
        if(s > 0)
        {
            d[0] = cfg.marker;
        }
        if(cfg.uppercase)
        {
            for(size_t i = 1; i < s; i++)
            {
                if((d[i] >= 'a') && (d[i] <= 'z'))
                {
                    d[i] = static_cast<uint8_t>(d[i] - 'a' + 'A');
                }
            }
        }
    }

    void onRX(embot::os::Thread *t)
    {
        if(false == socket->receive(*rxP))
        {
            return;
        }

        const embot::net::eth::SocketAddress from {rxP->address()};
        const size_t size {rxP->size()};
        const uint32_t ip {from.addr.value()};

        // the data shared w/ the host
        shared().update(vars::vEchoStats, [size](types::EchoSTATS &st)
        {
            st.rxframes++;
            st.rxbytes += static_cast<uint32_t>(size);
        }, tether::Origin::local);

        const types::EchoLAST last
        {
            static_cast<uint8_t>(ip & 0xff), static_cast<uint8_t>((ip >> 8) & 0xff),
            static_cast<uint8_t>((ip >> 16) & 0xff), static_cast<uint8_t>((ip >> 24) & 0xff),
            from.port, static_cast<uint16_t>(size), static_cast<uint64_t>(embot::core::now())
        };
        shared().write(vars::vEchoLast, last, tether::Origin::local);

        // the config given by the host
        types::EchoCONFIG cfg {};
        shared().read(vars::vEchoConfig, cfg);

        if(cfg.enabled)
        {
            prepare(cfg, *txP, *rxP);
            socket->transmit(*txP);
        }

        embot::core::print("echo: " + std::to_string(size) + " bytes from " + from.to_string() + (cfg.enabled ? ", replied" : ", not replied"));

        rxP->setsize(0);
        rxP->setaddress({});

        // one frame per event: if there are others, process them at the next round
        if(socket->input() > 0)
        {
            t->setEvent(evtRXframe);
        }
    }

    // OPechoHELLO: a frame to the last sender on the echo port
    void onHELLO()
    {
        types::EchoLAST last {};
        shared().read(vars::vEchoLast, last);
        if(0 == last.port)
        {
            return;
        }
        static constexpr char hello[] {"hello from amcmj1"};
        const embot::net::eth::SocketAddress to {embot::net::eth::IPaddress {last.ip[0], last.ip[1], last.ip[2], last.ip[3]}, last.port};
        txP->load(to, sizeof(hello) - 1, hello);
        socket->transmit(*txP);
        embot::core::print("echo: hello sent to " + to.to_string());
    }

    void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {
        if(0 == eventmask)
        {
            // the timeout of the event loop (5 ms): a scalar that the thread writes and the host can also set
            shared().update(vars::vTicks, [](uint32_t &n) { n++; }, tether::Origin::local);

            // the variables of the application: sines of period 1 s and amplitude 10.0
            constexpr float amplitude {10.0f};
            constexpr float twopi {6.2831853f};
            const float phase {static_cast<float>(embot::core::now() % embot::core::time1second) / static_cast<float>(embot::core::time1second)};
            wave = amplitude * std::sin(twopi * phase);

            const PWM p
            {
                amplitude * std::sin(twopi * phase),
                amplitude * std::sin(twopi * phase - twopi / 3.0f),
                amplitude * std::sin(twopi * phase - 2.0f * twopi / 3.0f)
            };
            {
                embot::os::rtos::Lock lock {pwmmutex};
                pwm = p;
            }
            return;
        }

        if(true == embot::core::binary::mask::check(eventmask, evtRXframe))
        {
            onRX(t);
        }

        if(true == embot::core::binary::mask::check(eventmask, evtTXframe))
        {
            shared().update(vars::vEchoStats, [](types::EchoSTATS &st) { st.txframes++; }, tether::Origin::local);
        }

        if(true == embot::core::binary::mask::check(eventmask, evtERframe))
        {
            shared().update(vars::vEchoStats, [](types::EchoSTATS &st) { st.errors++; }, tether::Origin::local);
            embot::core::print("echo: socket error");
        }

        if(true == embot::core::binary::mask::check(eventmask, evtHELLO))
        {
            onHELLO();
        }
    }

    void tECHO(void *p)
    {
        embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
        t->run();
    }

    void start()
    {
        const embot::os::EventThread::Config config
        {
            6*1024,
            embot::os::Priority::high40,        // below theIPservice (system50)
            startup,
            nullptr,
            5*embot::core::time1millisec,       // timeout of the event loop: IDticks counts it
            onevent,
            "tECHO"
        };

        embot::os::EventThread *t {new embot::os::EventThread};
        thread = t;                             // from now on the operations can signal it (udpdemo_echo.h)
        t->start(config, tECHO);
    }

} // namespace udpdemo::echo


// ---- the init thread, the idle thread, main()

void onIdle(embot::os::Thread *t, void *idleparam)
{
    static uint32_t i = 0;
    i++;
}

void initSystem(embot::os::Thread *t, void *initparam)
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

    // the lines of embot::core::print() also to host:9999. it is started first so that it keeps all the lines printed from
    // now on: its socket is opened by its thread, which runs after this init thread (so after theIPservice::initialise())
    embot::net::theLogger::getInstance().initialise(udpdemo::net::logger);

    // lwIP, the ETH and its switch, the ping: before every service that uses sockets
    embot::net::eth::theIPservice::getInstance().initialise(udpdemo::net::ipservice);

    // TETHER: its sockets 6666 and 7777, its threads, the SharedVariables of the Dictionary
    if(false == theTETHERservice::getInstance().initialise(tetherconfig))
    {
        embot::core::print("theTETHERservice::initialise() failed");
    }

    // the echo on port 666: after TETHER, because it uses its SharedVariables
    udpdemo::echo::start();

    embot::core::print("quitting the INIT thread. Normal scheduling starts");
}

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


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

