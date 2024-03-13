
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


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

#include "embot_hw_bsp_amcfocm7.h"

#include "embot_app_scope.h"

#include <vector>


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

embot::app::scope::Signal *signal {nullptr};


constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

void ON(){};

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
   
    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());
    
    //embot::core::TimeFormatter tf(embot::core::now());        
    
    constexpr embot::app::scope::SignalEViewer::Config cc{ON, embot::app::scope::SignalEViewer::Config::LABEL::one};
    signal = new embot::app::scope::SignalEViewer(cc);
    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);

}

void compute()
{
    static constexpr size_t N {10}; 
    static constexpr size_t M {10};
    static float data[N][M] = {0};
    
    std::memset(data, 0, sizeof(data));
    
    for(size_t n=0; n<N; n++)
    {
        for(size_t m=0; m<M; m++)
        {
            data[n][m] = 0.1*static_cast<float>(n+m)*(data[n][m] + 1.0f);
            data[std::max(N, m)][std::max(M, n)] = data[n][m] * data[n][m];
        }        
    }
    
}

void compute2()
{
    static constexpr size_t N {10}; 
    static constexpr size_t M {10};
    static float data[N][M] = {0};
    
    std::memset(data, 0, sizeof(data));
    
    for(size_t n=0; n<N; n++)
    for(size_t m=0; m<M; m++)
        data[n][m] = 0.1 * static_cast<float>(n) * static_cast<float>(m);
    
    for(size_t n=0; n<N; n++)
    {
        for(size_t m=0; m<M; m++)
        {
            data[n][m] = std::sin(0.1*static_cast<float>(n+m)*(data[n][m] + 1.0f));
            data[std::max(N, m)][std::max(M, n)] = data[n][m] * data[n][m];
        }        
    }
    
}


void testduration()
{
//    embot::hw::sys::delay(50);
    compute2();
}

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    { 
        signal->on();   
        testduration();   
        signal->off();    
        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));  
                  
    }
    

}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void tMAIN(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

void initSystem(embot::os::Thread *t, void* initparam)
{
    volatile uint32_t cpufreq = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    cpufreq = cpufreq;
    embot::core::print("this is a demo which shows that this code can run on a dev board. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
                    
    embot::core::print("INIT: creating the LED pulser: it will blink a LED at 1 Hz and run a 0.2 Hz waveform on another");
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);   
    
    theleds.get(embot::hw::LED::one).on(); 
    theleds.get(embot::hw::LED::two).on(); 
    theleds.get(embot::hw::LED::three).on(); 
    theleds.get(embot::hw::LED::one).off(); 
    theleds.get(embot::hw::LED::two).off(); 
    theleds.get(embot::hw::LED::three).off(); 
    
#if defined(dualcore_BOOT_cm7master)
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
#else
//    embot::app::LEDwaveT<64> ledwave(150*embot::core::time1millisec, 30, std::bitset<64>(0b01010101010101));
//    theleds.get(embot::hw::LED::one).wave(&ledwave); 
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
#endif    
    
    
    embot::core::print("INIT: creating the main thread. it will reveives one periodic tick event and one upon pressure of the blue button");  
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent,
        "mainThreadEvt"
    };
       
        
    // create the main thread 
    embot::os::EventThread *thr {nullptr};
    thr = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tMAIN
    thr->start(configEV, tMAIN); 
    
    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}


#include "embot_hw_dualcore.h"
#include "embot_core_binary.h"

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 

    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};

#if defined(dualcore_BOOT_cm7master)     
    embot::hw::dualcore::config({embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate});
#endif
    
    

    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()

    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



// - other code



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
