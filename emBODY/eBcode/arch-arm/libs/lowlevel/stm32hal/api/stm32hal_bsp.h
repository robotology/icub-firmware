

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_BSP_H_
#define _STM32HAL_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32hal_define.h"


#if defined(STM32HAL_BOARD_NUCLEO64)

#include "../src/board/nucleo64/bsp.h"
#include "../src/board/nucleo64/gpio.h"
#include "../src/board/nucleo64/main.h"
#include "../src/board/nucleo64/stm32l4xx_hal_conf_nucleo64.h"
#include "../src/board/nucleo64/usart.h"

#else
    #error STM32HAL: you must define a STM32HAL_BOARD_something
#endif



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard
