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
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


// - declaration of extern hidden variables ---------------------------------------------------------------------------


extern hl_boolval_t hl_can_comm_hid_rxfifo_full(hl_can_t id);
extern hl_boolval_t hl_can_comm_hid_txfifo_full(hl_can_t id);

extern void hl_can_comm_hid_rxfifo_full_clr(hl_can_t id);
extern void hl_can_comm_hid_txfifo_clr(hl_can_t id);

extern void hl_can_comm_hid_rxfifo_full_set(hl_can_t id);
extern void hl_can_comm_hid_txfifo_set(hl_can_t id);

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




