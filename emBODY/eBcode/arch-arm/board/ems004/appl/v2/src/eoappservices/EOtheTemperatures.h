/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOTHETEMPERATURES_H_
#define _EOTHETEMPERATURES_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheTemperatures.h
    @brief      this object implements what is required for managing the inertial sensors. it is version 3               
    @author     marco.accame@iit.it
    @date       28 march 2018
**/

/** @defgroup eo_EOtheTemperatures Object EOtheTemperatures
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

typedef struct EOtheTemperatures_hid EOtheTemperatures;

enum { eo_temperatures_maxnumberofCANboards = 15 }; // even if they can be up to 28 (the number of allowed can IDs on the two buses).


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------



extern EOtheTemperatures* eo_temperatures_Initialise(void);

extern EOtheTemperatures* eo_temperatures_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_temperatures_GetServiceState(EOtheTemperatures *p);
extern eOresult_t eo_temperatures_SendReport(EOtheTemperatures *p);


extern eOresult_t eo_temperatures_Verify(EOtheTemperatures *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_temperatures_Activate(EOtheTemperatures *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_temperatures_Deactivate(EOtheTemperatures *p);

extern eOresult_t eo_temperatures_Start(EOtheTemperatures *p);

extern eOresult_t eo_temperatures_SetRegulars(EOtheTemperatures *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_temperatures_Tick(EOtheTemperatures *p, eObool_t resetstatus); // we reset the status when there has just been a transmission of the regulars containing the inertial-status

extern eOresult_t eo_temperatures_Stop(EOtheTemperatures *p);

extern eOresult_t eo_temperatures_Transmission(EOtheTemperatures *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth protocol
extern eOresult_t eo_temperatures_Config(EOtheTemperatures *p, eOas_temperature_config_t* config);
extern eOresult_t eo_temperatures_AcceptCANframe(EOtheTemperatures *p, eOas_temperature_type_t type, eOcanframe_t *frame, eOcanport_t port);


/** @}            
    end of group eo_EOtheTemperatures
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


