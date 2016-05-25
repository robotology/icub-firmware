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

#include "EOCanMsgWatchdog.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOCanMsgWatchdog_hid.h"


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
static void s_eo_canmsg_watchdog_configure(EOCanMsgWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg);
static void s_eo_canmsg_watchdog_reset(EOCanMsgWatchdog* wd);
static void s_eo_canmsg_watchdog_run(EOCanMsgWatchdog* wd);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const eOcanmsg_watchdog_cfg_t cfg_default = {.period = 100}; //100 ms

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOCanMsgWatchdog* eo_canmsg_watchdog_new(eOcanmsg_watchdog_cfg_t *cfg)
{
    EOCanMsgWatchdog *retptr = NULL;    

    // I get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOCanMsgWatchdog), 1);
    
    s_eo_canmsg_watchdog_configure(retptr, cfg);
    
    s_eo_canmsg_watchdog_reset(retptr);

    return(retptr);

}


extern eOresult_t eo_canmsg_watchdog_updateconfig(EOCanMsgWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg)
{
    if(NULL == wd)
    {
        return(eores_NOK_generic);
    }
    
    eObool_t wasrunning = wd->isrunning;
    
    s_eo_canmsg_watchdog_reset(wd);
    s_eo_canmsg_watchdog_configure(wd, cfg);
    if(wasrunning)
    {
        s_eo_canmsg_watchdog_run(wd);
    }
    return(eores_OK);

}
extern eOresult_t eo_canmsg_watchdog_start(EOCanMsgWatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    
    s_eo_canmsg_watchdog_run(wd);
    
    return(eores_OK);
}


extern eObool_t eo_canmsg_watchdog_check(EOCanMsgWatchdog* wd)
{
    eOabstime_t time;
    if(NULL == wd)
    {
        return(eobool_false);
    }
    if(eobool_false == wd->isrunning)
    {
        return(eobool_false);
    }
    
    time = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    if( (time - wd->starttime) > wd->cfg.period)
    {
        return(eobool_false);
    }
    else
    {
        return(eobool_true);
    }
}

extern eOresult_t eo_canmsg_watchdog_rearm(EOCanMsgWatchdog* wd)
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

extern eOresult_t eo_canmsg_watchdog_stop(EOCanMsgWatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    
    s_eo_canmsg_watchdog_reset(wd);
    
    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_canmsg_watchdog_configure(EOCanMsgWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg)
{
    
    if(NULL == cfg)
    {
       
        memcpy(&(wd->cfg), &cfg_default, sizeof(eOcanmsg_watchdog_cfg_t));
    }
    else
    {
        memcpy(&(wd->cfg), cfg, sizeof(eOcanmsg_watchdog_cfg_t));
    }

}

static void s_eo_canmsg_watchdog_reset(EOCanMsgWatchdog* wd)
{
    wd->isrunning = eobool_false;
    wd->starttime = 0;
}

static void s_eo_canmsg_watchdog_run(EOCanMsgWatchdog* wd)
{
    wd->isrunning = eobool_true;
    wd->starttime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



