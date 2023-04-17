
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BOARD_AMCBLDC_MBD_H_
#define __EMBOT_APP_BOARD_AMCBLDC_MBD_H_



#include "embot_app_bldc_theApplication.h"


namespace embot::app::board::amcbldc::mbd {
    
    void Startup(embot::prot::can::Address adr);    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output);
    
    } // embot::app::board::amcbldc::mbd {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
