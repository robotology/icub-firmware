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
#ifndef _EOTHEARMENVIRONMENT_HID_H_
#define _EOTHEARMENVIRONMENT_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "eEcommon.h"
#include "shalINFO.h"
#include "EOVtheEnvironment.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheARMenvironment.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

struct EOtheARMenvironment_hid 
{
    // the base object
    EOVtheEnvironment           *env;
    // other stuff
    const eEmoduleInfo_t        *modinfo; 
    const eEboardInfo_t         *brdinfo;
    const shalinfo_deviceinfo_t *devinfo;
    uint32_t                    codeprocoffset;
    eEprocess_t                 eprocess; 
}; 


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


