
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_APP_YRI_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_APP_YRI_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if !defined(EMBOT_HW_BSP__appl_YRI) || !defined(STM32HAL_BOARD_AMCMJ1)
    #error this bsp config is for amcmj1.appl.yri for either cm7 or cm4 slave
#endif
    
#if !defined(EMBOT_CORE_master)
    #error amcmj1.appl.yri must be master
#endif

#if 0
the appl.yri must:
- manage EEPROM 
  EMBOT_ENABLE_hw_eeprom, EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_three 
- manage ETH + its switch
  EMBOT_ENABLE_hw_eth, EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_two
- manage HW timer for the control loop
  EMBOT_ENABLE_hw_timer
- manage ICC 
  EMBOT_ENABLE_hw_mtx, EMBOT_ENABLE_hw_icc_sig, EMBOT_ENABLE_hw_icc_mem, EMBOT_ENABLE_hw_icc_ltr
- manage encoders
  EMBOT_ENABLE_hw_encoder, EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_one
- perfom actions on specialize()  
  EMBOT_ENABLE_hw_bsp_specialize
- manage the UID if it is executed on the CM4, so redefine both setuniqueid() and uniqueid().
  NOTE: inside embot::app::eth::theApplication::Impl::synchUID() the UID is asked via ICC to the CM7 core 
  and then written into EEPROM. any later readin id done by reading the EEPROM
  
it does not need CAN because we prefer give the only CAN (for now) to appl.mot
BUT, in case we want it for test of FT service or for future needs we must ahve:
  - CAN + 5V
    EMBOT_ENABLE_hw_can, EMBOT_ENABLE_hw_can_one, EMBOT_ENABLE_hw_can_5V
#endif

    
    #define EMBOT_ENABLE_hw_bsp_specialize


    #define EMBOT_ENABLE_hw_mtx
    #define EMBOT_ENABLE_hw_icc_sig
    #define EMBOT_ENABLE_hw_icc_mem
    #define EMBOT_ENABLE_hw_icc_ltr

    #define EMBOT_ENABLE_hw_timer
    
    #define EMBOT_ENABLE_hw_spi
    #if defined(EMBOT_ENABLE_hw_spi)
    #warning EMBOT_ENABLE_hw_spi_one is for encoder and must be defined
//        #define EMBOT_ENABLE_hw_spi_one 
        #define EMBOT_ENABLE_hw_spi_two
        #define EMBOT_ENABLE_hw_spi_three
    #endif 
    
    #define EMBOT_ENABLE_hw_eeprom  


    #if defined(STM32HAL_CORE_CM4) 
        #define EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid 
        #define EMBOT_REDEFINE_hw_bsp_DRIVER_setuniqueid
    #endif // STM32HAL_CORE_CM4
    
    #define EMBOT_ENABLE_hw_eth

    //#define EMBOT_ENABLE_hw_can
    //#define EMBOT_ENABLE_hw_can_one
    //#define EMBOT_ENABLE_hw_can_5V    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


