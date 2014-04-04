/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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
#ifndef _HAL_LED_HID_H_
#define _HAL_LED_HID_H_


/* @file       hal_dled_hid.h
    @brief      This header file implements hidden interface to a led
    @author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       09/16/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_gpio.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_led.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    hal_gpio_map_t      led;       
} hal_led_gpiomap_t;


typedef struct
{
    hal_gpio_val_t          value_on;
    hal_gpio_val_t          value_off;
} hal_led_boardcommon_t;


typedef struct
{
    uint32_t                supportedmask;
    hal_led_boardcommon_t   boardcommon;
    hal_led_gpiomap_t       gpiomap[hal_leds_number];
} hal_led_boardconfig_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_led_boardconfig_t hal_led__theboardconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




