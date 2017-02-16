

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_CMSIS_H_
#define _STM32HAL_CMSIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32hal_define.h"


#if defined(STM32L476xx)

#include "../src/stm32l4/stm32l4xx.h"
#include "../src/stm32l4/system_stm32l4xx.h"

#include "../src/stm32l4/arm_common_tables.h"
#include "../src/stm32l4/arm_const_structs.h"
#include "../src/stm32l4/arm_math.h"
#include "../src/stm32l4/cmsis_armcc.h"
//#include "../src/stm32l4/cmsis_armcc_V6.h"
//#include "../src/stm32l4/cmsis_gcc.h"
//#include "../src/stm32l4/core_cm0.h"
//#include "../src/stm32l4/core_cm0plus.h"
//#include "../src/stm32l4/core_cm3.h"
#include "../src/stm32l4/core_cm4.h"
//#include "../src/stm32l4/core_cm7.h"
#include "../src/stm32l4/core_cmFunc.h"
#include "../src/stm32l4/core_cmInstr.h"
#include "../src/stm32l4/core_cmSimd.h"
//#include "../src/stm32l4/core_sc000.h"
//#include "../src/stm32l4/core_sc300.h"

#else
    #error STM32HAL: the MPU is undefined (e.g., STM32L476xx) 
#endif



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard
