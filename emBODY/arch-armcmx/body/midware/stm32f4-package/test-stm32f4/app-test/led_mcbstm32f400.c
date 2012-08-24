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

#include "led_mcbstm32f400.h"

#include "cmsis_stm32f4.h"

#include "stm32f4.h"

#include "stdint.h"



#define USEINITSTM32STDLIB
#define USESETSTM32STDLIB

typedef struct
{
    GPIO_TypeDef *  port;
    uint8_t         pin;
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


extern void led_mcbstm32f400_init (void) 
{

#ifndef USEINITSTM32STDLIB
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
#else

    GPIO_InitTypeDef  GPIO_InitStructure;

 
//    GPIO_DeInit(GPIOG);
//    GPIO_DeInit(GPIOH);
//    GPIO_DeInit(GPIOI);

    /* Enable the GPIO_LED Clock */

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    //RCC->AHB1ENR |= (1UL << 6) | (1UL << 7) | (1UL << 8) ;

 
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


#endif
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/

extern void led_mcbstm32f400_on (led_mcbstm32f400_t led) 
{
#ifndef USESETSTM32STDLIB
    s_led_data[led].port->BSRRL = 1UL << s_led_data[led].pin;
#else
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].gpiopin, Bit_SET);
#endif
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
extern void led_mcbstm32f400_off (led_mcbstm32f400_t led) 
{
#ifndef USESETSTM32STDLIB
    s_led_data[led].port->BSRRH = 1UL << s_led_data[led].pin;
#else
    GPIO_WriteBit(s_led_data[led].port, s_led_data[led].gpiopin, Bit_RESET);
#endif
}



// ------------------------- static ---------------------------------------------------------------



static void LED_Init (void) {
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

static void LED_Config(void) {

//  RCC->APB2ENR |=  1 <<  6;                     /* Enable GPIOE clock          */
//  GPIOE->CRH    = 0x33333333;                   /* Configure the GPIO for LEDs */

#if 0 
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_DeInit(GPIOI);

    /* Enable the GPIO_LED Clock */
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); 
 
    GPIO_StructInit(&GPIO_InitStructure);
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
#else
    /* Enable clock for GPIOI                                  */
    RCC->AHB1ENR |= (1UL << 6) | (1UL << 7) | (1UL << 8) ;

    /* Configure Port I pin PI10:                                               */
    /* Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  */
    GPIOI->MODER   &= ~(3UL << 2*10);
    GPIOI->MODER   |=  (1UL << 2*10);
    GPIOI->OTYPER  &= ~(1UL <<   10);
    GPIOI->OSPEEDR &= ~(3UL << 2*10);
    GPIOI->OSPEEDR |=  (2UL << 2*10);
    GPIOI->PUPDR   &= ~(3UL << 2*10);
    GPIOI->PUPDR   |=  (1UL << 2*10);

#endif
}


/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
static void LED_On (uint32_t pin) {
    
    uint8_t val = 3;

#if 0
    GPIOI->BSRRL = 1UL << 10;                          /* Turn On  LED */
#else
    GPIO_WriteBit(GPIOI, GPIO_Pin_10, Bit_SET);
#endif
    val = GPIO_ReadOutputDataBit(GPIOI, GPIO_Pin_10);
    val = val;
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
static void LED_Off (uint32_t pin) {

uint8_t val = 3;
#if 0
    GPIOI->BSRRH = 1UL << 10;                          /* Turn On  LED */
#else
    GPIO_WriteBit(GPIOI, GPIO_Pin_10, Bit_RESET);
#endif
    val = GPIO_ReadOutputDataBit(GPIOI, GPIO_Pin_10);
    val = val;

}






