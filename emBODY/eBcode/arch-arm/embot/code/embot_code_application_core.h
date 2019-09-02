
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CODE_APPLICATION_CORE_H_
#define _EMBOT_CODE_APPLICATION_CORE_H_

#include "embot_common.h"
#include "embot_sys_theTimerManager.h"
#include "embot_sys_theCallbackManager.h"


namespace embot::code::application::core {

    struct sysConfig
    {
        std::uint16_t taskINITstacksize {2048};                             
        void *initparam {nullptr};
        std::uint16_t taskIDLEstacksize {512};
        void *idleparam {nullptr};
        void *errparam {nullptr};
        embot::sys::theTimerManager::Config tmconfig {};
        embot::sys::theCallbackManager::Config cmconfig {};

        constexpr sysConfig() = default;

        constexpr sysConfig(std::uint16_t ssini, void *pini, std::uint16_t ssidl, void *pidl, void *perr,
                            const embot::sys::theTimerManager::Config &tmc, const embot::sys::theCallbackManager::Config &cmc) 
                                : taskINITstacksize(ssini), initparam(pini), taskIDLEstacksize(ssidl), idleparam(pidl), errparam(perr), tmconfig(tmc), cmconfig(cmc) {}
        
        bool isvalid() const
        {   // if any of tmconfig or cmconfig is not valid: the SYSTEMxfg is OK, but both services dont start
            return ((0 == taskINITstacksize) || (0 == taskIDLEstacksize)) ? false : true;
        }
    };
       
    class SYSTEM 
    {
    public:
        virtual ~SYSTEM() {};
            
        SYSTEM(const sysConfig &cfg) : config(cfg) {}

        sysConfig config {};
                         
        virtual void userdefInit(void *initparam) const {}
        virtual void userdefOnIdle(embot::sys::Task *t, void* idleparam) const {}
        virtual void userdefonOSerror(void *errparam) const {}
            
        bool isvalid() const { return config.isvalid(); }
    };
    

    class CFG
    {
    public:
        const SYSTEM *sys;
        
        CFG() = delete;
        constexpr CFG(const SYSTEM *s) : sys(s) {}
        bool isvalid() const
        {
            if(nullptr == sys)
            {
                return false;
            }
            return sys->isvalid();
        }
    };

    
       
    [[noreturn]] void run(const embot::code::application::core::CFG &cfg);

       
} // namespace embot::code::application::core {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
