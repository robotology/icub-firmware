
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CODE_APPLICATION_EVNTSKCAN_H_
#define _EMBOT_CODE_APPLICATION_EVNTSKCAN_H_




#include "embot_code_application_core.h"
#include "embot_sys_Task.h"
#include "embot_hw_can.h"
#include "embot_app_theCANboardInfo.h"

namespace embot::code::application::evntskcan {
       
    class SYSTEMevtcan : public embot::code::application::core::SYSTEM
    {
    public:
        virtual ~SYSTEMevtcan() {};
            
        SYSTEMevtcan(const embot::code::application::core::sysConfig& cfg) 
            : embot::code::application::core::SYSTEM(cfg) {}
            
        void userdefInit(void *initparam) const override final; // qui inizializzo evntsk, can and then                
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

    struct canConfig
    {
        std::uint8_t rxcapacity {8};
        std::uint8_t txcapacity {8};

        canConfig() = default;

        constexpr canConfig(std::uint8_t rx, std::uint8_t tx)
            : rxcapacity(rx), txcapacity(tx) {} 

        bool isvalid() const
        {
            return ((0 == rxcapacity) || (0 == txcapacity)) ? false : true;
        }  
    };
    
    class EVNTSKcan 
    {
    public:
        virtual ~EVNTSKcan() {};
            
        EVNTSKcan(const embot::code::application::evntskcan::evtConfig& ecfg, const embot::code::application::evntskcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
        : econfig(ecfg), cconfig(ccfg), applinfo(a) {} 

        embot::code::application::evntskcan::evtConfig econfig {};
        canConfig cconfig {};
        embot::app::theCANboardInfo::applicationInfo applinfo {};

        static constexpr embot::common::Event evRXcanframe { 0x00000001 << 0 };
               
        virtual void userdefStartup(embot::sys::Task *t, void *param) const {}
        virtual void userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const {}
        virtual void userdefOnEventRXcanframe(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &outframes) const {}
        virtual void userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const {}  

        bool isvalid() const { return econfig.isvalid() && cconfig.isvalid(); }           
    };

    class CFG
    {
    public:
        const SYSTEMevtcan *sys {nullptr};
        const EVNTSKcan *evt {nullptr};
        
        CFG() = delete;
        constexpr CFG(const SYSTEMevtcan *s, const EVNTSKcan *e) : sys(s), evt(e) {}
        bool isvalid() const
        {
            if((nullptr == sys) || (nullptr == evt))
            {
                return false;
            }
            return sys->isvalid() && evt->isvalid();
        }
    };

    
       
    [[noreturn]] void run(const embot::code::application::evntskcan::CFG &cfg);


    sys::Task * getEVTtask();
    
       
} // namespace embot::code::application::evntskcan {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
