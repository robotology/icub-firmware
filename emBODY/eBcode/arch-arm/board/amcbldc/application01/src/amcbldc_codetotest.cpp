
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

#define TEST_FOC

#if defined(TEST_FOC)
// foc code
#include "Current_FOC.h"               // Model's header file
#include "rtwtypes.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - code
// --------------------------------------------------------------------------------------------------------------------



    
namespace amcbldc { namespace codetotest {
    
#if defined(TEST_FOC)    
    Current_FOCModelClass Current_FOC_Obj;// Instance of model class
#endif
    
    void init() 
    {
#if defined(TEST_FOC)        
        // Initialize model
        Current_FOC_Obj.initialize();
#endif        
    }
        
    void tick()
    {
#if defined(TEST_FOC)
        // Step the model
        Current_FOC_Obj.step();
#else        
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
#endif        
    }
    
} } // namespace amcbldc { namespace codetotest {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


