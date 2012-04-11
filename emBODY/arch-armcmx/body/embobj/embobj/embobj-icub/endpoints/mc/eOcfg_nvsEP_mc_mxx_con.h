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
#ifndef _EOCFG_NVSEP_MC_MXX_CON_H_
#define _EOCFG_NVSEP_MC_MXX_CON_H_




/** @file       eOcfg_nvsEP_mc_mxx_con.h
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
#include "eOcfg_nvsEP_mc_con.h"


// - public #define  --------------------------------------------------------------------------------------------------


#define EOK_cfg_nvsEP_mc_mxx_NVID_off(mnum, mvarnum)                        (EOK_cfg_nvsEP_mc_con_bodypart_nvidstart_in_motor(mnum) + (mvarnum))

// the mvarnum is ...

#define EOK_cfg_nvsEP_mc_mvarnum_mconfig                                    (0)
#define EOK_cfg_nvsEP_mc_mvarnum_mconfig__pidcurrent                        (1)
#define EOK_cfg_nvsEP_mc_mvarnum_mconfig__maxvelocityofmotor                (2)
#define EOK_cfg_nvsEP_mc_mvarnum_mconfig__maxcurrentofmotor                 (3)
#define EOK_cfg_nvsEP_mc_mvarnum_mconfig__upto02descrforchameleon06         (4)

#define EOK_cfg_nvsEP_mc_mvarnum__mstatus                                   (5)

#define EOK_cfg_nvsEP_mc_mvarnumTOTAL                                       (6)



// -- the fun and typ of all the motor variable

#define EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig                               EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__pidcurrent                   EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__maxvelocityofmotor           EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__maxcurrentofmotor            EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)
#define EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__upto02descrforchameleon06    EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)

#define EOK_cfg_nvsEP_mc_mxx_NVFUNTYP__mstatus                              EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)




// -- the nvid of the joint variables

#define EOK_cfg_nvsEP_mc_NVID_mconfig(m)                                    EO_nv_ID(EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig, EOK_cfg_nvsEP_mc_mxx_NVID_off(m, EOK_cfg_nvsEP_mc_mvarnum_mconfig))
#define EOK_cfg_nvsEP_mc_NVID_mconfig__pidcurrent(m)                        EO_nv_ID(EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__pidcurrent, EOK_cfg_nvsEP_mc_mxx_NVID_off(m, EOK_cfg_nvsEP_mc_mvarnum_mconfig__pidcurrent))
#define EOK_cfg_nvsEP_mc_NVID_mconfig__maxvelocityofmotor(m)                EO_nv_ID(EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__maxvelocityofmotor, EOK_cfg_nvsEP_mc_mxx_NVID_off(m, EOK_cfg_nvsEP_mc_mvarnum_mconfig__maxvelocityofmotor))
#define EOK_cfg_nvsEP_mc_NVID_mconfig__maxcurrentofmotor(m)                 EO_nv_ID(EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__maxcurrentofmotor, EOK_cfg_nvsEP_mc_mxx_NVID_off(m, EOK_cfg_nvsEP_mc_mvarnum_mconfig__maxcurrentofmotor))
#define EOK_cfg_nvsEP_mc_NVID_mconfig__upto02descrforchameleon06(m)         EO_nv_ID(EOK_cfg_nvsEP_mc_mxx_NVFUNTYP_mconfig__upto02descrforchameleon06, EOK_cfg_nvsEP_mc_mxx_NVID_off(m, EOK_cfg_nvsEP_mc_mvarnum_mconfig__upto02descrforchameleon06))

#define EOK_cfg_nvsEP_mc_NVID__mstatus(m)                                   EO_nv_ID(EOK_cfg_nvsEP_mc_mxx_NVFUNTYP__mstatus, EOK_cfg_nvsEP_mc_mxx_NVID_off(m, EOK_cfg_nvsEP_mc_mvarnum__mstatus))



// - declaration of public user-defined types ------------------------------------------------------------------------- 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOmc_motor_t eo_cfg_nvsEP_mc_mxx_default;


// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group eo_uilsdede234567  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




