/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

the udpdemo.tether of amcmj1 (CM4, boot master): the UDP echo of udpdemo-embot-net plus TETHER, w/ a didactic Dictionary.

- the init thread starts theReporter (the reports of the board, printed: so on the ITM and on theLogger), theLogger
  (embot::core::print() also over UDP to the host, port 9999), theIPservice (lwIP, the ETH and its switch, ping), then
  theTETHERservice, then the echo thread and the alive thread
- the alive thread (tALIVE) reports w/ severity info every 5 s. w/ udpdemo::report::enableFAULT = true it causes a
  HardFault at its 3rd report: the fault handler gives the registers to theReporter, which keeps them in a RAM that
  survives the reset and resets the board. at the boot the first report is that fault (previous run), and the alive
  thread does not fault again until the next power-on
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
  it measures its latency, from a frame taken from the socket to its reply given to the socket, in a histogram of
  embot::instr (128 bins of 2 us): IDviewof_latency is a snapshot of it taken on read w/ echo::readlatency()
  and it executes OPechoHELLO: the operation, in the rpc thread of TETHER, posts evtHELLO to it (udpdemo_echo.h)
- the host: tether (embot/svc/host/tether), run from host/ of this example (see README.md of this example). the echo and the log: e.g. netcat

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

#include "embot_svc_theLEDmanager.h"
#include "embot_svc_theReporter.h"
#include "embot_instr.h"

#include "embot_net_eth.h"
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"
#include "embot_net_eth_theIPservice.h"
#include "embot_svc_theLogger.h"

#include "embot_svc_tether.h"
#include "embot_svc_theTETHERservice.h"

#include "dictionary.udpdemo.h"         // the Dictionary: udpdemo::dict::dictionary and the handles udpdemo::vars::vXxx
#include "udpdemo_echo.h"               // the events of the echo thread and how the operations signal it
#include "udpdemo_types.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

namespace tether = embot::svc::tether;
using embot::svc::theTETHERservice;


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

    constexpr embot::svc::theLogger::Config logger
    {
        embot::net::eth::SocketAddress {hostaddress, portLOG},     // destination at start: the host. the commands hello and dest change it
        localaddress,                                               // the IP shown by the decoration [10.0.1.99 @ time]
        portLOG,                                                    // the port of the board
        embot::svc::theLogger::Activation::ON,                      // the commands on / off change it
        embot::svc::theLogger::Decoration::ON,                      // the commands deco on / deco off change it
        embot::svc::theLogger::Commands::ON,                        // the text commands on port 9999 (see embot_svc_theLogger.h)
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
        true, udpdemo::net::hostaddress,                                   // force the ARP to the host at start
        embot::svc::tether::rop::maxsizeofframe                            // the biggest frame: 1472 bytes
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
    embot::svc::tether::SharedVariables &shared() { return theTETHERservice::getInstance().sharedvariables(); }

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

    // latency: the time from a frame taken from the socket to its reply given to the socket, in a ScopeHisto of
    // embot::instr: on() when the frame is taken, off() when the reply is given (only if the echo replies). this thread
    // calls off(), which adds to the histogram, w/ latencymutex taken, and readlatency() copies the histogram w/
    // latencymutex taken. same rule as pwmmutex: do not access the SharedVariables of TETHER w/ latencymutex taken.
    // the objects are created by startup(), in the echo thread
    constexpr embot::instr::ScopeHisto::Config latencyconfig
    {
        0, static_cast<embot::core::relTime>(types::HistogramSnapshot::numberofbins * latencystep), latencystep
    };
    static_assert(types::HistogramSnapshot::numberofbins == latencyconfig.hcfg.nsteps(), "the snapshot holds all the bins");

    embot::instr::ScopeHisto *latency {nullptr};
    embot::os::rtos::mutex_t *latencymutex {nullptr};

    void readlatency(types::HistogramSnapshot &snapshot)
    {
        if((nullptr == latency) || (nullptr == latencymutex))
        {
            snapshot = types::HistogramSnapshot {};     // the echo thread has not started: no latency yet
            return;
        }
        embot::os::rtos::Lock lock {latencymutex};
        embot::instr::snapshot(latency->histogram(), snapshot);
    }

    void alertRX(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtRXframe); }
    void alertTX(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtTXframe); }
    void alertER(void *p) { reinterpret_cast<embot::os::Thread *>(p)->setEvent(evtERframe); }

    void startup(embot::os::Thread *t, void *param)
    {
        pwmmutex = embot::os::rtos::mutex_new();
        latencymutex = embot::os::rtos::mutex_new();
        latency = new embot::instr::ScopeHisto(latencyconfig);
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
        latency->on();

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
            embot::os::rtos::Lock lock {latencymutex};
            latency->off();
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


// ---- theReporter: the reports of the board, and a HardFault on demand

namespace udpdemo::report {

    using embot::svc::theReporter;

    // true: the alive thread causes a HardFault (an undefined instruction) at its 3rd report, to try the record of the
    // fatal and the reset. it does it only if the board has no record of a previous fatal: so once per power-on
    constexpr bool enableFAULT {false};

    // the record of a fatal: in a RAM that the startup does not clear, so it survives the reset. the section .bss.noinit
    // is placed in its own region by the scatter file (amcmj1.2cm4-lwip-0offset.sct: RW_NOINIT, UNINIT) for uVision and
    // AC6, and by the linker script (amcmj1.2cm4-lwip-0offset.ld: .noinit, NOLOAD) for ATfE. it is not in an anonymous
    // namespace so that a debugger finds it: udpdemo::report::fatalmemory
    alignas(8) uint8_t fatalmemory[theReporter::fatalmemorysize] __attribute__((section(".bss.noinit")));

    // the safe state on a fatal, w/out the RTOS: this demo drives no motor, so it switches on all the LEDs
    void safestate()
    {
        embot::hw::led::on(embot::hw::LED::one);
        embot::hw::led::on(embot::hw::LED::two);
        embot::hw::led::on(embot::hw::LED::three);
    }

    constexpr theReporter::Config config
    {
        theReporter::Print::ON,                                         // the print sink: ITM and theLogger ...
        theReporter::Severity::trace,                                   // ... w/ every report
        16,                                                             // the queue
        embot::core::time1second,                                       // the window of the merge of the repeated reports
        8,                                                              // different reports followed by the merge
        theReporter::OnFatal::reset,                                    // on a fatal: reset (theReporter::OnFatal::hang to debug)
        safestate,
        100*embot::core::time1millisec,                                 // time left to theLogger to send the fatal
        embot::core::Data {fatalmemory, sizeof(fatalmemory)},
        embot::os::Thread::Props {embot::os::Priority::low12, 2*1024}   // above theLogger (low10)
    };

    // codes of this demo for the Descriptor (the iCub diagnostics have their own tables)
    constexpr uint32_t codeALIVE {0x00010001};

    // the alive thread: one report w/ severity info every 5 s
    constexpr embot::core::relTime aliveperiod {5*embot::core::time1second};
    constexpr uint32_t faultat {3};

    void onperiod(embot::os::Thread *t, void *param)
    {
        static uint32_t n {0};
        n++;
        const std::string text {"alive #" + std::to_string(n)};
        theReporter::getInstance().report(theReporter::Severity::info, theReporter::Descriptor {codeALIVE, 0, n}, text, "udpdemo");

        theReporter::Report previous {};
        if((true == enableFAULT) && (faultat == n) && (false == theReporter::getInstance().previousfatal(previous)))
        {
            embot::core::print("tALIVE: a HardFault in a moment (udpdemo::report::enableFAULT is true)");
            embot::os::rtos::thread_sleep(nullptr, 100*embot::core::time1millisec);     // theLogger can send the line
            __asm volatile ("udf #0");                                                  // an undefined instruction
        }
    }

    void tALIVE(void *p)
    {
        reinterpret_cast<embot::os::Thread *>(p)->run();
    }

    void start()
    {
        const embot::os::PeriodicThread::Config tc
        {
            2*1024,
            embot::os::Priority::normal24,
            nullptr,
            nullptr,
            aliveperiod,
            onperiod,
            "tALIVE"
        };
        embot::os::PeriodicThread *t {new embot::os::PeriodicThread};
        t->start(tc, tALIVE);
    }

} // namespace udpdemo::report

// the HardFault of the CM4: the registers to theReporter, which writes the record, calls the safe state and resets.
// it replaces the weak HardFault_Handler of the startup (stm32hal.h7.startup.amcmj1.c)
extern "C" void udpdemo_HardFault_C(const uint32_t *frame)          // the stacked r0, r1, r2, r3, r12, lr, pc, xpsr
{
    constexpr uintptr_t CFSR {0xE000ED28}, HFSR {0xE000ED2C}, MMFAR {0xE000ED34}, BFAR {0xE000ED38};
    const uint32_t cfsr {*reinterpret_cast<volatile uint32_t *>(CFSR)};
    const uint32_t hfsr {*reinterpret_cast<volatile uint32_t *>(HFSR)};
    const uint32_t address {(0 != (cfsr & 0x0080)) ? *reinterpret_cast<volatile uint32_t *>(MMFAR) :      // MMARVALID
                            (0 != (cfsr & 0x8000)) ? *reinterpret_cast<volatile uint32_t *>(BFAR) : 0};   // BFARVALID
    using embot::svc::theReporter;
    const theReporter::Fault fault {theReporter::CPU::cortexm, frame[6], frame[5], frame[7], cfsr, hfsr, address};
    theReporter::fatal(fault, udpdemo::report::config.fatalmemory, udpdemo::report::config.onfatal, udpdemo::report::config.safestate);
}

extern "C" __attribute__((naked)) void HardFault_Handler()
{   // the frame is on the stack in use when the fault happened: MSP or PSP, as bit 2 of LR (EXC_RETURN) tells
    __asm volatile
    (
        "tst lr, #4                 \n"
        "ite eq                     \n"
        "mrseq r0, msp              \n"
        "mrsne r0, psp              \n"
        "b udpdemo_HardFault_C      \n"
    );
}


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

    // the reports of the board: first, it does not need the network. its thread runs after this init thread and, if the
    // previous run ended w/ a fatal, its first report is that fatal
    embot::svc::theReporter::getInstance().initialise(udpdemo::report::config);

    static const std::initializer_list<embot::hw::LED> allleds =
    {
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three
    };

    embot::svc::theLEDmanager &theleds = embot::svc::theLEDmanager::getInstance();

    theleds.init(allleds);
    for(const auto &l : allleds)
    {
        theleds.get(l).on();
        theleds.get(l).off();
    }

    // the lines of embot::core::print() also to host:9999. it is started first so that it keeps all the lines printed from
    // now on: its socket is opened by its thread, which runs after this init thread (so after theIPservice::initialise())
    embot::svc::theLogger::getInstance().initialise(udpdemo::net::logger);

    // lwIP, the ETH and its switch, the ping: before every service that uses sockets
    embot::net::eth::theIPservice::getInstance().initialise(udpdemo::net::ipservice);

    // TETHER: its sockets 6666 and 7777, its threads, the SharedVariables of the Dictionary
    if(false == theTETHERservice::getInstance().initialise(tetherconfig))
    {
        embot::core::print("theTETHERservice::initialise() failed");
    }

    // the echo on port 666: after TETHER, because it uses its SharedVariables
    udpdemo::echo::start();

    // a report every 5 s (and the HardFault on demand)
    udpdemo::report::start();

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

