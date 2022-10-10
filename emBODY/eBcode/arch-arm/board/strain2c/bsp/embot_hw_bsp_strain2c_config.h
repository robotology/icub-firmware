
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_STRAIN2C_CONFIG_H_
#define _EMBOT_HW_BSP_STRAIN2C_CONFIG_H_


#if   defined(STM32HAL_BOARD_STRAIN2C)

    #undef  EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_pga308
    #define EMBOT_ENABLE_hw_adc
    #define EMBOT_ENABLE_hw_onewire
    #define EMBOT_ENABLE_hw_timer
    #define EMBOT_ENABLE_hw_si7051
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_bno055
        
#else
    #error this is the bsp config of STM32HAL_BOARD_STRAIN2 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


