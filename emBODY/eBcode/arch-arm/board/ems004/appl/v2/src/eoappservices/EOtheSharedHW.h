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
#ifndef _EOTHESHAREDHW_H_
#define _EOTHESHAREDHW_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheSharedHW.h
    @brief      this object implements what is required for managing the shared HW.                
    @author     marco.accame@iit.it
    @date       12/10/2015
**/

/** @defgroup eo_EOtheSharedHW Object EOtheSharedHW
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheSharedHW_hid EOtheSharedHW;


typedef struct
{
    uint8_t         dummy;
} eOsharedhw_cfg_t;


typedef enum
{
    eosharedhw_resource_I2C3    = 0,     
    
    eosharedhw_resource_unknown = 254,
    eosharedhw_resource_none    = 255    
} eOsharedhw_resource_t;

enum { eosharedhw_resources_numberof = 1 };

// readme: 
// i2c3 is used on: 1. micrel switch; 2. gyros on ems4; 3. accel on ems4.
// 1. micrel:
//    a. first call is done at init of the eth link inside funtion ipal_sys_start() called inside eom_emsappl_Initialise() 
//       with a call to eom_ipnet_Initialise(). In such a case, no other activities on i2c3 happen..
//    b. at regular queries done by a periodic task inside object EOtheETHmonitor. 
//       This object is initted inside eo_services_Initialise() and the tick of the task happens when the init task is stopped.
// 2. gyros:
//    a. at regular queries done by a reading of the gyroscope, if configured to run, inside the object EOtheIntertials2.
// 2. accel:
//    a. at regular queries done by a reading of the accelrometer, if configured to run, inside the object EOtheIntertials2.
// conclusion: protect i2c3 only in EOtheIntertials2 and EOtheETHmonitor.     

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOsharedhw_cfg_t eo_sharedhw_DefaultCfg;

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheSharedHW* eo_sharedhw_Initialise(const eOsharedhw_cfg_t *cf);

extern EOtheSharedHW* eo_sharedhw_GetHandle(void);

extern eOresult_t eo_sharedhw_Obtain(EOtheSharedHW *p, eOsharedhw_resource_t res, eOreltime_t timeout);

extern eOresult_t eo_sharedhw_Release(EOtheSharedHW *p, eOsharedhw_resource_t res);




/** @}            
    end of group eo_EOtheSharedHW
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


