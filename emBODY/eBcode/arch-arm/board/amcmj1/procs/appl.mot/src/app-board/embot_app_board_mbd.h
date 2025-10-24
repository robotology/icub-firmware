
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BOARD_MBD_H_
#define __EMBOT_APP_BOARD_MBD_H_



#include "embot_app_bldc_theApplication.h"


namespace embot::app::board::mbd {
    
    void Startup(embot::app::msg::ADR adr);    
    void OnTick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output);
    
} // embot::app::board::amcfoc::cm7::mbd {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
