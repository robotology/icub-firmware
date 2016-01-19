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
#ifndef _EOCANSERVICE_H_
#define _EOCANSERVICE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOcanService.h
    @brief      This file ...                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOcanService Object EOcanService
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOcanService_hid EOcanService;



typedef struct
{
    uint8_t     whatever;
} eOcanserv_cfg_t;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOcanService* eo_canserv_Initialise(eOcanserv_cfg_t *cfg);

extern EOcanService* eo_canserv_GetHandle(void);




/** @}            
    end of group eo_EOcanService
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


