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
#include "stdio.h"
#include "hal_trace.h"

#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheErrorManager.h"
#include "EOMtheEMSappl.h"

#include "eom_emsappl_info.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
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

static void s_eom_emsappl_main_startup_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOerrman_cfg_t  s_eom_emsappl_main_errcfg_startup = 
{
    .extfn.usr_on_error = s_eom_emsappl_main_startup_OnError
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{

    eom_sys_Initialise( &eom_emsappl_info_syscfg,
                        NULL,                         // mempool uses calloc
                        &s_eom_emsappl_main_errcfg_startup,                
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

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
    eOemsappl_cfg_t cfg;
    memcpy(&cfg, &eom_emsappl_DefaultCfg, sizeof(eOemsappl_cfg_t));
    cfg.emsappinfo = &eom_emsappl_info_modinfo;
    cfg.hostipv4addr = EO_COMMON_IPV4ADDR(10, 255, 72, 205);
    //cfg.hostipv4addr = EO_COMMON_IPV4ADDR(10, 0, 0, 254);
    cfg.hostipv4port = 33333;
    
    eom_emsappl_Initialise(&cfg);
}



static void s_eom_emsappl_main_startup_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[128];

    snprintf(str, sizeof(str)-1, "startup: [eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }
    
    eov_sys_Stop(eov_sys_GetHandle());

    for(;;);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



