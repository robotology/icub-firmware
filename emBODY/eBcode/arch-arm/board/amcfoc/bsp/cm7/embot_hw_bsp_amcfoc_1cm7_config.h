
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCFOC_1CM7_CONFIG_H_
#define __EMBOT_HW_BSP_AMCFOC_1CM7_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_AMCFOC_1CM7)

#if defined(EMBOT_HW_BSP__minimal)

    // this is a minimal bsp

    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash    
            
    #define EMBOT_ENABLE_hw_can
    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_two
    #endif

#elif defined(EMBOT_HW_BSP__ultraminimal)
    
    // this is a ultra minimal bsp that we use for the launcher
    // where it is best to have as less code as possible

    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash
    
#elif defined(EMBOT_HW_BSP__minimalMOTOR)

    // this is a minimal bps + the motor driver only

    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash   


    #undef EMBOT_ENABLE_hw_motor
    #define EMBOT_ENABLE_hw_motor_bldc
    #if defined(EMBOT_ENABLE_hw_motor_bldc)
            #define EMBOT_ENABLE_hw_motor_hall
            #define EMBOT_ENABLE_hw_motor_adc
            #define EMBOT_ENABLE_hw_motor_enc
            #define EMBOT_ENABLE_hw_motor_pwm
//            #define EMBOT_ENABLE_hw_analog_ish   
    #endif
  
//    #define EMBOT_ENABLE_hw_can
//    #if defined(EMBOT_ENABLE_hw_can)
//        #define EMBOT_ENABLE_hw_can_one
//    #endif   

#else

    // normal EMBOT_HW_BSP__
    
    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash    
    
    
    #undef EMBOT_ENABLE_hw_motor
    // we use EMBOT_ENABLE_hw_motor_bldc
    #define EMBOT_ENABLE_hw_motor_bldc
    #if defined(EMBOT_ENABLE_hw_motor_bldc)
        #define EMBOT_ENABLE_hw_motor_hall
        #define EMBOT_ENABLE_hw_motor_adc
        #define EMBOT_ENABLE_hw_motor_enc
        #define EMBOT_ENABLE_hw_motor_pwm
        #define EMBOT_ENABLE_hw_analog_ish        
    #endif
    
    #define EMBOT_ENABLE_hw_can
    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_two
    #endif
    
    #define EMBOT_ENABLE_hw_mtx
    #define EMBOT_ENABLE_hw_icc_sig
    #define EMBOT_ENABLE_hw_icc_mem
    #define EMBOT_ENABLE_hw_icc_ltr

#endif

#else
    #error this is the bsp config of STM32HAL_BOARD_AMCFOC_1CM7 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


