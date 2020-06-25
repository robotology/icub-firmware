
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_CONFIG_H_
#define _EMBOT_HW_BSP_CONFIG_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_NUCLEO64)

    #error pls define

#elif defined(STM32HAL_BOARD_MTB4)

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

#elif defined(STM32HAL_BOARD_STRAIN2)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_pga308
    #define EMBOT_ENABLE_hw_adc
    #define EMBOT_ENABLE_hw_onewire
    #define EMBOT_ENABLE_hw_timer
    #define EMBOT_ENABLE_hw_si7051
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_bno055
    
#elif   defined(STM32HAL_BOARD_RFE)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_timer
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_bno055
    
#elif   defined(STM32HAL_BOARD_PSC)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_timer
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_multisda
    #define EMBOT_ENABLE_hw_tlv493d
    
#elif   defined(STM32HAL_BOARD_SG3)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_flash
    //#define EMBOT_ENABLE_hw_pga308
    //#define EMBOT_ENABLE_hw_adc
    //#define EMBOT_ENABLE_hw_onewire
    #define EMBOT_ENABLE_hw_timer
    //#define EMBOT_ENABLE_hw_si7051
    #define EMBOT_ENABLE_hw_i2c
    //#define EMBOT_ENABLE_hw_bno055

#elif   defined(STM32HAL_BOARD_NUCLEOH7)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
#if !defined(STM32HAL_BOARD_NUCLEOH7_BASIC)    
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_bno055
    #define EMBOT_ENABLE_hw_ads122c04
#endif

#elif   defined(STM32HAL_BOARD_STM32G4EVAL)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    //#define EMBOT_ENABLE_hw_button
    
#elif   defined(STM32HAL_BOARD_PMC)

    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    //#define EMBOT_ENABLE_hw_button
    
    
#else
    #error pls sdefine which peripherals/chips are attached to your board
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


