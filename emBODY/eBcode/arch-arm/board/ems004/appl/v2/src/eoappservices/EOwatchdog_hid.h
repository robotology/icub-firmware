/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOWATCHDOG_HID_H_
#define _EOWATCHDOG_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOwatchdog.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------
struct EOwatchdog_hid
{
    eOwatchdog_cfg_t           cfg;
    eObool_t                   isrunning;
    eOabstime_t                starttime;
    uint16_t                   count_failures;
    eObool_t                   diagnostic_enabled;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




