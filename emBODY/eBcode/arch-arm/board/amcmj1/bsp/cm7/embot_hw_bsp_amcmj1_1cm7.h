
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCFOC_1CM7_H_
#define __EMBOT_HW_BSP_AMCFOC_1CM7_H_

#include "embot_core.h"
#include "embot_hw_types.h"

namespace embot::hw::bsp::amcfoc::cm7 {
    
    
    // adds in here constants or functions specific to the board

    embot::hw::BTN EXTFAULTbutton();
    
    embot::hw::LED EXTFAULTled();
}


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



