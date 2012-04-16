/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOEMSCANNETWORKTOPOLOGY_H_
#define _EOEMSCANNETWORKTOPOLOGY_H_


/** @file       EoemsCanNetworkTopology.h
	@brief      This header file gives 
	@author     valentina.gagger@iit.it
	@date       03/08/2012
**/

/** @defgroup eo_emsCanNetTopo Object EOemsCanNetTopo
    under costruction......
   
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EoUtilities.h"
#include "EoBoards.h"
#include "EoMotionControl.h"
#include "EoSensors.h"
#include "EOicubCanProto_specifications.h"
#include "EOconstLookupTbl.h"

#include "EOnv.h"
#include "EOnvsCfg.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOemsCanNetTopo_hid EOemsCanNetTopo
    @brief      .....
 **/  
typedef struct EOemsCanNetTopo_hid EOemsCanNetTopo;


/** @typedef    typedef     struct             eo_emsCanNetTopo_cfg_t
    @brief      contains data to configure an EOemsCanNetTopo object.
 **/
typedef struct
{
    EOnvsCfg                           *nvsCfg;
    const EOconstLookupTbl /*const*/  *joint2BoardCanLocation_LUTbl_ptr;
    const EOconstLookupTbl* const   *motorBoardCanLoc2NvsRef_LUTbl_ptr/*[eOutil_canports_num]*/;
    //if you need, put here other stuff 
} eo_emsCanNetTopo_cfg_t;


/** @typedef    enum { eo_emsCanNetTopo_canports_num = 2 }
    @brief      contains the number of can port on ems board.
    @warning    pay attention to eOcanport_t define d in eOcommon.h
 **/
enum { eo_emsCanNetTopo_canports_num = 2 }; 
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
//currently does not exist default config.
//extern const eo_icubCanProto_cfg_t eo_icubCanProto_cfg_default; 

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOemsCanNetTopo* eo_emsCanNetTopo_New(/*const*/ eo_emsCanNetTopo_cfg_t *cfg);

extern eOresult_t eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(EOemsCanNetTopo *p, eOcanport_t canPort, eo_icubCanProto_canBoardAddress_t boardAddr,
                                                                        eo_icubCanProto_motorAxis_t axis, EOnv** nvt_ptr);

extern eOresult_t eo_emsCanNetTopo_GetCanLocation_ByJointUniqueId(EOemsCanNetTopo *p, eOmc_jointUniqueId_t jUniqueId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr, eo_icubCanProto_motorAxis_t *axis);

extern eOresult_t eo_emsCanNetTopo_GetCanLocation_ByJointBoardId(EOemsCanNetTopo *p, eOmc_jointBoardId_t jBoardId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr, eo_icubCanProto_motorAxis_t *axis);

extern eOresult_t eo_emsCanNetTopo_GetCanLocation_BySensorId(EOemsCanNetTopo *p, eOsnsr_sensorId_t sensorId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr);

extern eOresult_t eo_emsCanNetTopo_GetCanLocation_ByMotorUniqueId(EOemsCanNetTopo *p, eOmc_motorUniqueId_t mUniqueId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr, eo_icubCanProto_motorAxis_t *axis);



// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group eo_emsCanNetTopo  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


