/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eom_appl_basic_main.c
	@brief      This file keeps the main of an application using the embobj
	@author     marco.accame@iit.it
    @date       01/08/2014
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

// abslayer 
#include "hal.h"
#include "hal_trace.h"
#include "osal.h"


// embobj  
#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
//#include "EOMtheApplication.h"



// keeps specialisation proper to the board function
#include "eom_applipnet_specialise.h"



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

static void s_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


extern const hal_cfg_t     hal_cfg;                 // externally defined and modifiable via hal_cfg.h
extern const osal_cfg_t    osal_cfg;                // externally defined and modifiable via osal_cfg.h


static const eOmsystem_cfg_t syscfg =
{
    .codespaceoffset    = 0,                        // the application has zero offset (no loader or updater)
    .halcfg             = &hal_cfg,                 // the used hal cfg
    .osalcfg            = &osal_cfg,                // the used osal cfg
    .fsalcfg            = NULL                      // forget it
};

static const eOmempool_alloc_config_t osalheapconf =
{
    .heap =
    {
        .allocate       = osal_base_memory_new,
        .reallocate     = osal_base_memory_realloc,
        .release        = osal_base_memory_del
    }
};

static const eOmempool_cfg_t memcfg =
{
    .mode               = eo_mempool_alloc_dynamic, // we use static allcoation
    .conf               = &osalheapconf
};

static const eOerrman_cfg_t  errcfg = 
{
    .extfn.usr_on_error = s_OnError
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    // init the system with basic services
    eom_sys_Initialise( &syscfg,
                        &memcfg,                                // mempool 
                        &errcfg,                                // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  
    
    // start the system and use eom_applipnet_specialise_system() to add more services ...
    eom_sys_Start(eom_sys_GetHandle(), eom_applipnet_specialise_system);
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// it is the function called by the EOtheErrorManager
static void s_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[128];

    snprintf(str, sizeof(str)-1, "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }

    for(;;);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



