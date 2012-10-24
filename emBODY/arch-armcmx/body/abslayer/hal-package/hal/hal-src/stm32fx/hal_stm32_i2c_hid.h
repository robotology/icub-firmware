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
#ifndef _HAL_STM32_I2C_HID_H_
#define _HAL_STM32_I2C_HID_H_


/* @file       hal_stm32_i2c_hid.h
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
    hal_i2c_speed_t         speed;      
    hal_gpio_cfg_t          scl;
    hal_gpio_cfg_t          sda;
    hal_bool_t              usedma;     // not supported yet
    hal_void_fp_void_t      ontimeout;    
} hal_i2c_hw_cfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_i2c_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_i2c_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);

extern hal_boolval_t hal_i2c_hid_initted_is(hal_i2c_port_t port);

extern hal_result_t hal_i2c_hid_standby(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr) ;



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




