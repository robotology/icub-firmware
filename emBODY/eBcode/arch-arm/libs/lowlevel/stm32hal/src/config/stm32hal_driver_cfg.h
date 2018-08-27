

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_DRIVER_CFG_H_
#define _STM32HAL_DRIVER_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

// path in this file is referred to stm32hal/api

#include "stm32hal_define.h" 


#if     defined(STM32HAL_BOARD_NUCLEO64)
    
    #include "../src/config/stm32hal_driver_cfg_of_nucleo64.h" 
    
#elif   defined(STM32HAL_BOARD_MTB4)
    
    #include "../src/config/stm32hal_driver_cfg_of_mtb4.h"   
    // we also need some ll includes contained in the driver section
    #include "../src/driver/stm32l4-v172/inc/stm32l4xx_ll_gpio.h"
    
#elif   defined(STM32HAL_BOARD_STRAIN2)
    
    #include "../src/config/stm32hal_driver_cfg_of_strain2.h"   
    // we also need some ll includes contained in the driver section
    #include "../src/driver/stm32l4-v172/inc/stm32l4xx_ll_gpio.h"
    
#else
    
        #error pls define a STM32HAL_BOARD_${BRD}
    
#endif


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

