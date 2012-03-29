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
#ifndef _EOCFG_NVSEP_MNGMNT_USR_LOC_BOARD_H_
#define _EOCFG_NVSEP_MNGMNT_USR_LOC_BOARD_H_




/** @file       eOcfg_nvsEP_mngmnt_usr_loc_board.h
	@brief      This header file gives the local configuration for the NVs in the management port of any board
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_sdfwervcer Local configuation of the management NVs for the board (an ems or mcbstm2x)
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"
#include "eOcfg_nvsEP_mngmnt_con.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const EOconstvector* const eo_cfg_nvsEP_mngmnt_usr_loc_board_constvector_of_EOnv_usr;

extern eo_cfg_nvsEP_mngmnt_t* eo_cfg_nvsEP_mngmnt_usr_loc_board_mem_local;



// - declaration of extern public functions ---------------------------------------------------------------------------

extern void eo_cfg_nvsEP_mngmnt_usr_loc_board_initialise(void* loc, void* rem);


/** @}            
    end of group eo_sdfwervcer  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




