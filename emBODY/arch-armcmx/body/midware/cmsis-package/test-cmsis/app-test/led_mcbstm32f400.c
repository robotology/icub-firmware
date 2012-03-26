
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

#include "led_mcbstm32f400.h"

#include "cmsis_stm32f4.h"

#include "stdint.h"



extern void led_mcbstm32f400_init (void) 
{
  /* Enable clock for GPIOG, GPIOH and GPIOI                                  */
  RCC->AHB1ENR |= (1UL << 6) | (1UL << 7) | (1UL << 8) ;

  /* Configure Port G pins PG6, PG7, PG8:                                     */
  /* Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  */
  GPIOG->MODER   &= ~((3UL << 2*6) | (3UL << 2*7) | (3UL << 2*8));
  GPIOG->MODER   |=  ((1UL << 2*6) | (1UL << 2*7) | (1UL << 2*8));
  GPIOG->OTYPER  &= ~((1UL <<   6) | (1UL <<   7) | (1UL <<   8));
  GPIOG->OSPEEDR &= ~((3UL << 2*6) | (3UL << 2*7) | (3UL << 2*8));
  GPIOG->OSPEEDR |=  ((2UL << 2*6) | (2UL << 2*7) | (2UL << 2*8));
  GPIOG->PUPDR   &= ~((3UL << 2*6) | (3UL << 2*7) | (3UL << 2*8));
  GPIOG->PUPDR   |=  ((1UL << 2*6) | (1UL << 2*7) | (1UL << 2*8));

  /* Configure Port H: PH2, PH3, PH6, PH7                                     */
  /* Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  */
  GPIOH->MODER   &= ~((3UL << 2*2) | (3UL << 2*3) | (3UL << 2*6) | (3UL << 2*7));
  GPIOH->MODER   |=  ((1UL << 2*2) | (1UL << 2*3) | (1UL << 2*6) | (1UL << 2*7));
  GPIOH->OTYPER  &= ~((1UL <<   2) | (1UL <<   3) | (1UL <<   6) | (1UL <<   7));
  GPIOH->OSPEEDR &= ~((3UL << 2*2) | (3UL << 2*3) | (3UL << 2*6) | (3UL << 2*7));
  GPIOH->OSPEEDR |=  ((2UL << 2*2) | (2UL << 2*3) | (2UL << 2*6) | (2UL << 2*7));
  GPIOH->PUPDR   &= ~((3UL << 2*2) | (3UL << 2*3) | (3UL << 2*6) | (3UL << 2*7));
  GPIOH->PUPDR   |=  ((1UL << 2*2) | (1UL << 2*3) | (1UL << 2*6) | (1UL << 2*7));

  /* Configure Port I pin PI10:                                               */
  /* Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  */
  GPIOI->MODER   &= ~(3UL << 2*10);
  GPIOI->MODER   |=  (1UL << 2*10);
  GPIOI->OTYPER  &= ~(1UL <<   10);
  GPIOI->OSPEEDR &= ~(3UL << 2*10);
  GPIOI->OSPEEDR |=  (2UL << 2*10);
  GPIOI->PUPDR   &= ~(3UL << 2*10);
  GPIOI->PUPDR   |=  (1UL << 2*10);
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/

extern void led_mcbstm32f400_on (uint32_t pin) 
{
    GPIOI->BSRRL = 1UL << 10;                          /* Turn On  LED PI10*/
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
extern void led_mcbstm32f400_off (uint32_t pin) 
{
    GPIOI->BSRRH = 1UL << 10;                          /* Turn On  LED PI10*/
}


