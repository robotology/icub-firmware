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
#include "cmsis_stm32f2.h"

#include "led_mcbstm32f200.h"


static const uint32_t usecintick = 10000;

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
__INLINE static void Delay (uint32_t milli) {
  uint32_t curTicks;
  uint32_t dlyTicks = milli * usecintick / 1000;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}




/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
    

  if (SysTick_Config(SystemCoreClock / usecintick)) {   /* Setup SysTick Timer   */
    while (1);                                          /* Capture error */
  }

  
  led_mcbstm32f200_init();                             
 
  while(1) {
    led_mcbstm32f200_on (0);                             /* Turn on the LED. */
    Delay (300);                                /* delay  300 Msec */
    led_mcbstm32f200_off (0);                            /* Turn off the LED. */
    Delay (700);                                /* delay  700 Msec */
  }
  
}

