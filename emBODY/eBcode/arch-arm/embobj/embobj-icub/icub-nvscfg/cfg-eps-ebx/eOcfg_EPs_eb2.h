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
#ifndef _EOCFG_EPS_EB2_H_
#define _EOCFG_EPS_EB2_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOcfg_EPs_eb2.h
	@brief      This header file contains a const configuration for the endpoints of board EB2 (left arm, lower)
	@author     marco.accame@iit.it
	@date       05/02/2012
**/

/** @defgroup eo_qw1234fr Configuation of the NVs for management of the ems board
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"
#include "EOnvsCfg.h"



// - public #define  --------------------------------------------------------------------------------------------------

// - in here there are the nvids plus ... 

#include "eOcfg_nvsEP_mn.h"
#include "eOcfg_nvsEP_mn_comm_con.h"
#include "eOcfg_nvsEP_mn_appl_con.h"
#include "eOcfg_nvsEP_mc_any_con_bodypart.h"
#include "eOcfg_nvsEP_mc_lowerarm_con.h"
#include "eOcfg_nvsEP_as_onemais_con.h"

#include "eOcfg_nvsEP_sk_emsboard_con.h"


// - in here there is access to ram of the endpoints
#include "eOcfg_nvsEP_mn_comm_usr.h"
#include "eOcfg_nvsEP_mn_appl_usr.h"
#include "eOcfg_nvsEP_mc_lowerarm_usr.h"
#include "eOcfg_nvsEP_as_onemais_usr.h"
#include "eOcfg_nvsEP_sk_emsboard_usr.h"




// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

/** @fn         extern void* eo_cfg_nvsEP_eb2_Get_remotelyownedRAM(eOnvEP_t ep, eOnvscfgOwnership_t ownership)
    @brief      retrieves the RAM of the endpoint remeotely owned specified by @e ep if loaded by the EOnvsCfg object with eo_cfg_EPs_vectorof_ebx .
    @param      ep              The endpoint.
    @param      ownnership      local or remote.
    @return     The pointer to teh memory or NULL if the memory is not initialised yet or if the ep is not recognised
 **/
extern void* eo_cfg_nvsEP_eb2_Get_remotelyownedRAM(eOnvEP_t ep, eOnvscfgOwnership_t ownership);

/** @fn         extern void* eo_cfg_nvsEP_eb2_Get_locallyownedRAM(eOnvEP_t ep)
    @brief      retrieves the RAM of the endpoint locally owned specified by @e ep if loaded by the EOnvsCfg object with eo_cfg_EPs_vectorof_ebx .
    @param      ep              The endpoint.
    @return     The pointer to teh memory or NULL if the memory is not initialised yet or if the ep is not recognised
 **/
extern void* eo_cfg_nvsEP_eb2_Get_locallyownedRAM(eOnvEP_t ep);


// EOconstvector where each element is a eOnvscfg_EP_t
extern const EOconstvector* const eo_cfg_EPs_vectorof_eb2;

// if not NULL it contains a mapping from EPs to index inside eo_cfg_EPs_vectorof_eb2
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_we1234fr  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




