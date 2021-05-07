
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "amcbldc_codetotest.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_hw_sys.h"
#include <array>

// --------------------------------------------------------------------------------------------------------------------
// - code
// --------------------------------------------------------------------------------------------------------------------

    
namespace amcbldc { namespace codetotest {
           
    void init() 
    {
        // nothing needed
    }
        
    void tick()
    {
        static constexpr std::array<embot::core::relTime, 5> usec = 
        {   
            100*embot::core::time1microsec, 
            200*embot::core::time1microsec, 
            300*embot::core::time1microsec, 
            400*embot::core::time1microsec, 
            500*embot::core::time1microsec
        }; 
        static uint8_t index {0};
        
        index = (index+1) % usec.size();
        embot::hw::sys::delay(usec[index]);                
    }
    
} } // namespace amcbldc { namespace codetotest {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


