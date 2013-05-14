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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHEEMSAPPLDIAGNOTICS_HID_H_
#define _EOTHEEMSAPPLDIAGNOTICS_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSsocket_hid.h
    @brief      This header file implements hidden interface to the error manager
 	@author     valentina.gaggero@iit.it
	@date       05/10/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheEMSapplDiagnostics.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

struct EOTheEMSdiagnostics_hid_t
{
    uint8_t dummy;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

