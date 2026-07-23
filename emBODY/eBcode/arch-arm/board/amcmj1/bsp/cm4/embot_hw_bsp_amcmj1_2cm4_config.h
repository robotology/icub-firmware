
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_2CM4_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_2CM4_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_AMCMJ1_2CM4)

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

// minimal bsp section

#define EMBOT_REDEFINE_hw_bsp_DRIVER_init      
#define EMBOT_ENABLE_hw_dualcore
#define EMBOT_ENABLE_hw_gpio
#define EMBOT_ENABLE_hw_led
#define EMBOT_ENABLE_hw_flash

// emulation of embot::hw::sys::random()

#define EMBOT_ENABLE_hw_sys_emulateRAND

// further configuration 

#if defined(EMBOT_HW_BSP__launcher) 
    
    #include "embot_hw_bsp_amcmj1_launcher_config.h"

#elif defined(EMBOT_HW_BSP__loader)

    #include "embot_hw_bsp_amcmj1_loader_config.h"

#elif defined(EMBOT_HW_BSP__updater) || defined(EMBOT_HW_BSP__maintainer)

    #include "embot_hw_bsp_amcmj1_updater_maintainer_config.h"

#elif defined(EMBOT_HW_BSP__appl_YRI)

    #include "embot_hw_bsp_amcmj1_app_yri_config.h"

#elif defined(EMBOT_HW_BSP__appl_MOT)

    #include "embot_hw_bsp_amcmj1_app_mot_config.h"
    
#else 

    #include "embot_hw_bsp_amcmj1_custom_config.h"
    
#endif

#else
    #error this is the bsp config of STM32HAL_BOARD_AMCMJ1_2CM4 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


