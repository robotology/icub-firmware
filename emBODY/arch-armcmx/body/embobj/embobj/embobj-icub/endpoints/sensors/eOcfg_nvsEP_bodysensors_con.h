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

#ifndef _EOCFG_NVSEP_BODYSENSORS_CON_H_
#define _EOCFG_NVSEP_BODYSENSORS_CON_H_




/** @file       eOcfg_nvsEP_bodysensors_con.h
	@brief      This header file gives the constant configuration for the NVs in the endpoint bodysensors
	@author     marco.accame@iit.it
	@date       04/05/2012
**/

/** @defgroup eo_werasfdgr234 Configuation of the NVs ...
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOconstvector.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoSensors.h"

#include "eOcfg_nvsEP_mc_any_con_maisxx.h"
#include "eOcfg_nvsEP_mc_any_con_strainxx.h"

 

// - public #define  --------------------------------------------------------------------------------------------------

#define EOK_cfg_nvsEP_bodysensors_NUMofSTRAINS                                      (4)
#define EOK_cfg_nvsEP_bodysensors_NUMofMAISES                                       (2)

#define EOK_cfg_nvsEP_bodysensors_STRAIN_leftarm                                    (0)
#define EOK_cfg_nvsEP_bodysensors_STRAIN_rigtharm                                   (1)
#define EOK_cfg_nvsEP_bodysensors_STRAIN_leftleg                                    (3)
#define EOK_cfg_nvsEP_bodysensors_STRAIN_rigthleg                                   (4)

#define EOK_cfg_nvsEP_bodysensors_MAIS_lefthand                                     (0)
#define EOK_cfg_nvsEP_bodysensors_MAIS_rigthhand                                    (1)

// the 16bit value to use to identify the endpoint
#define EOK_cfg_nvsEP_bodysensors_EP                                                 (0x0020)

// the total number of nv in the endpoint
#define EOK_cfg_nvsEP_bodysensors_NUMofVARS                                          ((EOK_cfg_nvsEP_mc_any_con_strainxx_snvindex_TOTALnumber*EOK_cfg_nvsEP_bodysensors_NUMofSTRAINS)+(EOK_cfg_nvsEP_mc_any_con_maisxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_bodysensors_NUMofMAISES))


// mettiamo: le due mais, e le 4 strain (con nomi relativi alle bodypart: lleg, larm, rleg, rarm).

// poi: mettiamo una cfg per strain ed una per mais. ed anche i valori segnalati
// in strain-cfg: datarate, txmode, signal_fullscale_now. 
// in mais-cfg: datarate, txmode.
// per mais-values: uso il tipo eOsnsr_arrayofupto20bytes_t
// per strain-values: uso il tipo eOsnsr_arrayofupto12bytes_t dove conosco l'ordine che e': 3 force e poi 3 torque.



// the 16bit used to identify the network variable inside the endpoint. they are function of the joint or motor number (jth or mth)
#define EOK_cfg_nvsEP_bodysensors_NVID_strain__txmode(jth)                                  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(jth)
#define EOK_cfg_nvsEP_bodysensors_NVID_strain__datarate(jth)                     EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(jth)                       
#define EOK_cfg_nvsEP_bodysensors_NVID_strain__signaloncefullscale(jth)                     EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(jth)                      
#define EOK_cfg_nvsEP_bodysensors_NVID_strain__values(jth)                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(jth)   
    
#define EOK_cfg_nvsEP_bodysensors_NVID_mais__txmode(jth)              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(jth)       
#define EOK_cfg_nvsEP_bodysensors_NVID_mais__datarate(jth)              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(jth)       
#define EOK_cfg_nvsEP_bodysensors_NVID_mais__resolutionmode(jth)         EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(jth)    
#define EOK_cfg_nvsEP_bodysensors_NVID_mais__values(jth)       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(jth)   





// - declaration of public user-defined types ------------------------------------------------------------------------- 



// - very important information: on ARM alignment is done using "#pragma pack(8)", thus ... there are rules to follow

typedef struct               
{
    //eOsnsr_strain_t             strain_larm; 
    //eOsnsr_strain_t             strain_rarm; 
    //eOsnsr_strain_t             strain_lleg; 
    //eOsnsr_strain_t             strain_rleg; 
    eOsnsr_strain_t             strains[EOK_cfg_nvsEP_bodysensors_NUMofSTRAINS];
    //eOsnsr_mais_t               mais_lhand;
    //eOsnsr_mais_t               mais_rhand;
    eOsnsr_mais_t               maises[EOK_cfg_nvsEP_bodysensors_NUMofMAISES];
} eo_cfg_nvsEP_bodysensors_t;   EO_VERIFYsizeof(eo_cfg_nvsEP_bodysensors_t, 160);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//extern const eo_cfg_nvsEP_bodysensors_t eo_cfg_nvsEP_bodysensors_default;

// EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_bodysensors_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_bodysensors_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_bodysensors_fptr_hashfunction_id2index;



// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_bodysensors_hashfunction_id2index(uint16_t id);





/** @}            
    end of group eo_werasfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







