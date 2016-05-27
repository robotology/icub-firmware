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
#ifndef _EOWATCHDOG_H_
#define _EOWATCHDOG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOwatchdog.h
    @brief      This file provides interfaces to watchdog
    @author     valentina.gaggero@iit.it
    @date       19/05/2016
**/

/** @defgroup eo_EOwatchdog Object EOwatchdog
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOwatchdog_hid EOwatchdog;


typedef struct
{
    uint16_t            numoffailures;
    eOvoid_fp_void_t    functiononfailure; //this function is invoked inside _check function at first failure and every "numoffailures" failures consecutive
    
} eOwatchdog_diagnostics_cfg_t;
typedef struct
{
    eOreltime_t                     period; //time expressed in microsec
    eOwatchdog_diagnostics_cfg_t    diagncfg;
} eOwatchdog_cfg_t;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/* ***** Default cfg:
//eOwatchdog_cfg_t cfg_default = 
{
    diagncfg = 
    {
        .numofnumoffailures = 0,
        .functiononfailure = NULL,
    },
    .period = 100000 //1 sec
}; 
*/

//if cfg is null, then default cfg will be used (see above)
extern EOwatchdog* eo_watchdog_new(eOwatchdog_cfg_t *cfg);

//update the period of watchdog. return eores_NOK_generic if watchdog ptr is null.
//if watchdog is running, then stops it configures new period and then start it again.
extern eOresult_t eo_watchdog_updateconfigperiod(EOwatchdog* wd, eOreltime_t period);

extern eOresult_t eo_watchdog_rearm(EOwatchdog* wd);

extern eOresult_t eo_watchdog_start(EOwatchdog* wd);

extern eOresult_t eo_watchdog_stop(EOwatchdog* wd);

//return false if watchdog has expired
extern eObool_t eo_watchdog_check(EOwatchdog* wd);


/** @}            
    end of group eo_EOwatchdog
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


