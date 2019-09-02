
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_code_application_evntsk.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_common.h"
#include "embot_sys_task.h"
//#include "embot_app_theApplication.h"

//#include "embot_hw_flash.h"
//#include "embot_sys_theTimerManager.h"
//#include "embot_sys_theCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot::code::application::evntsk {

    const CFG * extCFG {nullptr};
               
}  // namespace embot::code::application::evntsk {


namespace embot::code::application::evntsk {


    [[noreturn]] void run(const embot::code::application::evntsk::CFG &cfg)
    {
        
        if(false == cfg.isvalid())
        {
            for(;;);
        }
        
        // store params
        extCFG = &cfg;        
 
        // build a embot::code::application::core:CFG

        embot::code::application::core::CFG cfgcore { extCFG->sys };
        embot::code::application::core::run(cfgcore);
        for(;;); 
    }

  
} // namespace embot::code::application::evntsk {



namespace embot::code::application::evntsk {
    
    embot::sys::EventTask* maintask {nullptr};
    
    embot::sys::EventTask* start_evt_based(void);
    void start_usr_services(embot::sys::EventTask* evtsk, void *initparam);
    
    void SYSTEMevt::userdefInit(void *initparam) const
    {
        // start the main event-based task which will be scheduled after return of init()
        maintask = start_evt_based();     
        
        // start the other services and link them to the main task
        start_usr_services(maintask, initparam);   
    }

    void start_usr_services(embot::sys::EventTask* evtsk, void *initparam)
    {
        extCFG->sys->userdefInit_Extra(evtsk, initparam);
    }

} // namespace embot::code::application::evntsk {


namespace embot::code::application::evntsk {

#if 0
    void alerteventbasedtask(void *arg)
    {
        embot::sys::EventTask* evtsk = reinterpret_cast<embot::sys::EventTask*>(arg);
        if(nullptr != evtsk)
        {
            evtsk->setEvent(evRXcanframe);
        }
    }
#endif

    sys::Task * getEVNTSK()
    {
        return maintask;
    }

    void eventbasedtask_startup(embot::sys::Task *t, void *param)
    {      
        extCFG->evt->userdefStartup(t, param);
    }

    void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask eventmask, void *param)
    {   
        if(0 == eventmask)
        {   // timeout ... 
            extCFG->evt->userdefOnTimeout(t, eventmask, param);
            return;
        }

        extCFG->evt->userdefOnEvent(t, eventmask, param); 
    }


    embot::sys::EventTask* start_evt_based(void)
    {                           
        embot::sys::EventTask::Config configEV { 
            extCFG->evt->config.stacksize, 
            embot::sys::Priority::high200, 
            eventbasedtask_startup,
            extCFG->evt->config.param,
            extCFG->evt->config.timeout,
            eventbasedtask_onevent
        };
        
        // create the main task 
        embot::sys::EventTask* tsk = new embot::sys::EventTask;          
        // and start it
        tsk->start(configEV);
        
        return tsk;
    }


} // namespace embot::code::application::evntsk {


    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

