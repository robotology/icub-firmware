
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_CUSTOM_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_CUSTOM_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if !defined(EMBOT_HW_BSP__freertos_blink) || !defined(STM32HAL_BOARD_AMCMJ1)
    #error this bsp config is for amcmj1.freertos.blink for cm4 master
#endif

#if !defined(EMBOT_CORE_master)
    #error amcmj1.freertos.blink must be master
#endif


#if 0
the demo freertos.blink (embot::os on FreeRTOS) needs only what the minimal bsp of embot_hw_bsp_amcmj1_2cm4_config.h
already enables: dualcore, gpio, led, flash. Nothing else.
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

