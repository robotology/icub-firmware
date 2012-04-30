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
#ifndef _EOCFG_NVSEP_MC_ANY_CON_BODYPART_H_
#define _EOCFG_NVSEP_MC_ANY_CON_BODYPART_H_




/** @file       eOcfg_nvsEP_mc_any_con_bodypart.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_2uilsdede2345er Configuration of the bodypart 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EoMotionControl.h"
#include "eOcfg_nvsEP_mc.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t;
    @brief      It contains an index for all the joints in a bodypart. In a given bodypart, there are joints for a number lower
                than jointNumberBodypart_TOTALnumber. The numbers are consecutive and without holes, so that the enum value can be changed 
                by a normal index.
 **/
typedef enum
{
    jointNumberBodypart_00 = 0,   
    jointNumberBodypart_01, jointNumberBodypart_02, jointNumberBodypart_03, jointNumberBodypart_04, jointNumberBodypart_05, jointNumberBodypart_06, jointNumberBodypart_07, 
    jointNumberBodypart_08, jointNumberBodypart_09, jointNumberBodypart_10, jointNumberBodypart_11, jointNumberBodypart_12, jointNumberBodypart_13, jointNumberBodypart_14, jointNumberBodypart_15
} eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t;

enum { jointNumberBodypart_TOTALnumber = 16};



/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_motorNumberBodypart_t;
    @brief      It contains an index for all the motors in a bodypart. In a given bodypart, there are motors for a number lower
                than motorNumberBodypart_TOTALnumber. The numbers are consecutive and without holes.
 **/
typedef enum
{
    motorNumberBodypart_00 = 0,   
    motorNumberBodypart_01, motorNumberBodypart_02, motorNumberBodypart_03, motorNumberBodypart_04, motorNumberBodypart_05, motorNumberBodypart_06, motorNumberBodypart_07, 
    motorNumberBodypart_08, motorNumberBodypart_09, motorNumberBodypart_10, motorNumberBodypart_11, motorNumberBodypart_12, motorNumberBodypart_13, motorNumberBodypart_14, motorNumberBodypart_15
} eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t;

enum { motorNumberBodypart_TOTALnumber = 16};


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t j, eo_cfg_nvsEP_mc_jointNVindex_t jnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e jnvindex for the joint number @e j
    @param      j               the joint number 
    @param      jnvinxed        the index of the nv inside the joint
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t j, eo_cfg_nvsEP_mc_jointNVindex_t jnvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t m, eo_cfg_nvsEP_mc_motorNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the motor number @e m
    @param      m               the motor number 
    @param      mnvinxed        the index of the nv inside the motor
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t m, eo_cfg_nvsEP_mc_motorNVindex_t mnvindex);


/** @}            
    end of group eo_2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




