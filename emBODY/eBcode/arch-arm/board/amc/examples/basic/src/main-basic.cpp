
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "stm32hal.h"
#include "bsp_basic.h"


// stm32hal requires two functions: one which inits and one that gets the 1 ms time.
static void stm32hal_tick1msecinit();
static uint32_t stm32hal_tick1msecget();
static const stm32hal_config_t systickcfg = { stm32hal_tick1msecinit, stm32hal_tick1msecget };

// --------------------------------------------------------------------------------------------------------------------

#if defined(OFFSET384K)

static void relocatevectortable(std::uint32_t offset)
{
    SCB->VTOR = FLASH_BASE | (offset & (uint32_t)0x1FFFFF80);        
}

extern "C" {    
    void _platform_post_lib_init(void)
    {
        relocatevectortable(384*1024);        
    }      
}

#endif

int main(void)
{ 
    stm32hal_init(&systickcfg);
    
    HAL_Delay(500);
    
    bsp_basic_itm_puts("CM7: hello world");
    
    bsp_basic_led_init();
    bsp_basic_itm_puts("CM7: led initted");
        
    for(;;)
    {
        HAL_Delay(1000);
        bsp_basic_led_toggle();
        bsp_basic_itm_puts("CM7: led toggled @ 2 Hz");
    }    
}


// -------------------------------------------------------------------------------------------------------------------

// we need to provide stm32hal a timebase in units of 1 ms (as the hal driver by stm requires). 
// we can use what we want. typically it is used the systick timer which increments a volatile
// counter. the systick is initted @ 1 khz 

static volatile uint64_t s_1mstickcount = 0; // it must be volatile
constexpr uint32_t s_rate1khz = 1000;


#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
    s_1mstickcount++;
}
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

static void stm32hal_tick1msecinit()
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate1khz);
}

static uint32_t stm32hal_tick1msecget()
{
    return s_1mstickcount;
}


 
// this macro is seen through stm32hal.h
//#if defined(STM32HAL_removeWEAK_ETH)
//// others, required because they have been removed from stm32hal.h7.stm32h745disco.lib
//void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth) {}
//void HAL_ETH_PMTCallback(ETH_HandleTypeDef *heth) {}
//void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth) {}
//#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



