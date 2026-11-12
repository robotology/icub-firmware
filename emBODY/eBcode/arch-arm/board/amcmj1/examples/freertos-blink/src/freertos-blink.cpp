
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// freertos-blink: a small embot::os application on the CM4 of the amcmj1 (boot master), to compare FreeRTOS and RTX.
//
// the code is the same for the two RTOSes: the project has two targets that differ only in the RTOS
// - amcmj1.2cm4-freertos: -DEMBOT_USE_rtos_freertos and libs/highlevel/abslayer/freertos/lib/freertos.debug.cm4.lib
// - amcmj1.2cm4-cmsisrtx: -DEMBOT_USE_rtos_cmsisrtx and libs/highlevel/abslayer/cmsisrtx/lib/cmsisrtx.debug.cm4.lib
// both libs are the "debug" flavour: stack check, asserts / checks on, and the bridge to the Event Viewer of uVision.
//
// what it does:
// - LED one pulses at 1 Hz, driven by embot::app::theLEDmanager (an embot::os::Timer inside theTimerManager)
// - tBLINK, an embot::os::PeriodicThread at 500 ms, toggles LED two and every 4 periods sends evtFROMBLINK to tEVENT
// - tEVENT, an embot::os::EventThread, receives evtTICK from an embot::os::Timer every second and evtFROMBLINK from tBLINK:
//   it prints the time and the counters with embot::core::print() (ITM: Debug (printf) Viewer of uVision), and at every
//   evtTICK it starts a burst of the benchmark
// - the benchmark: tPING (high41) and tPONG (high42), two embot::os::EventThread, exchange events for benchroundtrips round
//   trips. The DWT cycle counter measures each direction and the round trip; tPING prints min / avg / max at the end of the
//   burst. In the Event Viewer the burst is the dense alternation of tPING and tPONG once per second.
//   - ping -> pong: tPONG has the higher priority, so setEvent() preempts tPING at once: event + context switch
//   - pong -> ping: tPING has the lower priority, so it runs when tPONG waits again: event + end of onevent() + wait + switch
//   both include what embot::os::EventThread adds (the return from the wait and the call of onevent()).
// --------------------------------------------------------------------------------------------------------------------

#if !defined(STM32HAL_dualcore_BOOT_cm4master)
    #error this application must run as cm4 master
#endif

#if defined(EMBOT_USE_rtos_freertos)
    constexpr const char *rtosname {"FreeRTOS"};
#elif defined(EMBOT_USE_rtos_cmsisrtx)
    constexpr const char *rtosname {"RTX"};
#else
    #error define EMBOT_USE_rtos_freertos or EMBOT_USE_rtos_cmsisrtx
#endif

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_sys.h"
#include "embot_hw_dualcore.h"

#include "embot_os.h"
#include "embot_os_Thread.h"
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"

#include "embot_app_theLEDmanager.h"

#include <string>


namespace blink {

    constexpr embot::os::Event evtTICK {embot::core::binary::mask::pos2mask<embot::os::Event>(0)};
    constexpr embot::os::Event evtFROMBLINK {embot::core::binary::mask::pos2mask<embot::os::Event>(1)};

    constexpr embot::core::relTime tickperiod {embot::core::time1second};
    constexpr embot::core::relTime blinkperiod {500*embot::core::time1millisec};
    constexpr uint32_t blinksperevent {4};

    // the event thread: it is started before tBLINK, which needs it
    embot::os::EventThread *tevent {nullptr};

    // written only by their own thread, read by tEVENT for the print: a torn read is not possible on a 32-bit core
    volatile uint32_t numofblinks {0};
    volatile uint32_t numofticks {0};

} // namespace blink


// ---- the benchmark: tPING and tPONG

namespace blink::bench {

    constexpr embot::os::Event evtGO {embot::core::binary::mask::pos2mask<embot::os::Event>(0)};
    constexpr embot::os::Event evtPING {embot::core::binary::mask::pos2mask<embot::os::Event>(1)};
    constexpr embot::os::Event evtPONG {embot::core::binary::mask::pos2mask<embot::os::Event>(2)};

    constexpr uint32_t benchroundtrips {1000};

    // the DWT cycle counter of the Cortex-M4 (registers of the ARMv7-M architecture)
    namespace dwt {
        constexpr uintptr_t DEMCR {0xE000EDFC};         // bit 24: TRCENA
        constexpr uintptr_t CTRL {0xE0001000};          // bit 0: CYCCNTENA
        constexpr uintptr_t CYCCNT {0xE0001004};
        constexpr uint32_t TRCENA {1U << 24};
        constexpr uint32_t CYCCNTENA {1U << 0};

        inline volatile uint32_t & reg(uintptr_t a) { return *reinterpret_cast<volatile uint32_t*>(a); }

        void enable()
        {
            reg(DEMCR) = reg(DEMCR) | TRCENA;
            reg(CTRL) = reg(CTRL) | CYCCNTENA;
        }

        inline uint32_t cycles() { return reg(CYCCNT); }
    }

    // min / avg / max of a number of cycles. constexpr constructor: the static objects have constant initialisation
    class Stats
    {
    public:
        constexpr Stats() : min(0xffffffff), max(0), sum(0), num(0) {}

        void reset() { min = 0xffffffff; max = 0; sum = 0; num = 0; }

        void add(uint32_t v)
        {
            if(v < min) { min = v; }
            if(v > max) { max = v; }
            sum += v;
            num++;
        }

        uint32_t average() const { return (0 == num) ? 0 : static_cast<uint32_t>(sum / num); }

        // "min / avg / max cycles = min / avg / max ns" with cpu at mhz MHz
        std::string to_string(uint32_t mhz) const
        {
            const auto ns = [mhz](uint32_t c) { return std::to_string((c * 1000ULL) / mhz); };
            return std::to_string(min) + " / " + std::to_string(average()) + " / " + std::to_string(max) + " cycles = " +
                   ns(min) + " / " + ns(average()) + " / " + ns(max) + " ns";
        }

    private:
        uint32_t min;
        uint32_t max;
        uint64_t sum;
        uint32_t num;
    };

    embot::os::EventThread *tping {nullptr};
    embot::os::EventThread *tpong {nullptr};

    uint32_t cpumhz {1};

    // written by one thread and read by the other one after the setEvent(): same core, the kernel call orders them
    volatile uint32_t sentping {0};
    volatile uint32_t sentpong {0};

    // used only by tPING (pingtopong also by tPONG, but never at the same time: tPING is preempted while tPONG adds)
    Stats pingtopong {};
    Stats pongtoping {};
    Stats roundtrip {};
    uint32_t count {0};
    uint32_t bursts {0};
    uint32_t burststart {0};
    uint32_t roundstart {0};

    void ping()
    {
        roundstart = dwt::cycles();
        sentping = dwt::cycles();
        tpong->setEvent(evtPING);
    }

    void onping(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {
        if(true == embot::core::binary::mask::check(eventmask, evtGO))
        {
            pingtopong.reset();
            pongtoping.reset();
            roundtrip.reset();
            count = 0;
            burststart = dwt::cycles();
            ping();
        }

        if(true == embot::core::binary::mask::check(eventmask, evtPONG))
        {
            const uint32_t now {dwt::cycles()};
            pongtoping.add(now - sentpong);
            roundtrip.add(now - roundstart);
            count++;
            if(count < benchroundtrips)
            {
                ping();
            }
            else
            {
                const uint32_t total {now - burststart};
                bursts++;
                embot::core::print(std::string("bench[") + rtosname + "] #" + std::to_string(bursts) + ": " +
                                   std::to_string(benchroundtrips) + " round trips in " + std::to_string(total / cpumhz) + " us");
                embot::core::print(std::string("bench[") + rtosname + "]   ping -> pong (preemption): " + pingtopong.to_string(cpumhz));
                embot::core::print(std::string("bench[") + rtosname + "]   pong -> ping (on wait):    " + pongtoping.to_string(cpumhz));
                embot::core::print(std::string("bench[") + rtosname + "]   round trip:                " + roundtrip.to_string(cpumhz));
            }
        }
    }

    void onpong(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {
        if(true == embot::core::binary::mask::check(eventmask, evtPING))
        {
            pingtopong.add(dwt::cycles() - sentping);
            sentpong = dwt::cycles();
            tping->setEvent(evtPONG);
        }
    }

    void tPING(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
    void tPONG(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

    void start()
    {
        dwt::enable();
        cpumhz = embot::hw::sys::clock(embot::hw::CLOCK::syscore) / 1000000;
        if(0 == cpumhz)
        {
            cpumhz = 1;
        }

        const embot::os::EventThread::Config pongconfig
        {
            1024,
            embot::os::Priority::high42,
            nullptr,
            nullptr,
            embot::core::reltimeWaitForever,
            onpong,
            "tPONG"
        };
        tpong = new embot::os::EventThread;
        tpong->start(pongconfig, tPONG);

        const embot::os::EventThread::Config pingconfig
        {
            2*1024,
            embot::os::Priority::high41,
            nullptr,
            nullptr,
            embot::core::reltimeWaitForever,
            onping,
            "tPING"
        };
        tping = new embot::os::EventThread;
        tping->start(pingconfig, tPING);

        embot::core::print(std::string("bench: tPING and tPONG exchange ") + std::to_string(benchroundtrips) + " events at every evtTICK");
    }

} // namespace blink::bench


// ---- tEVENT

namespace blink::event {

    void startup(embot::os::Thread *t, void *param)
    {
        // a timer of embot::os which sends evtTICK to this thread every tickperiod
        embot::os::Timer *tmr {new embot::os::Timer};
        const embot::os::Action act {embot::os::EventToThread(evtTICK, t)};
        const embot::os::Timer::Config cfg {tickperiod, act, embot::os::Timer::Mode::forever, 0};
        tmr->start(cfg);

        embot::core::print("tEVENT: started a timer which sends evtTICK every " + embot::core::TimeFormatter(tickperiod).to_string());
    }

    void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {
        if(0 == eventmask)
        {   // timeout: never, because the timeout is reltimeWaitForever
            return;
        }

        const std::string now {embot::core::TimeFormatter(embot::core::now()).to_string(embot::core::TimeFormatter::Mode::full)};

        if(true == embot::core::binary::mask::check(eventmask, evtTICK))
        {
            numofticks = numofticks + 1;
            embot::core::print("tEVENT: evtTICK #" + std::to_string(numofticks) + " @ " + now);
            // a burst of the benchmark: tPING and tPONG have higher priority, so it runs (and prints) before we go on
            bench::tping->setEvent(bench::evtGO);
        }

        if(true == embot::core::binary::mask::check(eventmask, evtFROMBLINK))
        {
            embot::core::print("tEVENT: evtFROMBLINK after " + std::to_string(numofblinks) + " blinks @ " + now);
        }
    }

    void tEVENT(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

    void start()
    {
        const embot::os::EventThread::Config config
        {
            2*1024,
            embot::os::Priority::high40,
            startup,
            nullptr,
            embot::core::reltimeWaitForever,
            onevent,
            "tEVENT"
        };

        tevent = new embot::os::EventThread;
        tevent->start(config, tEVENT);
    }

} // namespace blink::event


// ---- tBLINK

namespace blink::periodic {

    void startup(embot::os::Thread *t, void *param)
    {
        embot::core::print("tBLINK: it toggles LED two every " + embot::core::TimeFormatter(blinkperiod).to_string());
    }

    void onperiod(embot::os::Thread *t, void *param)
    {
        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).toggle();

        numofblinks = numofblinks + 1;
        if(0 == (numofblinks % blinksperevent))
        {
            tevent->setEvent(evtFROMBLINK);
        }
    }

    void tBLINK(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

    void start()
    {
        const embot::os::PeriodicThread::Config config
        {
            1024,
            embot::os::Priority::normal24,
            startup,
            nullptr,
            blinkperiod,
            onperiod,
            "tBLINK"
        };

        embot::os::PeriodicThread *t {new embot::os::PeriodicThread};
        t->start(config, tBLINK);
    }

} // namespace blink::periodic


// ---- the init thread, the idle thread, main()

void onIdle(embot::os::Thread *t, void *idleparam)
{
    static uint32_t i {0};
    i++;
}

void initSystem(embot::os::Thread *t, void *initparam)
{
    const uint32_t cpufreq {embot::hw::sys::clock(embot::hw::CLOCK::syscore)};
    embot::core::print(std::string("freertos-blink: embot::os on ") + rtosname + " on the CM4 of the amcmj1 @ " + std::to_string(cpufreq/1000000) + " MHz");

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
        theleds.get(l).off();
    }
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);

    blink::bench::start();
    blink::event::start();
    blink::periodic::start();

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

    if(true == embot::hw::dualcore::ismaster())
    {
        constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};
        embot::hw::dualcore::config(dualcoreconfig);
    }

    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);

    // it starts the RTOS: it does not return
    embot::os::start();
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

