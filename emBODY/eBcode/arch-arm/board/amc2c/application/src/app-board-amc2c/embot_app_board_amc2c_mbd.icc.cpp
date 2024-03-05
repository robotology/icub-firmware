
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
#include "embot_app_application_theCANtracer.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#if defined(TEST_embot_hw_icc)
#include "embot_hw_icc_sig.h"
#include "embot_hw_icc_mem.h"
#include "embot_hw_icc_ltr.h"
#endif

#if defined(TEST_theICCservice)

#include "embot_app_eth_theICCservice.h"

void regularTX(std::vector<embot::app::bldc::MSG> &output)
{
    // print over debugger ... ?
    // add a canprint to output queue _tCOMMoutframes
    // send a ...
    
    static constexpr embot::core::Time period {5000*embot::core::time1millisec};    
    static embot::core::Time lastcall {0};    
    embot::core::Time now = embot::core::now();
    embot::core::Time delta = now - lastcall;    
    if(delta < period)
    {
        return;
    }    
    lastcall = now;
    
    // and now what i have to do
     

    embot::core::print("emitting a CAN PRINT w/ string = 5sEc");    
    embot::app::theCANtracer::getInstance().print("5sEc", output);         
}

#endif

namespace embot::app::board::amc2c::mbd {
    
    void Startup(embot::app::msg::ADR adr)
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
    
    void OnTick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    {        
#if defined(TEST_theICCservice)
        
        #warning TEST_theICCservice  is defined ....        
        // i just print the messages
        
        if(false == input.empty())
        {
            embot::core::Time n {embot::core::now()};
            
            embot::core::print(embot::core::TimeFormatter(n).to_string() + ": received " +
            std::to_string(input.size()) + " ICC frames -> ");
            for(auto const &frame : input)
            {
                embot::core::print(frame.to_string());
            }
            embot::core::print("sending back a CAN PRINT w/ string = acKed");

            // ok, i will send back one frame then
            embot::app::theCANtracer::getInstance().print("acKed", output);            
        }
        
        regularTX(output);
        
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


