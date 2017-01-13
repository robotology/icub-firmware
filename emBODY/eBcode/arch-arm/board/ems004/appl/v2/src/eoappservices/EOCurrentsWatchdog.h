/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
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
#ifndef _EOCURRENTSWATCHDOG_H_
#define _EOCURRENTSWATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOCurrentsWatchdog.h
    @brief      This file provides interfaces to the watchdog of the currents                
    @author     davide.pollarolo@iit.it
    @date       27/10/2015
**/

/** @defgroup eo_EOCurrentsWatchdog Object EOCurrentsWatchdog
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOCurrentsWatchdog_hid EOCurrentsWatchdog;


typedef struct
{
    uint32_t*     i2t_thresh;
	uint16_t*     spike_thresh;
} eOcurrents_watchdog_cfg_t;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOCurrentsWatchdog* eo_currents_watchdog_Initialise(void);

extern EOCurrentsWatchdog* eo_currents_watchdog_GetHandle(void);

extern void eo_currents_watchdog_Tick(EOCurrentsWatchdog* p, int16_t voltage, int16_t *currents);

extern eOresult_t eo_currents_watchdog_UpdateCurrentLimits(EOCurrentsWatchdog* p, uint8_t motor);



/** @}            
    end of group eo_EOCurrentsWatchdog
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


