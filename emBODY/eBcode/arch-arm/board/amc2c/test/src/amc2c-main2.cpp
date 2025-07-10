
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_bldc_theApplication.h"

#include "embot_app_board_amc2c_info.h"
//#include "embot_app_board_amc2c_mbd.h"

constexpr embot::app::bldc::theApplication::Config cfg 
{
    {embot::app::board::amc2c::info::getCodePartition, embot::app::board::amc2c::info::getCANgentCORE},
    {}, // systCfg: the default is typically OK 
    {},
    {}        // CommCfg: the default is typically OK
    //{embot::app::board::amc2c::mbd::Startup, embot::app::board::amc2c::mbd::OnTick}  // CtrlCfg: the default stack is typically OK
};
    
[[noreturn]] void run();  

int main(void)
{
    // it instructs embot::hw::bsp::specialize() to unlock the cm4 core
    // embot::hw::bsp::amc::set(embot::hw::bsp::amc::OnSpecApplication);
    // or it just do nothing
    
    // run the system
    #if defined(AMC_TEST_CM4_RUN)
    run();
    #elif defined(AMC_TEST_CM4_IDLE)
    for(;;){;}
    #endif
}

// embot::os basic run()

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

void onIdle(embot::os::Thread *t, void* idleparam);
void onError(void *p);
void onInit(embot::os::Thread *t, void* initparam);

[[noreturn]] void run()
{

    constexpr embot::os::IdleThread::Config idlecfg = { 4*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = {onError, nullptr};
    constexpr embot::os::InitThread::Config initcfg = { 6*1024, onInit, nullptr };
    constexpr embot::os::Config osconfig 
    {
        embot::core::time1millisec, 
        initcfg, 
        idlecfg, 
        onOSerror,
        embot::hw::FLASHpartitionID::eapplication01 // tobe checked
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

void onInit(embot::os::Thread *t, void* initparam)
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
        embot::hw::LED::five, embot::hw::LED::six, embot::hw::LED::seven, embot::hw::LED::eight  
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::five).pulse(blinktime); 
    
    
    embot::core::print("INIT: creates the tTEST2 thread. it will receive one periodic tick event");  
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        evTHR_startup,
        nullptr,
        5000*embot::core::time1millisec,
        evTHR_onevent,
        "evthreadTEST2"
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
constexpr embot::core::relTime tickperiod = 2*1000*embot::core::time1millisec;

#include "testManagerSingleton.h"
#include "protocolManager.h"

void evTHR_startup(embot::os::Thread *t, void *param)
{   
   
    embot::core::print("tTEST2.startup(): starting timer which sends evtTick to itself every = " + embot::core::TimeFormatter(tickperiod).to_string());    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->name("TickTmr2");
    tmr->start(cfg);
    
    TestManagerSingleton& tInstance = TestManagerSingleton::getInstance();
  
    tInstance.testManagerSystemInit(t);    
    
}


void evTHR_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    TestManagerSingleton& tInstance = TestManagerSingleton::getInstance();
    
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {      
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tTEST2.onevent(): evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
        
        tInstance.testManagerTick();               
    }
    
    // this is the EVENT thread --> here we add the switch case with the calls to the different tests based on the index of the test
    //outframes.clear();   
    std::uint8_t remaining = 0;
    if(true == embot::core::binary::mask::check(eventmask, ProtocolManager::evRXcanframe))
    {      
        embot::core::print("In embot::core::binary::mask::check(eventmask, evRXcanframe)");
        tInstance.testManagerRun();
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
    
    embot::core::print(std::string("[") + severity + "] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);
    
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
        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::five).stop();
        for(;;)
        {
            embot::hw::led::toggle(embot::hw::LED::five);
            embot::core::wait(200*embot::core::time1millisec);            
        }
    }        
    
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


