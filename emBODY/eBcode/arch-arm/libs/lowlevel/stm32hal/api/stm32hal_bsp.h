

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_BSP_H_
#define _STM32HAL_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32hal_define.h"


#if defined(STM32HAL_BOARD_NUCLEO64)

#include "../src/board/nucleo64/inc/bsp.h"
#include "../src/board/nucleo64/inc/gpio.h"
#include "../src/board/nucleo64/inc/main.h"
#include "../src/board/nucleo64/inc/stm32l4xx_hal_conf_nucleo64.h"
#include "../src/board/nucleo64/inc/usart.h"

#else
    #error STM32HAL: you must define a STM32HAL_BOARD_something
#endif



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard
