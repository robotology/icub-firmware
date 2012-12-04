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
#ifndef _HAL_STM32_SENSOR_GYRO_HID_H_
#define _HAL_STM32_SENSOR_GYRO_HID_H_


/* @file       hal_stm32_sensor_gyro_hid.h
    @brief      This header file implements hidden interface to crc
    @author     marco.accame@iit.it
    @date       10/24/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_sensor_gyro.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef hal_result_t (*hal_sensor_gyro_hid_fn_read_t) (int32_t*, int32_t*, int32_t*);

typedef struct
{   
    hal_res_fp_voidp_t                      init;
    void*                                   initpar;
    hal_sensor_gyro_hid_fn_read_t           read;
} hal_sensor_gyro_hid_chip_interface_t;


typedef struct
{   
    hal_sensor_gyro_hid_chip_interface_t    chipif;
} hal_sensor_gyro_hid_dev_cfg_t;

typedef struct
{
    uint8_t                                 supported_mask;
    hal_sensor_gyro_hid_dev_cfg_t           devcfg[hal_sensor_gyros_number];
} hal_sensor_gyro_hid_brdcfg_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_sensor_gyro_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_sensor_gyro_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




