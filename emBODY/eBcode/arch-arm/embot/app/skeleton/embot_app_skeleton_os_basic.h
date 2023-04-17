
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CODE_APPLICATION_CORE_H_
#define _EMBOT_CODE_APPLICATION_CORE_H_

#include "embot_core.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"


namespace embot::app::skeleton::os::basic {

    struct sysConfig
    {
        std::uint16_t threadINITstacksize {2048};                             
        void *initparam {nullptr};
        std::uint16_t threadIDLEstacksize {512};
        void *idleparam {nullptr};
        void *errparam {nullptr};
        embot::os::theTimerManager::Config tmconfig {};
        embot::os::theCallbackManager::Config cmconfig {};
        embot::core::relTime tick {embot::core::time1millisec};

        constexpr sysConfig() = default;

        constexpr sysConfig(std::uint16_t ssini, void *pini, std::uint16_t ssidl, void *pidl, void *perr,
                            const embot::os::theTimerManager::Config &tmc, const embot::os::theCallbackManager::Config &cmc,
                            const embot::core::relTime _tk = embot::core::time1millisec) 
                                : threadINITstacksize(ssini), initparam(pini), threadIDLEstacksize(ssidl), idleparam(pidl), errparam(perr), 
                                  tmconfig(tmc), cmconfig(cmc),
                                  tick(_tk) 
                            {}
        
        bool isvalid() const
        {   // if any of tmconfig or cmconfig is not valid: the SYSTEMxfg is OK, but both services dont start
            return ((0 == threadINITstacksize) || (0 == threadIDLEstacksize) || (0 == tick)) ? false : true;
        }
    };
       
    class SYSTEM 
    {
    public:
        virtual ~SYSTEM() {};
            
        SYSTEM(const sysConfig &cfg) : config(cfg) {}

        sysConfig config {};
                         
        virtual void userdefInit(void *initparam) const {}
        virtual void userdefOnIdle(embot::os::Thread *t, void* idleparam) const {}
        virtual void userdefonOSerror(void *errparam) const {}
            
        bool isvalid() const { return config.isvalid(); }
    };
    

    class CFG
    {
    public:
        const SYSTEM *sys {nullptr};
        
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

    
       
    [[noreturn]] void run(const embot::app::skeleton::os::basic::CFG &cfg);

       
} // namespace embot::app::skeleton::os::basic {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
