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
#ifndef _EOTHEMC4BOARDS_H_
#define _EOTHEMC4BOARDS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheMC4boards.h
    @brief      this object implements what is required for managing the MAIS.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheMC4boards Object EOtheMC4boards
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "iCubCanProto_types.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheMC4boards_hid EOtheMC4boards;



typedef struct
{
    icubCanProto_velocityShift_t    velshift;
    icubCanProto_estimShift_t       estimshifts;
} eOmc4boards_shiftvalues_t;   


enum { eoemc4boards_broadcastpolicylistsize = 4 };
typedef struct
{
    uint8_t values[eoemc4boards_broadcastpolicylistsize];
} eOmc4boards_broadcastpolicy_t;       
    



typedef struct
{    
    eOmc4boards_broadcastpolicy_t   broadcastpolicy;  
    eOmc4boards_shiftvalues_t       shiftvalues;       
} eOmc4boards_config_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheMC4boards* eo_mc4boards_Initialise(const eOmc4boards_config_t *cfg);

extern EOtheMC4boards* eo_mc4boards_GetHandle(void);

extern eOresult_t eo_mc4boards_BroadcastStart(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_BroadcastStop(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_ConfigShiftValues(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_ConvertBLABLA(EOtheMC4boards *p, uint8_t from, uint8_t *to);





/** @}            
    end of group eo_EOtheMC4boards
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


