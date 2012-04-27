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

#ifndef _EOCFG_NVSEP_MC_LEFTLEG_CON_H_
#define _EOCFG_NVSEP_MC_LEFTLEG_CON_H_




/** @file       eOcfg_nvsEP_mc_leftleg_con.h
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


/** @typedef    typedef enum eo_cfg_nvsEP_mc_leftleg_con_jointNumber_t
    @brief      It contains an index for all the joints the left leg. It is a subset of those 
                in eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t
 **/
typedef enum
{
    jointLeftLeg_00 = 0,   
    jointLeftLeg_01, jointLeftLeg_02, jointLeftLeg_03, jointLeftLeg_04, jointLeftLeg_05
} eo_cfg_nvsEP_mc_leftleg_con_jointNumber_t;

enum { jointLeftLeg_TOTALnumber = 6}; 


/** @typedef    typedef eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t eo_cfg_nvsEP_mc_leftleg_con_jointNVindex_t
    @brief      It contains an index for all the network variables in a joint of the left leg. use the same type as in bodypart
 **/
typedef eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t eo_cfg_nvsEP_mc_leftleg_con_jointNVindex_t;


/** @typedef    typedef enum eo_cfg_nvsEP_mc_leftleg_con_motorNumber_t;
    @brief      It contains an index for all the motors the left leg. It is a subset of those 
                in eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t
 **/
typedef enum
{
    motorLeftLeg_00 = 0,   
    motorLeftLeg_01, motorLeftLeg_02, motorLeftLeg_03, motorLeftLeg_04, motorLeftLeg_05
} eo_cfg_nvsEP_mc_leftleg_con_motorNumber_t;

enum { motorLeftLeg_TOTALnumber = 6}; 


/** @typedef    typedef eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t eo_cfg_nvsEP_mc_leftleg_con_motorNVindex_t
    @brief      It contains an index for all the network variables in a motor of the left leg. use the same type as in bodypart
 **/
typedef eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t eo_cfg_nvsEP_mc_leftleg_con_motorNVindex_t;


/** @typedef    enum varsMCleftleg_TOTALnumber;
    @brief      It contains the total number of network variables managed by the endpoint mc leftleg
 **/
enum {varsMCleftleg_TOTALnumber = jointLeftLeg_TOTALnumber*jointNVindex_TOTALnumber + motorLeftLeg_TOTALnumber*motorNVindex_TOTALnumber };


/** @typedef    typedef struct eo_cfg_nvsEP_mc_leftleg_t;
    @brief      contains all the variables in the leftleg.
 **/
typedef struct                  // 112*6+40*6 = 912   152*6+40*6 = 1152            
{
    eOmc_joint_t                joints[jointLeftLeg_TOTALnumber]; 
    eOmc_motor_t                motors[motorLeftLeg_TOTALnumber];
} eo_cfg_nvsEP_mc_leftleg_t;    EO_VERIFYsizeof(eo_cfg_nvsEP_mc_leftleg_t, 1152);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//we prefer to omit this to save rom space: use the default joint and motor values instead ....
//extern const eo_cfg_nvsEP_mc_leftleg_t eo_cfg_nvsEP_mc_leftleg_default;

extern const eOmc_joint_t* eo_cfg_nvsEP_mc_leftleg_joint_defaultvalue;
extern const eOmc_motor_t* eo_cfg_nvsEP_mc_leftleg_motor_defaultvalue;



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern eOnvEP_t eo_cfg_nvsEP_mc_leftleg_EP_Get(void)
    @brief      This function retrieves the eOnvEP_t of this endpoint
    @return     the endpoint.
  */
extern eOnvEP_t eo_cfg_nvsEP_mc_leftleg_EP_Get(void);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_leftleg_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_leftleg_con_jointNumber_t j, eo_cfg_nvsEP_mc_leftleg_con_jointNVindex_t jnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e jnvindex for the joint number @e j
    @param      j               the joint number 
    @param      jnvinxed        the index of the nv inside the joint
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_leftleg_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_leftleg_con_jointNumber_t j, eo_cfg_nvsEP_mc_leftleg_con_jointNVindex_t jnvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_leftleg_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_leftleg_con_motorNumber_t m, eo_cfg_nvsEP_mc_leftleg_con_motorNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the motor number @e m
    @param      m               the motor number 
    @param      mnvinxed        the index of the nv inside the motor
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_leftleg_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_leftleg_con_motorNumber_t m, eo_cfg_nvsEP_mc_leftleg_con_motorNVindex_t mnvindex);



// - declarations used to load the endpoint into a EOnvsCfg object ----------------------------------------------------

// the 16bit value to use to identify the endpoint
#define EOK_cfg_nvsEP_mc_leftleg_EP                                                 (0x0010)

// it is a EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_leftleg_fptr_hashfunction_id2index;

// the size of the ram to be used
#define EOK_cfg_nvsEP_mc_leftleg_RAMSIZE                                            (sizeof(eo_cfg_nvsEP_mc_leftleg_t));

// the hash function
extern uint16_t eo_cfg_nvsEP_mc_leftleg_hashfunction_id2index(uint16_t id);



/** @}            
    end of group eo_asfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







