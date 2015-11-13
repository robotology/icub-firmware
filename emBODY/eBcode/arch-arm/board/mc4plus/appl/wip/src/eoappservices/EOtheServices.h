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
    @brief      This file ...                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOtheServices Object EOtheServices
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"

#include "EOmcService.h"
#include "EOtheMAIS.h"
#include "EOtheCANservice.h"
#include "EOtheCANdiscovery2.h"
#include "EOCurrentsWatchdog.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheServices_hid EOtheServices;



typedef struct
{
    uint8_t     whatever;
} eOserv_cfg_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheServices* eo_serv_Initialise(eOserv_cfg_t *cfg);

extern EOtheServices* eo_serv_GetHandle(void);

extern eOresult_t eo_serv_ConfigMC(EOtheServices *p, eOmcconfig_cfg_t *mccfg);

extern eOresult_t eo_serv_InitializeCurrentsWatchdog(EOtheServices *p);

extern eOresult_t eo_serv_ConfigCAN(EOtheServices *p, eOcanserv_cfg_t *cancfg);

extern eOresult_t eo_serv_StartCANdiscovery(EOtheServices *p);

extern eObool_t eo_serv_IsBoardReadyForControlLoop(EOtheServices *p);

extern eOresult_t eo_serv_SendDiscoveryFailureReport(EOtheServices *p);


/** @}            
    end of group eo_EOtheServices
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


