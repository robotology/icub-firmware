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
#ifndef _HAL_MPU_CAN_HID_H_
#define _HAL_MPU_CAN_HID_H_


/* @file       hal_mpu_can_hid.h
    @brief      This header file implements hidden interface to hal eth. 
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_base_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_can.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    uint8_t             supported_mask;             /**< bit in position hal_can_portx must be 1 if portx is supported */
    hal_gpio_map_t      gpio_rx[hal_cans_number];   /**< in position hal_can_portx there is mapping of gpio used for canx rx pin */
    hal_gpio_map_t      gpio_tx[hal_cans_number];   /**< in position hal_can_portx there is mapping of gpio used for canx tx pin */
} hal_can_hid_brdcfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------


extern hal_result_t hal_can_hid_static_memory_init(void);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




