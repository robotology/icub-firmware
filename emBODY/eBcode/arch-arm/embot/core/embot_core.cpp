
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace core {
       
    static embot::core::Time s_faketimenow()
    {
        static volatile uint64_t tt = 0;
        return tt++;
    }
    
    static bool s_initted = false;    
    static embot::core::fpGetU64 s_timenow = s_faketimenow;
           
    bool initialised() 
    {
        return s_initted;
    }
    
    bool init(const Config &config)
    {
        if(false == config.isvalid())
        {
            return false;
        }
        
        if(nullptr != config.timeconfig.init)
        {
            config.timeconfig.init();
        }
        
        if(nullptr != config.timeconfig.get)
        {
            s_timenow = config.timeconfig.get;
        }
        
        s_initted = true;
        
        return true;
    }
    
    // it returns time in microseconds as configured by Config::timeconfig:init()
    embot::core::Time now()
    {
        return s_timenow();
    }        
    
}} // namespace embot { namespace core {





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

