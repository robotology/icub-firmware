
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


namespace embot::app::board::amc2c::mbd {
    
    void Startup(embot::prot::can::Address adr)
    {
        embot::app::board::amc2c::theMBD::getInstance().initialise({adr});  
    }
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    {
        embot::app::board::amc2c::theMBD::getInstance().tick(input, output);    
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


