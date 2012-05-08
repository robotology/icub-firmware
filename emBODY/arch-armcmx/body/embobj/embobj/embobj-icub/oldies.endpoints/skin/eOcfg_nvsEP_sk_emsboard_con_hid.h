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
#ifndef _EOCFG_NVSEP_SK_EMSBOARD_CON_HID_H_
#define _EOCFG_NVSEP_SK_EMSBOARD_CON_HID_H_




/** @file       eOcfg_nvsEP_sk_emsboard_con_hid.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       05/02/2012
**/

/** @defgroup eo_h2uilsdede2345er Configuration of the bodypart
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"

#include "eOcfg_nvsEP_sk_emsboard_con.h"


// - public #define  --------------------------------------------------------------------------------------------------


// - the indices of the nv in the endpoint
#define EOK_cfg_nvsEP_sk_emsboard_con_nvindex_sconfig__sigmode              (skinNVindex_sconfig__sigmode)
#define EOK_cfg_nvsEP_sk_emsboard_con_nvindex_sstatus__arrayof10canframes   (skinNVindex_sstatus__arrayof10canframe)



// - the total number of nv
#define EOK_cfg_nvsEP_sk_emsboard_con_NUMofVARS                             varsSKemsboard_TOTALnumber

// -  macros whcih transforms the index in nvid-offset and the nvid-offset in index
#define EOK_cfg_nvsEP_sk_emsboard_con_NVIDoff(nvindex)                      (nvindex)
#define EOK_cfg_nvsEP_sk_emsboard_con_NVindex(nvidoffset)                   (nvidoffset)

// -- the fun and typ of all the nv in the endpoint
#define EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sconfig__sigmode                 EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sstatus__arrayof10canframes      EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr)


// -- the nvid of all the nv in the endpoint
#define EOK_cfg_nvsEP_sk_emsboard_NVID_sconfig__sigmode                     EO_nv_ID(EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sconfig__sigmode, EOK_cfg_nvsEP_sk_emsboard_con_NVIDoff(EOK_cfg_nvsEP_sk_emsboard_con_nvindex_sconfig__sigmode))
#define EOK_cfg_nvsEP_sk_emsboard_NVID_sstatus__arrayof10canframes          EO_nv_ID(EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sstatus__arrayof10canframes, EOK_cfg_nvsEP_sk_emsboard_con_NVIDoff(EOK_cfg_nvsEP_sk_emsboard_con_nvindex_sstatus__arrayof10canframes))



   


// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @}            
    end of group eo_h2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


