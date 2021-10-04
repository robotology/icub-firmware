/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

/* @file       ipal_igmp.c
	@brief      This file implements internal implementation of the ipal igmp module
	@author     valentina.gaggero@iit.it
    @date       01/07/2013
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_IGMP

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "lwip_interface.h"

/* Friend functions */
#include "ipal_hid.h"

/* IPAL APIs */
#include "ipal.h"

/* =======================================================================================================================
 * STATIC FUNCTIONS DECLARATION
 * ======================================================================================================================= */

/*
 * Resets internal RAM
 */
static void s_ipal_igmp_internal_ram_reset(void);

/*
 * Initializes internal RAM
 */
static void s_ipal_igmp_internal_ram_init(void);

/* =======================================================================================================================
 * PUBLIC FUNCTIONS (API)
 * ======================================================================================================================= */

/*
 * ipal_igmp_join
 * See header file for details
 */
ipal_result_t ipal_igmp_join(ipal_ipv4addr_t group)
{
	return(ipal_res_OK);
}                                         

/*
 * ipal_igmp_leave
 * See header file for details
 */
ipal_result_t ipal_igmp_leave(ipal_ipv4addr_t group)
{
	return(ipal_res_OK);
}

/* -----------------------------------------------------------------------------------------------------------------------
 * FRIEND "HIDDEN" FUNCTIONS
 * ----------------------------------------------------------------------------------------------------------------------- */

/* 
 * ipal_igmp_hid_getsize
 * See header file for details
 */
uint32_t ipal_igmp_hid_getsize(const ipal_cfg2_t *cfg2)
{
    // dont need memory
	return(0);
}

/* 
 * ipal_igmp_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_igmp_hid_setmem(const ipal_cfg2_t *cfg2, uint32_t *memory)
{
	return(ipal_res_OK); 
}

/* 
 * ipal_igmp_hid_init
 * See header file for details
 */
ipal_result_t ipal_igmp_hid_init(ipal_cfg2_t *cfg2)
{
	return(ipal_res_OK); 
}

/* 
 * ipal_igmp_hid_vars_init
 * See header file for details
 */
extern ipal_result_t ipal_igmp_hid_vars_init(const ipal_cfg2_t *cfg2)
{
	if(ipal_cfg2_supportedflag_igmp != (cfg2->support->supportedmodules & ipal_cfg2_supportedflag_igmp))
	{
        return(ipal_res_OK);
	}
	
	if( (IPAL_NUMBEROF_IGMPMAXGROUPS < cfg2->igmp->igmp_groupsnum) )
	{
        return(ipal_res_NOK_generic);
	}

	/* we reset the internally allocated memory and we init internal variables */
	s_ipal_igmp_internal_ram_reset();
	s_ipal_igmp_internal_ram_init();
	
	return(ipal_res_OK);
}

/* =======================================================================================================================
 * PRIVATE FUNCTIONS DEFINITIONS
 * ======================================================================================================================= */

/* 
 * s_ipal_igmp_internal_ram_reset
 * See declaratinon for details
 */
static void s_ipal_igmp_internal_ram_reset(void)
{
	/**/ 
}

/* 
 * s_ipal_igmp_internal_ram_init
 * See declaratinon for details
 */
static void s_ipal_igmp_internal_ram_init(void)
{
	/**/
}

#endif//IPAL_USE_IGMP


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




