

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
    The singleton `theETHmonitor` owns a thread which periodically checks the status of the ETH links
    and stores the result in the internals.
    The thread does not transmit any result by its own because the socket may be exclusive use of a
    given thread, so when any transmission is required the thread executes the callback called `ticker`
    inside `theETHmonitor::Config`. The callback must be initted in such a way that if alerts the
    thread owning the socket to call `theETHmonitor::tick()`. It is inside this method that the 
    transmission is done. So far, that happens through `theErrorManager`.  
    
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
            embot::core::Callback ticker {};
            constexpr Config() = default;
            constexpr Config(const embot::os::Thread::Props &t, 
                             embot::core::relTime p,
                             embot::core::relTime ro,
                             const embot::core::Callback &tk) 
                             : thread(t), period(p), reportOKperiod(ro), ticker(tk) {}
            constexpr bool isvalid() const { return thread.isvalid(); }
        }; 
                
        bool initialise(const Config &config);  
        
        bool set(const embot::core::Callback &ticker);
//        bool ticker(); // called .. we see it later. very likey to be moved inside implemenattion
        bool tick(); // called by the owner of the socket to send results out
                     
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
