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



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheServices_hid EOtheServices;

typedef void EOaService;

typedef enum
{
    eo_service_NONE             = eomn_serv_NONE,
    eo_service_MC_foc           = eomn_serv_MC_foc,
    eo_service_MC_mc4           = eomn_serv_MC_mc4,
    eo_service_MC_mc4plus       = eomn_serv_MC_mc4plus,
    eo_service_MC_mc4plusmais   = eomn_serv_MC_mc4plusmais,
    eo_service_MAIS             = eomn_serv_AS_mais,
    eo_service_STRAIN           = eomn_serv_AS_strain,    
    eo_service_INERTIAL         = eomn_serv_AS_inertial,
    eo_service_SKIN             = eomn_serv_SK_skin
} eOservice_t;

typedef eOresult_t (*eOservice_onendofoperation_fun_t) (EOaService* p, eObool_t operationisok);

// a service has methods: _Initialise(), _GetHandle(), _Verify(), _Activate(), _Deactivate(), _Start(), _Tick(), _Stop()
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheServices* eo_services_Initialise(void);

extern EOtheServices* eo_services_GetHandle(void);

extern eOresult_t eo_services_StartLegacyMode(EOtheServices *p, eOprotBRD_t brd);




/** @}            
    end of group eo_EOtheServices
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


