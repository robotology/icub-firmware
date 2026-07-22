
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_LAUNCHER_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_LAUNCHER_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"

#if !defined(EMBOT_HW_BSP__launcher) || !defined(STM32HAL_BOARD_AMCMJ1)
    #error this bsp config is for amcmj1.launcher for either cm7 or cm4 slave
#endif
    
#if !defined(EMBOT_CORE_master)
    #error amcmj1.launcher must be master
#endif

    
#if 0
    marco.accame: the launcher needs ... nothing else
    its only role is to activate the slave core
    only problem is if the other core runs amcmj1.appl.mot which needs teh ADC factory calibration values
    but we manage that in there
#endif
   


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


