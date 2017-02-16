

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_DEFINE_H_
#define _STM32HAL_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif


#if     defined(STM32HAL_BOARD_NUCLEO64)

    // use hal
    #if !defined(USE_HAL_DRIVER)
    #define USE_HAL_DRIVER
    #endif

    // it has a STM32L476xx mpu
    #if !defined(STM32L476xx)
    #define STM32L476xx
    #endif
    
    #if !defined(ARM_MATH_CM4)
    #define ARM_MATH_CM4
    #endif
      
    
#elif   defined(STM32HAL_BOARD_STRAIN2)

#else
        #error STM32HAL: you must define a STM32HAL_BOARD_something
#endif


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard
