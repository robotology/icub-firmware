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
#ifndef _EOTHESERVICES0_H_
#define _EOTHESERVICES0_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheServices0.h
    @brief      This file ...                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOtheServices0 Object EOtheServices0
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
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

typedef struct EOtheServices0_hid EOtheServices0;



typedef struct
{
    uint8_t     whatever;
} eOserv0_cfg_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheServices0* eo_serv0_Initialise(eOserv0_cfg_t *cfg);

extern EOtheServices0* eo_serv0_GetHandle(void);

extern eOresult_t eo_serv0_ConfigMC(EOtheServices0 *p, eOmcconfig_cfg_t *mccfg);

extern eOresult_t eo_serv0_InitializeCurrentsWatchdog(EOtheServices0 *p);

extern eOresult_t eo_serv0_ConfigCAN(EOtheServices0 *p, eOcanserv_cfg_t *cancfg);

extern eOresult_t eo_serv0_StartCANdiscovery(EOtheServices0 *p);

extern eObool_t eo_serv0_IsBoardReadyForControlLoop(EOtheServices0 *p);

extern eOresult_t eo_serv0_SetBoardReadyForControlLoop(EOtheServices0 *p);

extern eOresult_t eo_serv0_SendDiscoveryFailureReport(EOtheServices0 *p);


/** @}            
    end of group eo_EOtheServices0
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


