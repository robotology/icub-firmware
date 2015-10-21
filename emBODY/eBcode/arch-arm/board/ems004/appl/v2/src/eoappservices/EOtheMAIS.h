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

extern eOresult_t eo_mais_Verify(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

// it activates the mais service by loading the service configuration
extern eOresult_t eo_mais_Activate(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg);

// it deactivates service.
extern eOresult_t eo_mais_Deactivate(EOtheMAIS *p);


// Start() take
extern eOresult_t eo_mais_Start(EOtheMAIS *p);

extern eOresult_t eo_mais_Tick(EOtheMAIS *p);

extern eOresult_t eo_mais_Stop(EOtheMAIS *p);



extern eOresult_t eo_mais_TXstart(EOtheMAIS *p, uint8_t datarate, eOas_maismode_t mode, eOas_maisresolution_t resolution);

extern eOresult_t eo_mais_TXstop(EOtheMAIS *p);

extern eOresult_t eo_mais_Set(EOtheMAIS *p, eOas_mais_config_t* maiscfg);

extern eOresult_t eo_mais_SetMode(EOtheMAIS *p, eOas_maismode_t mode);

extern eOresult_t eo_mais_SetDataRate(EOtheMAIS *p, uint8_t datarate);

extern eOresult_t eo_mais_SetResolution(EOtheMAIS *p, eOas_maisresolution_t resolution);



/** @}            
    end of group eo_EOtheMAIS
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


