
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CODE_APPLICATION_EVNTSKCAN_H_
#define _EMBOT_CODE_APPLICATION_EVNTSKCAN_H_



#include "embot_core.h"
#include "embot_code_application_core.h"
#include "embot_sys_Task.h"
#include "embot_app_canprotocol.h"
#include "embot_app_theCANboardInfo.h"

namespace embot::code::application::evntskcan {
       
    class SYSTEMevtcan : public embot::code::application::core::SYSTEM
    {
    public:
        virtual ~SYSTEMevtcan() {};
            
        SYSTEMevtcan(const embot::code::application::core::sysConfig& cfg) 
            : embot::code::application::core::SYSTEM(cfg) {}
            
        void userdefInit(void *initparam) const override final; // qui inizializzo evntsk, can and then                
        void userdefOnIdle(embot::os::Thread *t, void* idleparam) const override {}
        void userdefonOSerror(void *errparam) const override {}

        virtual void userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const {};                  
    };
    
    struct evtConfig
    {
        std::uint16_t stacksize {2048};
        void *param {nullptr};
        embot::core::relTime timeout {50*embot::core::time1millisec};

        evtConfig() = default;

        constexpr evtConfig(std::uint16_t ss, void *pa, embot::core::relTime t)
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

        static constexpr embot::os::Event evRXcanframe { 0x00000001 << 0 };
               
        virtual void userdefStartup(embot::os::Thread *t, void *param) const {}
        virtual void userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const {}
        virtual void userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const {}
        virtual void userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const {}  

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


    embot::os::Thread * getEVTtask();
    
       
} // namespace embot::code::application::evntskcan {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
