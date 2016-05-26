/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOCANMESGWATCHDOG_H_
#define _EOCANMESGWATCHDOG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOCanMsgWatchdog.h
    @brief      This file provides interfaces to the watchdog of can messages receiving
    @author     valentina.gaggero@iit.it
    @date       19/05/2016
**/

/** @defgroup eo_EOCanMsgWatchdog Object EOCanMsgWatchdog
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOCanMSgWatchdog_hid EOCanMsgWatchdog;


typedef struct
{
    uint16_t    numoffailures;
    eOvoid_fp_void_t functiononfailure;
    
} eOcanmsg_watchdog_diagnostic_cfg_t;
typedef struct
{
    eOreltime_t  period;
    eOcanmsg_watchdog_diagnostic_cfg_t  diagncfg;
} eOcanmsg_watchdog_cfg_t;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// ***** Default cfg:
//eOcanmsg_watchdog_cfg_t cfg_default = {.period = 100}; //100 ms
//

//if cfg is null, then default cfg will be used
extern EOCanMsgWatchdog* eo_canmsg_watchdog_new(eOcanmsg_watchdog_cfg_t *cfg);

//update the configuration of watchdog. return eores_NOK_generic if watchdog ptr is null.
// if cfg is null, then default cfg will be used.
//if watchdog is running, then stops it configures new period and then start it again.
extern eOresult_t eo_canmsg_watchdog_updateconfigperiod(EOCanMsgWatchdog* wd, eOreltime_t period);

extern eOresult_t eo_canmsg_watchdog_rearm(EOCanMsgWatchdog* wd);

extern eOresult_t eo_canmsg_watchdog_start(EOCanMsgWatchdog* wd);

extern eOresult_t eo_canmsg_watchdog_stop(EOCanMsgWatchdog* wd);

//return false if watchdof has expired
extern eObool_t eo_canmsg_watchdog_check(EOCanMsgWatchdog* wd);


/** @}            
    end of group eo_EOCanMsgWatchdog
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


