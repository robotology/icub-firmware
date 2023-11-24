/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_hw_bsp_amc.h"

[[noreturn]] void run();  

int main(void)
{
    // it instructs embot::hw::bsp::specialize() to unlock the cm4 core
//    embot::hw::bsp::amc::set(embot::hw::bsp::amc::OnSpecApplication);
    embot::hw::bsp::amc::set({embot::hw::bsp::amc::OnSpecialize::CM4MODE::release, false, false});
    // or it just do nothing
    
    
    // run the system
    run();   
}

// embot::os basic run()

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

void onIdle(embot::os::Thread *t, void* idleparam);
void onError(void *p);
void initSystem(embot::os::Thread *t, void* initparam);

[[noreturn]] void run()
{

    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = {onError, nullptr};
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
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

void onError(void* param)
{
    static uint32_t i = 0;
    i++;
}

void tTEST(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}


#include "embot_hw_sys.h"
#include "embot_app_eth_theErrorManager.h"

void evTHR_startup(embot::os::Thread *t, void *param);
void evTHR_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);
void onERRMANemit(embot::app::eth::theErrorManager::Severity sev, const embot::app::eth::theErrorManager::Caller &caller, 
                  const embot::app::eth::theErrorManager::Descriptor &des, const std::string &str);

void initSystem(embot::os::Thread *t, void* initparam)
{    
    uint32_t cpufreq {0};
    cpufreq = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    embot::core::print("this is the INIT thread launched by embot::os running on the amc w/ clock = " 
                       + std::to_string(cpufreq/1000000) + " MHz");    
    
    
    embot::core::print("INIT: starts the timer manager + callback manager");
    
    // start the services with default params
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    constexpr embot::core::Time blinktime {2000*embot::core::time1millisec};
                    
    embot::core::print("INIT: starts the LED pulser which will blink a LED @ " + embot::core::TimeFormatter(blinktime).to_string());
 
    static const std::initializer_list<embot::hw::LED> allleds = 
    {   
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three  
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(blinktime); 
    
    
//    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b010101));
//    theleds.get(embot::hw::LED::two).wave(&ledwave); 
    
    
    embot::core::print("INIT: creates the tTEST thread. it will receive one periodic tick event");  
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        evTHR_startup,
        nullptr,
        5000*embot::core::time1millisec,
        evTHR_onevent,
        "evthreadTEST"
    };
       
        
    // create the main thread 
    embot::os::EventThread *thr {nullptr};
    thr = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tTEST
    thr->start(configEV, tTEST); 

    
    // initialise the theErrorManager
    embot::app::eth::theErrorManager::getInstance().initialise(onERRMANemit);
    embot::core::print("INIT thread is over. Normal scheduling starts");    
}

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 1*1000*embot::core::time1millisec;

#include "test.h"

void evTHR_startup(embot::os::Thread *t, void *param)
{   
   
    embot::core::print("tTEST.startup(): starting timer which sends evtTick to itself every = " + embot::core::TimeFormatter(tickperiod).to_string());    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->name("TickTmr");
    tmr->start(cfg);
    
  
    test_embot_hw_init();    
    
}


void evTHR_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {      
//        embot::core::TimeFormatter tf(embot::core::now());        
//        embot::core::print("tTEST.onevent(): evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   

        test_embot_hw_tick();               
    }

}

void onERRMANemit(  embot::app::eth::theErrorManager::Severity sev,
                    const embot::app::eth::theErrorManager::Caller &caller, 
                    const embot::app::eth::theErrorManager::Descriptor &des, 
                    const std::string &str)
{
    std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
    std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
    std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
    std::string severity = embot::app::eth::theErrorManager::to_cstring(sev);
    
    embot::core::print(std::string("[amc][") + severity + "] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);
    
    if(embot::app::eth::theErrorManager::Severity::trace == sev) 
    {
        return;
    } 

    // you may in here send the diagnostics message over a channel, maybe udp or can
    if(true == des.isvalid())
    {
        
    }
  
    if(embot::app::eth::theErrorManager::Severity::fatal == sev)
    {
        // fatal error. i must do something. maybe send the system in an error state and ...
        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).stop();
        for(;;)
        {
            embot::hw::led::toggle(embot::hw::LED::one);
            embot::core::wait(200*embot::core::time1millisec);            
        }
    }        
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

