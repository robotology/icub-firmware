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
#ifndef _HAL_MPU_I2C_HID_H_
#define _HAL_MPU_I2C_HID_H_


/* @file       hal_mpu_i2c_hid.h
    @brief      This header file implements hidden interface to i2c.
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_gpio.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_i2c.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------



typedef struct
{
    uint8_t             supported_mask;                     /**< bit in position hal_can_portx must be 1 if portx is supported */
    hal_gpio_cfg_t      gpio_scl[hal_i2c_ports_number];     /**< in position hal_i2c_portx there is configuration of gpio used for scl pin */
    hal_gpio_cfg_t      gpio_sda[hal_i2c_ports_number];     /**< in position hal_i2c_portx there is configuration of gpio used for sda pin */
} hal_i2c_hid_brdcfg_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_i2c_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_i2c_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);





#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




