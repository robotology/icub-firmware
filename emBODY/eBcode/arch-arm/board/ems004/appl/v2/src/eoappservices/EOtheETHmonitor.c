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
#include "EOVtheSystem.h"
#include "hal_trace.h"


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

static void s_eo_ethmonitor_query_micrel(void);

static void s_eo_ethmonitor_process_resultsofquery(void);

static void s_eo_ethmonitor_print_delta(const char *prefix, uint32_t delta);

static void s_eo_ethmonitor_print_timeoflife(const char *prefix);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheETHmonitor s_eo_theethmonitor = 
{
    .initted                = eobool_false,
    .enabled                = eobool_false,
    .newresultsavailable    = eobool_false,
    .taskworker             = NULL,
    .semaphoreworker        = NULL,
    .alertsemaphore         = NULL,
    .task2alert             = NULL,
    .alertevent             = 0,
    .upmask                 = 0,                 
    .portstatus             = {0}
};

static const char s_eobj_ownname[] = "EOtheETHmonitor";


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
    
    s_eo_theethmonitor.initted = eobool_false;
    s_eo_theethmonitor.newresultsavailable = eobool_false;
    
    
    s_eo_theethmonitor.alertsemaphore = osal_semaphore_new(1, 1);
    s_eo_theethmonitor.task2alert = NULL;
    s_eo_theethmonitor.alertevent = 0;    
    
    s_eo_theethmonitor.upmask = 0;
    memset(&s_eo_theethmonitor.portstatus, 0, sizeof(s_eo_theethmonitor.portstatus));
    
    
    s_eo_theethmonitor.initted = eobool_true;
    
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
    
    if(eobool_false == s_eo_theethmonitor.newresultsavailable)
    {   // nothing new is available
        return(eores_OK);        
    }
    
    // there is something available .... let's see if we can get this info. 
    
    // i try to lock: i get the semaphore but with zero wait.    
    if(osal_res_OK != osal_semaphore_decrement(s_eo_theethmonitor.semaphoreworker, osal_reltimeZERO))
    {   // the semaphore is busy ... i cannot wait
        return(eores_OK);
    }
    
    
    // get results and send diagnostics
    s_eo_ethmonitor_process_resultsofquery();
    
    // i unlock
    osal_semaphore_increment(s_eo_theethmonitor.semaphoreworker, osal_callerTSK);
    
    return(eores_OK);
}


extern eOresult_t eo_ethmonitor_SetAlert(EOtheETHmonitor *p, EOMtask *task, eOevent_t event)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }


    osal_semaphore_decrement(s_eo_theethmonitor.alertsemaphore, osal_reltimeINFINITE);
    
    s_eo_theethmonitor.task2alert = task;
    s_eo_theethmonitor.alertevent = event;
   
    osal_semaphore_increment(s_eo_theethmonitor.alertsemaphore, osal_callerTSK);
    
    return(eores_OK);    
}


extern eOresult_t eo_ethmonitor_Start(EOtheETHmonitor *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_true == s_eo_theethmonitor.enabled)
    {   // nothing to do because it is already enabled
        return(eores_OK);
    }
    
    s_eo_theethmonitor.enabled = eobool_true;
          
    
    return(eores_OK);
}


extern eOresult_t eo_ethmonitor_Stop(EOtheETHmonitor *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theethmonitor.enabled)
    {   // nothing to do because it is already stopped
        return(eores_OK);
    }
    
    s_eo_theethmonitor.enabled = eobool_false;
          
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
    // in here we query the micrel switch but we must: (1) be enabled to do that, and (2) wait for a semaphore which lock access to result data
    
    if(eobool_false == s_eo_theethmonitor.enabled)
    {   
        // we dont protect this variable vs concurrent access. in here we just read it and we dont really care if we do one less or ne more run of checks.
        return;
    }
    
    // we wait for the availability of the semaphore. at endo of processing we release it
    if(osal_res_OK != osal_semaphore_decrement(s_eo_theethmonitor.semaphoreworker, osal_reltimeINFINITE))
    {
        return;
    }
    
    // begin of activity.
    
    s_eo_ethmonitor_query_micrel();
    
    // end of activity
    
    osal_semaphore_increment(s_eo_theethmonitor.semaphoreworker, osal_callerTSK);
    
    
    // shall we alert any task?
    if(eobool_true == s_eo_theethmonitor.newresultsavailable)
    {
        osal_semaphore_decrement(s_eo_theethmonitor.alertsemaphore, osal_reltimeINFINITE);
    
        if((NULL != s_eo_theethmonitor.task2alert) && (0 != s_eo_theethmonitor.alertevent))
        {
            //s_eo_ethmonitor_print_timeoflife("ALERT");
            eom_task_SetEvent(s_eo_theethmonitor.task2alert, s_eo_theethmonitor.alertevent);
        }
   
        osal_semaphore_increment(s_eo_theethmonitor.alertsemaphore, osal_callerTSK);                
    }
    
}

//static char debugprint[128] = {0};

// the worker thread calls is regularly
static void s_eo_ethmonitor_query_micrel(void)
{
    uint8_t mask = 0;
    hal_ethtransceiver_phystatus_t phys[2] = {0};
    uint8_t i = 0;
    uint64_t start = 0;
    uint64_t stop = 0;
    uint32_t delta = 0;
    
    
    s_eo_theethmonitor.newresultsavailable = eobool_false;
    
//    s_eo_ethmonitor_print_timeoflife("QUERY");
    
//    start = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    hal_ethtransceiver_phy_linkupmask(&mask);
//    stop = eov_sys_LifeTimeGet(eov_sys_GetHandle());
//    delta = stop - start;
//    s_eo_ethmonitor_print_delta("hal_ethtransceiver_phy_linkupmask()", delta);

    
    s_eo_theethmonitor.portstatus[0].previouson = s_eo_theethmonitor.portstatus[0].on;
    s_eo_theethmonitor.portstatus[1].previouson = s_eo_theethmonitor.portstatus[1].on;
    
    s_eo_theethmonitor.portstatus[0].on = mask & 0x01;
    s_eo_theethmonitor.portstatus[1].on = (mask>>1) & 0x01;
    
//    start = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    hal_ethtransceiver_phy_status(phys, 2);
//    stop = eov_sys_LifeTimeGet(eov_sys_GetHandle());
//    delta = stop - start;
//    s_eo_ethmonitor_print_delta("hal_ethtransceiver_phy_status(phys, 2)", delta);

    
    for(i=0; i<eOethmonitor_numberofports; i++)
    {
        if(eobool_true == s_eo_theethmonitor.portstatus[i].on)
        {
            memcpy(&s_eo_theethmonitor.portstatus[i].phy, &phys[i], sizeof(hal_ethtransceiver_phystatus_t));
            
//            start = eov_sys_LifeTimeGet(eov_sys_GetHandle());
            hal_ethtransceiver_phy_errorinfo(i, hal_ethtransceiver_phyerror_rxCrc, &s_eo_theethmonitor.portstatus[i].rxcrc); 
//            stop = eov_sys_LifeTimeGet(eov_sys_GetHandle());            
//            delta = stop - start;
//            s_eo_ethmonitor_print_delta("hal_ethtransceiver_phy_errorinfo()", delta);

        }
    }
      
    s_eo_theethmonitor.newresultsavailable = eobool_true;    
}

// the control loop / the config task calls _Tick() and then calls this function. the semaphore protects it.
static void s_eo_ethmonitor_process_resultsofquery(void)
{
    if(eobool_false == s_eo_theethmonitor.newresultsavailable)
    {   // one more control under semaphore protection ... 
        return;
    }
    
    eOerrmanDescriptor_t errdes = {0};
    
    
    // ok, now i evaluate the results and if required i send proper diagnostics ...
    
//    s_eo_ethmonitor_print_timeoflife("PROCESS");
    
    uint8_t i = 0;
    
    for(i=0; i<eOethmonitor_numberofports; i++)
    {
        if(eobool_true == s_eo_theethmonitor.portstatus[i].on)
        {
            if(eobool_false == s_eo_theethmonitor.portstatus[i].previouson)
            {
                errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_link_goes_up);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.par16            = i;
                errdes.par64            = 0;
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);                
            }
            
            if((1 == s_eo_theethmonitor.portstatus[i].rxcrc.validvalue) && (s_eo_theethmonitor.portstatus[i].rxcrc.value != 0))
            {
                errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_error_rxcrc);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.par16            = i;
                errdes.par64            = s_eo_theethmonitor.portstatus[i].rxcrc.value;    
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);                
            }
        }
        else
        {
            if(eobool_true == s_eo_theethmonitor.portstatus[i].previouson)
            {
                errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_link_goes_down);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.par16            = i;
                errdes.par64            = 0;
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);                
            }            
        }
    }
     
    s_eo_theethmonitor.newresultsavailable = eobool_false;    
}


static void s_eo_ethmonitor_print_delta(const char *prefix, uint32_t delta)
{
    static char strA[64] = {0};  
    snprintf(strA, sizeof(strA), "%s takes %d usec", prefix, delta);    
    hal_trace_puts(strA);    
}

static void s_eo_ethmonitor_print_timeoflife(const char *prefix)
{
    static char strrr[64] = {0};  
    uint32_t sec = 0;
    uint32_t ms = 0;
    uint32_t us = 0;
    uint64_t start = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    sec = start / (1000*1000);
    start = start - 1000*1000*sec;
    ms = start / 1000; 
    us = start % 1000;

    snprintf(strrr, sizeof(strrr), "%s @ %ds %dm %du", prefix, sec, ms, us);    
    hal_trace_puts(strrr);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



