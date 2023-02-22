
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

//#define TEST_EMBOT_HW
#define TEST_EMBOT_OS


#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"
#include "embot_hw_can.h"
#include "embot_hw_timer.h"


#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_scope.h"

#include <vector>

constexpr embot::core::Time blinktime {1000*embot::core::time1millisec};

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

constexpr embot::core::relTime tickperiod = 5*1000*embot::core::time1millisec;


#if defined(TEST_EMBOT_OS)

void test_embot_os_init();
void test_embot_os_tick();

#endif


embot::os::Timer *tmrsometimeafter {nullptr};

constexpr embot::core::Time sometimeafter {3000*embot::core::time1millisec};

void onSomeTimeAfter(void *p)
{
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();       
    theleds.get(embot::hw::LED::two).pulse(blinktime); 
}


void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
   
    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->name("TickTmr");
    tmr->start(cfg); 
    
//    tmrsometimeafter = new embot::os::Timer;
//    tmrsometimeafter->name("sometimeafter");
//    
//    embot::os::Action a1 = embot::os::CallbackToThread({onSomeTimeAfter, tmrsometimeafter}, nullptr);
//    tmrsometimeafter->load({sometimeafter, a1, embot::os::Timer::Mode::oneshot}); 
//    tmrsometimeafter->start();    
    
#if defined(TEST_EMBOT_OS)
    test_embot_os_init();
#endif    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {      
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   

        
#if defined(TEST_EMBOT_OS)
        test_embot_os_tick();
#endif        
    }
    

}


#if defined(TEST_EMBOT_OS)

embot::os::Timer *tmrperiodic {nullptr};
embot::os::Timer *tmroneshot {nullptr};
embot::os::Timer *tmrsomeshots {nullptr};

void onForever(void *p)
{
    embot::os::Timer *t = reinterpret_cast<embot::os::Timer*>(p);

    embot::core::TimeFormatter tf(embot::core::now());        
    embot::core::print("@ " + tf.to_string(embot::core::TimeFormatter::Mode::full) + ": "+ t->name() + " w/ shots: " + std::to_string(t->shots()));    
    constexpr size_t max {5};
    if(t->shots() > max)
    {
        embot::core::print("stop it");    
        t->stop();     

        embot::core::print("and restart it");    
        t->start();                
    }
}

void onOneshot(void *p)
{
    embot::os::Timer *t = reinterpret_cast<embot::os::Timer*>(p);

    embot::core::TimeFormatter tf(embot::core::now());        
    embot::core::print("@ " + tf.to_string(embot::core::TimeFormatter::Mode::full) + ": "+ t->name() + " w/ shots: " + std::to_string(t->shots())); 

//    embot::core::print("restarting it");    
//    t->start();
    
}

void onSomeshots(void *p)
{
    embot::os::Timer *t = reinterpret_cast<embot::os::Timer*>(p);

    embot::core::TimeFormatter tf(embot::core::now());        
    embot::core::print("@ " + tf.to_string(embot::core::TimeFormatter::Mode::full) + ": "+ t->name() + " w/ shots: " + std::to_string(t->shots()));    
}

void test_embot_os_init() 
{
    
    tmrperiodic = new embot::os::Timer;
    tmrperiodic->name("PerTmr");
    
    tmroneshot = new embot::os::Timer;
    tmroneshot->name("OneTmr");
    
    tmrsomeshots = new embot::os::Timer;
    tmrsomeshots->name("SomTmr");
    
    embot::os::Action ap = embot::os::CallbackToThread({onForever, tmrperiodic}, nullptr);
    tmrperiodic->load({embot::core::time1second, ap, embot::os::Timer::Mode::forever});
 
    embot::os::Action a1 = embot::os::CallbackToThread({onOneshot, tmroneshot}, nullptr);
    tmroneshot->load({2*embot::core::time1second, a1, embot::os::Timer::Mode::oneshot});
    
    embot::os::Action as = embot::os::CallbackToThread({onSomeshots, tmrsomeshots}, nullptr);
    tmrsomeshots->load({3*embot::core::time1second, as, embot::os::Timer::Mode::someshots, 3});    
    
    tmrperiodic->start();
    tmroneshot->start();
    tmrsomeshots->start();
    
}

void test_embot_os_tick()
{
    
}

#endif



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

// entry point (first running thread)
void initSystem(embot::os::Thread *t, void* initparam)
{
    volatile uint32_t cpufreq = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    cpufreq = cpufreq;
    embot::core::print("this is a demo which shows that this code can run on a dev board. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    // start the services with default params
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
//    constexpr embot::core::Time blinktime {200*embot::core::time1millisec};
                    
    embot::core::print("INIT: creating the LED pulser: it will blink a LED @" + embot::core::TimeFormatter(blinktime).to_string());
        
    static const std::initializer_list<embot::hw::LED> allleds = 
    {   
        embot::hw::LED::one, embot::hw::LED::two 
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);     
    
    theleds.get(embot::hw::LED::one).pulse(blinktime); 

    
    
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

    tmrsometimeafter = new embot::os::Timer;
    tmrsometimeafter->name("sometimeafter");
    
//    embot::os::Action a1 = embot::os::CallbackToThread({onSomeTimeAfter, tmrsometimeafter}, nullptr);
//    tmrsometimeafter->load({sometimeafter, a1, embot::os::Timer::Mode::oneshot}); 
//    tmrsometimeafter->start();      
    
    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2. i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);
}



// - other code


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
