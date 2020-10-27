
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


#include "embot_os_rtos.h"

embot::os::rtos::mutex_t *mutex {nullptr};

embot::os::rtos::semaphore_t *sem {nullptr};

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtBTNreleased = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
//constexpr embot::os::Event evtRead = embot::core::binary::mask::pos2mask<embot::os::Event>(2);

constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;

void btncallback(void *p)
{
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtBTNreleased);
}

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{          
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
    

     embot::core::print("mainthread-startup: initted driver for tlv493d");  
         
    
     embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());
    
    embot::core::TimeFormatter tf(embot::core::now());        
     embot::core::print("taking mutex @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));  
    bool resu = true;
//    resu = embot::os::rtos::mutex_take(mutex, 10*1000*1000);
    
    embot::core::TimeFormatter tf1(embot::core::now());
     embot::core::print("mutex result is " + std::to_string(resu) + "@ time = " + tf1.to_string(embot::core::TimeFormatter::Mode::full)); 
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    // init the ext interrupt button
    embot::hw::button::init(buttonBLUE, {embot::hw::button::Mode::TriggeredOnRelease, {btncallback, t}, 0});
       
}

embot::os::PeriodicThread *pth {nullptr};


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {      
        embot::core::TimeFormatter tf(embot::core::now());        
//         embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
        if(embot::core::now() > 5*1000000)   
        {
            if(nullptr != pth)
            {
                
                 embot::core::print(std::string("the thread ") + t->getName() + " is about to delete thread " + pth->getName());
                delete pth;
                pth = nullptr;
            }
        }

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
     embot::core::print("this is a demo which shows that this code can run on a dev board and later on the real pmc board");    
    
     embot::core::print("starting the INIT thread");
    
     embot::core::print("creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    embot::hw::led::init(embot::hw::LED::one);
    embot::hw::led::on(embot::hw::LED::one);
    embot::hw::led::off(embot::hw::LED::one);
    
     embot::core::print("creating the LED pulser: it will blink a LED at 1 Hz.");
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 
    
#if 0    
//    for(;;)
//    {
//        constexpr embot::core::relTime tick = 10000;
//        constexpr uint32_t n = 1000;
//        
//        volatile uint32_t ciao = 0;
//        theleds.get(embot::hw::LED::one).toggle();
//        
//        for(int i=0; i<n; i++)
//        {
//            wait(tick);
//            ciao = osRtxInfo.kernel.tick;
//            ciao = ciao;
//        }
//        
//    }

    embot::os::EventThread::Config cc { 
        6*1024, 
        embot::os::Priority::medium100, 
        [](embot::os::Thread *t, void *param) { 
            static uint32_t zz{0};
            zz++;
        },
        nullptr,
        50*embot::core::time1millisec,
        [](embot::os::Thread *t, embot::os::EventMask eventmask, void *param) { 
            static uint32_t xx{0};
            xx++;
        }
    };

embot::os::Thread *th = nullptr;
embot::os::EventThread *eth {nullptr};


th = new embot::os::EventThread;
delete th;
th = nullptr;


eth = new embot::os::EventThread;
eth->start(cc);
delete eth;
eth = nullptr;


eth = new embot::os::EventThread;
delete eth;
eth = nullptr;

embot::os::rtos::timer_t *tt {nullptr};
tt = embot::os::rtos::timer_new();
embot::os::rtos::timer_delete(tt);

tt = embot::os::rtos::timer_new();
embot::os::rtos::timer_delete(tt);

tt = embot::os::rtos::timer_new();
embot::os::rtos::timer_delete(tt);
       
embot::os::rtos::messagequeue_t *mq {nullptr};
mq = embot::os::rtos::messagequeue_new(16);
embot::os::rtos::messagequeue_delete(mq);

mq = embot::os::rtos::messagequeue_new(16);
embot::os::rtos::messagequeue_delete(mq);

    mutex = embot::os::rtos::mutex_new();
    sem = embot::os::rtos::semaphore_new(1, 1);

volatile bool resu {true};
    resu = embot::os::rtos::semaphore_acquire(sem, 10*1000*1000);
    resu = embot::os::rtos::semaphore_acquire(sem, 5*1000*1000);

    resu = embot::os::rtos::mutex_take(mutex, 10000);
    resu = resu;
    resu = embot::os::rtos::mutex_take(mutex, 10*1000*1000);
    resu = resu;

//std::uint16_t st, Priority pr, Thread::fpStartup fpst, void* pa, core::relTime ti, Thread::fpOnPeriod fpon


#endif


    embot::os::PeriodicThread::Config pc { 
        1024, 
        embot::os::Priority::normal24, 
        nullptr, nullptr,
        100*embot::core::time1millisec,
        [](embot::os::Thread *t, void *param) { 
            static uint64_t xx{0};
            
            xx = embot::core::now();
            
            embot::core::TimeFormatter tf(embot::core::now());        
             embot::core::print("periodic thread: exec @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full)); 
            
        },
        "testPeriodic00"
    };

    pth = new embot::os::PeriodicThread;
    pth->start(pc);

    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent,
        "mainThreadEvt"
    };
    
     embot::core::print("creating the main thread");
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV); 

     embot::core::print("quitting the INIT thread. Normal scheduling starts");    
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