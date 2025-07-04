
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_1CM7_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_1CM7_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_AMCMJ1_1CM7)

#if defined(EMBOT_ENABLE_hw_minimal)

    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash    
            
#else

    // minimal section 
    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init      
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash    

 
     // not minimal section
    
    // shared
    #define EMBOT_ENABLE_hw_mtx
    #define EMBOT_ENABLE_hw_icc_sig
    #define EMBOT_ENABLE_hw_icc_mem
    #define EMBOT_ENABLE_hw_icc_ltr

    // portable  
    // cm7: spi, eeprom

// so far can is assigend to cm4
#if 0
//    #define EMBOT_ENABLE_hw_can
    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_one
    #endif 
#endif

    #define EMBOT_ENABLE_hw_spi
    #if defined(EMBOT_ENABLE_hw_spi)
//        #define EMBOT_ENABLE_hw_spi_one
//        #define EMBOT_ENABLE_hw_spi_two
        #define EMBOT_ENABLE_hw_spi_three
    #endif 
    
    #define EMBOT_ENABLE_hw_eeprom
    
    // cm7 specific
   
    
//    #undef EMBOT_ENABLE_hw_motor
//    // we use EMBOT_ENABLE_hw_motor_bldc
//    #define EMBOT_ENABLE_hw_motor_bldc
//    #if defined(EMBOT_ENABLE_hw_motor_bldc)
//        #define EMBOT_ENABLE_hw_motor_hall
//        #define EMBOT_ENABLE_hw_motor_adc
//        #define EMBOT_ENABLE_hw_motor_enc
//        #define EMBOT_ENABLE_hw_motor_pwm
//        #define  EMBOT_ENABLE_hw_analog_ish        
//    #endif
//    
////    #define EMBOT_ENABLE_hw_can
////    #if defined(EMBOT_ENABLE_hw_can)
////        #define EMBOT_ENABLE_hw_can_two
////    #endif
//    
////    #define EMBOT_ENABLE_hw_spi
////    #define EMBOT_ENABLE_hw_eeprom
////    #define EMBOT_ENABLE_hw_encoder
////    #define EMBOT_ENABLE_hw_eth
////    #define EMBOT_ENABLE_hw_timer
////    #define EMBOT_ENABLE_hw_testpoint
////    #define EMBOT_ENABLE_hw_button
//    
//    #define EMBOT_ENABLE_hw_mtx
//    #define EMBOT_ENABLE_hw_icc_sig
//    #define EMBOT_ENABLE_hw_icc_mem
//    #define EMBOT_ENABLE_hw_icc_ltr

#endif

#else
    #error this is the bsp config of STM32HAL_BOARD_AMCFOC_1CM7 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


