

#define STM32HAL_EXTERNAL_TICK_OSAL


#include "stm32hal.h"

#if     defined(STM32HAL_EXTERNAL_TICK_NONE)    

#elif   defined(STM32HAL_EXTERNAL_TICK_OSAL)
    #include "osal.h"
#elif   defined(STM32HAL_EXTERNAL_TICK_SYSTICK)

#else
    #error: choose a STM32HAL_EXTERNAL_TICK_* mode 
#endif    



#if     defined(STM32HAL_EXTERNAL_TICK_SYSTICK)

static uint64_t s_1mstickcount = 0;

void SysTick_Handler(void)
{
    s_1mstickcount++;
}

#endif


extern uint32_t stm32hal_external_ticks1ms_get(void)
{
#if     defined(STM32HAL_EXTERNAL_TICK_NONE) 
    #warning STM32HAL_EXTERNAL_TICK_NONE is defined: some funtions of stm32hal which use HAL_GetTick() to get delay will not work    
    return(0);  
#elif   defined(STM32HAL_EXTERNAL_TICK_OSAL)
    // osal.lib must be included and osal must be started.
    osal_abstime_t t = osal_system_ticks_abstime_get() / 1000; // now t is expressed in millisec
    return (uint32_t)t;
#elif   defined(STM32HAL_EXTERNAL_TICK_SYSTICK)
    // the systick must be started at 1 ms (or multiple/submultiple of n) and its handler must increment a uint32_t variable
    // in here we return the variable (or its multiplication/division with n)    
    return (uint32_t)s_1mstickcount;
#else
    #error: choose a STM32HAL_EXTERNAL_TICK_* mode 
#endif    

}

    
    

