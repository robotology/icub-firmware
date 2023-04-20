
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BSP_AMC2C_H_
#define __EMBOT_HW_MOTOR_BSP_AMC2C_H_

#include "embot_hw_bsp.h"
#include "embot_hw_motor_bsp.h"
#include "embot_hw_timer_bsp.h"

namespace embot::hw::motor::bsp {
    // declaration of MspInit() / MspDeInit() used by motor   
    void TIM_base_MspInit(TIM_HandleTypeDef* htim_base);
    void TIM_base_MspDeInit(TIM_HandleTypeDef* htim_base);
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



