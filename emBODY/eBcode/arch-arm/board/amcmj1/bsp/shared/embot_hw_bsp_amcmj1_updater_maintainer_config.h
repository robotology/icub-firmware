
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_UPDATER_MAINTAINER_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_UPDATER_MAINTAINER_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_AMCMJ1_2CM4) || defined(STM32HAL_BOARD_AMCMJ1_1CM7)


    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_sys_emulateRAND
     

    // shared
    #define EMBOT_ENABLE_hw_mtx
    #define EMBOT_ENABLE_hw_icc_sig
    #define EMBOT_ENABLE_hw_icc_mem
    #define EMBOT_ENABLE_hw_icc_ltr


// so far, spi and eeprom are assigned to the master core, so:
#if defined(EMBOT_CORE_master)


    #define EMBOT_ENABLE_hw_spi
    #if defined(EMBOT_ENABLE_hw_spi)
        #define EMBOT_ENABLE_hw_spi_two
        #define EMBOT_ENABLE_hw_spi_three
    #endif 
    
    
    #define EMBOT_ENABLE_hw_can 
    
    
    #define EMBOT_ENABLE_hw_eeprom

    #if defined(STM32HAL_CORE_CM4) 
    //#warning ... if the cm4 is also master then you need EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid + EMBOT_REDEFINE_hw_bsp_DRIVER_setuniqueid
        #define EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid 
        #define EMBOT_REDEFINE_hw_bsp_DRIVER_setuniqueid
    #endif // STM32HAL_CORE_CM4
    
    // and also eth is assigned to master
    #define EMBOT_ENABLE_hw_eth
    
    #if defined(_MAINTAINER_APPL_)
        #undef EMBOT_ENABLE_hw_can
    #endif
    
    
#endif // EMBOT_CORE_master



#else
    #error this is the bsp config of __EMBOT_HW_BSP_AMCMJ1_UPDATER_MAINTAINER ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


