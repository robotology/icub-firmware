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

#ifndef _EOTHEPSC_HID_H_
#define _EOTHEPSC_HID_H_


#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"
#include "EOwatchdog.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOthePSC.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { psc_maxRegulars = 1 }; // there cannot be more than 1 psc, and typically not more than 1 signalled variable.

struct EOthePSC_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eObool_t                                configured;
    
    eOas_psc_config_t                       pscconfig;
    
    eOprotID32_t                            id32;  
    eOas_psc_t*                             psc;
    EOarray*                                id32ofregulars;
    uint8_t                                 numberofowners;
    
    
    uint16_t                                not_heardof_target[2];
    uint16_t                                not_heardof_status[2];
    uint32_t                                not_heardof_counter;
    uint32_t                                not_heardof_checkrate;
    eObool_t                                transmissionisactive;
    eObool_t                                isalive;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



