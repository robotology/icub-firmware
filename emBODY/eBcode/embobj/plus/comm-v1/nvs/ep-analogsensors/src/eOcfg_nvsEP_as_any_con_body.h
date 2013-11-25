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
#ifndef _EOCFG_NVSEP_AS_ANY_CON_BODY_H_
#define _EOCFG_NVSEP_AS_ANY_CON_BODY_H_




/** @file       eOcfg_nvsEP_as_any_con_body.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_2uilsdede2345er Configuration of the bodypart 
    In here there are constants for the max number of strains and maiss in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EoMotionControl.h"
#include "eOcfg_nvsEP_as.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eo_cfg_nvsEP_as_any_con_body_strainNumber_t;
    @brief      It contains an index for all the strains in a bodypart. In a given bodypart, there are strains for a number lower
                than strainBody_TOTALnumber. The numbers are consecutive and without holes, so that the enum value can be changed 
                by a normal index.
 **/
typedef enum
{
    strainBody_00 = 0, strainBody_01, strainBody_02, strainBody_03
} eo_cfg_nvsEP_as_any_con_body_strainNumber_t;

enum { strainBody_TOTALnumber = 4};



/** @typedef    typedef enum eo_cfg_nvsEP_as_any_con_body_maisBody_t;
    @brief      It contains an index for all the maiss in a bodypart. In a given bodypart, there are maiss for a number lower
                than maisBody_TOTALnumber. The numbers are consecutive and without holes.
 **/
typedef enum
{
    maisBody_00 = 0, maisBody_01
} eo_cfg_nvsEP_as_any_con_body_maisNumber_t;

enum { maisBody_TOTALnumber = 2};


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOnvID_t eo_cfg_nvsEP_as_any_con_body_strain_NVID_Get(eo_cfg_nvsEP_as_any_con_body_strainNumber_t s, eOcfg_nvsEP_as_strainNVindex_t snvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e snvindex for the strain number @e s
    @param      s               the strain number 
    @param      snvinxed        the index of the nv inside the strain
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_as_any_con_body_strain_NVID_Get(eo_cfg_nvsEP_as_any_con_body_strainNumber_t s, eOcfg_nvsEP_as_strainNVindex_t snvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_as_any_con_body_mais_NVID_Get(eo_cfg_nvsEP_as_any_con_body_maisNumber_t m, eOcfg_nvsEP_as_maisNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the mais number @e m
    @param      m               the mais number 
    @param      mnvinxed        the index of the nv inside the mais
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_as_any_con_body_mais_NVID_Get(eo_cfg_nvsEP_as_any_con_body_maisNumber_t m, eOcfg_nvsEP_as_maisNVindex_t mnvindex);


/** @}            
    end of group eo_2uilsdede2345er  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




