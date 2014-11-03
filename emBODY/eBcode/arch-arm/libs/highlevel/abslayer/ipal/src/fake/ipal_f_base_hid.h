/*
 * Copyright (C) 2014 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
    @date       11/03/2014
**/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "ipal.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @typedef    typedef uint8_t * ipal_arrayaddr_t 
    @brief      Alternative IP address used in IPAL
 **/ 
typedef uint8_t * ipal_arrayaddr_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern ipal_cfg_t ipal_base_hid_cfg;

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void ipal_base_hid_on_fatalerror(ipal_fatalerror_t errorcode, const char * errormsg);

extern ipal_ipv4addr_t ipal_base_hid_addr_array_to_ipv4(ipal_arrayaddr_t iparray);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




