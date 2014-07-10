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


#include "stm32f4.h"

#include "stdint.h"
#include "stdlib.h"


#warning --> FOR MC4PLUS REMEMBER to define: -DHL_CFG_MPUSPEED_HSEBYPASS -DPLL_N=336 -DPLL_M=50 -DPLL_P=2 -DPLL_Q=7 -DHSE_VALUE=50000000 -DHSI_VALUE=16000000

#warning --> must map the gpios of the LEDs

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
    GPIO_InitTypeDef GPIO_InitStructure = 
    {
            .GPIO_Pin   = 0,
            .GPIO_Mode  = GPIO_Mode_OUT,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd  = GPIO_PuPd_UP         
    };
   
    // enable clock for GPIOE
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
   

    // configure PE8, 10, 12 PE15 as output           
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8  | GPIO_Pin_10 | 
                                    GPIO_Pin_12 | GPIO_Pin_15 ;

    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    
    // enable clock for GPIOD
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // configure as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_11 ;

    GPIO_Init(GPIOD, &GPIO_InitStructure);    
    
    
    // enable clock for GPIOB
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // configure as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 ;

    GPIO_Init(GPIOB, &GPIO_InitStructure);    


    board_led_off(board_led_0);
    board_led_off(board_led_1);
    board_led_off(board_led_2);
    board_led_off(board_led_3);
    board_led_off(board_led_4);
    board_led_off(board_led_5);
    board_led_off(board_led_6);
    board_led_off(board_led_7);
}


extern void board_led_on(board_led_t led)
{
    if(NULL == s_led_data[led].port)
    {
        return;
    }
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_RESET);
}


extern void board_led_off(board_led_t led) 
{
    if(NULL == s_led_data[led].port)
    {
        return;
    }    
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_SET);
}




