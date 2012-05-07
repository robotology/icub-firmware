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

#ifndef _EOCFG_NVSEP_SK_H_
#define _EOCFG_NVSEP_SK_H_




/** @file       eOcfg_nvsEP_sk.h
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

#include "EoSkin.h"

 

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section



// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef enum
{
    endpoint_sk_emsboard_leftlowerarm           = 0x0032,        
    endpoint_sk_emsboard_rightlowerarm          = 0x0034    
} eo_cfg_nvsEP_sk_endpoint_t;



/** @typedef    typedef struct eo_cfg_nvsEP_sk_skinNVindex_t
    @brief      It contains an index for all the network variables in the skin.
 **/
typedef enum
{
    skinNVindex_sconfig__sigmode                        =  0,
    skinNVindex_sstatus__arrayof10canframe              =  1
} eo_cfg_nvsEP_sk_skinNVindex_t;

enum { skinNVindex_TOTALnumber = 2 };

/** @typedef    typedef uint16_t eo_cfg_nvsEP_sk_skinNumber_t;
    @brief      It contains an index for a skin in a manner that is independnt from the endpoint.
 **/
typedef uint16_t eo_cfg_nvsEP_sk_skinNumber_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOskin_someskin_t* eo_cfg_nvsEP_sk_someskin_defaultvalue;


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern eOnvID_t eo_cfg_nvsEP_sk_NVID_Get(eo_cfg_nvsEP_sk_endpoint_t ep, eo_cfg_nvsEP_sk_skinNumber_t s, eo_cfg_nvsEP_sk_skinNVindex_t snvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e snvindex for the skin
    @param      ep              the endpoint
    @param      s               the skin number (always 0)
    @param      snvindex        the index of the nv inside the skin
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_sk_NVID_Get(eo_cfg_nvsEP_sk_endpoint_t ep, eo_cfg_nvsEP_sk_skinNumber_t s, eo_cfg_nvsEP_sk_skinNVindex_t snvindex);



/** @}            
    end of group eo_werasfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







