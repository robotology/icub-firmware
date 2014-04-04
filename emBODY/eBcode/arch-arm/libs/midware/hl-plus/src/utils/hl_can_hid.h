/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HL_CAN_HID_H_
#define _HL_CAN_HID_H_


/* @file       hl_can_hid.h
	@brief      This file contains internals for hl can utilities.
	@author     marco.accame@iit.it
    @date       11/11/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hl_can.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------

#define HL_can_id2index(p)              ( (uint8_t)((p)) )
#define HL_can_port2peripheral(p)       ( ( hl_can1 == (p) ) ? (CAN1) : (CAN2) )


// - definition of the hidden struct implementing the object ----------------------------------------------------------


// - declaration of extern hidden variables ---------------------------------------------------------------------------



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




