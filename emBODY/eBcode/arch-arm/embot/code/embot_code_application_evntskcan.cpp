
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_code_application_evntskcan.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_common.h"
#include "embot_sys_task.h"
#include "embot_hw_can.h"
#include <vector>
#include "embot_binary.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot::code::application::evntskcan {

    const CFG * extCFG {nullptr};
    
    embot::sys::EventTask* maintask {nullptr};
               
}  // namespace embot::code::application::evntskcan {


namespace embot::code::application::evntskcan {


    [[noreturn]] void run(const embot::code::application::evntskcan::CFG &cfg)
    {
        
        if(false == cfg.isvalid())
        {
            for(;;);
        }
        
        // store params
        extCFG = &cfg;        
 
        // build a embot::code::application::core:CFG and run core application
        embot::code::application::core::CFG cfgcore { extCFG->sys };
        embot::code::application::core::run(cfgcore);
        for(;;); 
    }
    
    sys::Task * getEVTtask()
    {
        return maintask;
    }
  
} // namespace embot::code::application::evntskcan {



namespace embot::code::application::evntskcan {
        
    embot::sys::EventTask* start_evt_based(void);
    void can_init(embot::sys::Task *t);
    void start_usr_services(embot::sys::EventTask* evtsk, void *initparam);
    
    void SYSTEMevtcan::userdefInit(void *initparam) const
    {
        // start the main event-based task which will be scheduled after return of init()
        maintask = start_evt_based(); 

        // we init can here inside the init task rather than in the startup of the event-based task
        can_init(maintask);    
        
        // start the other services and link them to the main task
        start_usr_services(maintask, initparam);   
    }

    void start_usr_services(embot::sys::EventTask* evtsk, void *initparam)
    {
        extCFG->sys->userdefInit_Extra(evtsk, initparam);
    }

} // namespace embot::code::application::evntskcan {


namespace embot::code::application::evntskcan {

    void alerteventbasedtask(void *arg)
    {
        embot::sys::EventTask* evtsk = reinterpret_cast<embot::sys::EventTask*>(arg);
        if(nullptr != evtsk)
        {
            evtsk->setEvent(EVNTSKcan::evRXcanframe);
        }
    }
    
    std::vector<embot::hw::can::Frame> outframes;

    void can_init(embot::sys::Task *t)
    {
        static bool initted = false;
        if(true == initted)
        {
            return;
        }
        initted = true;
        // init can but do not enabled yet
        embot::hw::can::Config canconfig;   // default is tx/rxcapacity=8
        canconfig.rxcapacity = extCFG->evt->cconfig.rxcapacity;
        canconfig.txcapacity = extCFG->evt->cconfig.txcapacity;
        canconfig.onrxframe = embot::common::Callback(alerteventbasedtask, t); 
        embot::hw::can::init(embot::hw::CAN::one, canconfig);
        embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress()); 
        // pre-allocate output vector of frames
        outframes.reserve(extCFG->evt->cconfig.txcapacity);
        // make sure that the application and can protocol versions are synched
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
        canbrdinfo.synch(extCFG->evt->applinfo);
    }

    void can_enable(embot::sys::Task *t)
    {
        embot::hw::can::enable(embot::hw::CAN::one);
    }

    void eventbasedtask_startup(embot::sys::Task *t, void *param)
    {
        //can_init(t);
        extCFG->evt->userdefStartup(t, param);
        can_enable(t);
    }

    void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask eventmask, void *param)
    {   
        if(0 == eventmask)
        {   // timeout ... 
            extCFG->evt->userdefOnTimeout(t, eventmask, param);
            return;
        }

        // we clear the frames to be trasmitted
        outframes.clear(); 

        if(true == embot::binary::mask::check(eventmask, EVNTSKcan::evRXcanframe))
        {        
            embot::hw::can::Frame frame;
            std::uint8_t remainingINrx = 0;
            if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, frame, remainingINrx))
            {            
                extCFG->evt->userdefOnEventRXcanframe(t, eventmask, param, frame, outframes);               
                
                if(remainingINrx > 0)
                {
                    t->setEvent(EVNTSKcan::evRXcanframe);                 
                }
            }        
        }        

        extCFG->evt->userdefOnEventANYother(t, eventmask, param, outframes); 

        // if we have any packet we transmit them
        std::uint8_t num = outframes.size();
        if(num > 0)
        {
            for(std::uint8_t i=0; i<num; i++)
            {
                embot::hw::can::put(embot::hw::CAN::one, outframes[i]);                                       
            }

            embot::hw::can::transmit(embot::hw::CAN::one);  
        } 

    }

    embot::sys::EventTask* start_evt_based(void)
    {                           
        embot::sys::EventTask::Config configEV { 
            extCFG->evt->econfig.stacksize, 
            embot::sys::Priority::high200, 
            eventbasedtask_startup,
            extCFG->evt->econfig.param,
            extCFG->evt->econfig.timeout,
            eventbasedtask_onevent
        };
        
        // create the main task 
        embot::sys::EventTask* tsk = new embot::sys::EventTask;          
        // and start it
        tsk->start(configEV);
        
        return tsk;
    }


} // namespace embot::code::application::evntskcan {


    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

