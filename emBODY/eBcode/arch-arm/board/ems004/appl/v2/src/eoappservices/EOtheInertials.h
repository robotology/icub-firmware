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
#ifndef _EOTHEINERTIALS_H_
#define _EOTHEINERTIALS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheInertials.h
    @brief      this object implements what is required for managing the inertial sensors.                
    @author     marco.accame@iit.it
    @date       08/19/2015
**/

/** @defgroup eo_EOtheInertials Object EOtheInertials
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

typedef struct EOtheInertials_hid EOtheInertials;

enum { eo_inertials_maxnumberofMTBboards = 15 }; // even if they can be up to 28 (the number of allowed can IDs on the two buses).


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb2;
extern const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb4;
extern const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb10;
extern const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb11;

// - declaration of extern public functions ---------------------------------------------------------------------------



extern EOtheInertials* eo_inertials_Initialise(void);

extern EOtheInertials* eo_inertials_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_inertials_GetServiceState(EOtheInertials *p);
extern eOresult_t eo_inertials_SendReport(EOtheInertials *p);


extern eOresult_t eo_inertials_Verify(EOtheInertials *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_inertials_Activate(EOtheInertials *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_inertials_Deactivate(EOtheInertials *p);

extern eOresult_t eo_inertials_Start(EOtheInertials *p);

extern eOresult_t eo_inertials_SetRegulars(EOtheInertials *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_inertials_Tick(EOtheInertials *p, eObool_t resetstatus); // we reset the status when there has just been a transmission of the regulars containing the inertial-status

extern eOresult_t eo_inertials_Stop(EOtheInertials *p);

extern eOresult_t eo_inertials_Transmission(EOtheInertials *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth protocol
extern eOresult_t eo_inertials_ServiceConfig(EOtheInertials *p, const eOas_inertial_serviceconfig_t* cfg);
extern eOresult_t eo_inertials_SensorsConfig(EOtheInertials *p, eOas_inertial_sensorsconfig_t* config);
extern eOresult_t eo_inertials_AcceptCANframe(EOtheInertials *p, eOas_inertial_type_t type, eOcanframe_t *frame, eOcanport_t port);


/** @}            
    end of group eo_EOtheInertials
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


