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


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _STM32GPIO_HID_H_
#define _STM32GPIO_HID_H_


/* @file       stm32gpio_hid.h
    @brief      This header file contains hidden interface for....
    @author     marco.accame@iit.it
    @date       09/14/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_stm32xx_include.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "stm32gpio.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section



// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const uint8_t stm32gpio_hid_maxports;


extern const uint32_t stm32gpio_hid_thegpioclocks[];
extern GPIO_TypeDef* const stm32gpio_hid_thegpioports[];
extern const uint8_t  stm32gpio_hid_thepinnums[]; 
extern const uint16_t  stm32gpio_hid_thepins[];

// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern uint32_t stm32i2c_hid_myfunction(void);

// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




