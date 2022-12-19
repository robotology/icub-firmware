
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
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

#include "embot_hw_can.h"
#include "embot_prot_can.h"

#include <vector>


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtCANrx = embot::core::binary::mask::pos2mask<embot::os::Event>(1);



constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

    void alerteventbasedthread(void *arg)
    {
        embot::os::EventThread* evthr = reinterpret_cast<embot::os::EventThread*>(arg);
        if(nullptr != evthr)
        {
            evthr->setEvent(evtCANrx);
        }
    }
    
    std::vector<embot::prot::can::Frame> outframes;
    
    embot::hw::CAN bus = embot::hw::CAN::two;

    void can_init(embot::os::Thread *t)
    {
        static bool initted = false;
        if(true == initted)
        {
            return;
        }
        initted = true;
        // init can but do not enabled yet
        embot::hw::can::Config canconfig;   // default is tx/rxcapacity=8
        canconfig.rxcapacity = 8;
        canconfig.txcapacity = 8;
        canconfig.onrxframe = embot::core::Callback(alerteventbasedthread, t); 
        embot::hw::can::init(bus, canconfig);
        embot::hw::can::setfilters(bus, 7); 
        // pre-allocate output vector of frames
        outframes.reserve(8);
        // make sure that the application and can protocol versions are synched
//        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
//        canbrdinfo.synch(extCFG->evt->applinfo);
    }

    void can_enable(embot::os::Thread *t)
    {
        embot::hw::can::enable(bus);
    }
    
void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
   
    can_init(t);
    can_enable(t);
    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());
              
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);

}

void userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes)
{
    
    outframes.push_back(frame);
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    
    // we clear the frames to be trasmitted
    outframes.clear();

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {      
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
        uint8_t da[8] = {7, 8, 9, 10, 11, 12, 13, 14}; 
        outframes.push_back({0x123, 2, da});
    }
    
    
    if(true == embot::core::binary::mask::check(eventmask, evtCANrx)) 
    {    
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtCANrx received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   

        embot::hw::can::Frame hwframe;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::can::get(bus, hwframe, remainingINrx))
        {            
            userdefOnEventRXcanframe(t, eventmask, param, {hwframe.id, hwframe.size, hwframe.data}, outframes);               
            
            if(remainingINrx > 0)
            {
                t->setEvent(evtCANrx);                 
            }
        }        
            
    }
    
    
    // if we have any packet we transmit them
    std::uint8_t num = outframes.size();
    if(num > 0)
    {
        for(std::uint8_t i=0; i<num; i++)
        {
            embot::hw::can::put(bus, {outframes[i].id, outframes[i].size, outframes[i].data});                                       
        }

        embot::hw::can::transmit(bus);  
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
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 
    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b010101));
    theleds.get(embot::hw::LED::two).wave(&ledwave); 
    
    
    embot::core::print("INIT: creating the main thread. it will receives one periodic tick event and one upon pressure of the blue button");  
    
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
