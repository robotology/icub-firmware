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


#include "eOcfg_nvsEP_mc_any_con_bodypart.h"

// - public #define  --------------------------------------------------------------------------------------------------

// -- max mumber of joints and motors in a bodypart
#define EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_joints                           jointNumberBodypart_TOTALnumber
#define EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_motors                           motorNumberBodypart_TOTALnumber


// -- the first nvid off of the joint jth
#define EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_joint(jth)                ((jth)*EOK_cfg_nvsEP_mc_any_con_jxx_maxnumof_nvs_in_joint) 

// -- the first nvid off of the motor mth
#define EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(mth)                ((mth)*EOK_cfg_nvsEP_mc_any_con_mxx_maxnumof_nvs_in_motor + EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_joint(EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_joints))                                               



// -- macro which computes the off part of the nvid of a nv in the joint jth when inserted inside a bodypart
//    jth is the joint number, jnvindex is the index of variable in the joint 
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(jth, jnvindex)            (EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_joint(jth) + (jnvindex))

// -- macro which computes the off part of the nvid of a nv in the motor mth when inserted inside a bodypart
//    mth is the motor number, mnvindex is the index of variable in the motor 
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(mth, mnvindex)            (EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(mth) + (mnvindex))



// -- the nvid of all the network variables of the joint as a function of the joint number j 
// -- use them only when required in constant initialisations and prefer using functions otherwise

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(j)                               EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(j)                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(j)                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(j)                    EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(j)                    EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__impedance, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__impedance))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(j)           EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(j)           EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(j)      EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(j)                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__controlmode, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__controlmode))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(j)            EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__motionmonitormode, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__motionmonitormode))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(j)   EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__des02FORjstatuschamaleon04, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__des02FORjstatuschamaleon04))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU01, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU01))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU02, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU02))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder02FFU03, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder02FFU03))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder02FFU04, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder02FFU04))

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(j)                               EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(j)                        EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__basic, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__basic))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(j)                        EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__ofpid, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__ofpid))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(j)                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__chamaleon04, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__chamaleon04))

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(j)     EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__externallymeasuredtorque, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__externallymeasuredtorque))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__holder02FFU01, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__holder02FFU01))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__holder04FFU02, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__holder04FFU02))

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(j)                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__calibration, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__calibration))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(j)                     EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__setpoint, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__setpoint))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(j)               EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__stoptrajectory, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__stoptrajectory))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU01(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU01, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU01))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU02, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU02))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(j)                EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU03, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU03))

        
// -- the nvid of all the network variables of the motor as a function of the motor number m 
// -- use them only when required in constant initialisations and prefer using functions otherwise

#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(m)                               EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(m)                   EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__pidcurrent, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__pidcurrent))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(m)           EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxvelocityofmotor, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxvelocityofmotor))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(m)            EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxcurrentofmotor, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxcurrentofmotor))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(m)   EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__des02FORmstatuschamaleon04, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__des02FORmstatuschamaleon04))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(m)                               EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mstatus))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(m)                        EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__basic, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mstatus__basic))
#define EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(m)                  EO_nv_ID(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__chamaleon04, EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mstatus__chamaleon04))
  

  

  


      


// - declaration of public user-defined types ------------------------------------------------------------------------- 

EO_VERIFYproposition(EOK_cfg_nvsEP_mc_any_con_bodypart, (1024 > EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(EOK_cfg_nvsEP_mc_any_con_mxx_maxnumof_nvs_in_motor)));


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @}            
    end of group eo_h2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


