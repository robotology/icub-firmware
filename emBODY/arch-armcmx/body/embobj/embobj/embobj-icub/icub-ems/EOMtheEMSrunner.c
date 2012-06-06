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

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"


#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "hal_timer.h"
#include "osal_task.h"

#include "EOMtask_hid.h" // to retrieve its osaltask pointer

#include "EOVtheIPnet.h"

#include "EOMtheEMSappl.h"

#include "EOaction_hid.h"
#include "EOMtheCallbackManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define EOMTHEEMSRUNNER_USE_HAL_TIMER_FOR_PERIOD


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg = 
{
    EO_INIT(.taskpriority)              {250,     251,     252},  
    EO_INIT(.taskstacksize)             {512,   512,   512},
    EO_INIT(.period)                    1000, 
    EO_INIT(.execDOafter)               500, 
    EO_INIT(.execTXafter)               700, 
    EO_INIT(.safetyGAP)                 10
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSrunRX(void *p);
extern void tskEMSrunDO(void *p);
extern void tskEMSrunTX(void *p);

static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t);


static void s_eom_emsrunner_enable_task(EOMtask *tsk, osal_caller_t osalcaller);

static void s_eom_emsrunner_start_cycle(void *arg);

static void s_eom_emsrunner_start_cycle_within_osaltimer(osal_timer_t *tmr, void *arg);

static void s_eom_emsrunner_warn_task(void *arg);

static void s_eom_emsrunner_start_taskDO(void *arg);

static void s_eom_emsrunner_start_taskTX(void *arg);


static void s_eom_emsrunner_cbk_activate_from_osaltmrman(osal_timer_t* tmr, void* par);

static void s_eom_emsrunner_cbk_activate_with_period_by_hal_timer(void* arg, osal_caller_t osalcaller);

static void s_eom_emsrunner_cbk_activate_with_period_by_osal_timer(void* arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSrunner";

 
static EOMtheEMSrunner s_theemsrunner = 
{
    EO_INIT(.task)              {NULL, NULL, NULL},
    EO_INIT(.cfg)               {0},
    EO_INIT(.cycleisrunning)    eobool_false,
    EO_INIT(.event)             eo_sm_emsappl_EVdummy,
    EO_INIT(.osaltimer)         NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSrunner * eom_emsrunner_Initialise(const eOemsrunner_cfg_t *cfg)
{
    if(NULL != s_theemsrunner.task[0])
    {
        return(&s_theemsrunner);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsrunner_DefaultCfg;
    }
    
    
    // verify that the cfg has timing that are compatible ... d2>d1+gap, period > d1+d2+gap
    
    // to be done
    
    // verify that the period is compatible with the used timer
#if !defined(EOMTHEEMSRUNNER_USE_HAL_TIMER_FOR_PERIOD)
    eo_errman_Assert(eo_errman_GetHandle(), ((cfg->period % osal_info_get_tick()) == 0), s_eobj_ownname, "cfg->period must be multiple of osaltick");
#endif    
    
    memcpy(&s_theemsrunner.cfg, cfg, sizeof(eOemsrunner_cfg_t));
    
    s_theemsrunner.cycleisrunning = eobool_false; 
    s_theemsrunner.event = eo_sm_emsappl_EVdummy;
    
    s_theemsrunner.osaltimer = osal_timer_new();
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theemsrunner.osaltimer), s_eobj_ownname, "osaltimer is NULL");
    
    s_theemsrunner.task[eo_emsrunner_taskid_runRX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runRX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runRX], 
                                                                  s_eom_emsrunner_taskRX_startup, s_eom_emsrunner_taskRX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) | (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunRX, "tskEMSrunRX");
 
    s_theemsrunner.task[eo_emsrunner_taskid_runDO] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runDO], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runDO], 
                                                                  s_eom_emsrunner_taskDO_startup, s_eom_emsrunner_taskDO_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) | (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunDO, "tskEMSrunDO"); 
                                                                  
    s_theemsrunner.task[eo_emsrunner_taskid_runTX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runTX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runTX], 
                                                                  s_eom_emsrunner_taskTX_startup, s_eom_emsrunner_taskTX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) | (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunTX, "tskEMSrunTX");                                                              
                                                   
    
    return(&s_theemsrunner);
}


extern EOMtheEMSrunner* eom_emsrunner_GetHandle(void) 
{
    if(NULL != s_theemsrunner.task)
    {
        return(&s_theemsrunner);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emsrunner_GetTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t id)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_theemsrunner.task[id]);
}


extern eOresult_t eom_emsrunner_Start(EOMtheEMSrunner *p)
{
    hal_result_t res;
    
    // instantiate a periodic hal timer which sends an evt eo_emsrunner_evt_execute every period and start a oneshot hal timer to ...
    // also, send an evt eo_emsrunner_evt_enable straigth away.
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 

#if 0 
    // start the hal timer straigth away

    hal_timer_cfg_t t2per_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  

    t2per_cfg.countdown         = p->cfg.period;
    t2per_cfg.callback_on_exp   = s_eom_emsrunner_start_cycle;

    // after a period time, the first task shall be executed
    res = hal_timer_init(hal_timer2, &t2per_cfg, NULL);  
    hal_timer_start(hal_timer2);

    // but now we need to enable the first task: taskRX. the other enables shall be send at teh end of the relevant tasks
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX], osal_callerTSK);
    
    s_theemsrunner.cycleisrunning = eobool_true;

#else
  
#if defined(EOMTHEEMSRUNNER_USE_HAL_TIMER_FOR_PERIOD)    
    
    // start the hal timer in synch with the systick
    {
        osal_timer_timing_t timing;
        osal_timer_onexpiry_t onexpiry;
        timing.startat  = OSAL_abstimeNONE;
        timing.count    = 1000; // or 0.
        timing.mode     = osal_tmrmodeONESHOT;   
        onexpiry.cbk    = s_eom_emsrunner_cbk_activate_from_osaltmrman;
        onexpiry.par    = p;        
        

        osal_timer_start(s_theemsrunner.osaltimer, &timing, &onexpiry, osal_callerTSK);
    }
    
#else
    
    s_eom_emsrunner_cbk_activate_with_period_by_osal_timer(&s_theemsrunner);    
    
#endif
    
#endif

    
    return(eores_OK);
}

extern eOresult_t eom_emsrunner_StopAndGoTo(EOMtheEMSrunner *p, eOsmEventsEMSappl_t ev)
{
    // stop the hal timers.

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
    
    if((eo_sm_emsappl_EVdummy == ev) || (eo_sm_emsappl_EVgo2run == ev))
    {   // cannot stop on dummy event or 
        return(eores_NOK_generic);
    }

#if defined(EOMTHEEMSRUNNER_USE_HAL_TIMER_FOR_PERIOD)     
    // simply stop timer2 ... that allows to finish the cycle without restarting it.   
    hal_timer_stop(hal_timer2);
#else
    // stop the osaltimer
    osal_timer_stop(s_theemsrunner.osaltimer, osal_callerTSK);
#endif    
    
    s_theemsrunner.cycleisrunning = eobool_false;
    s_theemsrunner.event = ev;
    
    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSrunRX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void tskEMSrunDO(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void tskEMSrunTX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 


__weak extern void eom_emsrunner_hid_userdef_taskRX_activity(void)
{   
    uint16_t numberofrxdatagrams = 0;
    uint16_t numberofrxrops = 0;
    eOabstime_t txtimeofrxropframe = 0;
    
    // A. perform a first activity before datagram reception
    eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(); 

    // B. receive and parse datagram(s)
    eom_emsrunner_hid_userdef_taskRX_activity_datagramreception(&numberofrxdatagrams, &numberofrxrops, &txtimeofrxropframe);
    
    // C. perform a third activity after datagram parsing
    eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(numberofrxdatagrams, numberofrxrops, txtimeofrxropframe);                
}


__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(void)
{
    
}

__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_datagramreception(uint16_t *numberofrxdatagrams, uint16_t *numberofrxrops, eOabstime_t *txtimeofrxropframe)
{
    EOpacket *rxpkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;
    eOresult_t res;
    static uint32_t lost_datagrams = 0;
    uint16_t received = 0;

 

       
    // 1. get the packet. we need passing just a pointer because the storage is inside the EOMtheEMSsocket       
    res = eom_emssocket_Receive(eom_emssocket_GetHandle(), &rxpkt, &remainingrxpkts);
            
    // 2. process the packet with the transceiver
    if(eores_OK == res)
    {
        res = eom_emstransceiver_Parse(eom_emstransceiver_GetHandle(), rxpkt, numberofrxrops, txtimeofrxropframe);
        received ++;
    }
    
    #warning ---> what to do if there are n > 1 datagram in teh queue ??? so far i remove them without parsing
    
    if(remainingrxpkts > 0)
    {
        uint8_t i;
        for(i=0;i<4;i++)
        {
            uint16_t tmp = 0;
            lost_datagrams++;
            remainingrxpkts = 0;
            res = eom_emssocket_Receive(eom_emssocket_GetHandle(), &rxpkt, &remainingrxpkts);
            if(eores_OK == res)
            {
                res = eom_emstransceiver_Parse(eom_emstransceiver_GetHandle(), rxpkt, &tmp, txtimeofrxropframe);
                received ++;
                *numberofrxrops += tmp;
            }
            if(0 == remainingrxpkts)
            {
                break;
            }
        }           
    }
    
    
    *numberofrxdatagrams = received;
           
}

__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(uint16_t numberofrxdatagrams, uint16_t numberofrxrops, eOabstime_t txtimeofrxropframe)
{
    
}

__weak extern void eom_emsrunner_hid_userdef_taskDO_activity(void)
{
    
}

__weak extern void eom_emsrunner_hid_userdef_taskTX_activity(void)
{
    uint16_t numberoftxrops = 0;

    // A. perform a first activity
    eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission();    
         
    // B. transmit a datagram
    eom_emsrunner_hid_userdef_taskTX_activity_datagramtransmission(&numberoftxrops);
    
    // C. perform an user-defined function after datagram transmission
    eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(numberoftxrops);           
}



__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(void)
{
    
}

__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_datagramtransmission(uint16_t *numberoftxrops)
{
    EOpacket* txpkt = NULL;
    eOresult_t res;
 

    // 1. call the former to retrieve a tx packet (even if it is an empty ropframe)        
    res = eom_emstransceiver_Form(eom_emstransceiver_GetHandle(), &txpkt, numberoftxrops);
    
    // 2.  send a packet back. but only if the former gave us a good one.
    if(eores_OK == res)
    {
        res = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt);
    }        
           
}

__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(uint16_t numberoftxrops)
{
    
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.
    
    
    //eov_ipnet_Activate(eov_ipnet_GetHandle());
    
    // perform the rx activity     
    eom_emsrunner_hid_userdef_taskRX_activity();
  
    //eov_ipnet_Deactivate(eov_ipnet_GetHandle());
    
    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runDO], osal_callerTSK);
}


static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t)
{
 
}



static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.


    // perform the do activity
    eom_emsrunner_hid_userdef_taskDO_activity();   
    

    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runTX], osal_callerTSK);
}


static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.
    
    //eov_ipnet_Activate(eov_ipnet_GetHandle());
    
    eom_emsrunner_hid_userdef_taskTX_activity();
    
    //eov_ipnet_Deactivate(eov_ipnet_GetHandle());


    // at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    if(eobool_true == s_theemsrunner.cycleisrunning)
    {
        s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX], osal_callerTSK);
    }
    else
    {   // we need to reset the event to the dummy value.
        eOsmEventsEMSappl_t ev = s_theemsrunner.event;
        s_theemsrunner.event = eo_sm_emsappl_EVdummy;
        // we process the event. it can be either eo_sm_emsappl_EVgo2err or eo_sm_emsappl_EVgo2cfg
        eom_emsappl_ProcessEvent(eom_emsappl_GetHandle(), ev); 
    }
}

static void s_eom_emsrunner_enable_task(EOMtask *tsk, osal_caller_t osalcaller)
{
    // send an event to enable tsk. 
    if(osal_callerTSK == osalcaller)
    {
        eom_task_SetEvent(tsk, eo_emsrunner_evt_enable);
    }
    else // isr or tmrman
    {
        eom_task_isrSetEvent(tsk, eo_emsrunner_evt_enable);
    }
}



static void s_eom_emsrunner_start_cycle(void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  

    // one-shot timer which verify if taskRX has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execDOafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
    // one-shot timer which starts taskDO.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execDOafter;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskDO;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 
    
    hal_timer_start(hal_timer3);
    hal_timer_start(hal_timer4);
 
    // send an event to execute taskRX
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_execute);
}


static void s_eom_emsrunner_start_cycle_within_osaltimer(osal_timer_t *tmr, void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  

    // one-shot timer which verify if taskRX has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execDOafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
    // one-shot timer which starts taskDO.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execDOafter;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskDO;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 
    
    hal_timer_start(hal_timer3);
    hal_timer_start(hal_timer4);
 
    // send an event to execute taskRX
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_execute);
}

static void s_eom_emsrunner_warn_task(void *arg)
{
    static uint32_t count = 0;
    EOMtask* task = (EOMtask*) arg;
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
        
    if(task->osaltask == scheduledosaltask)
    {
        count++;
        count = count;
        #warning ---> if execution is in here it means that the task has not finished its computing in the time it was given to it minus the guard    
        // we could ... send an event to the task to force a graceful finish ... issue a warning ... issue a fatal error ... whatever else.
        // un attimo ... potrebbe essere che il task interrotto sia uno dei task di ipnet ... in tal caso andrebbe bene?
        // oppure 
    }

}


static void s_eom_emsrunner_start_taskDO(void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  
    EOMtask* task = (EOMtask*) arg;
    EOMtask* prevtask = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
    
    // che controlli si potrebbe fare qui per evitare che ci siano problemi? ad esempio che lo scheduled task non sia quello prima di task?
    
    if(prevtask->osaltask == scheduledosaltask)
    {
        #warning --> ahi ahi ahi problemi.
    }
    
    // one-shot timer which verify if taskDO has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.execDOafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
    // one-shot timer which starts taskTX.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.execDOafter;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskTX;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runTX];
    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 

    hal_timer_start(hal_timer3);
    hal_timer_start(hal_timer4);
    
    // send event to activate the task in argument (taskDO)
    //eom_task_isrSetEvent(task, eo_emsrunner_evt_execute);
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runDO], eo_emsrunner_evt_execute);
}


static void s_eom_emsrunner_start_taskTX(void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  
    EOMtask* task = (EOMtask*) arg;
    EOMtask* prevtask = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
    
    // che controlli si potrebbe fare qui per evitare che ci siano problemi? ad esempio che lo scheduled task non sia quello prima di task?
    
    if(prevtask->osaltask == scheduledosaltask)
    {
        #warning --> ahi ahi ahi altri problemi.
    }
    
    // one-shot timer which verify if taskTX has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.period - s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
//    // one-shot timer which starts taskTX.
//    toneshot_cfg.countdown          = s_theemsrunner.cfg.period - s_theemsrunner.cfg.execTXafter;
//    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskRX;
//    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
//    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 

    hal_timer_start(hal_timer3);
//    hal_timer_start(hal_timer4);
    
    // send event to activate the task in argument (taskTX)
    //eom_task_isrSetEvent(task, eo_emsrunner_evt_execute);
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runTX], eo_emsrunner_evt_execute);
}


// static void s_eom_emsrunner_cbk_activate_from_isr(void* arg)
// {
//     s_eom_emsrunner_cbk_activate_with_period_by_hal_timer(arg, osal_callerISR);
// }

// static void s_eom_emsrunner_cbk_activate_from_tsk(void* arg)
// {
//     s_eom_emsrunner_cbk_activate_with_period_by_hal_timer(arg, osal_callerTSK);
// }

static void s_eom_emsrunner_cbk_activate_from_osaltmrman(osal_timer_t* tmr, void* par)
{
    s_eom_emsrunner_cbk_activate_with_period_by_hal_timer(par, osal_callerTMRMAN);
}

static void s_eom_emsrunner_cbk_activate_with_period_by_hal_timer(void* arg, osal_caller_t osalcaller)
{
    EOMtheEMSrunner *p = (EOMtheEMSrunner*)arg;
    hal_result_t res;
    hal_timer_cfg_t t2per_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  

    t2per_cfg.countdown         = p->cfg.period;
    t2per_cfg.callback_on_exp   = s_eom_emsrunner_start_cycle;

    // after a period time, the first task shall be executed
    res = hal_timer_init(hal_timer2, &t2per_cfg, NULL);  
    hal_timer_start(hal_timer2);

    // but now we need to enable the first task: taskRX. the other enables shall be send at teh end of the relevant tasks
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX], osalcaller);
    
    s_theemsrunner.cycleisrunning = eobool_true;    
}


static void s_eom_emsrunner_cbk_activate_with_period_by_osal_timer(void* arg)
{
    
    {
        osal_timer_timing_t timing;
        osal_timer_onexpiry_t onexpiry;
        timing.startat  = OSAL_abstimeNONE;
        timing.count    = s_theemsrunner.cfg.period; 
        timing.mode     = osal_tmrmodeFOREVER;   
        onexpiry.cbk    = s_eom_emsrunner_start_cycle_within_osaltimer;
        onexpiry.par    = arg;        
        

        osal_timer_start(s_theemsrunner.osaltimer, &timing, &onexpiry, osal_callerTSK);
    }

    // now we also need to enable the first task: taskRX. the other enables shall be send at the end of the relevant tasks
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX], osal_callerTSK);
    
    s_theemsrunner.cycleisrunning = eobool_true;    
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





