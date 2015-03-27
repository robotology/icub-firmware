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

/* @file       eom_emsappl_main.c
	@brief      This file keeps the main of an application on ems using the embobj
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




#include "stdint.h"
#include "stdlib.h"
#include "string.h"



#include "EOMtheSystem.h"

#include "EOMtheEMSapplCfg.h"
#include "EOMtheEMSappl.h"

#include "OPCprotocolManager_Cfg.h" 
#include "EOtheEMSapplDiagnostics.h"
#include "emBODYrobot.h"

#include "EOtheLEDpulser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emsappl_main_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_Initialise();

    eom_sys_Initialise( &emscfg->wsyscfg.msyscfg, 
                        &emscfg->wsyscfg.mempoolcfg,                         
                        &emscfg->wsyscfg.errmancfg,                 
                        &emscfg->wsyscfg.tmrmancfg, 
                        &emscfg->wsyscfg.cbkmancfg 
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

}

//#ifdef _TEST_SEQNUM_
//extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t expected_seqnum)
//{
//    char str[80];
//    snprintf(str, sizeof(str)-1, "SEQ_NUM: rec=%llu expeted=%llu ", rec_seqnum, expected_seqnum);
//    hal_trace_puts(str);
//  
//}
//#endif

extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t expected_seqnum)
{
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());    
    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon  , 0);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         static void s_eom_emsappl_main_init(void)
    @brief      It initialises the emsappl 
    @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init(void)
{      
    // init leds via the EOtheLEDpulser object
    eOledpulser_cfg_t ledpulsercfg =  
    {
        .led_enable_mask    = (1 << eo_ledpulser_led_zero | 1 << eo_ledpulser_led_one | 1 << eo_ledpulser_led_two | 1 << eo_ledpulser_led_three | 1 << eo_ledpulser_led_four | 1 << eo_ledpulser_led_five),
        .led_init           = (eOint8_fp_uint8_cvoidp_t) hal_led_init,
        .led_on             = (eOint8_fp_uint8_t) hal_led_on,
        .led_off            = (eOint8_fp_uint8_t) hal_led_off,
        .led_toggle         = (eOint8_fp_uint8_t) hal_led_toggle
    };

    eo_ledpulser_Initialise(&ledpulsercfg);
    
    // init the ems application
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();    
    eom_emsappl_Initialise(&emscfg->applcfg);
      
    // set the led reserved for link
    if(eores_OK == eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), emscfg->applcfg.hostipv4addr, 5*EOK_reltime100ms))
    {
        // set led0 to ON
       eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
    else
    {
        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



