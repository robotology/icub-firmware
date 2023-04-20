
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

namespace embot::hw::bsp::amc2c {
    
    // adds in here constants or functions specific to the board

    constexpr embot::hw::LED blinkLED {embot::hw::LED::one};
    constexpr embot::hw::LED can2LED {embot::hw::LED::two};
    
    // led
    constexpr embot::hw::LED redLED {embot::hw::LED::two};
    
    embot::hw::BTN EXTFAULTbutton();
    embot::hw::LED EXTFAULTled();
    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



