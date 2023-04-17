
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMC2C_H_
#define __EMBOT_HW_BSP_AMC2C_H_

#include "embot_core.h"
#include "embot_hw_types.h"

namespace embot { namespace hw { namespace bsp { namespace amc2c {
    
    // adds in here constants or functions specific to the board
    // orange led
    constexpr embot::hw::LED blinkLED {embot::hw::LED::one};
    constexpr embot::hw::LED can2LED {embot::hw::LED::one};
    
    // led
    constexpr embot::hw::LED redLED {embot::hw::LED::two};
    
    embot::hw::BTN EXTFAULTbutton();
    
}}}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



