

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEETHMONITOR_H_
#define __EMBOT_APP_ETH_THEETHMONITOR_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_os.h"

#include "embot_app_eth.h"


#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theETHmonitor` pweridicially checks vs the status of the ETH links    
#endif
            
    
    class theETHmonitor
    {
    public:
        static theETHmonitor& getInstance();
                
    public:
        
                        
        struct Config
        {   // use embot::os::Priority::belownorm22
            embot::os::Thread::Props thread {embot::os::Priority::belownorm22, 2*1024};
            embot::core::relTime period {100*embot::core::time1millisec};
            embot::core::relTime reportOKperiod {60*embot::core::time1second};
            constexpr Config() = default;
            constexpr Config(const embot::os::Thread::Props &t, 
                             embot::core::relTime p,
                             embot::core::relTime ro) 
                             : thread(t), period(p), reportOKperiod(ro) {}
            constexpr bool isvalid() const { return thread.isvalid(); }
        }; 
                
        bool initialise(const Config &config);  
        
                     
    private:
        theETHmonitor(); 
        ~theETHmonitor(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
