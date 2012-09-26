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
#ifndef _EOCFG_NVSEP_AS_ANY_CON_BODY_HID_H_
#define _EOCFG_NVSEP_AS_ANY_CON_BODY_HID_H_




/** @file       eOcfg_nvsEP_as_any_con_body_hid.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_h2uilsdede2345er Configuration of the bodypart
    In here there are constants for the max number of strains and maiss in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EoSensors.h"
#include "eOcfg_nvsEP_as_any_con_sxx.h"
#include "eOcfg_nvsEP_as_any_con_mxx.h"


#include "eOcfg_nvsEP_as_any_con_body.h"

// - public #define  --------------------------------------------------------------------------------------------------

// -- max mumber of strains and maises in the body
#define EOK_cfg_nvsEP_as_any_con_body_maxnumof_strains                          strainBody_TOTALnumber
#define EOK_cfg_nvsEP_as_any_con_body_maxnumof_maiss                            maisBody_TOTALnumber


// -- the first nvid off of the strain sth
#define EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_strain(sth)                ((sth)*EOK_cfg_nvsEP_as_any_con_sxx_maxnumof_nvs_in_strain) 

// -- the first nvid off of the mais mth
#define EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_mais(mth)                ((mth)*EOK_cfg_nvsEP_as_any_con_mxx_maxnumof_nvs_in_mais + EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_strain(EOK_cfg_nvsEP_as_any_con_body_maxnumof_strains))                                               



// -- macro which computes the off part of the nvid of a nv in the strain sth when inserted inside a bodypart
//    sth is the strain number, snvindex is the index of variable in the strain 
#define EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(sth, snvindex)            (EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_strain(sth) + (snvindex))

// -- macro which computes the off part of the nvid of a nv in the mais mth when inserted inside a bodypart
//    mth is the mais number, mnvindex is the index of variable in the mais 
#define EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(mth, mnvindex)            (EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_mais(mth) + (mnvindex))


// -- the nvid of all the network variables of the strain as a function of the strain number s

#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig(s)                                          EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__mode(s)                                    EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__mode, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__mode))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__datarate(s)                                EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__datarate, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__datarate))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__signaloncefullscale(s)                     EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__signaloncefullscale, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__signaloncefullscale))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus(s)                                          EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus))        
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__fullscale(s)                               EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__fullscale, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__fullscale))        
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__calibratedvalues(s)                        EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__calibratedvalues, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__calibratedvalues))  
#define EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__uncalibratedvalues(s)                      EO_nv_ID(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__uncalibratedvalues, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__uncalibratedvalues))  
 
// -- the nvid of all the network variables of the mais as a function of the mais number m

#define EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig(m)                                          EO_nv_ID(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__mode(m)                                    EO_nv_ID(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__mode, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__mode))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__datarate(m)                                EO_nv_ID(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__datarate, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__datarate))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__resolution(m)                              EO_nv_ID(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__resolution, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__resolution))
#define EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mstatus(m)                                          EO_nv_ID(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mstatus, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mstatus))        
#define EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mstatus__the15values(m)                             EO_nv_ID(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mstatus__the15values, EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mstatus__the15values))        

       
  


      


// - declaration of public user-defined types ------------------------------------------------------------------------- 

EO_VERIFYproposition(EOK_cfg_nvsEP_as_any_con_body, (1024 > EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_mais(EOK_cfg_nvsEP_as_any_con_mxx_maxnumof_nvs_in_mais)));


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @}            
    end of group eo_h2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


