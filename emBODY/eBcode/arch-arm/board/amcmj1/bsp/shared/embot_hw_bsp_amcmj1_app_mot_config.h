
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_APP_MOT_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_APP_MOT_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_AMCMJ1_2CM4) || defined(STM32HAL_BOARD_AMCMJ1_1CM7)


    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_sys_emulateRAND
     

    // shared
    #define EMBOT_ENABLE_hw_mtx
    #define EMBOT_ENABLE_hw_icc_sig
    #define EMBOT_ENABLE_hw_icc_mem
    #define EMBOT_ENABLE_hw_icc_ltr

    //motor
    #undef EMBOT_ENABLE_hw_motor
    #define EMBOT_ENABLE_hw_motor_bldc
    #if defined(EMBOT_ENABLE_hw_motor_bldc)       
        #define EMBOT_ENABLE_hw_motor_bldc_pwm
        #define EMBOT_ENABLE_hw_motor_bldc_hall
        #define EMBOT_ENABLE_hw_motor_bldc_adc
        #define EMBOT_ENABLE_hw_motor_bldc_qenc
        #define EMBOT_ENABLE_hw_analog
        #define EMBOT_ENABLE_hw_motor_bldc_extfault
    #endif
  
    // so far, if not commented out, CAN is assigned to the slave core
    #if defined(EMBOT_CORE_slave)
        #define EMBOT_ENABLE_hw_can    
    #endif // EMBOT_CORE_slave

    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_one
    #endif


#else
    #error this is the bsp config of STM32HAL_BOARD_AMCMJ1_APP_MOT ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


