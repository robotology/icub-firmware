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

#include "cmsis_stm32f1.h"


#include "stdint.h"
#include "stdlib.h"

typedef struct
{
   GPIO_TypeDef*   port;
   uint16_t        pin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOE, 8},
    {GPIOE, 10},
    {GPIOE, 12},
    {GPIOE, 15},
    {GPIOD, 11},
    {GPIOB, 7},
    {NULL, 0},
    {NULL, 0}
};


extern void board_led_init(void) 
{
    RCC->APB2ENR |=  1 <<  6;                      // enable GPIOE clock   
    GPIOE->CRH   &= ~0xf00f0f0f;                   // configure GPIOE pins 8, 10, 12, 15 for use as LEDs     
    GPIOE->CRH   |=  0x30030303;                   // configure GPIOE pins 8, 10, 12, 15 for use as LEDs 
    
    RCC->APB2ENR |=  1 <<  5;                      // enable GPIOD clock      
    GPIOD->CRH   &= ~0x0000f000;                   // configure GPIOD pins 11 for use as LEDs     
    GPIOD->CRH   |=  0x00003000;                   // configure GPIOD pins 11 for use as LEDs 
    GPIOD->CRL   &= ~0xf0000000;                   // configure GPIOD pin 7 for use as LEDs     
    GPIOD->CRL   |=  0x30000000;                   // configure GPIOD pin 7 for use as LEDs     
    
    RCC->APB2ENR |=  1 <<  3;                      // enable GPIOB clock      
    GPIOB->CRL   &= ~0xf0000000;                   // configure GPIOB pin 7 for use as LEDs     
    GPIOB->CRL   |=  0x30000000;                   // configure GPIOB pin 7 for use as LEDs    

    board_led_off(board_led_0);    
    board_led_off(board_led_1);
    board_led_off(board_led_2);
    board_led_off(board_led_3);
    board_led_off(board_led_4);
    board_led_off(board_led_5);
    board_led_off(board_led_6);
    board_led_off(board_led_7);
}


extern void board_led_off(board_led_t led)
{
    if(NULL == s_led_data[led].port)
    {
        return;
    }
    s_led_data[led].port->BSRR = 1UL << s_led_data[led].pin;
    //GPIOE->BSRR = 1UL << s_led_data[led].pin;
}


extern void board_led_on(board_led_t led) 
{
    if(NULL == s_led_data[led].port)
    {
        return;
    }    
    s_led_data[led].port->BRR = 1UL << s_led_data[led].pin;
    //GPIOE->BRR = 1UL << s_led_data[led].pin;
}




