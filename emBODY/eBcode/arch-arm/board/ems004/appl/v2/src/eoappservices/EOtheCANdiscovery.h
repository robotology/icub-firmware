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
#ifndef _EOTHECANDISCOVERY_H_
#define _EOTHECANDISCOVERY_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheCANdiscovery.h
    @brief      this object implements what is required for managing the MAIS.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheCANdiscovery Object EOtheCANdiscovery
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheCANdiscovery_hid EOtheCANdiscovery;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheCANdiscovery* eo_candiscovery_Initialise(void);

extern EOtheCANdiscovery* eo_candiscovery_GetHandle(void);


extern eOresult_t eo_candiscovery_Start(EOtheCANdiscovery *p);

extern eOresult_t eo_candiscovery_Stop(EOtheCANdiscovery *p);


extern eOresult_t eo_candiscovery_SignalDetectedCANboards(EOtheCANdiscovery *p);

extern eOresult_t eo_candiscovery_CheckCanBoardsAreReady(EOtheCANdiscovery *p, uint32_t dontaskmask);

extern eObool_t eo_candiscovery_areCanBoardsReady(EOtheCANdiscovery *p, uint32_t *canBoardsReady, uint32_t *canBoardsChecked);

extern eOresult_t eo_candiscovery_ManageDetectedBoard(EOtheCANdiscovery *p, eOcanmap_location_t loc, eObool_t match, eObrd_typeandversions_t *detected);

/** @}            
    end of group eo_EOtheCANdiscovery
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


