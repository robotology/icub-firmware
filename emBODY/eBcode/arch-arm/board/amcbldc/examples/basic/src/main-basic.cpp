
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
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
static int itm_puts(const char *str);

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    stm32hal_init(&systickcfg);
    
    itm_puts("hello world");
    
    led_init();
    itm_puts("led initted");
        
    for(;;)
    {
        HAL_Delay(500);
        led_toggle();
        itm_puts("led toggled");
    }  
}


// -------------------------------------------------------------------------------------------------------------------

// we need to provide stm32hal a timebase in units of 1 ms (as the hal driver by stm requires). 
// we can use what we want. typically it is used the systick timer which increments a volatile
// counter. the systick is initted @ 1 khz 

static volatile uint64_t s_1mstickcount = 0; // it must be volatile
constexpr uint32_t s_rate1khz = 1000;

void SysTick_Handler(void)
{
    s_1mstickcount++;
}

static void stm32hal_tick1msecinit()
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate1khz);
}

static uint32_t stm32hal_tick1msecget()
{
    return s_1mstickcount;
}

// the led section. we use PB0

#if defined(STM32HAL_BOARD_AMCBLDC)

    #warning we are using board amcbldc, led is on pb2
    // led1 is on PG9
    static GPIO_TypeDef * GPIOLED = GPIOB;
    static const uint16_t PinLED = GPIO_PIN_2;
    static void clockEnableLED() {  __HAL_RCC_GPIOB_CLK_ENABLE(); }

#endif

static void led_init()
{
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
}

static void led_toggle()
{
    HAL_GPIO_TogglePin(GPIOLED, PinLED);    
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


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



