/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_NUCLEO64_H_
#define _EMBOT_HW_BSP_NUCLEO64_H_

#include "embot_core.h"
#include "embot_hw_types.h"

#include "embot_hw_led.h"

namespace embot { namespace hw { namespace bsp { namespace nucleo64 {
    
    // adds in here constants or functions specific to the board

    constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;
    constexpr embot::hw::BTN buttonPB9 = embot::hw::BTN::two;

}}}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



