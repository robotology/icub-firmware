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
#ifndef _EOCFG_APPTHEDATABASE_H_
#define _EOCFG_APPTHEDATABASE_H_




/** @file       eOcfg_appTheDataBase.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       09/21/2012
**/

/** @defgroup eo_icubCanProto Configuation of the messages lookup tables.
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------
#include "Eocommon.h"
#include "EoBoards.h"
#include "EOconstvector.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern    const EOconstvector* const      eo_cfg_appDB_thecanboards;                    /**< item is a eOappTheDB_canboardinfo_t. it keeps the CAN boards on the ems */
extern    const EOconstvector* const      eo_cfg_appDB_thejoints_mapping2canboards;     /**< item is a eOappTheDB_mapping2canboard_t. it keeps info of can location of the CAN joints managed by the ems */
extern    const EOconstvector* const      eo_cfg_appDB_themotors_mapping2canboards;     /**< item is a eOappTheDB_mapping2canboard_t. it keeps info of can location of the CAN motors managed by the ems */
extern    const EOconstvector* const      eo_cfg_appDB_themaises_mapping2canboards;     /**< item is a eOappTheDB_mapping2canboard_t. it keeps info of can location of the CAN mais sensors managed by the ems */
extern    const EOconstvector* const      eo_cfg_appDB_thestrains_mapping2canboards;    /**< item is a eOappTheDB_mapping2canboard_t. it keeps info of can location of the CAN strain sensors managed by the ems */
extern    const EOconstvector* const      eo_cfg_appDB_theskins_info;                   /**< item is a eOappTheDB_cfg_skinInfo_t. it keeps info of the set of can skin boards managed by an EMS board */


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_icubCanProto 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




