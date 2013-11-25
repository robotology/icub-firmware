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
#ifndef _EOCFG_NVSEP_AS_ANY_CON_SXX_H_
#define _EOCFG_NVSEP_AS_ANY_CON_SXX_H_




/** @file       eOcfg_nvsEP_as_any_con_sxx.h
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
#include "EoSensors.h"



// - public #define  --------------------------------------------------------------------------------------------------

// -- max number of network variables in a strain

#define EOK_cfg_nvsEP_as_any_con_sxx_maxnumof_nvs_in_strain                                 (16)     


// -- the index of the network variables inside a strain

#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig                                       (0)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__mode                                 (1)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__datarate                             (2)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__signaloncefullscale                  (3)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus                                       (4)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__fullscale                            (5)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__calibratedvalues                     (6)
#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__uncalibratedvalues                   (7)

// -- the total number of network variables inside a strain

#define EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber                                   (8)
  





// -- the fun and typ of all the network variables in the strain
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig                                       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__mode                                 EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__datarate                             EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__signaloncefullscale                  EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus                                       EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__fullscale                            EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__calibratedvalues                     EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr)
#define EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__uncalibratedvalues                   EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr)







// - declaration of public user-defined types ------------------------------------------------------------------------- 

EO_VERIFYproposition(eo_cfg_nvsEP_as_any_con_sxx_check, (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber <= EOK_cfg_nvsEP_as_any_con_sxx_maxnumof_nvs_in_strain));
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// contains the funtyp ordered by nvindex
extern const uint8_t eo_cfg_nvsEP_as_any_con_sxx_funtyp[];

// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group eo_uilsdede234567  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





