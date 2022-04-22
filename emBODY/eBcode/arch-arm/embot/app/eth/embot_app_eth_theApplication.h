

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEAPPLICATION_H_
#define __EMBOT_APP_ETH_THEAPPLICATION_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_app_eth.h"

#include <memory>

#include "embot_os.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_eth_theErrorManager.h"
#include "embot_app_eth_theIPnet.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_eth_theListener.h"


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theApplication` is responsible to initialise ...
    
#endif
    
        
    class theApplication
    {
    public:
        static theApplication& getInstance();
                
    public:
       
        // this struct contains all the configurable parameters for the application
        // a constexpr instance of it called theApplication_Config MUST BE DEFINED inside
        // the file theApplication_config.h, but .... 
        // if we call [[noreturn]] void start(const Config &config) w/ a valid
        // config then we use this instead    
        struct Config
        {
            Property property {Process::none, {3, 0}, {2022, Month::Apr, Day::fourteen, 15, 16}};
            embot::core::relTime OStick {1000*embot::core::time1microsec}; // or 500 ...
            uint16_t OSstacksizeinit {6*1024};
            embot::core::Callback OSuserdefinit {}; // very important. it is executed in the INIT thread just before start of scheduling
            uint16_t OSstacksizeidle {2*1024};
            embot::os::Thread::fpOnIdle OSonidle {nullptr};
            embot::os::theTimerManager::Config TMRMANconfig {};
            embot::os::theCallbackManager::Config CBKMANconfig {};
            embot::app::eth::theErrorManager::Config ERRMANconfig {};  
            std::initializer_list<embot::hw::LED> allLEDs { embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three };
            embot::hw::LED pulseLED {embot::hw::LED::three}; 
            embot::core::relTime pulseFREQ {embot::core::time1second}; 
            embot::app::eth::theIPnet::Config IPNETconfig {}; 
            embot::app::eth::theListener::Config LISTENERconfig {};                
            constexpr Config() = default;
            constexpr bool isvalid() const { return Process::eApplication == property.process; }
        }; 
        
        
        [[noreturn]] void start(const Config &config);  
                     
    private:
        theApplication(); 
        ~theApplication(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
