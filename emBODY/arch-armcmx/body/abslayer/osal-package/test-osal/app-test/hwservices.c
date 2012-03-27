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

#include "stm32f1.h"  



#define LED_A   GPIO_Pin_8
#define LED_B   GPIO_Pin_9
#define LED_C   GPIO_Pin_10
#define LED_D   GPIO_Pin_11
#define LED_CLK GPIO_Pin_12
          

GPIO_InitTypeDef  GPIO_InitStructure;


void SetupLED(void) 
{

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | 
                                 GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                 GPIO_Pin_14 | GPIO_Pin_15 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}

void LED_On(uint16_t led)
{
    GPIO_WriteBit(GPIOE, led, Bit_SET);
}

void LED_Off(uint16_t led)
{
    GPIO_WriteBit(GPIOE, led, Bit_RESET);
}

void LED_toggle(void)
{
    static BitAction a = Bit_RESET;

    GPIO_WriteBit(GPIOE, GPIO_Pin_14, a);

    a = (Bit_RESET == a) ? (Bit_SET) : (Bit_RESET);

}
