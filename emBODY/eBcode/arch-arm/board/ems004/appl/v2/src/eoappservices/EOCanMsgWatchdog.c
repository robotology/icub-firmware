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
static eObool_t s_eo_canmsg_watchdog_configure(EOCanMsgsWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOCanMsgsWatchdog* eo_canmsg_watchdog_new(eOcanmsg_watchdog_cfg_t *cfg)
{
    EOCanMsgsWatchdog *retptr = NULL;    

    // I get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOCanMsgsWatchdog), 1);
    
    retptr->state = wd_st_uninitilized;
    
    s_eo_canmsg_watchdog_configure(cfg);

    return(retptr);

}

extern eObool_t eo_canmsg_watchdog_config(EOCanMsgsWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg)
{
    if(NULL == wd)
    {
        return(eobool_false);
    }
    return(s_eo_canmsg_watchdog_configure(cfg));
}

extern eOresult_t eo_canmsg_watchdog_start(EOCanMsgsWatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    if(wd_st_configured != wd->state)
    {
        return(eores_NOK_generic);
    }
    wd->state = wd_st_started;
    wd->starttime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    return(eores_OK);
}


extern eObool_t eo_canmsg_watchdog_check(EOCanMsgsWatchdog* wd)
{
    eOabstime_t time;
    if(NULL == wd)
    {
        return(eobool_false);
    }
    if(wd_st_started != wd->state)
    {
        return(eobool_false);
    }
    
    time = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    if( (time - wd->startedtime) > cfg->period)
    {
        return(eobool_false);
    }
    else
    {
        return(eobool_true);
    }
}

extern eOresult_t eo_canmsg_watchdog_rearm(EOCanMsgsWatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    if(wd_st_started != wd->state)
    {
        return(eores_NOK_generic);
    }
    
    wd->startedtime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    return(eores_OK);
}

extern eOresult_t eo_canmsg_watchdog_stop(EOCanMsgsWatchdog* wd)
{
    if(NULL == wd)
    {
        return(eores_NOK_nullpointer);
    }
    wd->startedtime = 0;
    wd->state = wd_st_configured;
    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eObool_t s_eo_canmsg_watchdog_configure(EOCanMsgsWatchdog* wd, eOcanmsg_watchdog_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        return(eobool_false);
    }
    
    memcpy(wd->cfg, cfg, sizeof(eOcanmsg_watchdog_cfg_t));
    wd->state = wd_st_configured;
    wd->startedtime = 0;
    return(eobool_true);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



