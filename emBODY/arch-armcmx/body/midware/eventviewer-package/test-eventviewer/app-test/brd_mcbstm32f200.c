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

#include "cmsis_stm32f2.h"


#include "stdint.h"

typedef struct
{
    GPIO_TypeDef*   port;
    uint16_t        pin;
    uint16_t        gpiopin;
} led_data_t;

static const led_data_t s_led_data[8] = 
{
    {GPIOH, 2, 2},
    {GPIOG, 8, 8},
    {GPIOG, 7, 7},
    {GPIOG, 6, 6},
    {GPIOI, 10, 10},
    {GPIOH, 7, 7},
    {GPIOH, 6, 6},
    {GPIOH, 3, 3}
};


extern void brd_mcbstm32x_led_init(void) 
{
    // Enable clock for GPIOG, GPIOH and GPIOI                                 
    RCC->AHB1ENR |= (1UL << 6) | (1UL << 7) | (1UL << 8) ;

    // Configure Port G pins PG6, PG7, PG8:                                     
    // Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  
    GPIOG->MODER   &= ~((3UL << 2*6) | (3UL << 2*7) | (3UL << 2*8));
    GPIOG->MODER   |=  ((1UL << 2*6) | (1UL << 2*7) | (1UL << 2*8));
    GPIOG->OTYPER  &= ~((1UL <<   6) | (1UL <<   7) | (1UL <<   8));
    GPIOG->OSPEEDR &= ~((3UL << 2*6) | (3UL << 2*7) | (3UL << 2*8));
    GPIOG->OSPEEDR |=  ((2UL << 2*6) | (2UL << 2*7) | (2UL << 2*8));
    GPIOG->PUPDR   &= ~((3UL << 2*6) | (3UL << 2*7) | (3UL << 2*8));
    GPIOG->PUPDR   |=  ((1UL << 2*6) | (1UL << 2*7) | (1UL << 2*8));    
  

    // Configure Port H: PH2, PH3, PH6, PH7                                     
    // Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  
    GPIOH->MODER   &= ~((3UL << 2*2) | (3UL << 2*3) | (3UL << 2*6) | (3UL << 2*7));
    GPIOH->MODER   |=  ((1UL << 2*2) | (1UL << 2*3) | (1UL << 2*6) | (1UL << 2*7));
    GPIOH->OTYPER  &= ~((1UL <<   2) | (1UL <<   3) | (1UL <<   6) | (1UL <<   7));
    GPIOH->OSPEEDR &= ~((3UL << 2*2) | (3UL << 2*3) | (3UL << 2*6) | (3UL << 2*7));
    GPIOH->OSPEEDR |=  ((2UL << 2*2) | (2UL << 2*3) | (2UL << 2*6) | (2UL << 2*7));
    GPIOH->PUPDR   &= ~((3UL << 2*2) | (3UL << 2*3) | (3UL << 2*6) | (3UL << 2*7));
    GPIOH->PUPDR   |=  ((1UL << 2*2) | (1UL << 2*3) | (1UL << 2*6) | (1UL << 2*7));

    // Configure Port I pin PI10:                                               
    // Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  
    GPIOI->MODER   &= ~(3UL << 2*10);
    GPIOI->MODER   |=  (1UL << 2*10);
    GPIOI->OTYPER  &= ~(1UL <<   10);
    GPIOI->OSPEEDR &= ~(3UL << 2*10);
    GPIOI->OSPEEDR |=  (2UL << 2*10);
    GPIOI->PUPDR   &= ~(3UL << 2*10);
    GPIOI->PUPDR   |=  (1UL << 2*10); 
}


extern void brd_mcbstm32x_led_on(brd_mcbstm32x_led_t led)
{
    //GPIOI->BSRRL = 1UL << 10; // pin 10 port I
    s_led_data[led].port->BSRRL = 1UL << s_led_data[led].pin;
}


extern void brd_mcbstm32x_led_off(brd_mcbstm32x_led_t led) 
{
    
     //GPIOI->BSRRH = 1UL << 10; // pin 10 port I
     s_led_data[led].port->BSRRH = 1UL << s_led_data[led].pin;
}




