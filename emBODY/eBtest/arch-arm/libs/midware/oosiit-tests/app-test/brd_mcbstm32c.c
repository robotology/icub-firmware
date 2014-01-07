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

#include "brd_mcbstm32x.h"

#include "cmsis_stm32f1.h"

#include "stm32f1.h"

#include "stdint.h"

typedef struct
{
    GPIO_TypeDef *  port;
    uint16_t        pin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOE, GPIO_Pin_8},
    {GPIOE, GPIO_Pin_9},
    {GPIOE, GPIO_Pin_10},
    {GPIOE, GPIO_Pin_11},
    {GPIOE, GPIO_Pin_12},
    {GPIOE, GPIO_Pin_13},
    {GPIOE, GPIO_Pin_14},
    {GPIOE, GPIO_Pin_15}
};


extern void brd_mcbstm32x_led_init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // enable clock for GPIOE
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    // configure PE8 to PE15 as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | 
                                    GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                    GPIO_Pin_14 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


extern void brd_mcbstm32x_led_on(brd_mcbstm32x_led_t led)
{
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_SET);
    //s_led_data[led].port->BSRRL = 1UL << s_led_data[led].pin;
}


extern void brd_mcbstm32x_led_off(brd_mcbstm32x_led_t led) 
{
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_RESET);
    //s_led_data[led].port->BSRRH = 1UL << s_led_data[led].pin;
}




