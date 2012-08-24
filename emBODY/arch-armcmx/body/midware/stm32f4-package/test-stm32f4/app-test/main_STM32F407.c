/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/




#include "stm32f4.h"


#include "led_mcbstm32f400.h"


static volatile uint32_t numofticks;                          

void SysTick_Handler(void) 
{
    numofticks++;                       
}


__inline static void delay(uint32_t ticks) 
{
    uint32_t curtick;

    curtick = numofticks;
    while((numofticks - curtick) < ticks);
}


void timer_start(uint32_t microsecs);


int main(void) 
{

    SystemInit();
    
    // tick is 1000 microseconds
    if (SysTick_Config(SystemCoreClock / 1000)) 
    { 
        while (1);                                  
    }
  
    
    led_mcbstm32f400_init();
  
    timer_start(500*1000); 
  
 
    while(1) 
    {
        led_mcbstm32f400_on (led_mcbstm32f400_0);                             
        led_mcbstm32f400_on (led_mcbstm32f400_1);
        led_mcbstm32f400_on (led_mcbstm32f400_2);
        led_mcbstm32f400_on (led_mcbstm32f400_3);
        led_mcbstm32f400_on (led_mcbstm32f400_4);    
        led_mcbstm32f400_on (led_mcbstm32f400_5);
        delay(300);                                
        led_mcbstm32f400_off (led_mcbstm32f400_0); 
        led_mcbstm32f400_off (led_mcbstm32f400_1);
        led_mcbstm32f400_off (led_mcbstm32f400_2);
        led_mcbstm32f400_off (led_mcbstm32f400_3);
        led_mcbstm32f400_off (led_mcbstm32f400_4);                           
        led_mcbstm32f400_off (led_mcbstm32f400_5); 
        delay(700);                                
  }
  
}


void timer_start(uint32_t microsecs)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);



    /* Enable the TIM7 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/

    // set the counter to expiry at multiples of one microsec.
    // the clock is divided by 1, we work at 120MHz, thus at freq 60mhz use a prescaler of 59 to be at 1 MHz (1 microsec).
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
    TIM_TimeBaseStructure.TIM_Period = microsecs/10;
    TIM_TimeBaseStructure.TIM_Prescaler = 600-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );
    // important: stop the timer 2 in debug mode
    DBGMCU_Config(DBGMCU_TIM2_STOP, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    // enable isr
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );
}


void TIM2_IRQHandler(void)
{
    static volatile uint32_t delta = 0;
    static volatile uint32_t prev = 0;

    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        delta = numofticks - prev;
        prev = numofticks;

        delta = delta;

        GPIO_ToggleBits(GPIOH, GPIO_Pin_3);
    }

}

