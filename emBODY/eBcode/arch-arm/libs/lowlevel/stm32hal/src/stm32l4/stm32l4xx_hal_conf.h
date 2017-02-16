

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __STM32L4xx_HAL_CONF_H
#define __STM32L4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


#include "../../api/stm32hal_define.h"


#if     defined(STM32HAL_BOARD_NUCLEO64)

//    #include "../../../src/board/nucleo64/stm32l4xx_hal_conf_nucleo64.h"
    
     #include "../src/board/nucleo64/stm32l4xx_hal_conf_nucleo64.h" // parto da api

#else
        #error pls define a BOARD
#endif


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

