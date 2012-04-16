/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HAL_DEBUGPIN_H_
#define _HAL_DEBUGPIN__H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_debugPin.h
    @brief      This header file provide simple utilities to move pins on p13(molex) of EMS board.
		see schematics for more information.
    @author     valentina.gaggero@iit.it
    @date       16/02/2011
**/

/** @defgroup arm_hal_debugPin HAL DEBUG PIN

    Use this file for debug purpose only.
    When include this header file in your source file you should compile with -o3 optimization or with --forceinline
    compiler's option, otherwise yiu will get linker's error.
    
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stm32f10x_gpio.h"


// - public #define  --------------------------------------------------------------------------------------------------
#define DEBUG_PIN_ON        GPIOB->BSRR   |= (1<<9);
#define DEBUG_PIN_OFF       GPIOB->BRR    |= (1<<9);

#define DEBUG_PIN3_ON       GPIOE->BSRR   |= (1<<7);
#define DEBUG_PIN3_OFF      GPIOE->BRR    |= (1<<7);

#define DEBUG_PIN4_ON       GPIOE->BSRR   |= (1<<9);
#define DEBUG_PIN4_OFF      GPIOE->BRR    |= (1<<9);

#define DEBUG_PIN5_ON       GPIOE->BSRR   |= (1<<11);
#define DEBUG_PIN5_OFF      GPIOE->BRR    |= (1<<11);

#define DEBUG_PIN6_ON       GPIOE->BSRR   |= (1<<13);
#define DEBUG_PIN6_OFF      GPIOE->BRR    |= (1<<13);

#define DEBUG_PIN7_ON       GPIOE->BSRR   |= (1<<14);
#define DEBUG_PIN7_OFF      GPIOE->BRR    |= (1<<14);

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void hal_debugPin_init(void)
    @brief      initilaise Debug pins on P13.
    @warning   since this function is inline, bu user to compile with --forceinline option
 **/
__inline void hal_debugPin_init(void)
{
    /* for pin on P13(molex)*/
    RCC->APB2ENR |= 0x00000070;

    /*pin3: TIM1_ETR (PE7)like gpout, other like input*/
    GPIOE->CRL &= 0x0FFFFFFF;
    GPIOE->CRL |= 0x30000000;
    //pc6 and pc2
    GPIOC->CRL &= 0xF0FFF0FF;
    GPIOC->CRL |= 0x08000800;

    /*pin4: TIM1_CH1 (PE9)like gpout, other like input*/
    GPIOE->CRH &= 0xFFFFFF0F;
    GPIOE->CRH |= 0x00000030;
    //PC7
    GPIOC->CRL &= 0x0FFFFFFF;
    GPIOC->CRL |= 0x80000000;
    //PD15
    GPIOD->CRH &= 0x0FFFFFFF;
    GPIOD->CRH |= 0x80000000;

    /*pin5: TIM1_CH2 (PE11)like gpout, other like input*/
    GPIOE->CRH &= 0xFFFF0FFF;
    GPIOE->CRH |= 0x000030000;
    //PC8
    GPIOC->CRH &= 0xFFFFFFF0;
    GPIOC->CRH |= 0x00000008;
    //PC4
    GPIOC->CRL &= 0xFFF0FFFF;
    GPIOC->CRL |= 0x00030000;
    //PD14
    GPIOD->CRH &= 0xF0FFFFFF;
    GPIOD->CRH |= 0x08000000;

    /*pin6: TIM1_CH3 (PE13)like gpout, other like input*/
    GPIOE->CRH &= 0xFF0FFFFF;
    GPIOE->CRH |= 0x00300000;
    //PC5
    GPIOC->CRL &= 0xFF0FFFFF;
    GPIOC->CRL |= 0x00300000;
    //PD13
    GPIOD->CRH &= 0xFF0FFFFF;
    GPIOD->CRH |= 0x00800000;

    /*pin7: TIM1_CH4 (PE14)like gpout, other like input*/
    GPIOE->CRH &= 0xF0FFFFFF;
    GPIOE->CRH |= 0x03000000;
    //PD12
    GPIOD->CRH &= 0xFFF0FFFF;
    GPIOD->CRH |= 0x00080000;
}
 
 
 /** @}            
    end of group arm_hal_debugPin
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



