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
#ifndef _IPAL_BASE_HID_H_
#define _IPAL_BASE_HID_H_


/* @file       ipal_base_hid.h
	@brief      This file implements hidden interface of the ipal base module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "ipal.h"


/* ====================================================================================================================
 * MACROS
 * ==================================================================================================================== */

/* Transform IPAL address to LWIP address */
#define ipal2lwip_ipv4addr(addr)        (addr)

/* Transform LWIP address to IPAL address */
#define lwip2ipal_ipv4addr(addr)        (addr)


/* ====================================================================================================================
 * TYPE DEFINITIONS
 * ==================================================================================================================== */

/* @typedef    typedef uint8_t * ipal_arrayaddr_t 
    @brief     Alternative IP address used in IPAL
 **/ 
typedef uint8_t * ipal_arrayaddr_t;


/* ====================================================================================================================
 * FRIENDS VARIABLES EXTERNALIZATION
 * ==================================================================================================================== */

extern ipal_cfg2_t ipal_base_hid_cfg2;
extern ipal_cfg2_eth_t ipal_base_hid_cfg2_eth;
extern ipal_cfg2_arp_t ipal_base_hid_cfg2_arp;
extern ipal_cfg2_udp_t ipal_base_hid_cfg2_udp;
extern ipal_cfg2_igmp_t ipal_base_hid_cfg2_igmp;
extern ipal_cfg2_extfn_t ipal_base_hid_cfg2_extfn2;

extern void *ipal_base_hid_mutex;

/* ====================================================================================================================
 * FRIEND FUNCTIONS PROTOTYPES
 * ==================================================================================================================== */

/**
 * Gets the size required by all the whole submodules
 * @param cfg - IPAL configuration
 */
uint32_t ipal_base_hid_getsize(const ipal_cfg2_t *cfg2);

/**
 * Gives memory to all submodules
 * @param cfg - IPAL configuration
 * @param memory - Pointer to the bunch of memory (externally allocated) required to be assigned to single modules
 */
ipal_result_t ipal_base_hid_setmem(const ipal_cfg2_t *cfg2, uint32_t *memory);

/**
 * Initialize all modules
 * @param cfg - IPAL configuration
 */
ipal_result_t ipal_base_hid_init(ipal_cfg2_t *cfg2);

/**
 * Initialize variables needed by all modules
 * @param cfg - IPAL configuration
 */
ipal_result_t ipal_base_hid_vars_init(const ipal_cfg2_t *cfg2);

/**
 * This function is invoked in case of fatal error
 * @param errorcode - Error code
 * @param errormsg - Error message
 */
void ipal_base_hid_on_fatalerror(ipal_fatalerror_t errorcode, const char * errormsg);

/**
 * Initializes the locking system
 */
void ipal_base_hid_threadsafety_init(void);

/**
 * Gets the lock
 */
void ipal_base_hid_threadsafety_lock(void);

/**
 * Releases the lock
 */
void ipal_base_hid_threadsafety_unlock(void);

/**
 * Just a conversion function
 */
ipal_ipv4addr_t ipal_base_hid_addr_array_to_ipv4(ipal_arrayaddr_t iparray);

#endif  

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




