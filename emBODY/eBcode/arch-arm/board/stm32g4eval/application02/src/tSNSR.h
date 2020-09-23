
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _TSNSR_H_
#define _TSNSR_H_


#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_os_rtos.h"
#include <vector>


namespace embot { namespace app { namespace ctrl {
    
        
    // this thread is derived from the application scheleton used so far by our simple applications
            
    class tSNSR 
    {
    public:
        
    enum class snsr : uint8_t { SNSR01, FAP01, other };
        
        struct Config
        {
            uint32_t tbd {0};
            // stack, etc.

            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {} 
            void clear() { tbd = 0;}
            bool isvalid() const 
            { 
                return true; 
            }
        };        
        
        tSNSR();
        ~tSNSR();    
        
        bool start(const Config &config);
        
        // the result of this call is that:
        // - the given sensor will be acquired somehow, 
        // - its value will be put into shared memory,
        // - its status will tx-ed to CAN
        bool start(snsr s, embot::core::relTime freq);
        
        // the result of this call is that:
        // all what start() does is stopped
        bool stop(snsr s);
        
    private:        
        struct Impl;
        Impl *pImpl;        
    };   

    // we get the thread with
    // embot::os::Thread * embot::app::skeleton::os::evthreadcan::getEVTthread();
    
    
}}} // namespace embot { namespace app { namespace ctrl {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



