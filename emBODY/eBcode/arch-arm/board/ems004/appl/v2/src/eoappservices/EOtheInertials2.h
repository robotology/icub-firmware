/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOTHEINERTIALS2_H_
#define _EOTHEINERTIALS2_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheInertials2.h
    @brief      this object implements what is required for managing the inertial sensors.                
    @author     marco.accame@iit.it
    @date       8 apr 2016
**/

/** @defgroup eo_EOtheInertials Object EOtheInertials2
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EOtheServices.h"

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheInertials2_hid EOtheInertials2;

enum { eo_inertials2_maxnumberofMTBboards = 15 }; // even if they can be up to 28 (the number of allowed can IDs on the two buses).


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------



extern EOtheInertials2* eo_inertials2_Initialise(void);

extern EOtheInertials2* eo_inertials2_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_inertials2_GetServiceState(EOtheInertials2 *p);
extern eOresult_t eo_inertials2_SendReport(EOtheInertials2 *p);


extern eOresult_t eo_inertials2_Verify(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_inertials2_Activate(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_inertials2_Deactivate(EOtheInertials2 *p);

extern eOresult_t eo_inertials2_Start(EOtheInertials2 *p);

extern eOresult_t eo_inertials2_SetRegulars(EOtheInertials2 *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_inertials2_Tick(EOtheInertials2 *p, eObool_t resetstatus); // we reset the status when there has just been a transmission of the regulars containing the inertial-status

extern eOresult_t eo_inertials2_Stop(EOtheInertials2 *p);

extern eOresult_t eo_inertials2_Transmission(EOtheInertials2 *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth protocol
extern eOresult_t eo_inertials2_Config(EOtheInertials2 *p, eOas_inertial_config_t* config);
extern eOresult_t eo_inertials2_AcceptCANframe(EOtheInertials2 *p, eOcanframe_t *frame, eOcanport_t port, eOas_inertial_type_t type);


/** @}            
    end of group eo_EOtheInertials
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


