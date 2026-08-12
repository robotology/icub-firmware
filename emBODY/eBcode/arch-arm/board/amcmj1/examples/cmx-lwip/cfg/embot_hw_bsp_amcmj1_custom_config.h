
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_CUSTOM_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_CUSTOM_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"



        
    
    #define EMBOT_ENABLE_hw_bsp_specialize  

    #define EMBOT_ENABLE_hw_spi
    #if defined(EMBOT_ENABLE_hw_spi)
    #warning EMBOT_ENABLE_hw_spi_one is for encoder and must be defined
        #define EMBOT_ENABLE_hw_spi_two
        #define EMBOT_ENABLE_hw_spi_three
    #endif 

    #define EMBOT_ENABLE_hw_eth
    #define EMBOT_ENABLE_hw_eth_LWIP

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


