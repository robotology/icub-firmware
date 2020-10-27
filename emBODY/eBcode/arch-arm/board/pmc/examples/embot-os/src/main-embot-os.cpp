
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_button.h"
#include "embot_hw_sys.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
#include "embot_tools.h"


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtBTNreleased = embot::core::binary::mask::pos2mask<embot::os::Event>(1);

constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;

void btncallback(void *p)
{
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtBTNreleased);
}

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{       
    embot::core::print("evthread-startup: ..." ); 
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    // init the ext interrupt button
    embot::hw::button::init(buttonBLUE, {embot::hw::button::Mode::TriggeredOnRelease, {btncallback, t}, 0});
    
    embot::core::print("evthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());

    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        

    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtBTNreleased)) 
    {    
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtBTNreleased received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));        
    }

}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

embot::os::EventThread* thr {nullptr};

void initSystem(embot::os::Thread *t, void* initparam)
{
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(2*embot::core::time1second); 
       
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent
    };
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV);   
}

// usart 921600

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 512, nullptr, nullptr, onIdle };
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



