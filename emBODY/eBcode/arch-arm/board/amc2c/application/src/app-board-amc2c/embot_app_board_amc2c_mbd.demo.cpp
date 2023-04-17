
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amc2c_mbd.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::board::amc2c::mbd {

    void Startup(embot::prot::can::Address adr)
    {
        embot::core::print("MBD is starting up on an amc2c");
    }
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    {
        static uint32_t cnt {0};
        if(0 == (cnt++ % 1))
        {
//            embot::core::print("MBD is ticking on an amc2c");
        
            uint8_t b0 = cnt&0xff;
            uint8_t b1 = (cnt>>8)&0xff;
            uint8_t b2 = (cnt>>16)&0xff;
            uint8_t b3 = (cnt>>24)&0xff;
            embot::prot::can::Frame fr1 {0x202, 4, {b0, b1, b2, b3, 0, 0, 0, 0}};
            output.push_back(fr1);            
        }
        
        size_t numRXframes = input.size();
        
        if(numRXframes > 0)
        {
            // just for test: i get the first only and i send it back 
            embot::prot::can::Frame f = input[0];
            output.push_back(f);    
        }        
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


