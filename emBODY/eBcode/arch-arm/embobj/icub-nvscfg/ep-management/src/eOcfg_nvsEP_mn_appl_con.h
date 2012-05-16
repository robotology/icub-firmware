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
#ifndef _EOCFG_NVSEP_MN_APPL_CON_H_
#define _EOCFG_NVSEP_MN_APPL_CON_H_




/** @file       eOcfg_nvsEP_mn_appl_con.h
	@brief      This header file gives the constant configuration for the NVs in the management endpoint port
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_silsdede Configuation of the NVs for management of the ems board
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"

#include "EOarray.h"
#include "EOnv.h"
#include "EoManagement.h"
#include "eOcfg_nvsEP_mn.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef struct eo_cfg_nvsEP_mn_appl_t;
    @brief      contains all the variables in the mn comm endpoint which is used to configure communication 
 **/
typedef struct                  // size is 24+0 = 128 bytes
{
    eOmn_appl_t                 application;        
} eo_cfg_nvsEP_mn_appl_t;       EO_VERIFYsizeof(eo_cfg_nvsEP_mn_appl_t, 24)

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eo_cfg_nvsEP_mn_appl_t *eo_cfg_nvsEP_mn_appl_defaultvalue;


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOnvID_t eo_cfg_nvsEP_mn_appl_appl_NVID_Get(eOcfg_nvsEP_mn_applNVindex_t nvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nvindex.
                As an alternative use the #define values in the _hid.h file.
    @param      nvinxed         the index of the nv
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mn_appl_appl_NVID_Get(eOcfg_nvsEP_mn_applNVindex_t nvindex);



// - declarations used to load the endpoint into a EOnvsCfg object ----------------------------------------------------



// it is a EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_mn_appl_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_mn_appl_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mn_appl_fnptr_hashfunction_id2index;

// the size of the ram to be used
#define EOK_cfg_nvsEP_mn_appl_RAMSIZE                                            (sizeof(eo_cfg_nvsEP_mn_appl_t))

// same as the eo_cfg_nvsEP_mn_appl_fnptr_hashfunction_id2index pointer  
extern uint16_t eo_cfg_nvsEP_mn_appl_hashfunction_id2index(uint16_t nvid);



/** @}            
    end of group eo_silsdede  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




