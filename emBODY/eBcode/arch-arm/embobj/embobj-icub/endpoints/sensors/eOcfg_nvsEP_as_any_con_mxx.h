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
#ifndef _EOCFG_NVSEP_AS_ANY_CON_MXX_H_
#define _EOCFG_NVSEP_AS_ANY_CON_MXX_H_




/** @file       eOcfg_nvsEP_as_any_con_mxx.h
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

// -- the index of the network variables inside a strain

#define EOK_cfg_nvsEP_as_any_con_mxx_mnvindex__txmode                                       (0)
#define EOK_cfg_nvsEP_as_any_con_mxx_mnvindex__datarate                                     (1)
#define EOK_cfg_nvsEP_as_any_con_mxx_mnvindex__resolutionmode                               (2)
#define EOK_cfg_nvsEP_as_any_con_mxx_mnvindex__values                                       (3)


// -- the total number of network variables inside a strain

#define EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber                                   (4)






// -- the fun and typ of all the network variables in the strain

#define EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__txmode                                       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__datarate                                     EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__resolutionmode                               EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08)
#define EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__values                                       EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)









// - declaration of public user-defined types ------------------------------------------------------------------------- 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group eo_uilsdede234567  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





