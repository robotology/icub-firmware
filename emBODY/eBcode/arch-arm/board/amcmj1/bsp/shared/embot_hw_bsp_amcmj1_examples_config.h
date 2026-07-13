
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_EXAMPLES_YRI_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_EXAMPLES_YRI_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if defined(STM32HAL_BOARD_AMCMJ1_2CM4) || defined(STM32HAL_BOARD_AMCMJ1_1CM7)

    #warning this is the bsp config of STM32HAL_BOARD_AMCMJ1_EXAMPLES_YRI, which is not a real bsp. 
    #warning It is not meant to be used in production. It is meant to be used only for testing and development purposes.

    
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_ENABLE_hw_bsp_specialize
             
    // not minimal section
    
    // shared
//    #define EMBOT_ENABLE_hw_mtx
//    #define EMBOT_ENABLE_hw_icc_sig
//    #define EMBOT_ENABLE_hw_icc_mem
//    #define EMBOT_ENABLE_hw_icc_ltr

    // portable  
    // cm7: spi, eeprom

//    #define EMBOT_ENABLE_hw_timer

        #define EMBOT_ENABLE_hw_can    
        #define EMBOT_ENABLE_hw_eth
        #define EMBOT_ENABLE_hw_eeprom
        
        #define EMBOT_ENABLE_hw_spi
        #if defined(EMBOT_ENABLE_hw_can)
            //#define EMBOT_ENABLE_hw_spi_one
            #define EMBOT_ENABLE_hw_spi_two
            #define EMBOT_ENABLE_hw_spi_three
        #endif  
    

    #define EMBOT_ENABLE_hw_can
    // so far can is assigned to the slave core
    #if defined(EMBOT_ENABLE_hw_can)
        #define EMBOT_ENABLE_hw_can_one
    #endif  


#else
    #error this is the bsp config of STM32HAL_BOARD_AMCMJ1_EXAMPLES_YRI ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


