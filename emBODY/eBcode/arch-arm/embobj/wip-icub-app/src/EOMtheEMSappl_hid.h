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
#ifndef _EOMTHEEMSAPPL_HID_H_
#define _EOMTHEEMSAPPL_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSappl_hid.h
    @brief      This header file implements hidden interface to the error manager
    @author     marco.accame@iit.it
    @date       10/15/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "osal.h"
#include "EOtheBOARDtransceiver.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSappl.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMtheEMSappl_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMSappl_hid 
{
    EOsm*               sm;
	eOemsappl_cfg_t     cfg;
    osal_timer_t        *timer4led;     //this timer is used to blink led (orange) when application is in configuration state.
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section

extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p);


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p);

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p);


extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p);

extern void eom_emsappl_hid_userdef_on_exit_ERR(EOMtheEMSappl* p);


extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p);

extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

