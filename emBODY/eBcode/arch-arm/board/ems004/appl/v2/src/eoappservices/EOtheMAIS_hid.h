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
#include "EOtheCANprotocol.h"
#include "EOtheCANdiscovery2.h"
#include "EOtimer.h"
#include "EOtheErrorManager.h"
#include "EOvector.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMAIS.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    eOmn_serv_configuration_t               servconfig;
    eObool_t                                initted;
    eObool_t                                active;  
    eObool_t                                activateafterverify;
    eObool_t                                running;
    eOmais_onendofoperation_fun_t           onverify;
} eOmais_objs_service_t;    

typedef struct
{
    EOvector*                               boardproperties;
    EOvector*                               entitydescriptor;
    eOcandiscovery_target_t                 discoverytarget;
    eOcandiscovery_onstop_t                 ondiscoverystop; 
    eOcanprot_command_t                     command;    
} eOmais_objs_sharedcan_t; // poi rinomina in objs_can_t

typedef struct
{
    EOtimer*                                reportTimer;
    eOreltime_t                             reportPeriod;  
    eOerrmanDescriptor_t                    errorDescriptor;
    eOerrmanErrorType_t                     errorType;
    uint8_t                                 errorCallbackCount;
    uint8_t                                 repetitionOKcase;
} eOmais_objs_diagnostics_t;

struct EOtheMAIS_hid
{
    eOmais_objs_service_t                   service;
    eOmais_objs_diagnostics_t               diagnostics;
    eOmais_objs_sharedcan_t                 sharedcan;
    
    eOprotID32_t                            id32;  
    eOas_mais_t*                            mais;    
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



