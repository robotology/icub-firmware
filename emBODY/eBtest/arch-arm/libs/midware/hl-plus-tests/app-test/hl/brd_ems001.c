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

#include "board.h"


#include "hl_core.h"

#include "stdint.h"

#include "stdlib.h"

typedef struct
{
    GPIO_TypeDef *  port;
    uint16_t        pin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOE, GPIO_Pin_8},
    {GPIOE, GPIO_Pin_10},
    {GPIOE, GPIO_Pin_12},
    {GPIOE, GPIO_Pin_15},
    {GPIOD, GPIO_Pin_11},
    {GPIOB, GPIO_Pin_7},
    {NULL, 0},
    {NULL, 0}
};


extern void board_led_init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // enable clock for GPIOE
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    // configure PE8 to PE15 as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8  | GPIO_Pin_10 | 
                                    GPIO_Pin_12 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    
    // enable clock for GPIOD
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // configure as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_11 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);    
    
    
    // enable clock for GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // configure as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    board_led_off(board_led_0);
    board_led_off(board_led_1);
    board_led_off(board_led_2);
    board_led_off(board_led_3);
    board_led_off(board_led_4);
    board_led_off(board_led_5);
}


extern void board_led_on(board_led_t led)
{
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_RESET);
}


extern void board_led_off(board_led_t led) 
{
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_SET);
}




