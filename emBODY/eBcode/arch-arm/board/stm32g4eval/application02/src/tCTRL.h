
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _TCTRL_H_
#define _TCTRL_H_


#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_os_rtos.h"
#include <vector>


namespace embot { namespace app { namespace ctrl {
    
        
    // this thread is derived from the application scheleton used so far by our simple applications
            
    class tCTRL 
    {
    public:
        
        
        struct Config
        {
            uint32_t tbd {0};
            // stack, etc.
            
            embot::core::relTime period {1000*embot::core::time1microsec};

            constexpr Config() = default;
            constexpr Config(uint32_t t, embot::core::relTime p) : tbd(t), period(p) {} 
            void clear() { tbd = 0;}
            bool isvalid() const 
            { 
                return true; 
            }
        };        
        
        tCTRL();
        ~tCTRL();    
        
        bool initialise(const Config &config);
        
        embot::os::Thread * getThread();
        
    private:        
        struct Impl;
        Impl *pImpl;        
    };   
    
    
}}} // namespace embot { namespace app { namespace ctrl {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



