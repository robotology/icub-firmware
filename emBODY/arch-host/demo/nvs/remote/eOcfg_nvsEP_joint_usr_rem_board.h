
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOCFG_NVSEP_JOINT_USR_REM_ANYDEV_H_
#define _EOCFG_NVSEP_JOINT_USR_REM_ANYDEV_H_




/** @file       eOcfg_nvsEP_joinit_usr_rem_board.h
	@brief      This header file gives the configuration for the NVs in the JOINT port board
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_asfdgr Configuation of the NVs for the updater
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"


#include "eOcfg_nvsEP_joint_con.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const EOconstvector* const eo_cfg_nvsEP_joint_usr_rem_board_constvector_of_EOnv_usr;

extern eo_cfg_nvsEP_joint_t* eo_cfg_nvsEP_joint_usr_rem_board_mem_local;
extern eo_cfg_nvsEP_joint_t* eo_cfg_nvsEP_joint_usr_rem_board_mem_remote;



// - declaration of extern public functions ---------------------------------------------------------------------------

extern void eo_cfg_nvsEP_joint_usr_rem_board_initialise(void* loc, void* rem);


/** @}            
    end of group eo_asfdgr  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




