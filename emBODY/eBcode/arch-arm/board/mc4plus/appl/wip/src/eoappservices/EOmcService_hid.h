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

#ifndef _EOCANSERVICE_HID_H_
#define _EOCANSERVICE_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EOemsController.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOmcService.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

#warning TBD: add the encoderreader object pointer in here.
// evaluate to save memory by changing the arrays in pointers
struct EOmcService_hid
{
    eObool_t            initted;
    eOmcconfig_cfg_t    config;
    eObool_t            resourcesareready;
    eOmc_joint_t**      thejoints;
    eOmc_motor_t**      themotors;
    EOemsController*    thelocalcontroller;
    void*               thelocalencoderreader;
    uint32_t*           valuesencoder;
    int16_t*            valuespwm;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



