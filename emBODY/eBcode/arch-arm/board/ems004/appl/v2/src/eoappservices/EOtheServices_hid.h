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


#ifdef __cplusplus
extern "C" {
#endif

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
    
 

// - macros which enable/disable the code for the services ------------------------------------------------------------    
    
#include "EOtheMotionController.h"
#include "EOtheSTRAIN.h"
#include "EOtheMAIS.h"
#include "EOtheSKIN.h"
#include "EOtheInertials2.h"
#include "EOtheInertials3.h"
#include "EOtheTemperatures.h"
#include "EOthePSC.h"
#include "EOthePOS.h"

// so far, i write them in here. later on we think of a board file or else...
#if 0

#define EOTHESERVICES_disable_theInertials2
//#define EOTHESERVICES_disable_theInertials3
//#define EOTHESERVICES_disable_theTemperatures
//#define EOTHESERVICES_disable_theMAIS
//#define EOTHESERVICES_disable_theSTRAIN
//#define EOTHESERVICES_disable_theSKIN
//#define EOTHESERVICES_disable_thePSC
////#define EOTHESERVICES_disable_thePOS
//#define EOTHESERVICES_disable_theMC4boards
//#define EOTHESERVICES_disable_theMotionController
//#define EOTHESERVICES_disable_theEncoderReader
//#define EOTHESERVICES_disable_CurrentsWatchdog

#endif

#if defined(EOTHESERVICES_customize_handV3)

    #define EOTHESERVICES_disable_theInertials2
    #define EOTHESERVICES_disable_theInertials3

#endif


#if defined(EOTHESERVICES_disable_theInertials2) && defined(EOTHESERVICES_disable_theInertials3)
    #define EOTHESERVICES_disable_theMEMs
#endif

// - definition of the hidden struct implementing the object ----------------------------------------------------------



// maybe move them into _hid.h
typedef struct
{
    eObool_t                                initted;
    eObool_t                                active;  
    eObool_t                                activateafterverify;
    eObool_t                                started;
    eOservice_onendofoperation_fun_t        onverify;
    eOmn_serv_state_t                       state;
    const eOmn_serv_configuration_t*        tmpcfg;
    eOmn_serv_configuration_t               servconfig;
} eOservice_core_t;    

typedef struct
{
    EOvector*                               boardproperties;    // of eObrd_canproperties_t
    EOvector*                               entitydescriptor;   // of eOcanmap_entitydescriptor_t
    EOarray*                                discoverytargets;   // of eOcandiscovery_target_t
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
    eo_service_inertials    = 4,
    eo_service_inertials3   = 5,
    eo_service_temperatures = 6,
    eo_service_psc          = 7,
    eo_service_pos          = 8,
    eo_service_none         = 255
} eOservice_type_t;

typedef struct
{
    eObool_t            allactivated;
    eOservice_type_t    failedservice;      
} eOservice_startupactivation_state_t;

struct EOtheServices_hid
{
    eObool_t            initted;
    EOnvSet*            nvset;      
    EOtimer*            timer;
    eOprotBRD_t         board;   
    eOmn_service_t*     mnservice;  
    eObool_t            running[eomn_serv_categories_numberof];  
    eOipv4addr_t        ipaddress;    
    eOservice_startupactivation_state_t startupactivationstate;
    uint8_t             mcservicetype;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern eOresult_t eo_service_hid_SynchServiceState(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_state_t servstate);
extern eOresult_t eo_service_hid_SetRegulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, eObool_t (*isID32relevant)(uint32_t), uint8_t* numberofthem);
extern eOresult_t eo_service_hid_AddRegulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, eObool_t (*isID32relevant)(uint32_t), uint8_t* numberofthem);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



