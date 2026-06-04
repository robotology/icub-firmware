
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

#undef EMBOT_CORE_master
#undef EMBOT_CORE_slave

#if defined(STM32HAL_dualcore_BOOT_cm4master)
    #if defined(STM32HAL_CORE_CM4)
        #define EMBOT_CORE_master
    #else
        #define EMBOT_CORE_slave
    #endif
#elif defined(STM32HAL_dualcore_BOOT_cm7master)
    #if defined(STM32HAL_CORE_CM7)
        #define EMBOT_CORE_master
    #else
        #define EMBOT_CORE_slave
    #endif
#endif

#if defined(CM7launcher) || defined(CM4launcher) || defined(OtherCORElauncher) 
    #define EMBOT_HW_BSP__ultraminimal
#endif


#if defined(EMBOT_HW_BSP__minimal)

    // this is a minimal bsp

    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_flash    

    // so far, if not commented out, CAN is assigned to the slave core
    #if defined(EMBOT_CORE_slave)
        #define EMBOT_ENABLE_hw_can    
    #endif // EMBOT_CORE_slave

    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_one
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
    
//    #define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation
//    #define EMBOT_ENABLE_hw_motor_bldc
//    #define EMBOT_ENABLE_hw_motor_bldc_pwm
//    #define EMBOT_ENABLE_hw_motor_bldc_hall
//    #define EMBOT_ENABLE_hw_motor_bldc_adc
//    #define EMBOT_ENABLE_hw_motor_bldc_enc
//    #define EMBOT_ENABLE_hw_analog
//    //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_adc
//    //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_pwm
//    //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_analog
//    //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_hall
    
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
        
        #define EMBOT_ENABLE_hw_motor_bldc
        #define EMBOT_ENABLE_hw_motor_bldc_pwm
        #define EMBOT_ENABLE_hw_motor_bldc_hall
        #define EMBOT_ENABLE_hw_motor_bldc_adc
        #define EMBOT_ENABLE_hw_motor_bldc_enc
        #define EMBOT_ENABLE_hw_analog
        #define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation
        #define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_enc
        //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_adc
        //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_pwm
        //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_analog
        //#define EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_hall
    #endif
  
    // so far, if not commented out, CAN is assigned to the slave core
    #if defined(EMBOT_CORE_slave)
//        #define EMBOT_ENABLE_hw_can    
    #endif // EMBOT_CORE_slave

    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_one
    #endif

#else

    // normal EMBOT_HW_BSP__
    
    // minimal bsp section 
    #define EMBOT_REDEFINE_hw_bsp_DRIVER_init    
    #define EMBOT_ENABLE_hw_dualcore
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
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

    // so far can, if not commented out, is assigned to the slave core
    #if defined(EMBOT_CORE_slave)
        #define EMBOT_ENABLE_hw_can    
    #else
        #define EMBOT_ENABLE_hw_eth
        #define EMBOT_ENABLE_hw_eeprom
        #define EMBOT_ENABLE_hw_spi
        #define EMBOT_ENABLE_hw_spi_two
        #define EMBOT_ENABLE_hw_spi_three
    
    #endif // EMBOT_CORE_slave 
    
    // so far can is assigned to the slave core
    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_one
    #endif  
#endif

#else
    #error this is the bsp config of STM32HAL_BOARD_AMCFOC_1CM7 ...
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


