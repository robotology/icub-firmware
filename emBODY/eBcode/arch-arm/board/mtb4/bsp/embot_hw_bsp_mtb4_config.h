
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_MTB4_CONFIG_H_
#define _EMBOT_HW_BSP_MTB4_CONFIG_H_


#if   defined(STM32HAL_BOARD_MTB4)

    #undef  EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_timer
    #define EMBOT_ENABLE_hw_si7051
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_bno055
    #define EMBOT_ENABLE_hw_multisda

#if defined(ENABLE_FAPreader)
    #define EMBOT_ENABLE_hw_i2ce
    #define EMBOT_ENABLE_hw_tlv493d
    #define EMBOT_ENABLE_hw_tlv493d_i2ceMODE    
#endif

#else
    #error this is the bsp config of STM32HAL_BOARD_MTB4 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


