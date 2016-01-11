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


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOTHEVIRTUALSTRAIN_HID_H_
#define _EOTHEVIRTUALSTRAIN_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheCANprotocol.h"
#include "EOtheCANmapping.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheVirtualStrain.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheVirtualStrain_hid
{
    eObool_t            initted;
//    eObool_t            itismc4can;
    eOcanmap_location_t location;
    uint16_t            torques[6];
    eObool_t            thereisanewvalueoftorque;
    uint8_t             countbetweentwoupdates;
    uint8_t             countwatchdog;
    eOcanprot_command_t command;   
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



