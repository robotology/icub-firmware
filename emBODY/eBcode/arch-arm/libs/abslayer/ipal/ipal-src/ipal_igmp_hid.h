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
#ifndef _IPAL_IGMP_HID_H_
#define _IPAL_IGMP_HID_H_


/* @file       ipal_igmp_hid.h
	@brief      This file implements hidden interface of the ipal igmp module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "ipal.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section


// - declaration of extern hidden variables ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t ipal_igmp_hid_getsize(const ipal_cfg_t *cfg);
extern ipal_result_t ipal_igmp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory);
extern ipal_result_t ipal_igmp_hid_init(ipal_cfg_t *cfg);
extern ipal_result_t ipal_igmp_hid_vars_init(const ipal_cfg_t *cfg);




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


