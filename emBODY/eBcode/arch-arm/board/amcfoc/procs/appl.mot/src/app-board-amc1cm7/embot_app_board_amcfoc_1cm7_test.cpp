
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amcfoc_1cm7_mbd.h"
#include "embot_app_board_amcfoc_1cm7_theMBD.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



namespace embot::app::board::amcfoc::cm7::mbd {
    

    void Startup(embot::app::msg::ADR adr)
    {
   

    }
    
    void OnTick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    {
        
        size_t numRXframes = input.size();
        
        if(numRXframes > 0)
        {
            embot::app::bldc::MSG m = input[0];
            
        }
        
    }


} // namespace embot::app::board::amcfoc::cm7::mbd {



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


