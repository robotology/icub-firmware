/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_MTB4FAP_H_
#define _EMBOT_HW_BSP_MTB4FAP_H_

#include "embot_core.h"
#include "embot_hw_types.h"

#include "embot_hw_si7051.h"
#include "embot_hw_bno055.h"

namespace embot { namespace hw { namespace bsp { namespace mtb4fap {
    
    // ads in here constants or functions specific to the board
        constexpr embot::hw::SI7051 thermometer = embot::hw::SI7051::one;
    constexpr embot::hw::si7051::Config thermometerconfig = {};

    constexpr embot::hw::BNO055 imuBOSCH = embot::hw::BNO055::one;
    constexpr embot::hw::bno055::Config imuBOSCHconfig = {};
    
}}}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



