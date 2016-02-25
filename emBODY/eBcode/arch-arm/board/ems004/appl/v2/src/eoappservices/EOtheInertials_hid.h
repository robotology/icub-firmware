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

#ifndef _EOTHEINERTIALS_HID_H_
#define _EOTHEINERTIALS_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheInertials.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { inertials_maxRegulars = 2 }; // there cannot be more than 1 inertials object, and typically not more than 1 signalled variable.

struct EOtheInertials_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;

    uint8_t                                 numofmtbs;
    
    // now the old ones
    eOas_inertial_sensorsconfig_t           sensorsconfig;
    eOas_inertial_serviceconfig_t           serviceconfig;
    uint64_t                                supportedmask64;
    EOvector*                               fifoofinertialdata;    
    
    eObool_t                                configured;

    // the inertial at the end
    eOas_inertial_t*                        inertial2;  
    EOarray*                                id32ofregulars;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



