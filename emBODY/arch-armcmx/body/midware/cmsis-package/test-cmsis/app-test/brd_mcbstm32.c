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


#include "stdint.h"

typedef struct
{
   GPIO_TypeDef*   port;
   uint16_t        pin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOB, 8},
    {GPIOB, 9},
    {GPIOB, 10},
    {GPIOB, 11},
    {GPIOB, 12},
    {GPIOB, 13},
    {GPIOB, 14},
    {GPIOB, 15}
};


extern void brd_mcbstm32x_led_init(void) 
{
    RCC->APB2ENR |= (1UL << 3);                   // enable GPIOB clock         
    GPIOB->CRH    = 0x33333333;                   // configure GPIOB for use as LEDs 
}


extern void brd_mcbstm32x_led_on(brd_mcbstm32x_led_t led)
{
     s_led_data[led].port->BSRR = 1UL << s_led_data[led].pin;
    //GPIOB->BSRR = 1UL << s_led_data[led].pin;
}


extern void brd_mcbstm32x_led_off(brd_mcbstm32x_led_t led) 
{
    s_led_data[led].port->BRR = 1UL << s_led_data[led].pin;
    //GPIOB->BRR = 1UL << s_led_data[led].pin;
}




