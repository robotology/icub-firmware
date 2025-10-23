
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_mbd.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#define testCODE

#if defined(testCODE)

#else
#include "embot_app_board_theMBD.h"
#endif




namespace embot::app::board::mbd {
    
    
    void Startup(embot::app::msg::ADR adr)
    {
#if defined(testCODE)

#else        
        embot::app::board::theMBD::getInstance().initialise({adr});
#endif        
    }
    
    void OnTick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    { 
#if defined(testCODE)
        static embot::core::Time lastprint = embot::core::now();
        constexpr embot::core::relTime period {embot::core::time1second};
        
        embot::core::Time now = embot::core::now();
        if((now - lastprint) >= period)
        {
            lastprint = now;
            embot::core::TimeFormatter tf {now};
            embot::core::print(tf.to_string() + ": hello from amcmj1.appl.mot");
        }
#else        
         embot::app::board::theMBD::getInstance().tick(input, output);
#endif 
        
    }
    
    

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




