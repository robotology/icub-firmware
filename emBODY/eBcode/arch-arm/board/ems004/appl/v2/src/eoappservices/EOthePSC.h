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
#ifndef _EOTHEPSC_H_
#define _EOTHEPSC_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOthePSC.h
    @brief      this object implements what is required for managing the PSC.                
    @author     marco.accame@iit.it
    @date       04 apr 2019
**/

/** @defgroup eo_EOthePSC Object EOthePSC
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EOtheServices.h"

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheCANdiscovery2.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOthePSC_hid EOthePSC;

enum { eo_psc_maxnumberofCANboards = 3 }; 

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOthePSC* eo_psc_Initialise(void);

extern EOthePSC* eo_psc_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_psc_GetServiceState(EOthePSC *p);
extern eOresult_t eo_psc_SendReport(EOthePSC *p);


extern eOresult_t eo_psc_Verify(EOthePSC *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_psc_Activate(EOthePSC *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_psc_Deactivate(EOthePSC *p);

extern eOresult_t eo_psc_Start(EOthePSC *p);

extern eOresult_t eo_psc_SetRegulars(EOthePSC *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_psc_Tick(EOthePSC *p);

extern eOresult_t eo_psc_Stop(EOthePSC *p);

extern uint8_t eo_psc_GetNumberOfOwners(EOthePSC *p); // start() increments owners, stop or deactivate() decrements them.

// it enables/disables transmission of the psc board. _Start() just starts the service, not the transmission
extern eOresult_t eo_psc_Transmission(EOthePSC *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth protocol
extern eOresult_t eo_psc_Config(EOthePSC *p, eOas_psc_config_t* config);
extern eOresult_t eo_psc_AcceptCANframe(EOthePSC *p, eOcanframe_t *frame, eOcanport_t port);

extern eObool_t eo_psc_isAlive(EOthePSC *p);



/** @}            
    end of group eo_EOthePSC
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


