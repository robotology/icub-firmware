/**************************************************************************//**
 * @file     main.c
 * @brief    CMSIS Cortex-M3 Blinky example
 *           Blink a LED using CM3 SysTick
 * @version  V1.03
 * @date     24. September 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

 /*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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


#if 1
    #include "stm32f1.h"
#else 
    #include "stm32f10x.h"
    #include "stm32f10x_rcc.h"
    #include "stm32f10x_gpio.h"
#endif

#include "flash.h"


volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*------------------------------------------------------------------------------
  configer LED pins
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Config(void) {

//  RCC->APB2ENR |=  1 <<  6;                     /* Enable GPIOE clock          */
//  GPIOE->CRH    = 0x33333333;                   /* Configure the GPIO for LEDs */

    uint8_t pin = 8;
    GPIO_InitTypeDef  GPIO_InitStructure;

        /* Enable the GPIO_LED Clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);    
        /* Configure the GPIO_LED pin */
        GPIO_InitStructure.GPIO_Pin = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_On (uint32_t pin) {

//  GPIOE->BSRR = (led);                          /* Turn On  LED */
  if(8 != pin)
  {
    return;
  }
  GPIO_WriteBit(GPIOE, (uint16_t)(0x0001 << pin), Bit_SET);
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Off (uint32_t pin) {

//  GPIOE->BRR  = (led);                          /* Turn Off LED */
  if(8 != pin)
  {
    return;
  }
  GPIO_WriteBit(GPIOE, (uint16_t)(0x0001 << pin), Bit_RESET);
}


void timer_start(uint16_t microsecs);
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

    uint32_t flashaddr = 0x08010000;
    

  if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 1 msec interrupts  */
    while (1);                                  /* Capture error */
  }
  
  LED_Config();  
  
  timer_start(10*1000); // 10 ms.
  
  hal_FLASH_Unlock();


  hal_FLASH_ErasePage(flashaddr);

  hal_FLASH_WriteU16(flashaddr, 0x1234);

  hal_FLASH_WriteU16(flashaddr+1024, 0x1234);
                            


 
  while(1) {
    LED_On (8);                             /* Turn on the LED. */
    Delay (300);                                /* delay  300 Msec */
    LED_Off (8);                            /* Turn off the LED. */
    Delay (700);                                /* delay  700 Msec */
  }
  
}


void timer_start(uint16_t microsecs)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* TIM7 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);


    /* Enable the TIM7 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/

    // set the counter to expiry at multiples of one microsec.
    // the clock is divided by 1, we work at 72MHz, thus use a prescaler of 71 to be at 1 MHz (1 microsec).
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
    TIM_TimeBaseStructure.TIM_Period = 1*microsecs;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM7, &TIM_TimeBaseStructure );
    // important: stop the timer 7 in debug mode
    DBGMCU_Config(DBGMCU_TIM7_STOP, ENABLE);
    TIM_Cmd( TIM7, ENABLE );

    // enable isr
    TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );
}


void TIM7_IRQHandler(void)
{
    static volatile uint32_t delta = 0;
    static volatile uint32_t prev = 0;

    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

        delta = msTicks - prev;
        prev = msTicks;

        delta = delta;
    }

}

