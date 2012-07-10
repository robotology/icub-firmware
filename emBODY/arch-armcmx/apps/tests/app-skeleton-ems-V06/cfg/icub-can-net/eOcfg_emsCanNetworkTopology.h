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
#ifndef _EOCFG_EOEMSCANNETWORKTOPOLOGY_H_
#define _EOCFG_EOEMSCANNETWORKTOPOLOGY_H_




/** @file       eOcfg_emsCanNetworkTopology.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
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

//extern const EOconstLookupTbl* const emsCanNetTopo_motorBoardCanLoc2NvsRef_LUTbl__ptr[eo_emsCanNetTopo_canports_num];
//extern const EOconstLookupTbl *emsCanNetTopo_joint2BoardCanLocation_LUTbl__ptr;


//extern const EOconstvector* const eo_cfg_emsCanNetTopo_joint_constvector_ems1__ptr;

extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_joints__ptr;
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_motors__ptr;
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_sensors__ptr;
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_skin__ptr;
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_boards__ptr;

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_icubCanProto 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




