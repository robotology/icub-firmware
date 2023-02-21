/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_STRAIN2_H_
#define _EMBOT_HW_BSP_STRAIN2_H_

#include "embot_core.h"
#include "embot_hw_types.h"

#include "embot_hw_led.h"
#include "embot_hw_si7051.h"
#include "embot_hw_bno055.h"
#include "embot_hw_pga308.h"

namespace embot { namespace hw { namespace bsp { namespace strain2 {
    
    // adds in here constants or functions specific to the board
    const embot::hw::LED ledBLUE = embot::hw::LED::one;
    const embot::hw::SI7051 thermometerSGAUGES = embot::hw::SI7051::one;
    const embot::hw::si7051::Config thermometerSGAUGESconfig {};
    const embot::hw::BNO055 imuBOSCH = embot::hw::BNO055::one;
    const embot::hw::bno055::Config imuBOSCHconfig {};
    
}}}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



