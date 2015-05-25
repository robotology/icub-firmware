/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Accame
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
#ifndef _HAL_5V_HID_H_
#define _HAL_5V_HID_H_


/* @file       hal_5v_hid.h
    @brief      This header file implements hidden interface to hal 5v. 
    @author     marco.accame@iit.it
    @date       05/25/2015
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_gpio.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_5v.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    uint8_t                 supported;  /**< 1 if supported*/
    hal_gpio_map_t          gpiomap;    /**< the pin */
    hal_gpio_cfg_t          gpiocfg;
} hal_5v_boardconfig_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_5v_boardconfig_t hal_5v__theboardconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------





#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




