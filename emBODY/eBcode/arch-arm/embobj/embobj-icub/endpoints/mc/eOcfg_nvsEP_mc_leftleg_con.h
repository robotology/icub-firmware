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

/** @defgroup eo_asfdgr234 Configuation of the NVs for the updater
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOconstvector.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoMotionControl.h"

 

// - public #define  --------------------------------------------------------------------------------------------------

// the starting number for the nvid
#define EOK_cfg_nvsEP_mc_leftleg_firstid                            (0)

// the 16bit value to use to identify the endpoint
#define EOK_cfg_nvsEP_mc_leftleg_endpoint                           (0x0010)


// -- the fun and typ of all the nv in the endpoint

#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig                           EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__pidposition              EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__pidvelocity              EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__pidtorque                EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__minpositionofjoint       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__maxpositionofjoint       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__velocitysetpointtimeout  EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)

#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__jstatus                          EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__calibrator                       EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__setpoint                         EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__controlmode                      EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u08)



#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig                           EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig__pidcurrent               EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig__maxvelocityofmotor       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32)
#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig__maxcurrentofmotor        EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16)

#define EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00__mstatus                          EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)



// -- the identifiers of all the nv in the endpoint

#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig                               EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig,                         0+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig__pidposition                  EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__pidposition,            1+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig__pidvelocity                  EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__pidvelocity,            2+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig__pidtorque                    EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__pidtorque,              3+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig__minpositionofjoint           EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__minpositionofjoint,     4+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig__maxpositionofjoint           EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__maxpositionofjoint,     5+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00_jconfig__velocitysetpointtimeout      EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00_jconfig__velocitysetpointtimeout,6+EOK_cfg_nvsEP_mc_leftleg_firstid)

#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00__jstatus                              EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__jstatus,                        7+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00__calibrator                           EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__calibrator,                     8+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00__setpoint                             EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__setpoint,                       9+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_joint00__controlmode                          EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_joint00__controlmode,                   10+EOK_cfg_nvsEP_mc_leftleg_firstid)


#define EOK_cfg_nvsEP_mc_leftleg_NVID_motor00_mconfig                               EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig,                        11+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_motor00_mconfig__pidcurrent                   EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig__pidcurrent,            12+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_motor00_mconfig__maxvelocityofmotor           EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig__maxvelocityofmotor,    13+EOK_cfg_nvsEP_mc_leftleg_firstid)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_motor00_mconfig__maxcurrentofmotor            EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00_mconfig__maxcurrentofmotor,     14+EOK_cfg_nvsEP_mc_leftleg_firstid)

#define EOK_cfg_nvsEP_mc_leftleg_NVID_motor00__mstatus                              EO_nv_ID(EOK_cfg_nvsEP_mc_leftleg_NVFUNTYP_motor00__mstatus,                       15+EOK_cfg_nvsEP_mc_leftleg_firstid)






// - declaration of public user-defined types ------------------------------------------------------------------------- 



// - very important information: on ARM alignment is done using "#pragma pack(8)", thus ... there are rules to follow

typedef struct               
{
    eOmc_joint_t                joints[6]; 
    eOmc_motor_t                motors[6];
} eo_cfg_nvsEP_mc_leftleg_t;    EO_VERIFYsizeof(eo_cfg_nvsEP_mc_leftleg_t, 864);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eo_cfg_nvsEP_mc_leftleg_t eo_cfg_nvsEP_mc_leftleg_default;

// EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_leftleg_fptr_hashfunction_id2index;



// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_mc_leftleg_hashfunction_id2index(uint16_t id);





/** @}            
    end of group eo_asfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







