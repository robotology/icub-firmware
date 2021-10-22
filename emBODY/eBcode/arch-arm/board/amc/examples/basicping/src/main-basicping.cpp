
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "stm32hal.h"


// stm32hal requires two functions: one which inits and one that gets the 1 ms time.
static void stm32hal_tick1msecinit();
static uint32_t stm32hal_tick1msecget();
static const stm32hal_config_t systickcfg = { stm32hal_tick1msecinit, stm32hal_tick1msecget };

// they use STM32 HAL 
static void led_init();
static void led_toggle();
int itm_puts(const char *str);

#include "ipal.h"
#include "ipal_cfg2.h"

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    stm32hal_init(&systickcfg);
    
    itm_puts("hello world");
    
    led_init();
    itm_puts("led initted");
    
#if defined(ICUBTECH_USE_IPAL)
    
    itm_puts("initting and starting IPAL");

    uint32_t *data04aligned = nullptr;
    uint32_t size04aligned = 0;
	ipal_base_memory_getsize2(&ipal_cfg2, &size04aligned);
    if(size04aligned > 0)
    {
        data04aligned = new uint32_t[size04aligned/4];
    }    
	ipal_base_initialise2(&ipal_cfg2, data04aligned);

	ipal_sys_start();
    
    volatile uint32_t prevtick = 0;
    const uint32_t tickperiodms = ipal_cfg2.system->sys_timetick / 1000;
    
    char msg[128] = {0};
    itm_puts("starting a forever loop where:");
    itm_puts("- ipal_sys_process_communication() is called at max rate");
    snprintf(msg, sizeof(msg), "- ipal_sys_timetick_increment() is called every %d ms", tickperiodms);
    itm_puts(msg);
    snprintf(msg, sizeof(msg), "- LED is toggled every %d ms", 20*tickperiodms);
    itm_puts(msg);
    
#endif    
        
    for(;;)
    {
        
#if defined(ICUBTECH_USE_IPAL)      
        ipal_sys_process_communication();
        volatile uint32_t now = HAL_GetTick();		 
		// Every 10 ms invokes the periodic ipal timer_tick function        
        if((now - prevtick) >= tickperiodms)
        {
            prevtick = now;
            ipal_sys_timetick_increment();
            
            // and i toggle the led every 200 ms
            static uint32_t tt = 0;
            if(0 == (++tt%20))
            {
                led_toggle();
            }
        }
#else  
        HAL_Delay(500);
        led_toggle();
        itm_puts("led toggled");        
#endif        
        
        
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

int itm_puts(const char* str) 
{    

    if(nullptr == str)
    {
        return(0);
    }


    std::uint32_t ch;
    int num = 0;
    while('\0' != (ch = *str))
    {
        ITM_SendChar(ch);
        str++;
        num++;
    }
     
    ITM_SendChar('\n');
    return(++num);    
}

// the led section. we use 

#if defined(STM32HAL_BOARD_AMC)    
    static GPIO_TypeDef * GPIOLED = GPIOH;
    static const uint16_t PinLED = GPIO_PIN_13;
    static void clockEnableLED() {  __HAL_RCC_GPIOH_CLK_ENABLE(); }
#endif

static void led_init()
{
    
#if defined(STM32HAL_BOARD_AMC) 
    // start clock
    clockEnableLED();
    
    // configure led
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PinLED;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOLED, &GPIO_InitStruct);
    
    // switch it on
    HAL_GPIO_WritePin(GPIOLED, PinLED, GPIO_PIN_SET); 
    
    // switch it off
    HAL_GPIO_WritePin(GPIOLED, PinLED, GPIO_PIN_RESET); 
#endif    
}

static void led_toggle()
{
#if defined(STM32HAL_BOARD_AMC)
    HAL_GPIO_TogglePin(GPIOLED, PinLED);  
#endif    
}
 
// // this macro is seen through stm32hal.h
//#if defined(STM32HAL_removeWEAK_ETH)
// others, required because they have been removed from stm32hal.h7.stm32h745disco.lib
//void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth) {}
//void HAL_ETH_PMTCallback(ETH_HandleTypeDef *heth) {}
//void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth) {}
//#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



