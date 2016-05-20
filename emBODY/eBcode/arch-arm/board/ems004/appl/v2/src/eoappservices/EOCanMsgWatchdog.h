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

typedef struct EOCanMSgWatchdog_hid EOCanMSgWatchdog;


typedef struct
{
    eOreltime_t  periodofCanMsg;
    eOreltime_t  timeout;
} eOcanmsg_watchdog_cfg_t;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOCanMsgsWatchdog* eo_canmsg_watchdog_new(eOcanmsg_watchdog_cfg_t *cfg);

//configure the watchdog. return true if watchdog is configured
extern eObool_t eo_canmsg_watchdog_config(EOCanMsgsWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg);

extern eOresult_t eo_canmsg_watchdog_rearm(EOCanMsgsWatchdog* wd);

extern eOresult_t eo_canmsg_watchdog_start(EOCanMsgsWatchdog* wd);

extern eOresult_t eo_canmsg_watchdog_stop(EOCanMsgsWatchdog* wd);

extern eObool_t eo_canmsg_watchdog_check(EOCanMsgsWatchdog* wd);


/** @}            
    end of group eo_EOCanMsgWatchdog
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


