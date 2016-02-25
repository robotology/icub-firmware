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

#ifndef _EOTHESERVICES_HID_H_
#define _EOTHESERVICES_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOtheCANprotocol.h"
#include "EOtheCANdiscovery2.h"
#include "EOtimer.h"
#include "EOtheErrorManager.h"
#include "EOvector.h"

#include "EOnvSet.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheServices.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------



// maybe move them into _hid.h
typedef struct
{
    const eOmn_serv_configuration_t*        tmpcfg;
    eOmn_serv_configuration_t               servconfig;
    eObool_t                                initted;
    eObool_t                                active;  
    eObool_t                                activateafterverify;
    eObool_t                                running;
    eOservice_onendofoperation_fun_t        onverify;
    eOmn_serv_state_t                       state;
} eOservice_core_t;    

typedef struct
{
    EOvector*                               boardproperties;
    EOvector*                               entitydescriptor;
    eOcandiscovery_target_t                 discoverytarget;
    eOcandiscovery_onstop_t                 ondiscoverystop; 
    eOcanprot_command_t                     command;    
} eOservice_cantools_t; 

typedef struct
{
    EOtimer*                                reportTimer;
    eOreltime_t                             reportPeriod;  
    eOerrmanDescriptor_t                    errorDescriptor;
    eOerrmanErrorType_t                     errorType;
    uint8_t                                 errorCallbackCount;
    uint8_t                                 repetitionOKcase;
} eOservice_diagnostics_t;

typedef enum
{
    eo_service_mc           = 0,
    eo_service_strain       = 1,
    eo_service_mais         = 2,
    eo_service_skin         = 3,
    eo_service_inertial     = 4,
    eo_service_none         = 5
} eOservice_type_t;

struct EOtheServices_hid
{
    eObool_t            initted;
    EOnvSet*            nvset;      
    EOtimer*            timer;
    eOprotBRD_t         board;   
    eObool_t            allactivated;
    eOservice_type_t    failedservice; 
    eOmn_service_t*     mnservice;  
    eObool_t            running[eomn_serv_categories_numberof];    
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern eOresult_t eo_service_hid_SynchServiceState(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_state_t servstate);

extern eOresult_t eo_service_hid_SetRegulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, eObool_t (*isID32relevant)(uint32_t), uint8_t* numberofthem);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



