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
} eOcandiscovery_cfg_t;  


typedef eOresult_t (*eOcandiscovery_onstop_fun_t) (void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

typedef struct
{
    eOcandiscovery_onstop_fun_t function;
    void*                       parameter;    
} eOcandiscovery_onstop_t;


typedef struct
{   // 6+2*2=10
    eObrd_info_t                info;  
    uint16_t                    canmap[eOcanports_number];  // use bitmap of required can addresses.
} eOcandiscovery_target_t;      EO_VERIFYsizeof(eOcandiscovery_target_t, 10);

typedef struct
{
    eObrd_info_t                info;       // the board info as given by the get-fw-version-reply message
    uint16_t                    time;       // the time in ms after the start of search when the get-fw-version-reply message has arrived
} eOcandiscovery_board_t;       EO_VERIFYsizeof(eOcandiscovery_board_t, 8);

typedef struct
{
    eObool_t                    allhavereplied;                     // tells if all boards in target have replied to the get-fw-error.
    eObool_t                    atleastonereplyisincompatible;      // tells if at least one board which has replied is not compatible with the target
    uint16_t                    duration;                           // in ms   
    uint16_t                    replies[eOcanports_number];         // bit mask of the boards which have replies: replies[eOcanport1] & 0x0080 == 0x0080 means board 3 on CAN1 has replies
    uint16_t                    incompatibilities[eOcanports_number]; // bit mask of board which have replies but are incompatible   
    uint16_t                    differentboardtype[eOcanports_number]; // contain the bitmask of not equal board type (there is a 1 if and only if detected board != board required
    uint16_t                    differentfirmwareversion[eOcanports_number]; // contain the bitmask of not equal fw version (there is a 1 if and only if fw detected != fw required
    uint16_t                    differentprotocolversion[eOcanports_number]; // contain the bitmask of not equal prot version (there is a 1 if and only if prot detected != prot required   
    eOcandiscovery_board_t      boards[eOcanports_number][15];      // contains the board information as given by the the get-fw-version replies + detection relative time in ms
} eOcandiscovery_detection_t;   EO_VERIFYsizeof(eOcandiscovery_detection_t, 264);
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOcandiscovery_cfg_t eo_candiscovery_default_cfg; // = { .period = 100*1000, .timeout = 3*1000*1000 };

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheCANdiscovery2* eo_candiscovery2_Initialise(const eOcandiscovery_cfg_t* cfg);

extern EOtheCANdiscovery2* eo_candiscovery2_GetHandle(void);


// call it to start the discovery procedure on a given set of can boards which share tyep, fw version, prot version
extern eOresult_t eo_candiscovery2_Start(EOtheCANdiscovery2 *p, const eOcandiscovery_target_t *target, eOcandiscovery_onstop_t* onstop);

//  call it in the can parser when a board replies to the get-fw-version request
extern eOresult_t eo_candiscovery2_OneBoardIsFound(EOtheCANdiscovery2 *p, eObrd_canlocation_t loc, eObool_t match, eObrd_info_t *detected);

// call it regularly inside the control-loop or upon event received by the configurator task.

extern eOresult_t eo_candiscovery2_Tick(EOtheCANdiscovery2 *p);

// call it only if you want to force a stop of the discovery .... Tick() and OneBoardisFound() already call it on timeout or on all board found.
extern eOresult_t eo_candiscovery2_Stop(EOtheCANdiscovery2 *p);


extern eOresult_t eo_candiscovery2_SendLatestSearchResults(EOtheCANdiscovery2 *p);

extern eObool_t eo_candiscovery2_IsSearchOK(EOtheCANdiscovery2 *p);

extern const eOcandiscovery_target_t* eo_candiscovery2_GetTarget(EOtheCANdiscovery2 *p);


extern const eOcandiscovery_detection_t* eo_candiscovery2_GetDetection(EOtheCANdiscovery2 *p);

/** @}            
    end of group eo_EOtheCANdiscovery2
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


