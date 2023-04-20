
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_theApplication.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"

#include <array>

#include "embot_os_rtos.h"
#include "embot_os_Thread.h"
#include "embot_os_Action.h"
#include "embot_os_Timer.h"

#include "embot_os_theScheduler.h"

#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include "embot_hw.h"
#include "embot_os.h"

#include "embot_app_application_CANagentCORE.h"
#include "embot_app_application_theCANagentCORE.h"

// the comm object
#include "embot_app_bldc_theCOMM.h"

// the mc control object
#include "embot_app_bldc_theCTRL.h"



// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::bldc::theApplication::Impl
{
      
    Config _config {};
        
    embot::app::application::CANagentCORE *_canagentcore {nullptr};

    Impl() = default;  
        
    static void onInit(embot::os::Thread *t, void* initparam);        
    static void onDefaultIdle(embot::os::Thread *t, void* idleparam);
    static void onDefaultError(void* errparam);        
    static constexpr embot::core::Callback _onDefaultOSerror {onDefaultError, nullptr};
        
    void justbeforeSchedulerStart(embot::os::Thread *t, void* initparam);
    
    [[noreturn]] void start(Config config);       

};

      
[[noreturn]] void embot::app::bldc::theApplication::Impl::start(const Config config)
{
    
    if(false == config.isvalid())
    {
        for(;;);
    }
    
    _config = config;
    
    embot::os::InitThread::Config initcfg 
    { 
        _config.syst.cfgInit.stacksize, 
        onInit, 
        this 
    };
    
    embot::os::IdleThread::Config idlecfg 
    { 
        _config.syst.cfgIdle.stacksize, 
        (_config.syst.cfgIdle.startup != nullptr) ? _config.syst.cfgIdle.startup : nullptr, 
        (_config.syst.cfgIdle.param != nullptr) ? _config.syst.cfgIdle.param : nullptr,
        (_config.syst.cfgIdle.onidle != nullptr) ? _config.syst.cfgIdle.onidle : onDefaultIdle 
    };
    
    embot::os::Config osconfig 
    {   
        _config.syst.ostick, 
        initcfg, 
        idlecfg, 
        _config.syst.onOSerror.isvalid() ? _config.syst.onOSerror : _onDefaultOSerror,
        _config.core.getcodepartition()
    };
    
//    embot::hw::setvectortablelocation(_config.core.getcodepartition());    
    embot::os::init(osconfig);     
    embot::os::start();
   
}

    
void embot::app::bldc::theApplication::Impl::onInit(embot::os::Thread *t, void* initparam)
{
    Impl *impl = reinterpret_cast<Impl*>(initparam);
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);       
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

    
    impl->justbeforeSchedulerStart(t, initparam);
}


void embot::app::bldc::theApplication::Impl::onDefaultIdle(embot::os::Thread *t, void* idleparam)
{
    static int a = 0;
    a++;      
}


void embot::app::bldc::theApplication::Impl::onDefaultError(void* errparam)
{
    static int code = 0;
    embot::os::theScheduler::getInstance().getOSerror(code);
    //for(;;);    
}


void embot::app::bldc::theApplication::Impl::justbeforeSchedulerStart(embot::os::Thread *t, void* initparam)
{
    Impl *impl = reinterpret_cast<Impl*>(initparam);    

    // we perform in here all operations formerly done by theCANboardInfo such as synch the application info etc.
    impl->_canagentcore = impl->_config.core.getagentcore();
    // we start communication
    embot::app::bldc::theCOMM::Config commConfig 
    {
        embot::os::Priority::high44,
        impl->_config.comm.stacksize,
        50*embot::core::time1millisec,
        impl->_canagentcore,
        impl->_config.comm.fifoRXcapacity, impl->_config.comm.fifoTXcapacity
    };      
    embot::app::bldc::theCOMM::getInstance().initialise(commConfig);    
    
    // we start control
    embot::app::bldc::theCTRL::Config ctrlConfig 
    {
        embot::os::Priority::high45,
        impl->_config.ctrl.stacksize,
        1*embot::core::time1millisec,
        impl->_canagentcore,
        &embot::app::bldc::theCOMM::getInstance(),
        impl->_config.ctrl.ctrlStartup,
        impl->_config.ctrl.ctrlTick 
    };        
    embot::app::bldc::theCTRL::getInstance().initialise(ctrlConfig);
        
    // we link control w/ communication
    embot::app::bldc::theCTRL::getInstance().subscribe(&embot::app::bldc::theCOMM::getInstance());       
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::bldc::theApplication& embot::app::bldc::theApplication::getInstance()
{
    static theApplication* p = new theApplication();
    return *p;
}

embot::app::bldc::theApplication::theApplication()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::bldc::theApplication::~theApplication() { }
        
[[noreturn]] void embot::app::bldc::theApplication::start(const Config &config)
{
    pImpl->start(config);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


