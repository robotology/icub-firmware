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
#ifndef _EOCFG_NVSEP_MC_ANY_CON_BODYPART_HID_H_
#define _EOCFG_NVSEP_MC_ANY_CON_BODYPART_HID_H_




/** @file       eOcfg_nvsEP_mc_any_con_bodypart_hid.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_h2uilsdede2345er Configuration of the bodypart
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EoMotionControl.h"
#include "eOcfg_nvsEP_mc_any_con_jxx.h"
#include "eOcfg_nvsEP_mc_any_con_mxx.h"

#include "eOcfg_nvsEP_mc_any_con_bodypart_hid.h"

// - public #define  --------------------------------------------------------------------------------------------------

// -- max mumber of joints and motors in a bodypart
#define EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_joints                           20
#define EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_motors                           20

// -- max number of network variables in a joint
#define  EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_joint                    32

// -- max number of network variables in a motor
#define  EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_motor                    16


// -- the first nvid off of the joint jth
#define EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_joint(jth)                ((jth)*EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_joint) 

// -- the first nvid off of the motor mth
#define EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(mth)                ((mth)*EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_motor + EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_joint(EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_joints))                                               



// -- macro which computes the off part of the nvid of a nv in the joint jth when inserted inside a bodypart
//    jth is the joint number, jnvindex is the index of variable in the joint 
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(jth, jnvindex)            (EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_joint(jth) + (jnvindex))

// -- macro which computes the off part of the nvid of a nv in the motor mth when inserted inside a bodypart
//    mth is the motor number, mnvindex is the index of variable in the motor 
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(mth, mnvindex)            (EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(mth) + (mnvindex))



// -- the nvid of all the network variables of the joint as a function of the joint number j

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(j)                                    EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(j)                       EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(j)                       EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(j)                         EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(j)           EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(j)         EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__upto02descrforchameleon02))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(j)                                   EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__jstatus, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__jstatus))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(j)                                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__calibrator, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__calibrator))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(j)                                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__setpoint, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__setpoint))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(j)                               EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__controlmode, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__controlmode))
        

        
// -- the nvid of all the network variables of the motor as a function of the motor number m

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(m)                                    EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(m)                        EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__pidcurrent, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__pidcurrent))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(m)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxvelocityofmotor, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxvelocityofmotor))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(m)                 EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxcurrentofmotor, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxcurrentofmotor))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(m)         EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__upto02descrforchameleon06, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__upto02descrforchameleon06))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(m)                                   EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP__mstatus, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex__mstatus))
      


      


// - declaration of public user-defined types ------------------------------------------------------------------------- 

EO_VERIFYproposition(EOK_cfg_nvsEP_mc_any_con_bodypart, (1024 > EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_motor)));


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @}            
    end of group eo_h2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


