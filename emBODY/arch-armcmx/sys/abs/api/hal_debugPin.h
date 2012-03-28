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
    @brief      This header file provide simple utilities to move the pin B9.
		Currently SDL group has only one ems board with a wire connected on B9.
    @author     valentina.gaggero@iit.it
    @date       16/02/2011
**/

/** @defgroup arm_hal_debugPin HAL DEBUG PIN

    The HAL DEBUG PIN ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stm32f10x_gpio.h"


// - public #define  --------------------------------------------------------------------------------------------------
#define DEBUG_PIN_ON        GPIOB->BSRR   |= (1<<9);
#define DEBUG_PIN_OFF       GPIOB->BRR    |= (1<<9);


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         inline void debugPin_init(int ch)
    @brief      initilaise pin B9
 **/
inline void hal_debugPin_init()
{
    RCC->APB2ENR |= 0x00000008;
    GPIOB->CRH &= 0xFFFFFF0F;
    GPIOB->CRH |= 0x00000010;
}

 
 
 /** @}            
    end of group arm_hal_debugPin
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



