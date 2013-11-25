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
#ifndef _EOCFG_SM_EMSAPPL_HID_H_
#define _EOCFG_SM_EMSAPPL_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       eOcfg_sm_EMSappl_hid.h
    @brief      This header file implements hidden interface to a uml state machine. it contains weakly defined prototypes.
    @author     marco.accame@iit.it
    @date       05/17/2012
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "eOcfg_sm_EMSappl.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

 
// -- init
extern void eo_cfg_sm_EMSappl_hid_init(EOsm *s);  

// -- reset
extern void eo_cfg_sm_EMSappl_hid_reset(EOsm *s);


// -- on entry
extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s);
extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s);
extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s);



// -- on exit 
extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s);
extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s);
extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s);



// -- on trans

extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2run(EOsm *s);
extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2err(EOsm *s);

extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2cfg(EOsm *s);
extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2err(EOsm *s);




#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



