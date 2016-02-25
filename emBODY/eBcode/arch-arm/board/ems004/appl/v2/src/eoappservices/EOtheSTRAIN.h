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
#ifndef _EOTHESTRAIN_H_
#define _EOTHESTRAIN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheSTRAIN.h
    @brief      this object implements what is required for managing the STRAIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheSTRAIN Object EOtheSTRAIN
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

typedef struct EOtheSTRAIN_hid EOtheSTRAIN;



//typedef eOresult_t (*eOstrain_onendofoperation_fun_t) (EOtheSTRAIN* p, eObool_t operationisok);
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheSTRAIN* eo_strain_Initialise(void);

extern EOtheSTRAIN* eo_strain_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_strain_GetServiceState(EOtheSTRAIN *p);
extern eOresult_t eo_strain_SendReport(EOtheSTRAIN *p);


extern eOresult_t eo_strain_Verify(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_strain_Activate(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_strain_Deactivate(EOtheSTRAIN *p);

extern eOresult_t eo_strain_Start(EOtheSTRAIN *p);

extern eOresult_t eo_strain_SetRegulars(EOtheSTRAIN *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_strain_Tick(EOtheSTRAIN *p);

extern eOresult_t eo_strain_Stop(EOtheSTRAIN *p);

// it enables/disables transmission of the strain board. _Start() just starts the service, not the transmission
extern eOresult_t eo_strain_Transmission(EOtheSTRAIN *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth protocol
extern eOresult_t eo_strain_GetFullScale(EOtheSTRAIN *p, eOservice_onendofoperation_fun_t overrideonfullscaleready);
extern eOresult_t eo_strain_Set(EOtheSTRAIN *p, eOas_strain_config_t *cfg);
extern eOresult_t eo_strain_SetMode(EOtheSTRAIN *p, eOas_strainmode_t mode);
extern eOresult_t eo_strain_SetDataRate(EOtheSTRAIN *p, uint8_t datarate);



/** @}            
    end of group eo_EOtheSTRAIN
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


