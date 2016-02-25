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
#ifndef _EOTHEMAIS_H_
#define _EOTHEMAIS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheMAIS.h
    @brief      this object implements what is required for managing the MAIS.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheMAIS Object EOtheMAIS
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

typedef struct EOtheMAIS_hid EOtheMAIS;


//typedef eOresult_t (*eOmais_onendofoperation_fun_t) (EOtheMAIS* p, eObool_t operationisok);


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheMAIS* eo_mais_Initialise(void);

extern EOtheMAIS* eo_mais_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_mais_GetServiceState(EOtheMAIS *p);
extern eOresult_t eo_mais_SendReport(EOtheMAIS *p);


extern eOresult_t eo_mais_Verify(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_mais_Activate(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_mais_Deactivate(EOtheMAIS *p);

extern eOresult_t eo_mais_Start(EOtheMAIS *p);

extern eOresult_t eo_mais_SetRegulars(EOtheMAIS *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_mais_Tick(EOtheMAIS *p);

extern eOresult_t eo_mais_Stop(EOtheMAIS *p);

// it enables/disables transmission of the mais board. _Start() just starts the service, not the transmission
extern eOresult_t eo_mais_Transmission(EOtheMAIS *p, eObool_t on);



// we can call them if _Activate() was called. they are used by the callbacks of eth protocol if robotInterface uses a mais device.
extern eOresult_t eo_mais_Set(EOtheMAIS *p, eOas_mais_config_t* maiscfg);
extern eOresult_t eo_mais_SetMode(EOtheMAIS *p, eOas_maismode_t mode);
extern eOresult_t eo_mais_SetDataRate(EOtheMAIS *p, uint8_t datarate);
extern eOresult_t eo_mais_SetResolution(EOtheMAIS *p, eOas_maisresolution_t resolution);



/** @}            
    end of group eo_EOtheMAIS
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


