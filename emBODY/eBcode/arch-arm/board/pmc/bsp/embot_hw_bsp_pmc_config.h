
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_PMC_CONFIG_H_
#define _EMBOT_HW_BSP_PMC_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if   defined(STM32HAL_BOARD_PMC)

    #define  EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_can
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_tlv493d
    //#define EMBOT_ENABLE_hw_tlv493d_emulatedMODE
    //#define EMBOT_ENABLE_hw_button
    
    //#define EMBOT_ENABLE_hw_tlv493d_U27off
    //#define EMBOT_ENABLE_hw_tlv493d_J13off
        
#else
    #error this is the bsp config of STM32HAL_BOARD_PMC ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


