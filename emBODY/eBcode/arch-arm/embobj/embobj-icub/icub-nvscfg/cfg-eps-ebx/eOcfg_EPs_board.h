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



// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_board_EPs_numberof_get(const EOconstvector* constvectorof_nvscfgep);

extern eOnvscfg_EP_t* eo_cfg_nvsEP_board_EPs_cfg_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex);

extern eOnvEP_t eo_cfg_nvsEP_board_EPs_endpoint_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex);

extern uint16_t eo_cfg_nvsEP_board_NVs_total_numberof_get(const EOconstvector* constvectorof_nvscfgep);

extern uint16_t eo_cfg_nvsEP_board_NVs_endpoint_numberof_get(const EOconstvector* constvectorof_nvscfgep, eOnvEP_t ep);

extern eOnvID_t eo_cfg_nvsEP_board_NVs_endpoint_NVID_get(const EOconstvector* constvectorof_nvscfgep, eOnvEP_t ep, uint16_t nvindex);


/** @}            
    end of group eo_asdgfgr1111234fr  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




