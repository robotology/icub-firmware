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
#ifndef _EOCFG_NVSEP_MC_ANY_CON_JXX_H_
#define _EOCFG_NVSEP_MC_ANY_CON_JXX_H_




/** @file       eOcfg_nvsEP_mc_any_con_jxx.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_uilsdede234567 Configuation of the ......
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMotionControl.h"
#include "EOnv.h"



// - public #define  --------------------------------------------------------------------------------------------------

// -- max number of network variables in a joint

#define  EOK_cfg_nvsEP_mc_any_con_jxx_maxnumof_nvs_in_joint                                 32


// -- the index of the network variables inside a joint

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig                                       (0)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition                          (1)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity                          (2)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque                            (3)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__impedance                            (4)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint                   (5)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint                   (6)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout              (7)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU00                        (8)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__motionmonitormode                    (9)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__encoderconversionfactor              (10)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__encoderconversionoffset              (11)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__des02FORjstatuschamaleon04           (12)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU01                        (13)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU02                        (14)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder02FFU03                        (15)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder02FFU04                        (16)

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus                                       (17)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__basic                                (18)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__ofpid                                (19)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__chamaleon04                          (20)

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs                                       (21)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__externallymeasuredtorque             (22)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__holder02FFU01                        (23)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__holder04FFU02                        (24)

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__calibration                          (25)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__setpoint                             (26)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__stoptrajectory                       (27)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__controlmode                          (28)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU02                        (29)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU03                        (30)

// -- the total number of network variables inside a joint

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber                                   (31)






// -- the fun and typ of all the network variables in the joint

#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig                                       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition                          EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity                          EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque                            EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__impedance                            EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint                   EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint                   EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout              EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU00                        EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__motionmonitormode                    EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__encoderconversionfactor              EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__encoderconversionoffset              EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__des02FORjstatuschamaleon04           EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU01                        EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU02                        EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder02FFU03                        EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder02FFU04                        EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)

#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus                                       EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__basic                                EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__ofpid                                EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__chamaleon04                          EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_u32)

#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs                                       EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__externallymeasuredtorque             EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u16)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__holder02FFU01                        EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u16)                       
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__holder04FFU02                        EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u32)

#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__calibration                          EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__setpoint                             EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__stoptrajectory                       EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__controlmode                          EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU02                        EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU03                        EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08)





// - declaration of public user-defined types ------------------------------------------------------------------------- 

EO_VERIFYproposition(eo_cfg_nvsEP_mc_any_con_jxx_check, (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber <= EOK_cfg_nvsEP_mc_any_con_jxx_maxnumof_nvs_in_joint));
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// contains the funtyp ordered by nvindex
extern const uint8_t eo_cfg_nvsEP_mc_any_con_jxx_funtyp[];


// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group eo_uilsdede234567  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





