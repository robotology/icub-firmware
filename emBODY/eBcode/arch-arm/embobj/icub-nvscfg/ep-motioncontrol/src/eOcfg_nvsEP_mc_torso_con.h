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

#ifndef _EOCFG_NVSEP_MC_TORSO_CON_H_
#define _EOCFG_NVSEP_MC_TORSO_CON_H_




/** @file       eOcfg_nvsEP_mc_torso_con.h
	@brief      This header file gives the constant configuration for the NVs in the endpoint left leg
	@author     marco.accame@iit.it
	@date       04/05/2012
**/

/** @defgroup eo_asfdgr234 Configuation of the NVs ..
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOconstvector.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoMotionControl.h"
#include "eOcfg_nvsEP_mc_any_con_bodypart.h"


 

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section



// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eo_cfg_nvsEP_mc_torso_con_jointNumber_t
    @brief      It contains an index for all the joints the left leg. It is a subset of those 
                in eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t
 **/
typedef enum
{
    jointTorso_00 = 0,   
    jointTorso_01, jointTorso_02
} eo_cfg_nvsEP_mc_torso_con_jointNumber_t;

enum { jointTorso_TOTALnumber = 3}; 


/** @typedef    typedef eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t eo_cfg_nvsEP_mc_torso_con_jointNVindex_t
    @brief      It contains an index for all the network variables in a joint of the left leg. use the same type as in bodypart
 **/
typedef eOcfg_nvsEP_mc_jointNVindex_t eo_cfg_nvsEP_mc_torso_con_jointNVindex_t;


/** @typedef    typedef enum eo_cfg_nvsEP_mc_torso_con_motorNumber_t;
    @brief      It contains an index for all the motors the left leg. It is a subset of those 
                in eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t
 **/
typedef enum
{
    motorTorso_00 = 0,   
    motorTorso_01, motorTorso_02
} eo_cfg_nvsEP_mc_torso_con_motorNumber_t;

enum { motorTorso_TOTALnumber = 3}; 


/** @typedef    typedef eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t eo_cfg_nvsEP_mc_torso_con_motorNVindex_t
    @brief      It contains an index for all the network variables in a motor of the left leg. use the same type as in bodypart
 **/
typedef eOcfg_nvsEP_mc_motorNVindex_t eo_cfg_nvsEP_mc_torso_con_motorNVindex_t;


/** @typedef    enum varsMCleg_TOTALnumber;
    @brief      It contains the total number of network variables managed by the endpoint mc leg
 **/
enum {varsMCtorso_TOTALnumber = jointTorso_TOTALnumber*jointNVindex_TOTALnumber + motorTorso_TOTALnumber*motorNVindex_TOTALnumber };


/** @typedef    typedef struct eo_cfg_nvsEP_mc_torso_t;
    @brief      contains all the variables in the leg.
 **/
typedef struct                  // 152*3+40*3 = 576               
{
    eOmc_joint_t                joints[jointTorso_TOTALnumber]; 
    eOmc_motor_t                motors[motorTorso_TOTALnumber];
} eo_cfg_nvsEP_mc_torso_t;      EO_VERIFYsizeof(eo_cfg_nvsEP_mc_torso_t, 576);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//we prefer to omit this to save rom space: use the default joint and motor values instead ....
//extern const eo_cfg_nvsEP_mc_torso_t eo_cfg_nvsEP_mc_torso_default;

extern const eOmc_joint_t* eo_cfg_nvsEP_mc_torso_joint_defaultvalue;
extern const eOmc_motor_t* eo_cfg_nvsEP_mc_torso_motor_defaultvalue;



// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_torso_joint_NVID_Get(eo_cfg_nvsEP_mc_torso_con_jointNumber_t j, eo_cfg_nvsEP_mc_torso_con_jointNVindex_t jnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e jnvindex for the joint number @e j
    @param      j               the joint number 
    @param      jnvinxed        the index of the nv inside the joint
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_torso_joint_NVID_Get(eo_cfg_nvsEP_mc_torso_con_jointNumber_t j, eo_cfg_nvsEP_mc_torso_con_jointNVindex_t jnvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_torso_motor_NVID_Get(eo_cfg_nvsEP_mc_torso_con_motorNumber_t m, eo_cfg_nvsEP_mc_torso_con_motorNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the motor number @e m
    @param      m               the motor number 
    @param      mnvinxed        the index of the nv inside the motor
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_torso_motor_NVID_Get(eo_cfg_nvsEP_mc_torso_con_motorNumber_t m, eo_cfg_nvsEP_mc_torso_con_motorNVindex_t mnvindex);



// - declarations used to load the endpoint into a EOnvsCfg object ----------------------------------------------------


// it is a EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_mc_torso_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_mc_torso_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_torso_fptr_hashfunction_id2index;

// the size of the ram to be used
#define EOK_cfg_nvsEP_mc_torso_RAMSIZE                                            (sizeof(eo_cfg_nvsEP_mc_torso_t));

// the hash function
extern uint16_t eo_cfg_nvsEP_mc_torso_hashfunction_id2index(uint16_t id);



/** @}            
    end of group eo_asfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







