/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _IPAL_SYS_HID_H_
#define _IPAL_SYS_HID_H_

#include <stdint.h>
#include "ipal.h"

/* ====================================================================================================================
 * FRIEND VARIABLES EXTERNALIZATION
 * ==================================================================================================================== */

/* Is IPAL SYS started? */
extern uint8_t ipal_sys_hid_started;


/* ====================================================================================================================
 * FRIEND FUNCTIONS PROTOTYPES
 * ==================================================================================================================== */

/**
 * Returns the size required by this module
 * @param cfg - Configuration 
 * @return required size
 */
uint32_t ipal_sys_hid_getsize(const ipal_cfg2_t *cfg2);

/**
 * Inizialize memory required by this module 
 * @param cfg - IPAL configuration
 * @param memory - Previously allocated memory
 * @return Result pf the operation
 */
ipal_result_t ipal_sys_hid_setmem(const ipal_cfg2_t *cfg2, uint32_t *memory);

/**
 * Initialize internals structures of this module
 * @cfg - IPAL configuration
 * @return Result of the operation
 */
ipal_result_t ipal_sys_hid_init(ipal_cfg2_t *cfg2);

/**
 * Initialize internal variables
 * @cfg - IPAL configuration
 * @return Result of the operation
 */
ipal_result_t ipal_sys_hid_vars_init(const ipal_cfg2_t *cfg2);

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


