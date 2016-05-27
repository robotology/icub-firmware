/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOwatchdog.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOwatchdog_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_watchdog_configure(EOwatchdog* wd, eOwatchdog_cfg_t *cfg);
static void s_eo_watchdog_reset(EOwatchdog* wd);
static void s_eo_watchdog_run(EOwatchdog* wd);
static void s_eo_watchdog_diagnosticconfig(EOwatchdog* wd, eOwatchdog_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const eOwatchdog_cfg_t cfg_default = 
{
    .diagncfg = 
    {
        .numoffailures = 0,
        .functiononfailure = NULL,
    },
    .period = 100000 //1 sec
}; 

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOwatchdog* eo_watchdog_new(eOwatchdog_cfg_t *cfg)
{
    EOwatchdog *retptr = NULL;    

    // I get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOwatchdog), 1);
    
    s_eo_watchdog_configure(retptr, cfg);
    
    s_eo_watchdog_reset(retptr);
    
    s_eo_watchdog_diagnosticconfig(retptr, cfg);

    return(retptr);

}


extern eOresult_t eo_watchdog_updateconfigperiod(EOwatchdog* wd, eOreltime_t period)
{
    if(NULL == wd)
    {
        return(eores_NOK_generic);
    }
    
    eObool_t wasrunning = wd->isrunning;
    
    s_eo_watchdog_reset(wd);
    //s_eo_watchdog_configure(wd, cfg);
    wd->cfg.period = period;
    if(wasrunning)
    {
        s_eo_watchdog_run(wd);
    }
    return(eores_OK);

}
extern eOresult_t eo_watchdog_start(EOwatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    
    s_eo_watchdog_run(wd);
    
    return(eores_OK);
}


extern eObool_t eo_watchdog_check(EOwatchdog* wd)
{
    eOabstime_t time;
    eObool_t ret = eobool_false;
    
    if(NULL == wd)
    {
        return(ret);
    }
    
    if(eobool_false == wd->isrunning)
    {
        return(ret);
    }
    
    time = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    
    if( (time - wd->starttime) <= wd->cfg.period)
    {
        ret = eobool_true;
        wd->count_failures = 0; //resets num of failure
    }
    
    if((!ret) && (wd->diagnostic_enabled)) //if check is failed and diagnostic is enabled
    {
        wd->count_failures++;
        if((wd->count_failures == 1) || (wd->count_failures > wd->cfg.diagncfg.numoffailures)) //if is the first failure or num of failures are already bigger than configured number, then I call function
        {
            wd->cfg.diagncfg.functiononfailure();
        }
        
        if(wd->count_failures > wd->cfg.diagncfg.numoffailures)
        {
            wd->count_failures = 0;
        }
    }
    
    return(ret);
}

extern eOresult_t eo_watchdog_rearm(EOwatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == wd->isrunning)
    {
        return(eores_NOK_generic);
    }
    
    wd->starttime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    return(eores_OK);
}

extern eOresult_t eo_watchdog_stop(EOwatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    
    s_eo_watchdog_reset(wd);
    
    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_watchdog_configure(EOwatchdog* wd, eOwatchdog_cfg_t *cfg)
{
    
    if(NULL == cfg)
    {
       
        memcpy(&(wd->cfg), &cfg_default, sizeof(eOwatchdog_cfg_t));
    }
    else
    {
        memcpy(&(wd->cfg), cfg, sizeof(eOwatchdog_cfg_t));
    }

}

static void s_eo_watchdog_reset(EOwatchdog* wd)
{
    wd->isrunning = eobool_false;
    wd->starttime = 0;
}

static void s_eo_watchdog_run(EOwatchdog* wd)
{
    wd->isrunning = eobool_true;
    wd->starttime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
}

static void s_eo_watchdog_diagnosticconfig(EOwatchdog* wd, eOwatchdog_cfg_t *cfg)
{
    wd->count_failures = 0;
    wd->diagnostic_enabled = 0;
    

    if((NULL != cfg) && (NULL != cfg->diagncfg.functiononfailure))
    {
        wd->diagnostic_enabled = 1;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



