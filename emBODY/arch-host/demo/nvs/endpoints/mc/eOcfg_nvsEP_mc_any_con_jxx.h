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



// - public #define  --------------------------------------------------------------------------------------------------

// -- the index of the network variables inside a joint

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig                                       (0)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition                          (1)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity                          (2)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque                            (3)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint                   (4)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint                   (5)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout              (6)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__upto02descrforchameleon02            (7)

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__jstatus                                      (8)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__calibrator                                   (9)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__setpoint                                     (10)
#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__controlmode                                  (11)


// -- the total number of network variables inside a joint

#define EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber                                   (12)






// -- the fun and typ of all the network variables in the joint

#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig                                       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition                          EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity                          EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque                            EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint                   EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint                   EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout              EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02            EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)


#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__jstatus                                      EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__calibrator                                   EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__setpoint                                     EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__controlmode                                  EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u08)








// - declaration of public user-defined types ------------------------------------------------------------------------- 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group eo_uilsdede234567  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





