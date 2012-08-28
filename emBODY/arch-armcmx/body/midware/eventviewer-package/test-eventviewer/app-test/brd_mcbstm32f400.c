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

#include "cmsis_stm32f4.h"

#include "stm32f4.h"

#include "stdint.h"

typedef struct
{
    GPIO_TypeDef *  port;
    uint16_t        pin;
    uint16_t        gpiopin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOH, 2, GPIO_Pin_2},
    {GPIOG, 8, GPIO_Pin_8},
    {GPIOG, 7, GPIO_Pin_7},
    {GPIOG, 6, GPIO_Pin_6},
    {GPIOI, 10, GPIO_Pin_10},
    {GPIOH, 7, GPIO_Pin_7},
    {GPIOH, 6, GPIO_Pin_6},
    {GPIOH, 3, GPIO_Pin_3}
};


extern void brd_mcbstm32x_led_init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // enable clock for various GPIOx
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    
    GPIO_StructInit(&GPIO_InitStructure);
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOH, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

}


extern void brd_mcbstm32x_led_on(brd_mcbstm32x_led_t led)
{
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].gpiopin, Bit_SET);
}


extern void brd_mcbstm32x_led_off(brd_mcbstm32x_led_t led) 
{
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].gpiopin, Bit_RESET);
}




