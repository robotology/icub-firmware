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

#ifndef _EOTHESTRAIN_HID_H_
#define _EOTHESTRAIN_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"
#include "EOCanMsgWatchdog.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheSTRAIN.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { strain_maxRegulars = 2 }; // there cannot be more than 1 strain, and typically not more than 1 signalled variable.

struct EOtheSTRAIN_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eOprotID32_t                            id32;
    eOservice_onendofoperation_fun_t        overrideonfullscaleready; 
    eOas_strain_t*                          strain;   
    EOarray*                                id32ofregulars;    
    EOCanMsgWatchdog                        *canmsgwatchdog;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



