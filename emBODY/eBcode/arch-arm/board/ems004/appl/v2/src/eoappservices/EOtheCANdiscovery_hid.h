/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#error ::: DONT USE IT: USE EOtheCANdiscovery2 INSTEAD

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOTHECANDISCOVERY_HID_H_
#define _EOTHECANDISCOVERY_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheCANprotocol.h"
#include "EOtimer.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheCANdiscovery.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheCANdiscovery_hid
{
    eObool_t            initted;
    EOtimer             *discoverytimer;
    uint32_t            readymask;          // i-th bit correspond to CAN address (16LSB-> CAN1, 16MSB->CAN2)
    uint32_t            tobecheckedmask;    // ""
    uint32_t            incorrectprotmask;
    eObool_t            isMCready;
    eObool_t            isMAISready;
    eObool_t            isSTRAINready;
    uint8_t             numberofattempts;
    //...
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



