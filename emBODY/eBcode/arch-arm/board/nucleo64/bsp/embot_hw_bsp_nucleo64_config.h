
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_HW_BSP_NUCLEO64_CONFIG_H_
#define EMBOT_HW_BSP_NUCLEO64_CONFIG_H_


#if   defined(STM32HAL_BOARD_NUCLEO64)
    
    #undef  EMBOT_ENABLE_hw_bsp_specialize    
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_button
    #define EMBOT_ENABLE_hw_flash
        
#else
    #error this is the bsp config of STM32HAL_BOARD_NUCLEO64 ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


