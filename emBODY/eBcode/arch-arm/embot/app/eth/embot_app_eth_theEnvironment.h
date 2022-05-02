

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEENVIRONMENT_H_
#define __EMBOT_APP_ETH_THEENVIRONMENT_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_app_eth.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theEnvironment` is responsible to initialise the environment in which the eApplication runs.
    
    In particular, it inits the library eEsharedServices (based onto shalPART, shalINFO and shalBASE) and hides
    its complexity.
    
    It also synchronises the properties of the application, stored into a eEmoduleInfo_t variable with the content 
    of the partition table stored in EEPROM.  

    We must call theEnvironment::initialise() only when a basic `embot::hw` system is initialized, such as 
    with `embot::os::start()`, because we need to use a number of embot::hw namespaces.    
    
    For the eApplication also require that some module has placed a variable such as this in FLASH
    constexpr eEmoduleExtendedInfo_t eapplication_modinfo_extended  
    __attribute__((section(EENV_MODULEINFO_APPLICATION_AT))) =
    { ... };
    

#endif
    
        
    class theEnvironment
    {
    public:
        static theEnvironment& getInstance();
                
    public:
       
        // this singleton uses a constexpr Config that MUST be defined:
        // - inside `theEnvironment_config.h` 
        // - constexpr embot::app::eth::theEnvironment::Config embot::app::eth::theEnvironment_Config { ... }    
        struct Config
        {
            Property applprop {};
            constexpr Config() = default;
            constexpr Config(const Property &p) : applprop(p) {}
            constexpr bool isvalid() const { return true; }
        }; 
        
        static constexpr const Config& getconfig();
                
        bool initialise();  
        bool get(IPconfig &cfg);
                     
    private:
        theEnvironment(); 
        ~theEnvironment(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
