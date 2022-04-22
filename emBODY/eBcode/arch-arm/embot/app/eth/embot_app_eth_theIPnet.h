

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEIPNET_H_
#define __EMBOT_APP_ETH_THEIPNET_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_app_eth.h"
#include "embot_os_Thread.h"
#include "ipal_base_cfg2.h"

#include <memory>


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theIPnet` is responsible to initialise the ...
   
#endif
           
    
    class theIPnet
    {
    public:
        static theIPnet& getInstance();
                
    public:

#if 0
        // sockets 
        .numberofsockets = 4,
        .maxdatagramenqueuedintx = 4
        
        // threads 
        .procpriority = embot::core::tointegral(embot::os::Priority::high44), // default = 220 .. but we must give a different value
        .procstacksize = 4*1024, // default = 1024 .. 
        .procmaxidletime = 25*embot::core::time1millisec, 
        .procwakeuponrxframe = eobool_true, 
        .tickpriority = embot::core::tointegral(embot::os::Priority::high43), // default = 219 .. but we must give a different value
        .tickstacksize = 4*1024 // default = 128 .. but that does not allow to run prints or else
        // pointer to iplacfg
        ipal_cfg_any_t *
        // ipconfig
        bool IPconfig-from-environmnet or from external file
#endif   
        struct SocketConfig
        {
            uint8_t numberofsockets {4};
            uint8_t maxdatagramenqueuedintx {4};  
            constexpr SocketConfig() = default;
        };
        
        struct IPALConfig
        {
            ipal_cfg2_t *cfg2 {nullptr};
            bool useenvironment {true};
            constexpr IPALConfig(ipal_cfg2_t *i, bool e) : cfg2(i), useenvironment(e) {}           
            constexpr IPALConfig() = default;
            constexpr bool isvalid() const { return nullptr != cfg2; }
        }; 
        
        struct Config
        {
            IPALConfig ipal {nullptr, true};
            embot::os::Thread::Props proc {embot::os::Priority::high44, 4*1024};
            embot::os::Thread::Props tick {embot::os::Priority::high43, 4*1024};
            embot::core::relTime maxidletime {25*embot::core::time1millisec};
            SocketConfig sockets {4, 4};
            constexpr Config() = default;
            constexpr bool isvalid() const { return ipal.isvalid(); }
        };
                
        bool initialise(const Config &config); 
        bool isinitted() const;        
        bool get(IPconfig &cfg);
                     
    private:
        theIPnet(); 
        ~theIPnet(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
