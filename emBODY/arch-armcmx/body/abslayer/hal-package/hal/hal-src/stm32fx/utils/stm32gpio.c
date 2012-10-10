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


/* @file       stm32gpio.c
	@brief      This file implements init / deinit of i2c using stm32 apis.
	@author     marco.accame@iit.it
    @date       09/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
//#include "hal_brdcfg_modules.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_stm32xx_include.h"

//#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "stm32gpio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "stm32gpio_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(USE_STM32F1)

const uint8_t stm32gpio_hid_maxports                = 7;


const uint32_t stm32gpio_hid_thegpioclocks[]        = { RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, 
                                                        RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOE, RCC_APB2Periph_GPIOF, 
                                                        RCC_APB2Periph_GPIOG };  
GPIO_TypeDef* const stm32gpio_hid_thegpioports[]    = { GPIOA, GPIOB, GPIOC, 
                                                        GPIOD, GPIOE, GPIOF, 
                                                        GPIOG };  
const uint8_t  stm32gpio_hid_thepinnums[]           = { 0,  1,  2,  3,
                                                        4,  5,  6,  7,
                                                        8,  9,  10, 11,
                                                        12, 13, 14, 15 }; 
const uint16_t  stm32gpio_hid_thepins[]             = { GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
                                                        GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
                                                        GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
                                                        GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15 }; 
                                                                
 

#elif   defined(USE_STM32F4)

const uint8_t s_stm32gpio_hid_maxports              = 9;

const uint32_t stm32gpio_hid_thegpioclocks[]        = { RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC, 
                                                        RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF, 
                                                        RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOI };  
GPIO_TypeDef* const stm32gpio_hid_thegpioports[]    = { GPIOA, GPIOB, GPIOC, 
                                                        GPIOD, GPIOE, GPIOF, 
                                                        GPIOG, GPIOH, GPIOI };  
const uint8_t  stm32gpio_hid_thepinnums[]           = { GPIO_PinSource0,  GPIO_PinSource1,  GPIO_PinSource2,  GPIO_PinSource3,
                                                        GPIO_PinSource4,  GPIO_PinSource5,  GPIO_PinSource6,  GPIO_PinSource7,
                                                        GPIO_PinSource8,  GPIO_PinSource9,  GPIO_PinSource10, GPIO_PinSource11,
                                                        GPIO_PinSource12, GPIO_PinSource13, GPIO_PinSource14, GPIO_PinSource15 }; 
const uint16_t  stm32gpio_hid_thepins[]             = { GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
                                                        GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
                                                        GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
                                                        GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15 }; 
                                                                

#endif


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

//extern uint32_t stm32gpio_hid_myfunction(void)
//{
//}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

