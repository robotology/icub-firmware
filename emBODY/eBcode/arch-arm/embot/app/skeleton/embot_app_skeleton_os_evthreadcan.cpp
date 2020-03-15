
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_skeleton_os_evthreadcan.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_hw_can.h"
#include <vector>
#include "embot_core_binary.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot::app::skeleton::os::evthreadcan {

    const CFG * extCFG {nullptr};
    
    embot::os::EventThread* mainthread {nullptr};
               
}  // namespace embot::app::skeleton::os::evthreadcan {


namespace embot::app::skeleton::os::evthreadcan {


    [[noreturn]] void run(const embot::app::skeleton::os::evthreadcan::CFG &cfg)
    {
        
        if(false == cfg.isvalid())
        {
            for(;;);
        }
        
        // store params
        extCFG = &cfg;        
 
        // build a embot::app::skeleton::os::basic:CFG and run core application
        embot::app::skeleton::os::basic::CFG cfgcore { extCFG->sys };
        embot::app::skeleton::os::basic::run(cfgcore);
        for(;;); 
    }
    
    embot::os::Thread * getEVTthread()
    {
        return mainthread;
    }
  
} // namespace embot::app::skeleton::os::evthreadcan {



namespace embot::app::skeleton::os::evthreadcan {
        
    embot::os::EventThread* start_evt_based(void);
    void can_init(embot::os::Thread *t);
    void start_usr_services(embot::os::EventThread* evthr, void *initparam);
    
    void SYSTEMevtcan::userdefInit(void *initparam) const
    {
        // start the main event-based thread which will be scheduled after return of init()
        mainthread = start_evt_based(); 

        // we init can here inside the init thread rather than in the startup of the event-based thread
        can_init(mainthread);    
        
        // start the other services and link them to the main thread
        start_usr_services(mainthread, initparam);   
    }

    void start_usr_services(embot::os::EventThread* evthr, void *initparam)
    {
        extCFG->sys->userdefInit_Extra(evthr, initparam);
    }

} // namespace embot::app::skeleton::os::evthreadcan {


namespace embot::app::skeleton::os::evthreadcan {

    void alerteventbasedthread(void *arg)
    {
        embot::os::EventThread* evthr = reinterpret_cast<embot::os::EventThread*>(arg);
        if(nullptr != evthr)
        {
            evthr->setEvent(evThreadCAN::evRXcanframe);
        }
    }
    
    std::vector<embot::prot::can::Frame> outframes;

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
        canconfig.rxcapacity = extCFG->evt->cconfig.rxcapacity;
        canconfig.txcapacity = extCFG->evt->cconfig.txcapacity;
        canconfig.onrxframe = embot::core::Callback(alerteventbasedthread, t); 
        embot::hw::can::init(embot::hw::CAN::one, canconfig);
        embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress()); 
        // pre-allocate output vector of frames
        outframes.reserve(extCFG->evt->cconfig.txcapacity);
        // make sure that the application and can protocol versions are synched
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
        canbrdinfo.synch(extCFG->evt->applinfo);
    }

    void can_enable(embot::os::Thread *t)
    {
        embot::hw::can::enable(embot::hw::CAN::one);
    }

    void eventbasedthread_startup(embot::os::Thread *t, void *param)
    {
        //can_init(t);
        extCFG->evt->userdefStartup(t, param);
        can_enable(t);
    }

    void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {   
        if(0 == eventmask)
        {   // timeout ... 
            extCFG->evt->userdefOnTimeout(t, eventmask, param);
            return;
        }

        // we clear the frames to be trasmitted
        outframes.clear(); 

        if(true == embot::core::binary::mask::check(eventmask, evThreadCAN::evRXcanframe))
        {        
            embot::hw::can::Frame hwframe;
            std::uint8_t remainingINrx = 0;
            if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, hwframe, remainingINrx))
            {            
                extCFG->evt->userdefOnEventRXcanframe(t, eventmask, param, {hwframe.id, hwframe.size, hwframe.data}, outframes);               
                
                if(remainingINrx > 0)
                {
                    t->setEvent(evThreadCAN::evRXcanframe);                 
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
                embot::hw::can::put(embot::hw::CAN::one, {outframes[i].id, outframes[i].size, outframes[i].data});                                       
            }

            embot::hw::can::transmit(embot::hw::CAN::one);  
        } 

    }

    embot::os::EventThread* start_evt_based(void)
    {                           
        embot::os::EventThread::Config configEV { 
            extCFG->evt->econfig.stacksize, 
            embot::os::Priority::high200, 
            eventbasedthread_startup,
            extCFG->evt->econfig.param,
            extCFG->evt->econfig.timeout,
            eventbasedthread_onevent
        };
        
        // create the main thread 
        embot::os::EventThread* thr = new embot::os::EventThread;          
        // and start it
        thr->start(configEV);
        
        return thr;
    }


} // namespace embot::app::skeleton::os::evthreadcan {


    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

