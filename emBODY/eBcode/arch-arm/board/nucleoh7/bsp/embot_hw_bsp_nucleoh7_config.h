
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_NUCLEOH7_CONFIG_H_
#define _EMBOT_HW_BSP_NUCLEOH7_CONFIG_H_


#if   defined(STM32HAL_BOARD_NUCLEOH7)

    
#if defined(STM32HAL_BOARD_NUCLEOH7_BASIC) 
    #undef  EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_tlv493d 
#else 
    #undef  EMBOT_ENABLE_hw_bsp_specialize    
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_i2c
    #define EMBOT_ENABLE_hw_bno055
    #define EMBOT_ENABLE_hw_ads122c04 
    #define EMBOT_ENABLE_hw_ad7147
#endif
        
#else
    #error this is the bsp config of STM32HAL_BOARD_NUCLEOH7 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


