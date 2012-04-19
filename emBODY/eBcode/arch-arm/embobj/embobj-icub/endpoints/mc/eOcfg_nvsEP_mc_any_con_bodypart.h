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
#include "eOcfg_nvsEP_mc_any_con_jxx.h"
#include "eOcfg_nvsEP_mc_any_con_mxx.h"


// - public #define  --------------------------------------------------------------------------------------------------

     


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t;
    @brief      It contains an index for all the joints in a bodypart. In a given bodypart, there are joints for a number lower
                than jointNumber_TOTALnumber. The numbers are consecutive and without holes.
 **/
typedef enum
{
    jointNumber_00 = 0,   
    jointNumber_01, jointNumber_02, jointNumber_03, jointNumber_04, jointNumber_05, jointNumber_06, jointNumber_07, jointNumber_08, jointNumber_09, jointNumber_10,
    jointNumber_11, jointNumber_12, jointNumber_13, jointNumber_14, jointNumber_15, jointNumber_16, jointNumber_17, jointNumber_18, jointNumber_19
} eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t;

enum { jointNumber_TOTALnumber = 20};


/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t;
    @brief      It contains an index for all the network variables in a joint. The indices are consecutive and without
                holes.
 **/
typedef enum
{
    jointNVindex_jconfig                                =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig,                                  /**< 0 */
    jointNVindex_jconfig__pidposition                   =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition,                     /**< 1 */
    jointNVindex_jconfig__pidvelocity                   =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity,                     /**< 2 */
    jointNVindex_jconfig__pidtorque                     =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque,                       /**< 3 */
    jointNVindex_jconfig__minpositionofjoint            =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint,              /**< 4 */
    jointNVindex_jconfig__maxpositionofjoint            =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint,              /**< 5 */
    jointNVindex_jconfig__velocitysetpointtimeout       =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout,         /**< 6 */
    jointNVindex_jconfig__upto02descrforchameleon02     =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__upto02descrforchameleon02,       /**< 7 */
    jointNVindex__jstatus                               =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__jstatus,                                 /**< 8 */
    jointNVindex__calibrator                            =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__calibrator,                              /**< 9 */
    jointNVindex__setpoint                              =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__setpoint,                                /**< 10 */
    jointNVindex__controlmode                           =   EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__controlmode                              /**< 11 */
} eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t;

enum { jointNVindex_TOTALnumber = EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber};



/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t;
    @brief      It contains an index for all the motors in a bodypart. In a given bodypart, there are motors for a number lower
                than motorNumber_TOTALnumber. The numbers are consecutive and without holes.
 **/
typedef enum
{
    motorNumber_00 = 0,   
    motorNumber_01, motorNumber_02, motorNumber_03, motorNumber_04, motorNumber_05, motorNumber_06, motorNumber_07, motorNumber_08, motorNumber_09, motorNumber_10,
    motorNumber_11, motorNumber_12, motorNumber_13, motorNumber_14, motorNumber_15, motorNumber_16, motorNumber_17, motorNumber_18, motorNumber_19
} eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t;

enum { motorNumber_TOTALnumber = 20};


/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t;
    @brief      It contains an index for all the network variables in a motor. The indices are consecutive and without
                holes.
 **/
typedef enum
{
    motorNVindex_mconfig                                =   EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig,
    motorNVindex_mconfig__pidcurrent                    =   EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__pidcurrent,
    motorNVindex_mconfig__maxvelocityofmotor            =   EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxvelocityofmotor,
    motorNVindex_mconfig__maxcurrentofmotor             =   EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxcurrentofmotor,
    motorNVindex_mconfig__upto02descrforchameleon06     =   EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__upto02descrforchameleon06,
    motorNVindex__mstatus                               =   EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex__mstatus
} eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t;

enum { motorNVindex_TOTALnumber = EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber};
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t j, eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t jnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e jnvindex for the joint number @e j
    @param      j               the joint number 
    @param      jnvinxed        the index of the nv inside the joint
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t j, eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t jnvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t m, eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the motor number @e m
    @param      m               the motor number 
    @param      mnvinxed        the index of the nv inside the motor
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t m, eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t mnvindex);


/** @}            
    end of group eo_2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




