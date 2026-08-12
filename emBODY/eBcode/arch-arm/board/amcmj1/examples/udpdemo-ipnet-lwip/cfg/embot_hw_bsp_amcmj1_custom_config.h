
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


#if !defined(EMBOT_HW_BSP__udpdemoipnet) || !defined(STM32HAL_BOARD_AMCMJ1)
    #error this bsp config is for amcmj1.udpdemoipnet for cm4 master
#endif
    
#if !defined(EMBOT_CORE_master)
    #error amcmj1.udpdemoipnet must be master
#endif



#if 0
the demo udpnet must:
- manage EEPROM 
  EMBOT_ENABLE_hw_eeprom, EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_three 
- manage ETH + its switch
  EMBOT_ENABLE_hw_eth, EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_two

- perfom actions on specialize()  
  EMBOT_ENABLE_hw_bsp_specialize
- ??? manage the UID if it is executed on the CM4, so redefine uniqueid() so that it is read from EEPROM

we may need CAN even if we prefer give the only CAN (for now) to appl.mot because the updater does not run together
  - CAN + 5V
    EMBOT_ENABLE_hw_can, EMBOT_ENABLE_hw_can_one, EMBOT_ENABLE_hw_can_5V
#endif

    #define EMBOT_ENABLE_hw_bsp_specialize

    
    #define EMBOT_ENABLE_hw_spi
    #if defined(EMBOT_ENABLE_hw_spi)
    #warning EMBOT_ENABLE_hw_spi_one is for encoder and must be defined
        #define EMBOT_ENABLE_hw_spi_two
        #define EMBOT_ENABLE_hw_spi_three
    #endif 
    
//    #define EMBOT_ENABLE_hw_eeprom  


//    #if defined(STM32HAL_CORE_CM4) 
//        #define EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid 
//    #endif // STM32HAL_CORE_CM4
    
    #define EMBOT_ENABLE_hw_eth
    #define EMBOT_ENABLE_hw_eth_LWIP

//    #define EMBOT_ENABLE_hw_can
//    #define EMBOT_ENABLE_hw_can_one
//    #define EMBOT_ENABLE_hw_can_5V  


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


