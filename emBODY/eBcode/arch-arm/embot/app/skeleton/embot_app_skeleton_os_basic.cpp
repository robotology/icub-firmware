
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_skeleton_os_basic.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_theApplication.h"

#include "embot_hw_flash.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot::app::skeleton::os::basic {

    const CFG * extCFG {nullptr};
               
}  // namespace embot::app::skeleton::os::basic {


namespace embot::app::skeleton::os::basic {

    void init(embot::os::Thread *t, void* initparam);
    
    void onidle(embot::os::Thread *t, void* idleparam)
    {
        extCFG->sys->userdefOnIdle(t, idleparam);
    }
    
    void onerror(void* errparam)
    {
        extCFG->sys->userdefonOSerror(errparam);
    }

    static const std::uint32_t address = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
    constexpr bool initBSP = true;   
    constexpr bool initSYSservices = true; 


    [[noreturn]] void run(const embot::app::skeleton::os::basic::CFG &cfg)
    {
        
        if(false == cfg.isvalid())
        {
            for(;;);
        }
        
        // store params
        extCFG = &cfg;        
        constexpr embot::os::Thread::fpStartup idlestartup = nullptr;
        embot::os::InitThread::Config initcfg = { extCFG->sys->config.threadINITstacksize, init, extCFG->sys->config.initparam };
        embot::os::IdleThread::Config idlecfg = { extCFG->sys->config.threadIDLEstacksize, idlestartup, extCFG->sys->config.idleparam, onidle };
        embot::core::Callback onOSerror = { onerror, extCFG->sys->config.errparam };
                
        embot::app::theApplication::Config config { address, initBSP, embot::core::time1millisec, {initcfg, idlecfg, onOSerror} };
        embot::app::theApplication &appl = embot::app::theApplication::getInstance();    

        // it prepares the system to run at a given flash address, it inits the hw::bsp, 
        // and finally start scheduler which sets the callback executed by the scheduler on OS error, 
        // starts the idle thread and the init thread. this latter is executed at maximum priority, launches
        // its startup function and exits.
        appl.execute(config);  
        for(;;);
    }

  
} // namespace embot::app::skeleton::os::basic {




namespace embot::app::skeleton::os::basic {
   
    void start_sys_services();

    void init(embot::os::Thread *t, void* initparam)
    { 
        // start sys services: timer manager & callback manager
        start_sys_services();    
                              
        extCFG->sys->userdefInit(initparam);        
    }

    void start_sys_services()
    {
        // start them both only if their config is OK
        if((extCFG->sys->config.tmconfig.isvalid()) && (extCFG->sys->config.cmconfig.isvalid()))
        {
            embot::os::theTimerManager::getInstance().start(extCFG->sys->config.tmconfig);     
            embot::os::theCallbackManager::getInstance().start(extCFG->sys->config.cmconfig);   
        } 
    }

} // namespace embot::app::skeleton::os::basic {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

