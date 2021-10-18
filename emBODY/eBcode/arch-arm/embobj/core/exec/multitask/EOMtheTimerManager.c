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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOaction.h"

#include "EOtheErrorManager.h"

#include "EOVtheTimerManager_hid.h"
#include "EOtimer_hid.h"
#include "EOMmutex.h"
#include "EOMtheCallbackManager_hid.h"

#include "EOMtask_hid.h"

#include "osal.h"

#if defined(FATALERR_trace_TMRMAN)
extern volatile uint32_t FATALERR_tmrman[8];
#define FT_0 0
#define FT_1 1
#define FT_2 2
#define FT_3 3
#define FT_4 4
#define FT_5 5
// in here we use up to FT_5
//#define FT_6 6
//#define FT_7 7
#define FATALERR_TM_set(a, b)           FATALERR_tmrman[(a)] = (uint32_t)(b)
#define FATALERR_TM_setcond(a, b, c)    FATALERR_tmrman[(a)] = (c) ? ((uint32_t)(b)) : (0)
#else
#define FATALERR_TM_set(a, b) 
#define FATALERR_TM_setcond(a, b, c) 
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheTimerManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheTimerManager_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define _EOM_TIMERMAN_EVENTS_DIRECTLY_SENT_BY_OSAL_


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOmtimerman_cfg_t eom_timerman_DefaultCfg = 
{
    EO_INIT(.priority)          240, 
    EO_INIT(.stacksize)         512, 
    EO_INIT(.messagequeuesize)  8
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_timerman_tskproc_forever(EOMtask *rt, uint32_t msg);

static void s_eom_timerman_ProcessExpiry(eOmessage_t msg);
static eOresult_t s_eom_timerman_OnNewTimer(EOVtheTimerManager* tm, EOtimer *t);
static eOresult_t s_eom_timerman_OnDelTimer(EOVtheTimerManager* tm, EOtimer *t);
static eOresult_t s_eom_timerman_AddTimer(EOVtheTimerManager* tm, EOtimer *t);
static eOresult_t s_eom_timerman_RemTimer(EOVtheTimerManager* tm, EOtimer *t);

static void s_eom_timerman_OnExpiry(osal_timer_t *osaltmr, void *tmr);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
 
static const char s_eobj_ownname[] = "EOMtheTimerManager";
 
static EOMtheTimerManager s_eom_thetimermanager = 
{
    EO_INIT(.tmrman)    NULL,             // tmrman
    EO_INIT(.tskproc)   NULL              // tskproc
}; 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

 
extern EOMtheTimerManager * eom_timerman_Initialise(const eOmtimerman_cfg_t *tmrmancfg) 
{

    if(NULL != s_eom_thetimermanager.tmrman) 
    {
        // already initialised
        return(&s_eom_thetimermanager);
    }

    if(NULL == tmrmancfg)
    {
        tmrmancfg = &eom_timerman_DefaultCfg;
    }
    
    // trying to initialise with wrong params error
    eo_errman_Assert(eo_errman_GetHandle(), (0 != tmrmancfg->messagequeuesize), "eom_timerman_Initialise(): 0 messagequeuesize", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    eo_errman_Assert(eo_errman_GetHandle(), (0 != tmrmancfg->stacksize), "eom_timerman_Initialise(): 0 stacksize", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    eo_errman_Assert(eo_errman_GetHandle(), (0 != tmrmancfg->priority), "eom_timerman_Initialise(): 0 priority", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    
    // i get a basic timer manager with onnew, add and rem functions proper for osal. and an EOMmutex (by the way ... eom_mutex_New() never returns NULL).
    s_eom_thetimermanager.tmrman = eov_timerman_hid_Initialise(s_eom_timerman_OnNewTimer, s_eom_timerman_OnDelTimer, s_eom_timerman_AddTimer, s_eom_timerman_RemTimer, eom_mutex_New()); 

    // i prepare the task able to process actions associated to expiry of the timers 
    s_eom_thetimermanager.tskproc = eom_task_New(eom_mtask_MessageDriven,                          // type 
                                              tmrmancfg->priority,                                 // priority
                                              tmrmancfg->stacksize,                               // stacksize
                                              NULL,                                             // startup_fn 
                                              s_eom_timerman_tskproc_forever,                   // run_fn 
                                              tmrmancfg->messagequeuesize,                      // maxmessages: msg fifo can hold all timers 
                                              eok_reltimeINFINITE,                                  // timeoutorperiod
                                              NULL,
                                              sys_timerman,                               // nameofthetask_fn
                                              "tmrman");                              
    
    
   
    return(&s_eom_thetimermanager);
}    

    
extern EOMtheTimerManager* eom_timerman_GetHandle(void) 
{
    if(NULL == s_eom_thetimermanager.tmrman) 
    {
        return(NULL);
    }
    
    return(&s_eom_thetimermanager);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

 
/*  @fn        static void s_eom_timerman_OnExpiry(void *osaltmr, void *tmr)
     @brief     this function is called by the expired osal timer to alert the ruling task. the osal automatically 
                reloads the timer if it was created as a periodic timer.
 **/    
static void s_eom_timerman_OnExpiry(osal_timer_t *osaltmr, void *tmr) 
{

#ifdef _EOM_TIMERMAN_EVENTS_DIRECTLY_SENT_BY_OSAL_

    EOtimer *t = (EOtimer*)tmr;
    FATALERR_TM_set(FT_0, 32);
    FATALERR_TM_set(FT_3, t);
    FATALERR_TM_setcond(FT_4, &(t->onexpiry), NULL != t);  

    if(eo_actypeEvent == t->onexpiry.actiontype)
    {
        FATALERR_TM_set(FT_0, 33);
        if(NULL != t->onexpiry.data.evt.totask)
        {
            FATALERR_TM_set(FT_0, 34);
            osal_eventflag_set(t->onexpiry.data.evt.event, ((EOMtask*)t->onexpiry.data.evt.totask)->osaltask, osal_callerISR);
            FATALERR_TM_set(FT_0, 35); 
        }

        if(EOTIMER_MODE_ONESHOT == t->mode) 
        {
            FATALERR_TM_set(FT_0, 36); 
            // sets dummy values for the timer, which is completed 
            eo_timer_hid_Reset_but_not_osaltime(t, eo_tmrstat_Completed); 
            //t->osaltimer = osaltimer;   
            FATALERR_TM_set(FT_0, 37); 
        }
    }
    else
    {
        FATALERR_TM_set(FT_0, 38); 
        eom_task_isrSendMessage(s_eom_thetimermanager.tskproc, (eOmessage_t)tmr);
        FATALERR_TM_set(FT_0, 39); 
    }


#else
    // EOtimer *eotimer = (EOtimer*)tmr;
    // if timer is expired .... make it clear by the executer task
    eom_task_isrSendMessage(s_eom_thetimermanager.tskproc, (eOmessage_t)tmr);
#endif
} 


static eOresult_t s_eom_timerman_OnNewTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    eOresult_t res = eores_NOK_generic;
    
    FATALERR_TM_set(FT_0, 64); 
    
    if((NULL == tm) || (NULL == t)) 
    {
        FATALERR_TM_set(FT_0, 65);
        return(eores_NOK_nullpointer);    
    }
    
    if(NULL != t->envir.osaltimer)
    {
        FATALERR_TM_set(FT_0, 66); 
        // this timer has already been created ..... return error
        return(eores_NOK_generic);
    }

// no need to protect data structures of the timer manager    
//    if(eores_OK != eom_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
//    {
//        // cannot lock it ... bye bye
//         return(eores_NOK_generic);
//    }
    
    FATALERR_TM_set(FT_0, 67);
    // create the osal timer
    t->envir.osaltimer = osal_timer_new();
    t->status = EOTIMER_STATUS_IDLE;
    
    FATALERR_TM_set(FT_0, 68); 
    
    // it may be that osal does not have any more timers available.
    if(NULL != t->envir.osaltimer)
    {
        res = eores_OK;
    }
    else
    {
        FATALERR_TM_set(FT_0, 69);
        res = eores_NOK_generic;
    }
    
    
//    // unlock the manager
//    eom_mutex_Release((EOMmutex*)tm->mutex);

    return(res);  
   
}


static eOresult_t s_eom_timerman_OnDelTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    //eOresult_t res = eores_NOK_generic;
    FATALERR_TM_set(FT_0, 96); 
    
    if((NULL == tm) || (NULL == t)) 
    {
        FATALERR_TM_set(FT_0, 97); 
        return(eores_NOK_nullpointer);    
    }
    
    if(NULL == t->envir.osaltimer)
    {
        FATALERR_TM_set(FT_0, 98); 
        // this timer has already been deleted 
        return(eores_OK);
    }

// no need to protect data structures of the timer manager    
//    if(eores_OK != eom_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
//    {
//        // cannot lock it ... bye bye
//         return(eores_NOK_generic);
//    }
    
    FATALERR_TM_set(FT_0, 99); 
    // delete the osal timer
    osal_timer_delete((osal_timer_t*)t->envir.osaltimer);
    FATALERR_TM_set(FT_0, 100);
    t->status = EOTIMER_STATUS_IDLE;
    t->envir.osaltimer = NULL;
        
    
//    // unlock the manager
//    eom_mutex_Release((EOMmutex*)tm->mutex);

    return(eores_OK);  
   
}


static eOresult_t s_eom_timerman_AddTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    eOresult_t res = eores_NOK_generic;
    osal_timer_timing_t   timing;
    osal_timer_onexpiry_t onexpi;

    FATALERR_TM_set(FT_0, 128); 
    
    if((NULL == tm) || (NULL == t)) 
    {
        FATALERR_TM_set(FT_0, 129);
        return(eores_NOK_nullpointer);    
    }
    
    if(EOTIMER_STATUS_RUNNING == t->status)
    {
        FATALERR_TM_set(FT_0, 130);
        // this timer is already running ..... return error
        return(eores_NOK_generic);
    }

    if(NULL == t->envir.osaltimer)
    {
        FATALERR_TM_set(FT_0, 131);
        // this timer does not have a osal timer ..... return error
        return(eores_NOK_generic);
    }

    FATALERR_TM_set(FT_0, 132);
//    if(eores_OK != eov_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE)) 
    if(eores_OK != eom_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
    {
        FATALERR_TM_set(FT_0, 133); 
        // cannot lock it ... bye bye
         return(eores_NOK_generic);
    }
    
    FATALERR_TM_set(FT_0, 134);
    
    // start the osal timer
    timing.startat  = t->startat; 
    timing.count    = t->expirytime; 
    timing.mode     = ( (EOTIMER_MODE_FOREVER == t->mode) ? (osal_tmrmodeFOREVER) : (osal_tmrmodeONESHOT) );
    onexpi.cbk      = s_eom_timerman_OnExpiry;
    onexpi.par      = t;

    FATALERR_TM_set(FT_0, 135); 
    res = (eOresult_t)osal_timer_start((osal_timer_t*)t->envir.osaltimer, &timing, &onexpi, osal_callerTSK);
    FATALERR_TM_set(FT_0, 136); 
    if(eores_OK == res)
    {
        t->status   = EOTIMER_STATUS_RUNNING;
    }
    else
    {
        t->status   = EOTIMER_STATUS_IDLE;
    }
    
    FATALERR_TM_set(FT_0, 137); 
    // unlock the manager
    //eov_mutex_Release((EOMmutex*)tm->mutex);
    eom_mutex_Release((EOMmutex*)tm->mutex);
    FATALERR_TM_set(FT_0, 138);

    return(res);  
   
}


static eOresult_t s_eom_timerman_RemTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    FATALERR_TM_set(FT_0, 196);

    if((NULL == tm) || (NULL == t)) 
    {
        FATALERR_TM_set(FT_0, 197);
        return(eores_NOK_nullpointer);
    }
  
    FATALERR_TM_set(FT_0, 198);
  
    //if(eores_OK != eov_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE)) 
    if(eores_OK != eom_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
    {
        FATALERR_TM_set(FT_0, 199);
        return(eores_NOK_generic);
    }

    // store it ... for later
    //osaltimer = t->envir.osaltimer;
    
    FATALERR_TM_set(FT_0, 200);
    // stop the osal timer. operation is null safe.
    osal_timer_stop((osal_timer_t*)t->envir.osaltimer, osal_callerTSK);

    FATALERR_TM_set(FT_0, 201);    
    // reset the values of the timer but do not set envir.osaltimer to NULL
    eo_timer_hid_Reset_but_not_osaltime(t, eo_tmrstat_Idle);
    // thus reassign the value of osaltimer
    //t->osaltimer = osaltimer;
    FATALERR_TM_set(FT_0, 202);
    
    // unlock the manager
    // eov_mutex_Release((EOMmutex*)tm->mutex);
    eom_mutex_Release((EOMmutex*)tm->mutex);

    FATALERR_TM_set(FT_0, 203);
    
    return(eores_OK);
}


// name of the task as it is shown in uvision
void sys_timerman(void *p)
{
    eom_task_Start((EOMtask*)p);
}


/* @fn         static void s_eom_timerman_tskproc_forever(EOMtask *rt, uint32_t msg)
    @brief      this is the forever funtion of the rtos task.
                It is called when a message coming from osal is received.
                This functions simply calls s_eom_timerman_ProcessExpiry().
 **/
 
static void s_eom_timerman_tskproc_forever(EOMtask *rt, uint32_t msg)
{
    FATALERR_TM_set(FT_0, 0);
    FATALERR_TM_set(FT_1, 0);
    FATALERR_TM_set(FT_2, 0);
    FATALERR_TM_set(FT_3, 0);
    FATALERR_TM_set(FT_4, 0);
    FATALERR_TM_set(FT_5, 0);    
    s_eom_timerman_ProcessExpiry(msg);
    FATALERR_TM_set(FT_0, 255);
}
 


/* @fn         static void s_eom_timerman_ProcessExpiry(eOmessage_t msg)
    @brief      this function is called by the forever method of the internal task s_eom_thetimermanager.tskproc
                in order to manage the expiry of the rtos timer.
 
 **/
static void s_eom_timerman_ProcessExpiry(eOmessage_t msg) 
{
    // important ... the message contains the pointer to the target EOtimer
    EOtimer *t = (EOtimer *) msg; 
    //osal_timer_t osaltimer = NULL;

    FATALERR_TM_set(FT_0, 1);
    FATALERR_TM_set(FT_1, t);
    
    // we dont have a valid pointer to a EOtimer
    if(NULL == t)
    {
        FATALERR_TM_set(FT_0, 2);
        return;
    }

    //osaltimer = t->osaltimer;
    FATALERR_TM_set(FT_0, 3);
    FATALERR_TM_set(FT_2, &(t->onexpiry));

    // get the mutex of the timer manager.  
    if(eores_OK != eom_mutex_Take((EOMmutex*)s_eom_thetimermanager.tmrman->mutex, eok_reltimeINFINITE)) 
    {
        FATALERR_TM_set(FT_0, 4);
        return;
    }

    FATALERR_TM_set(FT_0, 5);
    FATALERR_TM_set(FT_5, eo_action_GetType(&t->onexpiry));
    
    // 1. perform the action

    eo_action_Execute(&t->onexpiry, eok_reltimeZERO);

    FATALERR_TM_set(FT_0, 6);
    
    // 2. clear if one shot
    if(EOTIMER_MODE_ONESHOT == t->mode) 
    {
        FATALERR_TM_set(FT_0, 7);
        // sets dummy values for the timer, which is completed 
        eo_timer_hid_Reset_but_not_osaltime(t, eo_tmrstat_Completed); 
        // but do not erase the osaltimer.
        //t->osaltimer = osaltimer;   
        FATALERR_TM_set(FT_0, 8);
    }

    FATALERR_TM_set(FT_0, 9);
    
    // release the mutex of the timer manager
    eom_mutex_Release((EOMmutex*)s_eom_thetimermanager.tmrman->mutex);

    FATALERR_TM_set(FT_0, 10);
    
    return;
}

   

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





