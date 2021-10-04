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
#ifndef _IPAL_UDP_HID_H_
#define _IPAL_UDP_HID_H_


/* @file       ipal_udp_hid.h
	@brief      This file implements hidden interface of the ipal udp module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/
 
#include "ipal.h"

/* ====================================================================================================================
 * FRIEND FUNCTIONS PROTOTYPES
 * ==================================================================================================================== */

/**
 * Gets the memory size required by this module (in this case, it will be allocated at higher level)
 * @param cfg - IPAL configuration
 */
uint32_t 			ipal_udp_hid_getsize(const ipal_cfg_t *cfg);

/**
 * Gives memory to this module
 * @param cfg 		- IPAL configuration
 * @param memory 	- Pointer to the memory reserved for this module
 */
ipal_result_t ipal_udp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory);

/**
 * Initializes this module
 * @param cfg 		- IPAL configuration
 */
ipal_result_t ipal_udp_hid_init(ipal_cfg_t *cfg);

/**
 * Initializes internal variables of this module
 * @param cfg 		- IPAL configuration
 */
ipal_result_t ipal_udp_hid_vars_init(const ipal_cfg_t *cfg);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


