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
#ifndef _EOCFG_NVSEP_MNGMNT_CON_HID_H_
#define _EOCFG_NVSEP_MNGMNT_CON_HID_H_


/* @file       eOcfg_nvsEP_mngmnt_con_hid.h
    @brief      This header file implements hidden interface to ...
    @author     marco.accame@iit.it
    @date       09/06/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"

#include "eOcfg_nvsEP_mngmnt_con.h"


// - declaration of extern public interface ---------------------------------------------------------------------------



// - the indices of the nv in the endpoint
#define EOK_cfg_nvsEP_mngmnt_con_nvindex__ropsigcfgassign                       (mngmntNVindex__ropsigcfgassign)
#define EOK_cfg_nvsEP_mngmnt_con_nvindex__ropsigcfgcommand                      (mngmntNVindex__ropsigcfgcommand)


// - the total number of nv
#define EOK_cfg_nvsEP_mngmnt_con_NUMofVARS                                      mngmntNVindex_TOTALnumber

// -  macros whcih transforms the index in nvid-offset and the nvid-offset in index
#define EOK_cfg_nvsEP_mngmnt_con_NVIDoff(nvindex)                               (nvindex)
#define EOK_cfg_nvsEP_mngmnt_con_NVindex(nvidoffset)                            (nvidoffset)

// -- the fun and typ of all the nv in the endpoint
#define EOK_cfg_nvsEP_mngmnt_NVFUNTYP__ropsigcfgassign                          EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_arr)
#define EOK_cfg_nvsEP_mngmnt_NVFUNTYP__ropsigcfgcommand                         EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_pkd)


// -- the nvid of all the nv in the endpoint

#define EOK_cfg_nvsEP_mngmnt_NVID__ropsigcfgassign                              EO_nv_ID(EOK_cfg_nvsEP_mngmnt_NVFUNTYP__ropsigcfgassign, EOK_cfg_nvsEP_mngmnt_con_NVIDoff(EOK_cfg_nvsEP_mngmnt_con_nvindex__ropsigcfgassign))
#define EOK_cfg_nvsEP_mngmnt_NVID__ropsigcfgcommand                             EO_nv_ID(EOK_cfg_nvsEP_mngmnt_NVFUNTYP__ropsigcfgcommand, EOK_cfg_nvsEP_mngmnt_con_NVIDoff(EOK_cfg_nvsEP_mngmnt_con_nvindex__ropsigcfgcommand))


   


// - #define used with hidden struct ----------------------------------------------------------------------------------



// - definition of the hidden struct implementing the object ----------------------------------------------------------




// - declaration of extern hidden functions ---------------------------------------------------------------------------



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




