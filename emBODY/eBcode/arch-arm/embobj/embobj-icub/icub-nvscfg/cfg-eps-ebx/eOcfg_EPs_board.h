/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOCFG_EPS_BOARD_H_
#define _EOCFG_EPS_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file           
	@brief      This header file contains public interface of helper functions which work on the data structure exported by
                the eOcfg_EPs_ebX.h file, where X = [1, ...].    
	@author     marco.accame@iit.it
	@date       09/10/2012
**/

/** @defgroup eo_asdgfgr1111234fr Info retrieval from the NVs of the ems board
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EOconstvector.h"
#include "EOnvsCfg.h"

#warning "This include (eOcfg_EPs_allboards) will generate chaos on EMS code!"
#include "eOcfg_EPs_allboards.h"

// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


// - declaration of extern public functions ---------------------------------------------------------------------------

// Legend:
// eOnvEP_t ep 		= unique EP identifier with semantic
// uint16_t epindex = index of ith EP inside that particular board
// eOnvID_t nv		= unique NV identifier with semantic
// eOnvEP_t nvindex	= index of ith NV inside that particular EP

// return constvectorof_nvscfgep for a given board
extern EOconstvector* eo_cfg_nvsEP_board_EPs_nvscfgep_get(uint8_t boardNum);

// return number of EP handled by this board
extern uint16_t eo_cfg_nvsEP_board_EPs_numberof_get(const EOconstvector* constvectorof_nvscfgep);

// return to the configuration structure of the ith EP
extern eOnvscfg_EP_t* eo_cfg_nvsEP_board_EPs_cfg_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex);

// return unique eOnvEP_t EP identifier from epindex
extern eOnvEP_t eo_cfg_nvsEP_board_EPs_endpoint_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex);

// Return epindex from eOnvEP_t ep, this is board-dependent
extern uint16_t eo_cfg_nvsEP_board_EPs_epindex_get(uint8_t boardNum, eOnvEP_t ep);

// return the number of NVs handled by the whole board (sum of all EPs)
extern uint16_t eo_cfg_nvsEP_board_NVs_total_numberof_get(const EOconstvector* constvectorof_nvscfgep);

// return the number of NVs contained in the EP ep
extern uint16_t eo_cfg_nvsEP_board_NVs_endpoint_numberof_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex);

// return the NVID of the i-th NV of the EP ep
extern eOnvID_t eo_cfg_nvsEP_board_NVs_endpoint_NVID_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex, uint16_t nvindex);

// Return the nvindex from the eOnvID_t
extern uint16_t eo_cfg_nvsEP_board_NVs_endpoint_Nvindex_get(const eOnvscfg_EP_t* EPcfg, eOnvID_t nvid);

/** @}            
    end of group eo_asdgfgr1111234fr  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




