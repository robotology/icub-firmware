
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amc2c_mbd.h"
#include "embot_app_board_amc2c_theMBD.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#define TEST_theICCservice

#if defined(TEST_embot_hw_icc)
#include "embot_hw_icc_sig.h"
#include "embot_hw_icc_mem.h"
#include "embot_hw_icc_ltr.h"
#endif

namespace embot::app::board::amc2c::mbd {
    
    void Startup(embot::prot::can::Address adr)
    {
#if defined(TEST_theICCservice)
        
#elif defined(TEST_embot_hw_icc)
      
//        embot::hw::icc::sig::test::init(embot::hw::icc::DIR::rx); 
//        embot::hw::icc::mem::test::init(embot::hw::icc::DIR::rx); 
        embot::hw::icc::ltr::test::init(embot::hw::icc::DIR::tx); 
        
#else
        embot::app::board::amc2c::theMBD::getInstance().initialise({adr});          
#endif        

    }
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    {
        
#if defined(TEST_theICCservice)
        
#elif defined(TEST_embot_hw_icc)
      
        static uint32_t tt {0};
        tt++;
        
        if(0 == (tt % 500))
        {
            // embot::hw::icc::mem::test::tick(); 
            embot::core::TimeFormatter tf {embot::core::now() };  
            embot::hw::icc::ltr::test::tick(tf.to_string());
        }   
        
#else
        embot::app::board::amc2c::theMBD::getInstance().tick(input, output);    
#endif   
                
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


