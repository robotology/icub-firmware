
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CODE_APPLICATION_EVNTSK_H_
#define _EMBOT_CODE_APPLICATION_EVNTSK_H_



#include "embot_app_skeleton_os_basic.h"
#include "embot_os_Thread.h"


namespace embot::app::skeleton::os::evnthr {
       
    class SYSTEMevt : public embot::app::skeleton::os::basic::SYSTEM
    {
    public:
        virtual ~SYSTEMevt() {};
            
        SYSTEMevt(const embot::app::skeleton::os::basic::sysConfig& cfg) 
                      : embot::app::skeleton::os::basic::SYSTEM(cfg) {}
          
        void userdefInit(void *initparam) const override final;                 
        void userdefOnIdle(embot::sys::Task *t, void* idleparam) const override {}
        void userdefonOSerror(void *errparam) const override {}

        virtual void userdefInit_Extra(embot::sys::EventTask* evtsk, void *initparam) const {};
            
    };

    struct evtConfig
    {
        std::uint16_t stacksize {2048};
        void *param {nullptr};
        common::relTime timeout {50*common::time1millisec};

        evtConfig() = default;

        constexpr evtConfig(std::uint16_t ss, void *pa, embot::common::relTime t)
            : stacksize(ss), param(pa), timeout(t) {} 

        bool isvalid() const
        {
            return ((0 == stacksize) || (0 == timeout)) ? false : true;
        }  
    };
    
    class EVNTSK 
    {
    public:
        virtual ~EVNTSK() {};
            
        EVNTSK(const evtConfig& cfg) 
            : config(cfg) {} 

        evtConfig config {};
               
        virtual void userdefStartup(embot::sys::Task *t, void *param) const {}
        virtual void userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const {}
        virtual void userdefOnEvent(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const {}  

        bool isvalid() const { return config.isvalid(); }           
    };

    class CFG
    {
    public:
        const SYSTEMevt *sys;
        const EVNTSK *evt;
        
        CFG() = delete;
        constexpr CFG(const SYSTEMevt *s, const EVNTSK *e) : sys(s), evt(e) {}
        bool isvalid() const
        {
            if((nullptr == sys) || (nullptr == evt))
            {
                return false;
            }
            return sys->isvalid() && evt->isvalid();
        }
    };

    
       
    [[noreturn]] void run(const embot::app::skeleton::os::evnthr::CFG &cfg);


    sys::Task * getEVNTSK();
    
       
} // namespace embot::app::skeleton::os::evnthr {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
