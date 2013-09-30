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
#ifndef _EOTHEEMASBACKDOORCFG_H_
#define _EOTHEEMASBACKDOORCFG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


/** @file       OPCprotocolManager_Cfg.h
    @brief      This file provides interface to the configuration of OPCprotoclManager shared between pc104 and ems boards.
    @author     valentina.gaggero@iit.it
    @date       04/29/2013
**/

/** @defgroup OPCprotocolManager_Cfg
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "OPCprotocolManager.h"


// - public #define  --------------------------------------------------------------------------------------------------
#define eodgn_nvidbdoor_cmds            1
#define eodgn_nvidbdoor_errorlog        2
#define eodgn_nvidbdoor_emsperiph       3
#define eodgn_nvidbdoor_emsapplcommon   4
#define eodgn_nvidbdoor_emsapplmc       5
#define eodgn_nvidbdoor_motorstatus     6
#define eodgn_nvidbdoor_jointsStateCmd  7



#define eodgn_nvidbdoor_max             8



//use nv id of backdoor from 11 because values from 1 to 10 are used for diagnostics porpouse.
#define opcCfg_nvid_bdoor_dgn_offset                       10

//pay attention: update nvid_bdoor_user_num value. it is the numeber of nvid you use.
#define  opcCfg_nvid_bdoor_user_num                         0


/*example: 
#define 	eom_ipnet_hid_DEBUG_id 		nvid_bdoor_offset +	1
#define  nvid_bdoor_user_num            1
*/

// - declaration of public user-defined types ------------------------------------------------------------------------- 




// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
extern opcprotman_cfg_t* OPCprotocolManager_Cfg_getconfig(void);
  

/** @}            
    end of group OPCprotocolManager_Cfg
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif  
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


