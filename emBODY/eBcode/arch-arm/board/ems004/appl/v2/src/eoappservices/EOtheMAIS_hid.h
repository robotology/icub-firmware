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

#ifndef _EOTHEMAIS_HID_H_
#define _EOTHEMAIS_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"
#include "EOwatchdog.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMAIS.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { mais_maxRegulars = 2 }; // there cannot be more than 1 mais, and typically not more than 1 signalled variable.

struct EOtheMAIS_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eOprotID32_t                            id32;  
    eOas_mais_t*                            mais;
    EOarray*                                id32ofregulars;
    uint8_t                                 numberofowners; // increased / decreased at _Start() _Stop()
    EOwatchdog*                             watchdog; //watchdog for can messages reception
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



