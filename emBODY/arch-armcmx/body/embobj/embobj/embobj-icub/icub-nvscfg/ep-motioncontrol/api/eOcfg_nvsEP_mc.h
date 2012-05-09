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
#ifndef _EOCFG_NVSEP_MC_H_
#define _EOCFG_NVSEP_MC_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOcfg_nvsEP_mc.h
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


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eo_cfg_nvsEP_mc_endpoint_t;
    @brief      It contains all the endpoints used for motion control. The grouping is done by sub-bodypart
                in such a way that each ep is contained by a exactly one EMS board in iCub ver 2.5
 **/
typedef enum
{
    endpoint_mc_leftupperarm                        = 0x0011,   /**< used by EB1 board */
    endpoint_mc_leftlowerarm                        = 0x0012,   /**< used by EB2 board */
    
    endpoint_mc_rightupperarm                       = 0x0013,   /**< used by EB3 board */
    endpoint_mc_rightlowerarm                       = 0x0014,   /**< used by EB4 board */

    endpoint_mc_torso                               = 0x0015,   /**< used by EB5 board */

    endpoint_mc_leftupperleg                        = 0x0016,   /**< used by EB6 board */
    endpoint_mc_leftlowerleg                        = 0x0017,   /**< used by EB7 board */
    
    endpoint_mc_rightupperleg                       = 0x0018,   /**< used by EB8 board */
    endpoint_mc_rightlowerleg                       = 0x0019    /**< used by EB9 board */  

} eo_cfg_nvsEP_mc_endpoint_t;

enum { endpoint_mc_TOTALnumber = 9};


/** @typedef    typedef uint16_t eo_cfg_nvsEP_mc_jointNumber_t;
    @brief      It contains an index for a joint in a manner that is independent from the endpoint.
                The maximum value is jointNumberMAX.
 **/
typedef uint16_t eo_cfg_nvsEP_mc_jointNumber_t;

enum { jointNumberMAX = 12};



/** @typedef    typedef enum eo_cfg_nvsEP_mc_jointNVindex_t;
    @brief      It contains an index for all the network variables in a joint. The indices are consecutive and without
                holes, so that the enum value can be changed by a normal index.
 **/
typedef enum
{
    jointNVindex_jconfig                                    =  0,
    jointNVindex_jconfig__pidposition                       =  1,
    jointNVindex_jconfig__pidvelocity                       =  2,
    jointNVindex_jconfig__pidtorque                         =  3,
    jointNVindex_jconfig__impedance                         =  4,
    jointNVindex_jconfig__minpositionofjoint                =  5,
    jointNVindex_jconfig__maxpositionofjoint                =  6,
    jointNVindex_jconfig__velocitysetpointtimeout           =  7,
    jointNVindex_jconfig__controlmode                       =  8,
    jointNVindex_jconfig__motionmonitormode                 =  9,
    jointNVindex_jconfig__des02FORjstatuschamaleon04        = 10,
    jointNVindex_jconfig__holder01FFU01                     = 11,
    jointNVindex_jconfig__holder01FFU02                     = 12,
    jointNVindex_jconfig__holder02FFU03                     = 13,  
    jointNVindex_jconfig__holder02FFU04                     = 14,
    
    jointNVindex_jstatus                                    = 15,
    jointNVindex_jstatus__basic                             = 16,
    jointNVindex_jstatus__ofpid                             = 17,
    jointNVindex_jstatus__chamaleon04                       = 18,
    
    jointNVindex_jinputs__externallymeasuredtorque          = 19,    
    jointNVindex_jinputs__holder02FFU01                     = 20,
    jointNVindex_jinputs__holder04FFU02                     = 21,
    
    jointNVindex_jcmmnds__calibration                       = 22,
    jointNVindex_jcmmnds__setpoint                          = 23,
    jointNVindex_jcmmnds__stoptrajectory                    = 24,
    jointNVindex_jcmmnds__holder01FFU01                     = 25,
    jointNVindex_jcmmnds__holder01FFU02                     = 26,
    jointNVindex_jcmmnds__holder01FFU03                     = 27,
} eo_cfg_nvsEP_mc_jointNVindex_t;

enum { jointNVindex_TOTALnumber = 28};



/** @typedef    typedef uint16_t eo_cfg_nvsEP_mc_motorNumber_t;
    @brief      It contains an index for a motor in a manner that is independnt from the endpoint of bodypart.
                It maximum number in every endpoint is motorNumberMAX
 **/
typedef uint16_t eo_cfg_nvsEP_mc_motorNumber_t;

enum { motorNumberMAX = 12};


/** @typedef    typedef enum eo_cfg_nvsEP_mc_motorNVindex_t;
    @brief      It contains an index for all the network variables in a motor. The indices are consecutive and without
                holes.
 **/
typedef enum
{
    motorNVindex_mconfig                                    = 0,
    motorNVindex_mconfig__pidcurrent                        = 1,
    motorNVindex_mconfig__maxvelocityofmotor                = 2,
    motorNVindex_mconfig__maxcurrentofmotor                 = 3,
    motorNVindex_mconfig__des02FORmstatuschamaleon04        = 4,
    
    motorNVindex_mstatus                                    = 5,
    motorNVindex_mstatus__basic                             = 6,
    motorNVindex_mstatus__chamaleon04                       = 7
} eo_cfg_nvsEP_mc_motorNVindex_t;

enum { motorNVindex_TOTALnumber = 8};


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------


extern uint16_t eo_cfg_nvsEP_mc_joint_numbermax_Get(eo_cfg_nvsEP_mc_endpoint_t ep);


extern uint16_t eo_cfg_nvsEP_mc_motor_numbermax_Get(eo_cfg_nvsEP_mc_endpoint_t ep);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_motor_NVID_Get(eo_cfg_nvsEP_mc_endpoint_t ep, eo_cfg_nvsEP_mc_motorNumber_t m, eo_cfg_nvsEP_mc_motorNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e jnvindex for the joint number @e j, given the endpoint @e ep
    @param      ep              the endpoint
    @param      j               the joint number 
    @param      jnvinxed        the index of the nv inside the joint
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_joint_NVID_Get(eo_cfg_nvsEP_mc_endpoint_t ep, eo_cfg_nvsEP_mc_jointNumber_t j, eo_cfg_nvsEP_mc_jointNVindex_t jnvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_mc_motor_NVID_Get(eo_cfg_nvsEP_mc_endpoint_t ep, eo_cfg_nvsEP_mc_motorNumber_t m, eo_cfg_nvsEP_mc_motorNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the motor number @e m, given the endpoint @e ep
    @param      ep              the endpoint
    @param      m               the motor number 
    @param      mnvinxed        the index of the nv inside the motor
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mc_motor_NVID_Get(eo_cfg_nvsEP_mc_endpoint_t ep, eo_cfg_nvsEP_mc_motorNumber_t m, eo_cfg_nvsEP_mc_motorNVindex_t mnvindex);


/** @}            
    end of group eo_2uilsdede2345er  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




