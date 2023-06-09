
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

//#define TEST_embot_hw_icc

#if defined(TEST_embot_hw_icc)
#include "embot_hw_icc_sig.h"
#include "embot_hw_icc_mem.h"
#endif

namespace embot::app::board::amc2c::mbd {
    
    void Startup(embot::prot::can::Address adr)
    {
#if !defined(TEST_embot_hw_icc)
        embot::app::board::amc2c::theMBD::getInstance().initialise({adr});  
#else        
//        embot::hw::icc::sig::test::init(embot::hw::icc::DIR::rx); 
        embot::hw::icc::mem::test::init(embot::hw::icc::DIR::rx); 
#endif        

    }
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    {
#if! defined(TEST_embot_hw_icc)
        embot::app::board::amc2c::theMBD::getInstance().tick(input, output);    
#else        
        static uint32_t tt {0};
        tt++;
        
        if(0 == (tt % 500))
        {
            embot::hw::icc::mem::test::tick();   
        }   
#endif                 
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


