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
#ifndef _EOTHEETHMONITOR_H_
#define _EOTHEETHMONITOR_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheETHmonitor.h
    @brief      this object implements what is required for managing the SKIN.                
    @author     marco.accame@iit.it
    @date       12/12/2015
**/

/** @defgroup eo_EOtheETHmonitor Object EOtheETHmonitor
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheETHmonitor_hid EOtheETHmonitor;

typedef struct
{
    uint8_t         priority;          
    uint16_t        stacksize;
    eOreltime_t     period; 
} eOethmonitor_cfg_t;
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOethmonitor_cfg_t eo_ethmonitor_DefaultCfg;

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheETHmonitor* eo_ethmonitor_Initialise(const eOethmonitor_cfg_t *cfg);

extern EOtheETHmonitor* eo_ethmonitor_GetHandle(void);

extern eOresult_t eo_ethmonitor_Start(EOtheETHmonitor *p);

extern eOresult_t eo_ethmonitor_SetAlert(EOtheETHmonitor *p, EOMtask *task, eOevent_t event);

extern eOresult_t eo_ethmonitor_Tick(EOtheETHmonitor *p);

extern eOresult_t eo_ethmonitor_Stop(EOtheETHmonitor *p);






/** @}            
    end of group eo_EOtheETHmonitor
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


