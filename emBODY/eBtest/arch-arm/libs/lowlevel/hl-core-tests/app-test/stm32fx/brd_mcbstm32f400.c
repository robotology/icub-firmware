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



typedef struct
{
    GPIO_TypeDef *  port;
    uint16_t        pin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOH, GPIO_Pin_3},
    {GPIOH, GPIO_Pin_6},
    {GPIOH, GPIO_Pin_7},
    {GPIOI, GPIO_Pin_10},
    {GPIOG, GPIO_Pin_6},
    {GPIOG, GPIO_Pin_7},
    {GPIOG, GPIO_Pin_8},
    {GPIOH, GPIO_Pin_2}
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
   
    // enable clock for GPIOH
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    
   

    // configure  as output           
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_2  | GPIO_Pin_3 | 
                                    GPIO_Pin_6 | GPIO_Pin_7 ;

    GPIO_Init(GPIOH, &GPIO_InitStructure);
    
    
    // enable clock for GPIOI
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    // configure as outputs               
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_10 ;

    GPIO_Init(GPIOI, &GPIO_InitStructure);    
    
    
    // enable clock for GPIOG
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

    // configure as outputs push-pull, max speed 50 MHz              
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;

    GPIO_Init(GPIOG, &GPIO_InitStructure);    


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
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_SET);
}


extern void board_led_off(board_led_t led) 
{
    if(NULL == s_led_data[led].port)
    {
        return;
    }    
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].pin, Bit_RESET);
}




