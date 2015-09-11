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
#ifndef _EOTHECANDISCOVERY2_H_
#define _EOTHECANDISCOVERY2_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheCANdiscovery2.h
    @brief      this object implements what is required for managing the MAIS.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheCANdiscovery2 Object EOtheCANdiscovery2
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheCANmapping.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheCANdiscovery2_hid EOtheCANdiscovery2;

typedef struct
{
    eOreltime_t             period;     /**< period of sending the get-fw-version can messages, expressed in microsec */
    eOreltime_t             timeout;    /**< timeout of a search procedure stared by eo_candiscovery2_Start(), expressed in microsec */
} eo_candiscovery_cfg_t;   

typedef struct
{
    uint8_t                 type;               // use eObrd_cantype_t
    uint8_t                 filler[3];
    eObrd_version_t         firmwareversion;
    eObrd_version_t         protocolversion;    
    uint16_t                canmap[2];          // use bitmap of required can addresses.
} eo_candiscovery_set_t;    EO_VERIFYsizeof(eo_candiscovery_set_t, 12);
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eo_candiscovery_cfg_t eo_candiscovery_default_cfg; // = { .period = 100*1000, .timeout = 3*1000*1000 };

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheCANdiscovery2* eo_candiscovery2_Initialise(const eo_candiscovery_cfg_t* cfg);

extern EOtheCANdiscovery2* eo_candiscovery2_GetHandle(void);


// call it to start the discovery procedure on a given set of can boards which share tyep, fw version, prot version
extern eOresult_t eo_candiscovery2_Start(EOtheCANdiscovery2 *p, const eo_candiscovery_set_t *set);

//  call it in the can parser when a board replies to the get-fw-version request
extern eOresult_t eo_candiscovery2_OneBoardIsFound(EOtheCANdiscovery2 *p, eOcanmap_location_t loc, eObool_t match, eObrd_typeandversions_t *detected);

// call it regularly inside the control-loop or upon event received by the configurator task.

extern eOresult_t eo_candiscovery2_Tick(EOtheCANdiscovery2 *p);

// call it only if you want to force a stop of the discovery .... Tick() and OneBoardisFound() already call it on timeout or on all board found.
extern eOresult_t eo_candiscovery2_Stop(EOtheCANdiscovery2 *p);






/** @}            
    end of group eo_EOtheCANdiscovery2
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


