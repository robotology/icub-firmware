
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

    in here i will test the external fault mechanism.
    there is a thread running @ 1 ms period, one volatile variable called EXTFAULTispressed
    then there is a push button attached to PB8 which is pressed when it is conencted to 3V3.
    this is a case similar to the amcbldc board
#endif


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

#include <vector>

volatile bool EXTFAULTisPRESSED {false};
volatile bool prevEXTFAULTisPRESSED {false};

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

constexpr embot::core::relTime ostick = 500*embot::core::time1microsec;
constexpr embot::core::relTime tickperiod = 1*embot::core::time1millisec;

constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;
constexpr embot::hw::BTN buttonPB9 = embot::hw::BTN::two;

constexpr embot::hw::BTN buttonOntest = buttonPB9; // buttonPB9 or buttonBLUE


void btncallback_pressed_released(void *p)
{
    EXTFAULTisPRESSED = embot::hw::button::pressed(buttonOntest);
}


void printInfoOnEXTFAULT()
{
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();        
    embot::core::TimeFormatter tf(embot::core::now()); 
    if(true == EXTFAULTisPRESSED)
    {
        theleds.get(embot::hw::LED::one).pulse(100*embot::core::time1millisec);
        embot::core::print("PRESSED @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));
    }
    else
    {

        theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 
        embot::core::print("RELEASES @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));
    }
}


void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
      

    embot::core::print("mainthread-startup: activated button which sets EXTFAULTisPRESSED = 1 / 0 each time is pressed or released");
    // init the ext interrupt button
    embot::hw::button::init(buttonOntest, {embot::hw::button::Mode::TriggeredOnPressAndRelease, {btncallback_pressed_released, t}, 0});             
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = embot::hw::button::pressed(buttonOntest); 

    printInfoOnEXTFAULT();    

    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());   
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);    
    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{  
    static uint32_t count {0};  
    static embot::core::Time tprev {0};
    
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }   
    
    
    if(prevEXTFAULTisPRESSED != EXTFAULTisPRESSED)
    {
        prevEXTFAULTisPRESSED = EXTFAULTisPRESSED;       
        printInfoOnEXTFAULT();
    }

    // everything else the tick must do
    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {  
        constexpr embot::core::Time delta {5*embot::core::time1second};
        embot::core::Time tnow = embot::core::now();
        count++;
        
        if((tnow-tprev) > delta)
        {
            tprev = tnow;
                        
            embot::core::TimeFormatter tf(tnow); 
            embot::core::print("CTRL thread @ time = " + tf.to_string() + " sees EXTFAULT = " + std::to_string(EXTFAULTisPRESSED));
        }
        
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
    embot::core::print("this is a demo which shows that this code can run on a nucleo64 board. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
                    
    embot::core::print("INIT: creating the LED pulser: it will blink a LED at 1 Hz and run a 0.2 Hz waveform on another");
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 
    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b010101));
    theleds.get(embot::hw::LED::two).wave(&ledwave); 
    
    
    embot::core::print("INIT: creating the main thread. it will reveives one periodic tick event");  
    
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


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {ostick, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



// - other code



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------