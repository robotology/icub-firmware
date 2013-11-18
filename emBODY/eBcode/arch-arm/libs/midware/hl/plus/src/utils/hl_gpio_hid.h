/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HL_GPIO_HID_H_
#define _HL_GPIO_HID_H_


/* @file       hl_gpio_hid.h
    @brief      This header file implements hidden interface to hl gpio.
    @author     marco.accame@iit.it
    @date       10/21/2013
 **/


// - modules to be built: contains the HL_USE_* macros ----------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"

#include "hl_core.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hl_gpio.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------




// - declaration of extern hidden variables ---------------------------------------------------------------------------

// used by other peripherals which need to initialise their pins

extern const uint8_t hl_gpio_hid_maxports;
extern GPIO_TypeDef *const hl_gpio_hid_ports[];
extern const uint32_t hl_gpio_hid_clocks[];

// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




