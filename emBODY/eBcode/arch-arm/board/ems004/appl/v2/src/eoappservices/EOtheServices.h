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
#ifndef _EOTHESERVICES_H_
#define _EOTHESERVICES_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheServices.h
    @brief      this object implements what is required for managing all services in the ETH board.    
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheServices Object EOtheServices
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#if defined(USE_EMBOT_theServices)    
#error USE_EMBOT_theServices is defined, so file EOtheServices.h must not be used 
#endif

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheServices_hid EOtheServices;

typedef void EOaService;


typedef eOresult_t (*eOservice_onendofoperation_fun_t) (EOaService* p, eObool_t operationisok);

// a service has methods: _Initialise(), _GetHandle(), _Verify(), _Activate(), _Deactivate(), _Start(), _Tick(), _Stop()
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// call it at startup
extern EOtheServices* eo_services_Initialise(eOipv4addr_t ipaddress);

// retrieve handle
extern EOtheServices* eo_services_GetHandle(void);

// use it to process commands of type eOmn_service_cmmnds_command_t coming from robotInterface
extern eOresult_t eo_services_ProcessCommand(EOtheServices *p, eOmn_service_cmmnds_command_t *command);

// use it to know about the state of a service
extern eOmn_serv_state_t eo_service_GetState(EOtheServices *p, eOmn_serv_category_t category);





/** @}            
    end of group eo_EOtheServices
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


