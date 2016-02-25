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
#ifndef _EOTHESKIN_H_
#define _EOTHESKIN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheSKIN.h
    @brief      this object implements what is required for managing the SKIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheSKIN Object EOtheSKIN
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#include "EOtheServices.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheSKIN_hid EOtheSKIN;


//typedef eOresult_t (*eOskin_onendofoperation_fun_t) (EOtheSKIN* p, eObool_t operationisok);

enum { eo_skin_maxnumberofMTBboards = 15 }; // even if they can be up to 28 (the number of allowed can IDs on the two buses).
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// the skin is a singleton but can have 1 or 2 patches (actually up to 4 are supported) which must be configured all together

extern EOtheSKIN* eo_skin_Initialise(void);

extern EOtheSKIN* eo_skin_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_skin_GetServiceState(EOtheSKIN *p);
extern eOresult_t eo_skin_SendReport(EOtheSKIN *p);


extern eOresult_t eo_skin_Verify(EOtheSKIN *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_skin_Activate(EOtheSKIN *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_skin_Deactivate(EOtheSKIN *p);

extern eOresult_t eo_skin_Start(EOtheSKIN *p);

extern eOresult_t eo_skin_SetRegulars(EOtheSKIN *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_skin_Tick(EOtheSKIN *p, eObool_t resetstatus); // we reset the status when there has just been a transmission of the regulars containing the skin-status

extern eOresult_t eo_skin_Stop(EOtheSKIN *p);

// it enables/disables transmission of all the skin patches. _Start() just starts the service, not the transmission. 
extern eOresult_t eo_skin_Transmission(EOtheSKIN *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth or can protocol
extern eOresult_t eo_skin_SetMode(EOtheSKIN *p, uint8_t patchindex, eOsk_sigmode_t mode);
extern eOresult_t eo_skin_SetBoardsConfig(EOtheSKIN *p, uint8_t patchindex, eOsk_cmd_boardsCfg_t *brdcfg);
extern eOresult_t eo_skin_SetTrianglesConfig(EOtheSKIN *p, uint8_t patchindex, eOsk_cmd_trianglesCfg_t *trgcfg);
extern eOresult_t eo_skin_AcceptCANframe(EOtheSKIN *p, eOcanframe_t *frame, eOcanport_t port);



/** @}            
    end of group eo_EOtheSKIN
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


