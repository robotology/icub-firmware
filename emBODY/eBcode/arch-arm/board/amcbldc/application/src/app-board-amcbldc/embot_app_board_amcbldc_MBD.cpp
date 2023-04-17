
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amcbldc_MBD.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



namespace embot::app::board::amcbldc::mbd {

    void Startup(embot::prot::can::Address adr)
    {
        embot::core::print("MBD is starting up");
    }
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    {
//        static uint32_t cnt {0};
//        if(0 == (cnt++ % 3000))
//        {
//            embot::core::print("MBD is ticking");
//        }
//        
//        size_t numRXframes = input.size();
//        
//        if(numRXframes > 0)
//        {
//            // just for test: i get the first only and i send it back twice
//            embot::prot::can::Frame f = input[0];
//            embot::prot::can::Frame fr1 {0x101, 3, {1, 2, 3, 0, 0, 0, 0, 0}};
//            output.push_back(fr1);
//            output.push_back(f);    
//        }        
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


