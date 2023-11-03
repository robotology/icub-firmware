
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
//#include "embot_app_scope.h"



void initSystem(embot::os::Thread *t, void* initparam);
void onIdle(embot::os::Thread *t, void* idleparam);

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2. i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig 
    {
        embot::core::time1millisec, 
        initcfg, 
        idlecfg, 
        onOSerror,
        embot::hw::FLASHpartitionID::eapplication00
    };
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os
    embot::os::start();
}



void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void tTEST(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

// entry point (first running thread)

#include "embot_hw_sys.h"

void eventbasedthread_startup(embot::os::Thread *t, void *param);
void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

void initSystem(embot::os::Thread *t, void* initparam)
{
    volatile uint32_t cpufreq {0};
    cpufreq = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    embot::core::print("this is a demo which shows that this code can run on a dev board. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    // start the services with default params
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    constexpr embot::core::Time blinktime {2000*embot::core::time1millisec};
                    
    embot::core::print("INIT: creating the LED pulser: it will blink a LED @" + embot::core::TimeFormatter(blinktime).to_string());
 
    static const std::initializer_list<embot::hw::LED> allleds = 
    {   
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three  
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(blinktime); 
    
    
//    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b010101));
//    theleds.get(embot::hw::LED::two).wave(&ledwave); 
    
    
    embot::core::print("INIT: creating the main thread. it will receives one periodic tick event");  
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        5000*embot::core::time1millisec,
        eventbasedthread_onevent,
        "threadTEST"
    };
       
        
    // create the main thread 
    embot::os::EventThread *thr {nullptr};
    thr = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tMAIN
    thr->start(configEV, tTEST); 

    
    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 2*1000*embot::core::time1millisec;

#include "test.h"

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
   
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->name("TickTmr");
    tmr->start(cfg);
    
  
    test_embot_hw_init();    
    
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

        test_embot_hw_tick();
               
    }

}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



