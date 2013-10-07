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
#ifndef _EOCFG_EPS_REM_BOARD_H_
#define _EOCFG_EPS_REM_BOARD_H_




/** @file       eOcfg_EPs_rem_board.h
	@brief      This header file contains a const configuration for the local endpoints managed by a given board for remote use
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_1234frr Configuation of the NVs for management of the ems board for use of teh remote host pc104
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"
#include "EOnvsCfg.h"



// - public #define  --------------------------------------------------------------------------------------------------

// include EP definition
#include "eOcfg_nvsEP_base_con.h"
#include "eOcfg_nvsEP_base_usr_rem_anydev.h"

#include "eOcfg_nvsEP_mngmnt_con.h"
#include "eOcfg_nvsEP_mngmnt_usr_rem_board.h"

#include "eOcfg_nvsEP_mc_any_con_bodypart.h"

#include "eOcfg_nvsEP_mc_leftleg_con.h"
#include "eOcfg_nvsEP_mc_leftleg_usr_rem_ebx.h"


// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// EOconstvector where each element is a eOnvscfg_EP_t
extern const EOconstvector* const eo_cfg_EPs_vectorof_rem_board;

// if not NULL it contains a mapping from EPs to index inside eo_cfg_EPs_vectorof_rem_board
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_rem_board_fptr_hashfunction_ep2index;


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_1234frr  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




