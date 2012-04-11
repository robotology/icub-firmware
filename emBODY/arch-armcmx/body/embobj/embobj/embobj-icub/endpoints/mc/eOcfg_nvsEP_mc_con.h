/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOCFG_NVSEP_MC_CON_H_
#define _EOCFG_NVSEP_MC_CON_H_




/** @file       eOcfg_nvsEP_mc_con.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_uilsdede2345er Configuation of the ......
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------

// -- max mumber of joints and motors
#define EOK_cfg_nvsEP_mc_con_bodypart_maxnumof_joints                            20
#define EOK_cfg_nvsEP_mc_con_bodypart_maxnumof_motors                            20

// -- max number of id per joint
#define EOK_cfg_nvsEP_mc_con_joint_maxnumof_id                                   32

// -- max number of id per motor
#define EOK_cfg_nvsEP_mc_con_motor_maxnumof_id                                   16

#warning --> add a control so that max number of id per motor and joint is not higher than 1024. if it is ... then ... dont use EO_nv_FUNTYP() 

// -- the starting id of a joint: max 32 id per joint
#define EOK_cfg_nvsEP_mc_con_bodypart_nvidstart_in_joint(j)                     ((j)*EOK_cfg_nvsEP_mc_con_joint_maxnumof_id) 
// -- the starting id of a motor: max 16 id per motor
#define EOK_cfg_nvsEP_mc_con_bodypart_nvidstart_in_motor(m)                     (EOK_cfg_nvsEP_mc_con_bodypart_nvidstart_in_joint(EOK_cfg_nvsEP_mc_con_bodypart_maxnumof_joints)+(m)*EOK_cfg_nvsEP_mc_con_motor_maxnumof_id)                                               




// - declaration of public user-defined types ------------------------------------------------------------------------- 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group eo_uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




