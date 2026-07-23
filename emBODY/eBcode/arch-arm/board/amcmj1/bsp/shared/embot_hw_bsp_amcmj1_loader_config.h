
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_LOADER_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_LOADER_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"



#if !defined(EMBOT_HW_BSP__loader) || !defined(STM32HAL_BOARD_AMCMJ1)
    #error this bsp config is for amcmj1.loader for either cm7 or cm4 slave
#endif
    
#if !defined(EMBOT_CORE_master)
    #error amcmj1.loader must be master
#endif

#if 0

the loader needs only:
- to use eeprom: that it inits in runtime and not inside specialize(), so:
      EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_three, EMBOT_ENABLE_hw_eeprom  
    - when it runs on the cm4 core, to redefine the embot::hw::sys:uniqueid() so that it is read from eeprom,
      but no need to set the unique id. in our case it is the appl.yri that does that.
      EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid

no need for:
    - specialize() as the eeprom is initialzed when it is needed
    - hw mtx and icc. the embot::hw::dualcore::bsp uses an embot::hw::MTX but through HAL calls
    - to redefine the embot::hw::sys:setuniqueid(). someone else will write it on the eeprom. the appl.yri will do that.
#endif

    #define EMBOT_ENABLE_hw_spi
    #define EMBOT_ENABLE_hw_spi_three    
    #define EMBOT_ENABLE_hw_eeprom
    
    #if defined(STM32HAL_CORE_CM4) 
        #define EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid 
    #endif // STM32HAL_CORE_CM4
    

    // #define EMBOT_ENABLE_hw_bsp_specialize
    
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


