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

#error ::: DONT USE IT: USE EOtheCANdiscovery2 INSTEAD

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHECANDISCOVERY_H_
#define _EOTHECANDISCOVERY_H_


#ifdef __cplusplus
extern "C" {
#endif

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
#include "EOtheCANmapping.h"

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

extern eOresult_t eo_candiscovery_ResetAndStartProcedure(EOtheCANdiscovery *p);

extern eOresult_t eo_candiscovery_CheckRemainingCanBoards(EOtheCANdiscovery *p);

extern eOresult_t eo_candiscovery_EvaluateDiscoveredResources(EOtheCANdiscovery *p);

extern eOresult_t eo_candiscovery_ManageDetectedBoard(EOtheCANdiscovery *p, eOcanmap_location_t loc, eObool_t match, eObrd_typeandversions_t *detected);

extern eObool_t eo_candiscovery_isMCReady(EOtheCANdiscovery *p);

extern eObool_t eo_candiscovery_isMAISReady(EOtheCANdiscovery *p);

extern void eo_candiscovery_SendDiagnosticsAboutBoardsWithIssues(EOtheCANdiscovery *p);

/* DEPRECATED APIs */
extern eOresult_t eo_candiscovery_CheckCanBoardsAreReady(EOtheCANdiscovery *p, uint32_t dontaskmask);
extern eObool_t eo_candiscovery_areCanBoardsReady(EOtheCANdiscovery *p, uint32_t *canBoardsReady, uint32_t *canBoardsChecked);

/** @}            
    end of group eo_EOtheCANdiscovery
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


