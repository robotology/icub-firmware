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

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheSTRAIN_hid EOtheSTRAIN;



typedef eOresult_t (*eOstrain_onendofoperation_fun_t) (EOtheSTRAIN* p, eObool_t operationisok);
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheSTRAIN* eo_strain_Initialise(void);

extern EOtheSTRAIN* eo_strain_GetHandle(void);


// it verifies if the service as defined in te configuration is possible (is there a good strain board or not?), it executes a callback
// (which may send a confirmation to the entity which asked fot verification), and then it may activate the strain service by calling  eo_strain_Activate().
extern eOresult_t eo_strain_Verify(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg, eOstrain_onendofoperation_fun_t onverify, eObool_t activateafterverify);

// it activates the strain service by loading the service configuration
extern eOresult_t eo_strain_Activate(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg);

// it deactivates service.
extern eOresult_t eo_strain_Deactivate(EOtheSTRAIN *p);


// i can call the following only if ... service is activated.

extern eOresult_t eo_strain_TXstart(EOtheSTRAIN *p, uint8_t datarate, eOas_strainmode_t mode);

extern eOresult_t eo_strain_TXstop(EOtheSTRAIN *p);


// if overrideonfullscaleready is NULL the function eo_strain_GetFullScale() signals the fullscale to robotInterface ....
// if overrideonfullscaleready is not NULL the function eo_strain_GetFullScale() just calls it
extern eOresult_t eo_strain_GetFullScale(EOtheSTRAIN *p, eOstrain_onendofoperation_fun_t overrideonfullscaleready);


// the following are more tricky funtions .... shall we keep tehm?

// it sets the strain tx mode according to the mode is kept in the ram of the entity.
// this function is used only in RUN:on-entry. 
// i suggest to remove this thing.
extern eOresult_t eo_strain_SendTXmode(EOtheSTRAIN *p);


// following funtions apply the settings in argument by sending proper messages to the strain board
extern eOresult_t eo_strain_Set(EOtheSTRAIN *p, eOas_strain_config_t *cfg);

extern eOresult_t eo_strain_SetMode(EOtheSTRAIN *p, eOas_strainmode_t mode);

extern eOresult_t eo_strain_SetDataRate(EOtheSTRAIN *p, uint8_t datarate);



/** @}            
    end of group eo_EOtheSTRAIN
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


