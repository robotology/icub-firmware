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

#include "EOtransceiver.h"
#include "EOtransmitter.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "hal_timer.h"
#include "osal.h"


#include "EOMtask_hid.h" // to retrieve its osaltask pointer

#include "EOVtheIPnet.h"

#include "EOMtheIPnet.h"

#include "EOMtheEMSappl.h"

#include "EOaction.h"
#include "EOMtheCallbackManager.h"

#include "eventviewer.h"

#include "OPCprotocolManager_Cfg.h"

#include "EoError.h"




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

//#define EOM_EMSRUNNER_EVIEW_MEASURES


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg = 
{
    .taskpriority               = {250,   251,    252},  
    .taskstacksize              = {2048,  1024,   2048},
    .haltimerstart              = {hal_timer2, hal_timer3, hal_timer4},    
    .haltimeralert              = {hal_timer5, hal_timer6, hal_timer7},  
    .period                     = 1000,  
    .execRXafter                = 0, 
    .safeRXexecutiontime        = 300,
    .execDOafter                = 400, 
    .safeDOexecutiontime        = 250,
    .execTXafter                = 700,
    .safeTXexecutiontime        = 250,    
    .maxnumofRXpackets          = 3,
    .maxnumofTXpackets          = 1,
    .modeatstartup              = eo_emsrunner_mode_besteffort, //eo_emsrunner_mode_softrealtime //eo_emsrunner_mode_besteffort //eo_emsrunner_mode_softrealtime
    .defaultTXdecimationfactor  = 1
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES)

void meas_rx_start(void){}
void meas_rx_stop(void){}
void meas_do_start(void){}
void meas_do_stop(void){}    
void meas_tx_start(void){}
void meas_tx_stop(void){}
    
static const evEntityId_t ev_meas_rx_start = ev_ID_first_usrdef+15; 
static const evEntityId_t ev_meas_rx_stop = ev_ID_first_usrdef+16;    
static const evEntityId_t ev_meas_do_start = ev_ID_first_usrdef+17; 
static const evEntityId_t ev_meas_do_stop = ev_ID_first_usrdef+18;       
static const evEntityId_t ev_meas_tx_start = ev_ID_first_usrdef+19; 
static const evEntityId_t ev_meas_tx_stop = ev_ID_first_usrdef+20; 
    
//static const evEntityId_t ev_meas_start[3] = {ev_meas_rx_start, ev_meas_do_start, ev_meas_tx_start};    
static const evEntityId_t ev_meas_stop[3] = {ev_meas_rx_stop, ev_meas_do_stop, ev_meas_tx_stop};

#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static eObool_t s_eom_emsrunner_timing_is_compatible(const eOemsrunner_cfg_t *cfg);

static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t);   
static void s_eom_emsrunner_userdef_taskRX_activity(EOMtheEMSrunner *p);
static void s_eom_emsrunner_userdef_taskRX_activity_datagramreception(EOMtheEMSrunner *p);
    

static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_userdef_taskTX_activity(EOMtheEMSrunner *p);
static void s_eom_emsrunner_userdef_taskTX_activity_datagramtransmission(EOMtheEMSrunner *p);

static void s_eom_emsrunner_6HALTIMERS_stop(void);
static void s_eom_emsrunner_6HALTIMERS_start(EOMtheEMSrunner *p);


static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_execute_task(void *arg);

static void s_eom_emsrunner_6HALTIMERS_execute_task(void *arg);




static void s_eom_emsrunner_tasktiming_on_entry(eOemsrunner_taskid_t taskid);

static void s_eom_emsrunner_tasktiming_on_exit(eOemsrunner_taskid_t taskid);

static void s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eOemsrunner_taskid_t taskid);
static void s_eom_emsrunner_tasktiming_eval(void);


static void s_eom_runner_overflow_set(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);
static void s_eom_runner_overflow_clr(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);


static void s_eom_emsrunner_6HALTIMERS_safestop_check_task(void *arg);
static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check(void *arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOMtheEMSrunner";

 
static EOMtheEMSrunner s_theemsrunner = 
{
    .cfg                    = {0},
    .task                   = {NULL, NULL, NULL},
    .event                  = eo_sm_emsappl_EVdummy,
    .osaltimer              = NULL,
    .haltimer_start         = {hal_timer2, hal_timer3, hal_timer4},
    .haltimer_safestop         = {hal_timer5, hal_timer6, hal_timer7},
    .numofrxpackets         = 0,  
    .numofrxrops            = 0,
    .numoftxpackets         = 0,
    .numoftxrops            = 0,
    .mode                   = eo_emsrunner_mode_softrealtime,
    .numofpacketsinsidesocket = 0,
    .waitudptxisdone        = NULL,
    .osaltaskipnetexec      = NULL,
    .usedTXdecimationfactor = 1,
    .txropsnumberincycle    = {0, 0, 0},
    .txcan1frames           = 0,
    .txcan2frames           = 0,
    .cycletiming            = { .cycleisrunning = eobool_false, .iterationnumber = 0, { {.timestarted = 0, .timestopped = 0, .duration = {0, 0}, .isexecuting = eobool_false, .isabout2overflow = eobool_false, .isoverflown = eobool_false} } } 
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
      
 
    eo_errman_Assert(eo_errman_GetHandle(), (eobool_true == s_eom_emsrunner_timing_is_compatible(cfg)), "eom_emsrunner_Initialise(): timings of tasks not compatible", s_eobj_ownname, NULL);

    eo_errman_Assert(eo_errman_GetHandle(), (cfg->maxnumofTXpackets <= 1), "eom_emsrunner_Initialise(): maxnumofTXpackets is > 1", s_eobj_ownname, NULL);
    
    memcpy(&s_theemsrunner.cfg, cfg, sizeof(eOemsrunner_cfg_t));
    

    // init the hal timers   
    memcpy(&s_theemsrunner.haltimer_start, cfg->haltimerstart, sizeof(s_theemsrunner.haltimer_start));
    memcpy(&s_theemsrunner.haltimer_safestop, cfg->haltimeralert, sizeof(s_theemsrunner.haltimer_safestop));
    // verify that they are all different
    //#warning TODO: in eom_emsrunner_Initialise() you may write code to check that all 6 hal timers are different
    
    s_theemsrunner.cycletiming.cycleisrunning = eobool_false; 
    
    eom_emsrunner_SetMode(&s_theemsrunner, cfg->modeatstartup);
    
    s_theemsrunner.event = eo_sm_emsappl_EVdummy;
    
    s_theemsrunner.osaltimer = osal_timer_new();
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theemsrunner.osaltimer), "eom_emsrunner_Initialise(): osaltimer is NULL", s_eobj_ownname, NULL);
    
    s_theemsrunner.numofpacketsinsidesocket = 0;
    
    s_theemsrunner.waitudptxisdone = osal_semaphore_new(255, 0);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theemsrunner.waitudptxisdone), "eom_emsrunner_Initialise(): waitudptxisdone is NULL", s_eobj_ownname, NULL);
    
    
    s_theemsrunner.cycletiming.iterationnumber = 0;    
    s_theemsrunner.usedTXdecimationfactor = cfg->defaultTXdecimationfactor;
    if(0 == s_theemsrunner.usedTXdecimationfactor)
    {
        s_theemsrunner.usedTXdecimationfactor = 1;
    }
    
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
     
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES)
    eventviewer_load(ev_meas_rx_start, meas_rx_start);
    eventviewer_load(ev_meas_rx_stop, meas_rx_stop);
    eventviewer_load(ev_meas_do_start, meas_do_start);
    eventviewer_load(ev_meas_do_stop, meas_do_stop);    
    eventviewer_load(ev_meas_tx_start, meas_tx_start);
    eventviewer_load(ev_meas_tx_stop, meas_tx_stop);
#endif

    return(&s_theemsrunner);
}


extern EOMtheEMSrunner* eom_emsrunner_GetHandle(void) 
{
    if(NULL != s_theemsrunner.task[0])
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


extern eOresult_t eom_emsrunner_Set_TXdecimationFactor(EOMtheEMSrunner *p, uint8_t txdecimationfactor)
{  
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }   

    if(0 == txdecimationfactor)
    {
        txdecimationfactor = 1;
    }
    p->usedTXdecimationfactor = txdecimationfactor;

    return(eores_OK);    
}


extern eOresult_t eom_emsrunner_Set_TXcanframes(EOMtheEMSrunner *p, uint8_t txcan1, uint8_t txcan2)
{  
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }   

    p->txcan1frames = txcan1;
    p->txcan2frames = txcan2;
    
    return(eores_OK);    
}


extern eObool_t eom_emsrunner_CycleHasJustTransmittedRegulars(EOMtheEMSrunner *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    } 
    
    eObool_t ret = (0 == p->txropsnumberincycle.numberofregulars) ? (eobool_false) : (eobool_true);

    return(ret);   
}


extern eOresult_t eom_emsrunner_SetTiming(EOMtheEMSrunner *p, const eOemsrunner_timing_t *timing)
{
    if((NULL == p) || (NULL == timing))
    {
        return(eores_NOK_nullpointer);
    } 
    
    if(eobool_true == s_theemsrunner.cycletiming.cycleisrunning)
    {
        return(eores_NOK_generic);        
    }
    
    p->cfg.execRXafter = timing->rxstartafter;
    p->cfg.safeRXexecutiontime = timing->dostartafter - timing->rxstartafter - timing->safetygap;
    p->cfg.execDOafter = timing->dostartafter;
    p->cfg.safeDOexecutiontime = timing->txstartafter - timing->dostartafter - timing->safetygap;
    p->cfg.execTXafter = timing->txstartafter;
    p->cfg.safeTXexecutiontime = timing->period - timing->txstartafter + timing->rxstartafter - timing->safetygap;
    p->cfg.period = timing->period;
    
    return(eores_OK);
}



extern uint64_t eom_emsrunner_Get_IterationNumber(EOMtheEMSrunner *p)
{  
    if(NULL == p)
    {
        return(0);
    }   

    return(p->cycletiming.iterationnumber);    
}


extern eOreltime_t eom_emsrunner_Get_Period(EOMtheEMSrunner *p)
{  
    if(NULL == p)
    {
        return(1);
    }   

    return(p->cfg.period);    
}


extern eOresult_t eom_emsrunner_Start(EOMtheEMSrunner *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
   
    // put in here with protection vs double initialisation. 
    // we could put also in initialise, but i want to avoid that the runner is initialised and there is not an osal task yet for ipnet
    if(NULL == p->osaltaskipnetexec)
    {   // compute it only once in life
        EOMtask * taskipnetexec = eom_ipnet_GetTask(eom_ipnet_GetHandle(), eomipnet_task_proc);
        p->osaltaskipnetexec = taskipnetexec->osaltask;
    } 

    
    p->numofpacketsinsidesocket = 0;      
    p->mode = p->cfg.modeatstartup;    
    osal_semaphore_set(s_theemsrunner.waitudptxisdone, 0);    
    
    // set the ems-transceiver to have a given tx-rate.
    EOtransmitter * transmitter = eo_transceiver_GetTransmitter(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()));    
    // replies and occasionals are not decimated. only regulars are decimated. in this way we have prompt responses to ask<> rops and we can send up occasionals asap.
    eo_transmitter_TXdecimation_Set(transmitter, 1, s_theemsrunner.usedTXdecimationfactor, 1);

    // now i start the timers for rx, do, tx and also for guards ...    
    s_eom_emsrunner_6HALTIMERS_start(p);
    
    return(eores_OK);
}


extern eOresult_t eom_emsrunner_Stop(EOMtheEMSrunner *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }  

    // set the ems-transceiver to have the standard tx-rate.
    EOtransmitter * transmitter = eo_transceiver_GetTransmitter(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()));    
    eo_transmitter_TXdecimation_Set(transmitter, 1, 1, 1);
        
    s_eom_emsrunner_6HALTIMERS_stop();    

    return(eores_OK);    
}


extern eOresult_t eom_emsrunner_GracefulStopAndGoTo(EOMtheEMSrunner *p, eOsmEventsEMSappl_t ev)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
    
    if(eo_sm_emsappl_EVdummy == ev)
    {   // cannot stop on dummy event or 
        return(eores_NOK_generic);
    }

    if(eo_sm_emsappl_EVgo2run == ev)
    {   // already in run state. nothing to do
        return(eores_OK);
    }
    
    s_theemsrunner.cycletiming.cycleisrunning = eobool_false;
    s_theemsrunner.event = ev;
    
    // the rest of the job is done inside the various tasks rx, do, and tx:
    // in rx: hal timers for rx are stopped. do task is activated and enabled.
    // in do: hal timers for do (and rx if not already) are stopped. tx task is activated and enabled.
    // in tx: hal timers for tx (and rx and do if not already) are stopped and state machine is sent to new state (err or cfg). no activation / enabling is done anymore.
    
    return(eores_OK);
}


extern eObool_t eom_emsrunner_SafeDurationExpired(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    if(NULL == p)
    {
        return(eobool_true);
    } 

    return(s_theemsrunner.cycletiming.tasktiming[taskid].isabout2overflow);
}


extern eObool_t eom_emsrunner_OverflownToNextTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    if(NULL == p)
    {
        return(eobool_true);
    } 

    return(s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown);
}


extern eOresult_t eom_emsrunner_SetMode(EOMtheEMSrunner *p, eOemsrunner_mode_t mode)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
    

    s_theemsrunner.mode = mode;
    
    // note: "eo_emsrunner_mode_hardrealtime requires a change of policy in the parsing of a ropframe"
    eo_errman_Assert(eo_errman_GetHandle(), (eo_emsrunner_mode_hardrealtime != mode), "eom_emsrunner_SetMode(): eo_emsrunner_mode_hardrealtime ... see note", s_eobj_ownname, NULL);
     
    return(eores_OK);
}


extern void eom_emsrunner_OnUDPpacketTransmitted(EOMtheEMSrunner *p)
{
    osal_semaphore_increment(s_theemsrunner.waitudptxisdone, osal_callerTSK);
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



__weak extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
{

}


__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runRX))
    {
        itissafetoquit_asap =  eobool_true;
    } 
    
    itissafetoquit_asap = itissafetoquit_asap;

    // if itissafetoquit_asap then ... do nothing or very little.
}


__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runRX))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;   
    
    // if itissafetoquit_asap then ... do nothing or very little.    
}


__weak extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runDO))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;    

    // if itissafetoquit_asap then ... do nothing or very little.
}



__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runTX))
    {
        itissafetoquit_asap =  eobool_true;
    }   

    itissafetoquit_asap = itissafetoquit_asap;
    
    // if itissafetoquit_asap then ... do nothing or very little.       
}




__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runTX))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;
    
    // if itissafetoquit_asap then ... do nothing or very little.       
}



__weak extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    char str[64];
    eOerrmanErrorType_t errortype = (eo_emsrunner_mode_hardrealtime == p->mode) ? (eo_errortype_error) : (eo_errortype_warning);
    snprintf(str, sizeof(str), "failed tx of a packet"); 
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eo_errman_code_sys_ctrloop_udptxfailure;
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;      
    eo_errman_Error(eo_errman_GetHandle(), errortype, str, s_eobj_ownname, &errdes); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_runner_overflow_set(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    p->cycletiming.tasktiming[taskid].isoverflown = eobool_true;
}


static void s_eom_runner_overflow_clr(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    p->cycletiming.tasktiming[taskid].isoverflown = eobool_false;
}


static eObool_t s_eom_emsrunner_timing_is_compatible(const eOemsrunner_cfg_t *cfg)
{
    // verify that the cfg has timing that are compatible ... 
    eObool_t timingisok = eobool_true;
    
    if(((cfg->execRXafter+cfg->safeRXexecutiontime)  >= cfg->execDOafter)   ||
       ((cfg->execDOafter+cfg->safeDOexecutiontime) >= cfg->execTXafter)    ||
       ((cfg->execTXafter+cfg->safeTXexecutiontime) >= cfg->period)         ||
       (cfg->safeRXexecutiontime == 0)                                      ||                                                                
       (cfg->safeDOexecutiontime == 0)                                      ||
       (cfg->safeTXexecutiontime == 0)                                      
      )
      {
            timingisok = eobool_false;
      }

    return(timingisok);
    
}


static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t)
{
    // nothing to do
}


static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t)
{
    s_theemsrunner.cycletiming.iterationnumber ++;
    
    s_eom_emsrunner_tasktiming_on_entry(eo_emsrunner_taskid_runRX);
    

    // we eval the timings of the previous cycle
    s_eom_emsrunner_tasktiming_eval();
         
    // perform the rx activity    
    if(eobool_true == eom_runner_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runRX))
    {
        s_eom_emsrunner_userdef_taskRX_activity(&s_theemsrunner);
    }
          

    // Y. if someone has stopped the cycle...
    if(eobool_false == s_theemsrunner.cycletiming.cycleisrunning)
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]);        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
    }
    
   
    s_eom_emsrunner_tasktiming_on_exit(eo_emsrunner_taskid_runRX);
    
    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runDO], eo_emsrunner_evt_enable);
}


static void s_eom_emsrunner_userdef_taskRX_activity(EOMtheEMSrunner *p)
{   
    
    p->numofrxrops = 0;
    p->numofrxpackets = 0;
    
    // A. perform a first activity before datagram reception
    eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(p); 

    
    // B. receive and parse datagram(s)
    s_eom_emsrunner_userdef_taskRX_activity_datagramreception(p);


    // C. perform a third activity after datagram parsing
    eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(p);       
}


static void s_eom_emsrunner_userdef_taskRX_activity_datagramreception(EOMtheEMSrunner *p)
{
    EOpacket *rxpkt = NULL;
    
    eOsizecntnr_t remainingrxpkts = 1;    
    uint16_t processedpkts = 0;
    eObool_t processreception = eobool_true;
    eOresult_t res;
    
    // evaluate if we can enter in the reception loop
    if((0 == p->cfg.maxnumofRXpackets) || (eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runRX)))
    {
        processreception = eobool_false;
    }     
    
    // the reception loop
    while(eobool_true == processreception)
    {  
        eOresult_t resrx;
        
        // 1. process one packet        
        processedpkts++;

        // 1.1 get the packet. we need passing just a pointer because the storage is inside the EOMtheEMSsocket       
        resrx = eom_emssocket_Receive(eom_emssocket_GetHandle(), &rxpkt, &remainingrxpkts);
        
        // 1.2 process the packet with the transceiver
        if(eores_OK == resrx)
        {
            uint16_t tmp = 0;  
            res = eom_emstransceiver_Parse(eom_emstransceiver_GetHandle(), rxpkt, &tmp, NULL);
            if(eores_OK != res)
            {
                eom_emsrunner_hid_userdef_onemstransceivererror(eom_emstransceiver_GetHandle());
            }
            p->numofrxrops += tmp;
            p->numofrxpackets++;
        }
        
        // 2. evaluate quit from the loop
        if((0 == remainingrxpkts) || (processedpkts >= p->cfg.maxnumofRXpackets) || 
           (eores_OK != resrx) || (eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runRX)))
        {
            processreception = eobool_false;
        }        
    
    }
 
       
}


static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t)
{
    // nothing to do
}



static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t)
{
    s_eom_emsrunner_tasktiming_on_entry(eo_emsrunner_taskid_runDO);
    
    
    if(eobool_true == eom_runner_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runDO))
    {
        // perform the do activity
        eom_emsrunner_hid_userdef_taskDO_activity(&s_theemsrunner);
    }
       
    
    // Y. if someone has stopped the cycle...
    if(eobool_false == s_theemsrunner.cycletiming.cycleisrunning)
    {
        
        // better to stop also the rx timers if they are not stopped yet. that may happen if something inside task-rx has askd for a stop of running
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]))
        {
            hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]); 
        } 
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]))
        {        
            hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
        }
        
        // then .. stop do timers
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]);        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]);
    }


    s_eom_emsrunner_tasktiming_on_exit(eo_emsrunner_taskid_runDO);
       
		
    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runTX], eo_emsrunner_evt_enable);
}


static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t)
{
    // nothing to do
}


static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t)
{
    s_eom_emsrunner_tasktiming_on_entry(eo_emsrunner_taskid_runTX);
    
        
    s_theemsrunner.numoftxpackets = 0;
    
    s_eom_emsrunner_userdef_taskTX_activity(&s_theemsrunner);
    
    
    for(;;)
    {
        // after the function s_eom_emsrunner_userdef_taskTX_activity(), the variable numofpacketsinsidesocket is surely updated
        // the function eo_emsrunner_mode_besteffort() always regturn true with eo_emsrunner_mode_besteffort
        // THUS: we wait with the semaphore only if there are packets to be tx and we can wait
        if((0 == s_theemsrunner.numofpacketsinsidesocket) || (eobool_false == eom_runner_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runTX)))
        {
            break;
        }
       
        // the semaphore is incremented when the task IPnet gives the packet to the IPAL, which in turns directly writes into ETH peripheral
        osal_semaphore_decrement(s_theemsrunner.waitudptxisdone, osal_reltimeINFINITE);
        s_theemsrunner.numofpacketsinsidesocket--;
        //#warning --> marco.accame: we wait for osal_reltimeINFINITE that the udp packet is sent ... can we think of a timeout???
    }

   
    s_eom_emsrunner_tasktiming_on_exit(eo_emsrunner_taskid_runTX);
    
    
    if(eobool_false == s_theemsrunner.cycletiming.cycleisrunning)
    {  

        // better to stop also the rx and do timers if they are not stopped yet. that may happen if something inside task-do/tx has asked for a stop of running
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]))
        {
            hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]); 
        } 
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]))
        {        
            hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
        }
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]))
        {
            hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]); 
        } 
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]))
        {        
            hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]);
        }
        
        // stop the tx timers ...
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]);        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runTX]);
        
        
        // and now ... finally we sent the state machine into the new state
        // reset the event to the dummy value.
        eOsmEventsEMSappl_t ev = s_theemsrunner.event;
        s_theemsrunner.event = eo_sm_emsappl_EVdummy;
        // we process the event. it can be either eo_sm_emsappl_EVgo2err or eo_sm_emsappl_EVgo2cfg
        eom_emsappl_SM_ProcessEvent(eom_emsappl_GetHandle(), ev); 
    }
    else
    {                  
        eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_enable);
    }
}


static void s_eom_emsrunner_userdef_taskTX_activity(EOMtheEMSrunner *p)
{
    p->numoftxrops = 0;
    p->numoftxpackets = 0;
    
    // A. perform a first activity
    eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(p);    
         
    // B. transmit a datagram
    s_eom_emsrunner_userdef_taskTX_activity_datagramtransmission(p);
    
    // C. perform an user-defined function after datagram transmission
    eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(p);        
}



static void s_eom_emsrunner_userdef_taskTX_activity_datagramtransmission(EOMtheEMSrunner *p)
{
    EOpacket *txpkt = NULL;
    
    uint16_t numberoftxrops = 0;    
    uint16_t processedpkts = 0;
    eObool_t processtransmission = eobool_true;
    

    
    // evaluate if we can enter in the transmission loop
    //if((0 == p->cfg.maxnumofTXpackets) || (eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runTX)))
    if(0 == p->cfg.maxnumofTXpackets)
    {
        processtransmission = eobool_false;
    } 
    else
    {
        processtransmission = eobool_true;
    }

//    if(s_theemsrunner.usedTXdecimationfactor > 1)   // if 0 we cannot divide, if 1 there is no need, if 2 or more we divide 
//    {
//        if(0 != (s_theemsrunner.iterationnumber % s_theemsrunner.usedTXdecimationfactor))
//        {
//            processtransmission = eobool_false;
//        }
//    }
    
    
        
    // the transmission section
    if(eobool_true == processtransmission)
    {
        eOresult_t resformer;
        
        // 1. process one packet        
        processedpkts++;   
        
        // 1.1 call the former to retrieve a tx packet (even if it is an empty ropframe)        
        resformer = eom_emstransceiver_Form(eom_emstransceiver_GetHandle(), &txpkt, &numberoftxrops, &s_theemsrunner.txropsnumberincycle);
        
       
        // 1.2  send a packet back. but only if the former gave us a good one and if we have at least 1 rop inside
        if(eores_OK == resformer)
        {
            if(numberoftxrops > 0)
            {
                eOresult_t restx = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt);
                if(eores_OK == restx)
                {
                    p->numoftxrops += numberoftxrops;
                    p->numoftxpackets++;
                    p->numofpacketsinsidesocket++;
                }
                else
                {
                    eom_emsrunner_hid_userdef_onfailedtransmission(p);
                }
            }
        }
        else
        {
            eom_emsrunner_hid_userdef_onemstransceivererror(eom_emstransceiver_GetHandle());
        }

        // 2. evaluate quit from the loop
        //if((processedpkts >= p->cfg.maxnumofTXpackets) || (eobool_true == s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runTX].isabout2overflow))
        //{
            processtransmission = eobool_false;
        //}   
        
    }    
           
}


static void s_eom_emsrunner_6HALTIMERS_stop(void)
{
    // rx timers
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]); 
    } 
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]))
    {        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
    }
    
    // do timers
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]); 
    } 
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]))
    {        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]);
    }
    
    // tx timers
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]); 
    } 
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runTX])) 
    {        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runTX]);
    }
    
}


static void s_eom_emsrunner_6HALTIMERS_execute_task(void *arg)
{
    eOemsrunner_taskid_t taskID2execute = (eOemsrunner_taskid_t) (int32_t)arg;
    EOMtask *task2execute = s_theemsrunner.task[taskID2execute];
           
    eOemsrunner_taskid_t taskIDprevious = (eo_emsrunner_taskid_runRX == taskID2execute) ? (eo_emsrunner_taskid_runTX) : ((eOemsrunner_taskid_t)((uint8_t)taskID2execute-1));
//    EOMtask* taskprevious = s_theemsrunner.task[taskIDprevious];
    
    if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskIDprevious].isexecuting)
    {
        // damn ... the previous task has not finished yet .... i must mark an overflow for that task.
        s_eom_runner_overflow_set(&s_theemsrunner, taskIDprevious);
    }

		 
    // send event to activate the task in argument
    eom_task_isrSetEvent(task2execute, eo_emsrunner_evt_execute);

   
    s_theemsrunner.cycletiming.tasktiming[taskID2execute].timestarted = osal_system_abstime_get();
}


static void s_eom_emsrunner_6HALTIMERS_safestop_check_task(void *arg)
{
    eOemsrunner_taskid_t taskID2check = (eOemsrunner_taskid_t) (int32_t)arg;
    //EOMtask *task2check = s_theemsrunner.task[taskID2check];
           
    if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskID2check].isexecuting)
    {
        // damn ... the previous task has not finished yet .... i must mark an overflow for that task.
        s_theemsrunner.cycletiming.tasktiming[taskID2check].isabout2overflow = eobool_true;
    }
    // ok, now we must check the value isabout2overflow inside the task  
}




static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check(void *arg)
{
    eOemsrunner_taskid_t taskid = (eOemsrunner_taskid_t) (int32_t)arg;
    
    hal_timer_cfg_t periodic_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL
    }; 

    periodic_cfg.countdown         = s_theemsrunner.cfg.period;
    periodic_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_safestop_check_task;
    periodic_cfg.arg               = arg;
    hal_timer_init(s_theemsrunner.haltimer_safestop[taskid], &periodic_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_safestop[taskid]);    
}


static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_execute_task(void *arg)
{
    eOemsrunner_taskid_t taskid = (eOemsrunner_taskid_t) (int32_t)arg;
    
    hal_timer_cfg_t periodic_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL
    }; 

    periodic_cfg.countdown         = s_theemsrunner.cfg.period;
    periodic_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_execute_task;
    periodic_cfg.arg               = arg;
    hal_timer_init(s_theemsrunner.haltimer_start[taskid], &periodic_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_start[taskid]);    
}



static void s_eom_emsrunner_tasktiming_on_entry(eOemsrunner_taskid_t taskid)
{
    s_theemsrunner.cycletiming.tasktiming[taskid].isexecuting = eobool_true;   
}


static void s_eom_emsrunner_tasktiming_on_exit(eOemsrunner_taskid_t taskid)
{
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES)
    evEntityId_t prev = eventviewer_switch_to(ev_meas_stop[taskid]);      
    eventviewer_switch_to(prev); 
#endif
    
    s_theemsrunner.cycletiming.tasktiming[taskid].timestopped = osal_system_abstime_get();
    s_theemsrunner.cycletiming.tasktiming[taskid].duration[1] = s_theemsrunner.cycletiming.tasktiming[taskid].duration[0]; 
    s_theemsrunner.cycletiming.tasktiming[taskid].duration[0] = s_theemsrunner.cycletiming.tasktiming[taskid].timestopped - s_theemsrunner.cycletiming.tasktiming[taskid].timestarted;
    s_theemsrunner.cycletiming.tasktiming[taskid].isexecuting = eobool_false;      
}


static void s_eom_emsrunner_tasktiming_eval(void)
{
    // we call it at teh beginning of the ctrl loop. we dont check isabout2overflow, only isoverflown
    s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eo_emsrunner_taskid_runRX);
    s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eo_emsrunner_taskid_runDO);
    s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eo_emsrunner_taskid_runTX);    
}



static void s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eOemsrunner_taskid_t taskid)
{
    static const eOerror_value_SYS_t errorvalue[3] = { eoerror_value_SYS_ctrloop_execoverflowRX, eoerror_value_SYS_ctrloop_execoverflowDO, eoerror_value_SYS_ctrloop_execoverflowTX };
    static uint64_t last4durations = 0;
    
    uint64_t currduration = s_theemsrunner.cycletiming.tasktiming[taskid].duration[0];

    
//    char strtime[64] = {0};
    
	eOerrmanDescriptor_t errdes = {0};
	errdes.sourcedevice = eo_errman_sourcedevice_localboard;
  	errdes.sourceaddress    = 0;
	eOerrmanErrorType_t errortype = (eo_emsrunner_mode_hardrealtime == s_theemsrunner.mode) ? (eo_errortype_error) : (eo_errortype_warning);
    
	switch (taskid)
	{
		case eo_emsrunner_taskid_runRX:
        {
            
//            snprintf(strtime, sizeof(strtime), "RX %d us", (int)currduration); 
//            eo_errman_Trace(eo_errman_GetHandle(), strtime, s_eobj_ownname);
            
			if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown)
			{
				errdes.code             = eoerror_code_get(eoerror_category_System, errorvalue[taskid]);
				errdes.par16            = currduration & 0xffff;
                errdes.par64            = last4durations; 
				eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
			}

        } break;
            
		case eo_emsrunner_taskid_runDO:
        {
            
//            snprintf(strtime, sizeof(strtime), "DO %d us", (int)currduration); 
//            eo_errman_Trace(eo_errman_GetHandle(), strtime, s_eobj_ownname);
            
			if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown)
			{
				errdes.code             = eoerror_code_get(eoerror_category_System, errorvalue[taskid]);
				errdes.par16            = currduration & 0xffff;
                errdes.par64            = last4durations; 
				eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
			}

        } break;
            
		case eo_emsrunner_taskid_runTX:
        {
            
//            snprintf(strtime, sizeof(strtime), "TX %d us", (int)currduration); 
//            eo_errman_Trace(eo_errman_GetHandle(), strtime, s_eobj_ownname);
            
			if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown)
			{
				errdes.code             = eoerror_code_get(eoerror_category_System, errorvalue[taskid]);
				errdes.par16            = currduration & 0xffff;
                uint64_t canframes = s_theemsrunner.txcan1frames | (s_theemsrunner.txcan2frames << 8);
                errdes.par64            = (last4durations & 0x0000ffffffffffff) | (canframes << 48); 
				eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
			}
          
        } break;
	}
    
    // must clear
    s_eom_runner_overflow_clr(&s_theemsrunner, taskid);
    
    last4durations = last4durations << 16;
    last4durations = last4durations & 0xffffffffffff0000;
    last4durations = last4durations | (currduration & 0xffff);    
}


static void s_eom_emsrunner_6HALTIMERS_start(EOMtheEMSrunner *p)
{    
    // now i start the timers for rx, do, tx and also for guards ...
    // i start the hal timers in one-shot mode with countdown = 1 period + the shift for {rx, do, tx, guards}.
    // at their expiry i restarte each timer to be periodic with countdown = 1 period. 
    // the periodic expiries will send the relevant eo_emsrunner_evt_execute to {rx, do, tx} and will set flags for the guards 
 
    
    // reset all flags telling about overflow of a task into the safety gap or into the next task.
    
    p->cycletiming.tasktiming[eo_emsrunner_taskid_runRX].isabout2overflow = eobool_false;
    p->cycletiming.tasktiming[eo_emsrunner_taskid_runDO].isabout2overflow = eobool_false;
    p->cycletiming.tasktiming[eo_emsrunner_taskid_runTX].isabout2overflow = eobool_false;

    s_eom_runner_overflow_clr(&s_theemsrunner, eo_emsrunner_taskid_runRX);
    s_eom_runner_overflow_clr(&s_theemsrunner, eo_emsrunner_taskid_runDO);
    s_eom_runner_overflow_clr(&s_theemsrunner, eo_emsrunner_taskid_runTX);
    
    // init the hal timers for rx, do, tx. first time they are one-shot
    
    {
        eOemsrunner_cfg_t* cfg = &p->cfg;
        
        hal_timer_cfg_t oneshot_cfg  = 
        {
            .prescaler          = hal_timer_prescalerAUTO,         
            .countdown          = 0,
            .priority           = hal_int_priority02,
            .mode               = hal_timer_mode_oneshot,
            .callback_on_exp    = s_eom_emsrunner_6HALTIMERS_start_periodic_timer_execute_task,
            .arg                = NULL
        }; 
               
        
        // timer for task rx
        oneshot_cfg.countdown         = cfg->period + cfg->execRXafter; 
        oneshot_cfg.arg               = (void*)eo_emsrunner_taskid_runRX;
        hal_timer_init(p->haltimer_start[eo_emsrunner_taskid_runRX], &oneshot_cfg, NULL); 
        

        // timer for task do
        oneshot_cfg.countdown          = cfg->period + cfg->execDOafter;  
        oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runDO;
        hal_timer_init(p->haltimer_start[eo_emsrunner_taskid_runDO], &oneshot_cfg, NULL);     
        
        // timer for task tx
        oneshot_cfg.countdown          = cfg->period + cfg->execTXafter; 
        oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runTX;
        hal_timer_init(p->haltimer_start[eo_emsrunner_taskid_runTX], &oneshot_cfg, NULL);  
        
        // TODO: those for warn RX, DO, TX
        
        // warn for task rx:
        if(hal_timerNONE != p->haltimer_safestop[eo_emsrunner_taskid_runRX])
        {
            oneshot_cfg.countdown          = cfg->period + cfg->execRXafter + cfg->safeRXexecutiontime;
            oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check; 
            oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runRX;
            hal_timer_init(p->haltimer_safestop[eo_emsrunner_taskid_runRX], &oneshot_cfg, NULL); 
        }        
        // warn for task do:
        if(hal_timerNONE != p->haltimer_safestop[eo_emsrunner_taskid_runDO])
        {
            oneshot_cfg.countdown          = cfg->period + cfg->execDOafter + cfg->safeDOexecutiontime;
            oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check;
            oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runDO;
            hal_timer_init(p->haltimer_safestop[eo_emsrunner_taskid_runDO], &oneshot_cfg, NULL);     
        }     
        // warn for task tx:
        if(hal_timerNONE != p->haltimer_safestop[eo_emsrunner_taskid_runTX])
        {
            oneshot_cfg.countdown          = cfg->period + cfg->execTXafter + cfg->safeTXexecutiontime;
            oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check;
            oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runTX;
            hal_timer_init(p->haltimer_safestop[eo_emsrunner_taskid_runTX], &oneshot_cfg, NULL);  
        }   
                   
    }
   

    // we also need to enable the first task: taskRX. the other tasks will be enabled at the end of the previous tasks
    
    eom_task_SetEvent(p->task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_enable);
    
    // we impose that the cycle is running    
    p->cycletiming.cycleisrunning = eobool_true;
    p->cycletiming.iterationnumber = 0;
    
    memset(&s_theemsrunner.cycletiming.tasktiming, 0, sizeof(s_theemsrunner.cycletiming.tasktiming));

    // finally: we activate the hal timers. it is best to avoid any interruption in here, thus we disable scheduling
    //osal_system_scheduling_suspend();
    
    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runRX]);
    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runDO]);
    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runTX]);    
    
    if(hal_timerNONE != p->haltimer_safestop[eo_emsrunner_taskid_runRX])
    {
        hal_timer_start(p->haltimer_safestop[eo_emsrunner_taskid_runRX]);
    }

    if(hal_timerNONE != p->haltimer_safestop[eo_emsrunner_taskid_runDO])
    {
        hal_timer_start(p->haltimer_safestop[eo_emsrunner_taskid_runDO]);
    }

    if(hal_timerNONE != p->haltimer_safestop[eo_emsrunner_taskid_runTX])
    {
        hal_timer_start(p->haltimer_safestop[eo_emsrunner_taskid_runTX]);
    }
    
    //osal_system_scheduling_restart();
          
}

extern eObool_t eom_runner_cansafelyexecute(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    eObool_t ret = eobool_false;

    switch(p->mode)
    {
        case eo_emsrunner_mode_besteffort:
        {
            ret = eobool_true;
        } break;
        
        case eo_emsrunner_mode_softrealtime:
        {
            ret = (eobool_false == p->cycletiming.tasktiming[taskid].isabout2overflow) ? (eobool_true) : (eobool_false);
        } break; 
        
        case eo_emsrunner_mode_hardrealtime:
        {
            ret = (eobool_false == p->cycletiming.tasktiming[taskid].isabout2overflow) ? (eobool_true) : (eobool_false);
        } break;  

        default:
        {
            ret = eobool_false;
        } break;
        
    }   
    
    return(ret);
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




