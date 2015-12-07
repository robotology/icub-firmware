/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheETHmonitor.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheETHmonitor_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const eOethmonitor_cfg_t eo_ethmonitor_DefaultCfg = 
{
    .priority           = 10,
    .stacksize          = 1024,
    .period             = 100*EOK_reltime1ms
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_ethmonitor_taskworker_run(EOMtask *rt, uint32_t n);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheETHmonitor s_eo_theethmonitor = 
{
    .initted                = eobool_false,
    .started                = eobool_false,
    .newresultsavailable    = eobool_false,
    .taskworker             = NULL,
    .semaphoreworker        = NULL
};

//static const char s_eobj_ownname[] = "EOtheETHmonitor";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheETHmonitor* eo_ethmonitor_Initialise(const eOethmonitor_cfg_t *cfg)
{
    if(eobool_true == s_eo_theethmonitor.initted)
    {
        return(&s_eo_theethmonitor);
    }
    
    if(NULL == cfg)
    {
        cfg = &eo_ethmonitor_DefaultCfg;       
    }
    
    // create all synch data (semaphore, results, etc)
    
    s_eo_theethmonitor.semaphoreworker = osal_semaphore_new(1, 1);

    // start the periodic task
    
    s_eo_theethmonitor.taskworker = eom_task_New(   eom_mtask_Periodic, cfg->priority, cfg->stacksize,
                                                    NULL, s_eo_ethmonitor_taskworker_run,
                                                    0, cfg->period,
                                                    NULL, 
                                                    eo_ethmonitor,
                                                    "ethmonitor"
                                                    );
    
    return(&s_eo_theethmonitor);   
}


extern EOtheETHmonitor* eo_ethmonitor_GetHandle(void)
{
    if(eobool_true == s_eo_theethmonitor.initted)
    {
        return(&s_eo_theethmonitor);
    }
    return(NULL);
}


extern eOresult_t eo_ethmonitor_Tick(EOtheETHmonitor *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // we decrement a semaphore w/ zero timeout. 
    // if we succeeds then:     if we have a new result { process it, set result to old}, we increment
    // else:                    we do nothing (it means that the periodic thread is busy querying the micrel switch)
    
    return(eores_OK);
}


extern eOresult_t eo_ethmonitor_Start(EOtheETHmonitor *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_true == s_eo_theethmonitor.started)
    {   // nothing to do because it is already started
        return(eores_OK);
    }
    
    s_eo_theethmonitor.started = eobool_true;
          
    // now, i do things. 
    
 
    
    return(eores_OK);

}


extern eOresult_t eo_ethmonitor_Stop(EOtheETHmonitor *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theethmonitor.started)
    {   // nothing to do because it is already stopped
        return(eores_OK);
    }
    
    s_eo_theethmonitor.started = eobool_false;
          
    // now, i do things. 
    
 
    
    return(eores_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// name of the task as it is shown in uvision
void eo_ethmonitor(void *p)
{
     eom_task_Start(p);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_ethmonitor_taskworker_run(EOMtask *rt, uint32_t n)
{
    // we do query of the micrel switch but we wait for the availability of the semaphore. then we release it
    if(osal_res_OK != osal_semaphore_decrement(s_eo_theethmonitor.semaphoreworker, osal_reltimeINFINITE))
    {
        return;
    }
    
    // begin of activity.
    
    
    
    
    // end of activity
    
    osal_semaphore_increment(s_eo_theethmonitor.semaphoreworker, osal_callerTSK);
}


#if 0
//#define EOSKIN_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE
static eObool_t s_eo_ethmonitor_activeskin_can_accept_canframe(void)
{
#if     defined(EOSKIN_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE)
    // in this case the service.active skin always accepts can frames, even if the run mode is not.
    // it is up to robotInterface to put system in run mode, so that the overflow of fifo is avoided
    return(eobool_true);
#else
    // in this mode the service.active skin accepts can frames only if we are in run mode, thus the fifo can be emptied.    
    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STerr;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun == applstate)
    {
        return(eobool_true);
    }    
    return(eobool_false);    
#endif       
}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



