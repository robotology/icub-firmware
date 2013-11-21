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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"


#include "EONtask.h"

#include "EOVtask.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EONtheCallbackManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EONtheCallbackManager_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_eon_callbackman_tsk_aftercallbackorontimeout(EONtask *rt, uint32_t msg);

static eOresult_t s_eon_callbackman_execute(EOVtheCallbackManager *v, eOcallback_t cbk, void *arg, eOreltime_t tout);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EONtheCallbackManager";
 
static EONtheCallbackManager s_eon_callbackmanager = 
{
    .vcm    = NULL,
    .tsk    = NULL
}; 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EONtheCallbackManager * eon_callbackman_Initialise(const eOmcallbackman_cfg_t *cbkmancfg) 
{
    if(NULL != s_eon_callbackmanager.tsk) 
    {
        // already initialised
        return(&s_eon_callbackmanager);
    }


 
    // i prepare the task able to execute callbacks actions associated to expiry of the timers or on gpio
    s_eon_callbackmanager.tsk = eon_task_New();

    // i initialise the base callback manager
    eov_callbackman_hid_Initialise(s_eon_callbackman_execute, s_eon_callbackmanager.tsk);
    
    return(&s_eon_callbackmanager);
}    


extern EONtheCallbackManager* eon_callbackman_GetHandle(void)
{
    if(NULL == s_eon_callbackmanager.tsk) 
    {
        return(NULL);
    }
    
    return(&s_eon_callbackmanager);
}


extern eOresult_t eon_callbackman_Execute(EONtheCallbackManager *p, eOcallback_t cbk, void *arg, eOreltime_t tout) 
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    return(eores_NOK_generic);
}


extern EONtask * eon_callbackman_GetTask(EONtheCallbackManager *p) 
{
    if(NULL == p) 
    {
        return(NULL);
    }
    
    return(s_eon_callbackmanager.tsk);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





