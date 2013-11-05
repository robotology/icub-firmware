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
#ifndef _HL_PERIPH_I2C_HID_H_
#define _HL_PERIPH_I2C_HID_H_


/* @file       hl_periph_i2c_hid.h
    @brief      This header file implements hidden interface to i2c.
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_gpio.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hl_i2c.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    uint8_t         supported_mask;             /**< bit in position hl_can_portx must be 1 if portx is supported */
    hl_gpio_t       gpio_scl[hl_i2cs_number];   /**< in position hl_i2cx there is mapping of gpio used for its scl pin */
    hl_gpio_t       gpio_sda[hl_i2cs_number];   /**< in position hl_i2cx there is mapping of gpio used for its sda pin */
    hl_gpio_altf_t  altf_scl[hl_i2cs_number];   /**< in position hl_i2cx there is alt config of scl pin */
    hl_gpio_altf_t  altf_sda[hl_i2cs_number];   /**< in position hl_i2cx there is alt config of sda pin */    
} hl_i2c_hid_brdcfg_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hl_i2c_hid_brdcfg_t hl_brdcfg_i2c__theconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section





#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




