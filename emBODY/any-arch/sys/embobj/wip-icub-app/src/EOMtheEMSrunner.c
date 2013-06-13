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
#include "osal.h"


#include "EOMtask_hid.h" // to retrieve its osaltask pointer

#include "EOVtheIPnet.h"

#include "EOMtheIPnet.h"

#include "EOMtheEMSappl.h"

#include "EOaction_hid.h"
#include "EOMtheCallbackManager.h"

#include "eventviewer.h"

#include "OPCprotocolManager_Cfg.h"


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

#if defined(EVIEWER_ENABLED)

#define EVIEWER_IDbase                  (ev_ID_first_isr+80)
#define EVIEWER_IDofTSKstart(tskid)     (EVIEWER_IDbase+2*(tskid))
#define EVIEWER_IDofTSKalert(tskid)     (EVIEWER_IDbase+2*(tskid)+1)

#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg = 
{
    EO_INIT(.taskpriority)              {250,   251,    252},  
    EO_INIT(.taskstacksize)             {1024,  1024,   1024},
    EO_INIT(.period)                    1000,  
    EO_INIT(.execRXafter)               0, 
    EO_INIT(.safeRXexecutiontime)       300,
    EO_INIT(.execDOafter)               400, 
    EO_INIT(.safeDOexecutiontime)       250,
    EO_INIT(.execTXafter)               700,
    EO_INIT(.safeTXexecutiontime)       250,    
    EO_INIT(.maxnumofRXpackets)         3,
    EO_INIT(.maxnumofTXpackets)         1,
    EO_INIT(.modeatstartup)             eo_emsrunner_mode_besteffort, //eo_emsrunner_mode_softrealtime //eo_emsrunner_mode_besteffort //eo_emsrunner_mode_softrealtime
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emsrunner_update_dignosticsinfo_timing(void);
static void s_eom_emsrunner_update_dignosticsinfo_exeoverflows(eOemsrunner_taskid_t taskid);
static void s_eom_emsrunner_update_dignosticsinfo_txfailure(void);

static eObool_t s_eom_emsrunner_timing_is_compatible(const eOemsrunner_cfg_t *cfg);

static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_enable_task(EOMtask *tsk, osal_caller_t osalcaller);

static void s_eom_emsrunner_6HALTIMERS_start_oneshotosalcbk_for_rxdotx_cycle(void* arg);
static void s_eom_emsrunner_6HALTIMERS_start_rxdotx_cycle_ultrabasic(osal_timer_t *tmr, void *arg);
static void s_eom_emsrunner_6HALTIMERS_start_task_ultrabasic(void *arg);
static void s_eom_emsrunner_6HALTIMERS_activate_warn_ultrabasic(void *arg);
static void s_eom_emsrunner_6HALTIMERS_activate_task_ultrabasic(void *arg);
static void s_eom_emsrunner_6HALTIMERS_activate_taskrx_ultrabasic(void *arg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSrunner";

 
static EOMtheEMSrunner s_theemsrunner = 
{
    EO_INIT(.cfg)                   {0},
    EO_INIT(.task)                  {NULL, NULL, NULL},
    EO_INIT(.event)                 eo_sm_emsappl_EVdummy,
    EO_INIT(.osaltimer)             NULL,
    EO_INIT(.cycleisrunning)        eobool_false,
    EO_INIT(.safeDurationExpired)   {eobool_false, eobool_false, eobool_false},
    EO_INIT(.overflownToNextTask)   {eobool_false, eobool_false, eobool_false},
    EO_INIT(.haltimer_start)        {hal_timer2, hal_timer3, hal_timer4},
    EO_INIT(.haltimer_alert)        {hal_timer5, hal_timer6, hal_timer7},
    EO_INIT(.numofrxpackets)        0,  
    EO_INIT(.numofrxrops)           0,
    EO_INIT(.numoftxpackets)        0,
    EO_INIT(.numoftxrops)           0,
    EO_INIT(.mode)                  eo_emsrunner_mode_softrealtime,
    EO_INIT(.numofpacketsinsidesocket) 0,
    EO_INIT(.waitudptxisdone)       NULL,
    EO_INIT(.osaltaskipnetexec)     NULL
};


static eOemsrunner_diagnosticsinfo_t s_eom_emsrunner_diagnosticsinfo   =
{
    .numberofperiods                            = 0,
    .cumulativeabsoluteerrorinperiod            = 0,
    .meanofabsoluteerrorinperiod                = 0,
    .movingmeanofabsoluteerrorinperiod          = 0,
    .maxabsoluteerrorinperiod                   = 0,
    .minabsoluteerrorinperiod                   = 1000000000,
    .executionoverflows                         = {0, 0, 0},
    .datagrams_failed_to_go_in_txsocket         = 0  
};
//static const hal_timer_t s_eom_runner_timers_task[3] = {hal_timer2, hal_timer3, hal_timer4};
//static const hal_timer_t s_eom_runner_timers_warn[3] = {hal_timer5, hal_timer6, hal_timer7};

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
    
    
 
    eo_errman_Assert(eo_errman_GetHandle(), (eobool_true == s_eom_emsrunner_timing_is_compatible(cfg)), s_eobj_ownname, "timings of tasks are not compatible");

    eo_errman_Assert(eo_errman_GetHandle(), (cfg->maxnumofTXpackets <= 1), s_eobj_ownname, "cfg->maxnumofTXpackets must be 0 or 1");
    
    memcpy(&s_theemsrunner.cfg, cfg, sizeof(eOemsrunner_cfg_t));
    

    
    s_theemsrunner.cycleisrunning = eobool_false; 
    
    eom_emsrunner_SetMode(&s_theemsrunner, cfg->modeatstartup);
    
    s_theemsrunner.event = eo_sm_emsappl_EVdummy;
    
    s_theemsrunner.osaltimer = osal_timer_new();
    
    s_theemsrunner.numofpacketsinsidesocket = 0;
    
    s_theemsrunner.waitudptxisdone = osal_semaphore_new(255, 0);
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
     

    
#if defined(EVIEWER_ENABLED)    
    eventviewer_load(EVIEWER_IDofTSKstart(eo_emsrunner_taskid_runRX), evRXstart);
    eventviewer_load(EVIEWER_IDofTSKalert(eo_emsrunner_taskid_runRX), evRXalert);
    eventviewer_load(EVIEWER_IDofTSKstart(eo_emsrunner_taskid_runDO), evDOstart);
    eventviewer_load(EVIEWER_IDofTSKalert(eo_emsrunner_taskid_runDO), evDOalert);    
    eventviewer_load(EVIEWER_IDofTSKstart(eo_emsrunner_taskid_runTX), evTXstart);    
    eventviewer_load(EVIEWER_IDofTSKalert(eo_emsrunner_taskid_runTX), evTXalert);
#endif    
        
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
//    hal_result_t res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
    

    // put in here with protection vs double iniytialisation. 
    // we could put also in initialise, but i want to avoid that the runner is initisalised and ther eis not an osal task yet for ipnet
    if(NULL == p->osaltaskipnetexec)
    {   // compute it only once in life
        EOMtask * taskipnetexec = eom_ipnet_GetTask(eom_ipnet_GetHandle(), eomipnet_task_proc);
        p->osaltaskipnetexec = taskipnetexec->osaltask;
    } 

    
    p->numofpacketsinsidesocket = 0;      
    p->mode = p->cfg.modeatstartup;    
    osal_semaphore_set(s_theemsrunner.waitudptxisdone, 0);
    

    s_eom_emsrunner_6HALTIMERS_start_oneshotosalcbk_for_rxdotx_cycle(&s_theemsrunner);
  
    return(eores_OK);
}

extern eOresult_t eom_emsrunner_StopAndGoTo(EOMtheEMSrunner *p, eOsmEventsEMSappl_t ev)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
    
    if((eo_sm_emsappl_EVdummy == ev) || (eo_sm_emsappl_EVgo2run == ev))
    {   // cannot stop on dummy event or 
        return(eores_NOK_generic);
    }

    
    s_theemsrunner.cycleisrunning = eobool_false;
    s_theemsrunner.event = ev;
    
    // the hal timers are stopped within the ...
    
    return(eores_OK);
}

eObool_t eom_emsrunner_SafeDurationExpired(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    if(NULL == p)
    {
        return(eobool_true);
    } 

    return(s_theemsrunner.safeDurationExpired[taskid]);
}


eObool_t eom_emsrunner_OverflownToNextTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    if(NULL == p)
    {
        return(eobool_true);
    } 

    return(s_theemsrunner.overflownToNextTask[taskid]);
}

extern eOresult_t eom_emsrunner_SetMode(EOMtheEMSrunner *p, eOemsrunner_mode_t mode)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 
    

    s_theemsrunner.mode = mode;
    
    eo_errman_Assert(eo_errman_GetHandle(), (eo_emsrunner_mode_hardrealtime != mode), s_eobj_ownname, "eo_emsrunner_mode_hardrealtime requires a change of policy in the parsing of a ropframe");
 
    
    return(eores_OK);
}

extern void eom_emsrunner_OnUDPpacketTransmitted(EOMtheEMSrunner *p)
{
    osal_semaphore_increment(s_theemsrunner.waitudptxisdone, osal_callerTSK);
}


extern eOresult_t eom_emsrunner_GetDiagnosticsInfo(EOMtheEMSrunner *p, eOemsrunner_diagnosticsinfo_t *dgn_ptr)
{
    if((NULL == p)|| (NULL == dgn_ptr))
    {
        return(eores_NOK_nullpointer);
    } 
    
    memcpy(dgn_ptr, &s_eom_emsrunner_diagnosticsinfo, sizeof(eOemsrunner_diagnosticsinfo_t));
    return(eores_OK);
}

extern eOemsrunner_diagnosticsinfo_t * eom_emsrunner_GetDiagnosticsInfoHandle(EOMtheEMSrunner *p)
{
    if(NULL == p)
    {
        return(NULL);
    } 
    
    return(&s_eom_emsrunner_diagnosticsinfo);
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
;
}


/*__weak*/ extern void eom_emsrunner_hid_userdef_taskRX_activity(EOMtheEMSrunner *p)
{   
    s_eom_emsrunner_update_dignosticsinfo_timing();
    
    p->numofrxrops = 0;
    p->numofrxpackets = 0;
    
    // A. perform a first activity before datagram reception
    eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(p); 

    
    // B. receive and parse datagram(s)
    eom_emsrunner_hid_userdef_taskRX_activity_datagramreception(p);


    // C. perform a third activity after datagram parsing
    eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(p);  
     
}


__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
   eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runRX))
    {
        itissafetoquit_asap =  eobool_true;
    } 
    
    itissafetoquit_asap = itissafetoquit_asap;

    // if itissafetoquit_asap then ... do nothing or very little.
}



/*__weak*/ extern void eom_emsrunner_hid_userdef_taskRX_activity_datagramreception(EOMtheEMSrunner *p)
{
    EOpacket *rxpkt = NULL;
    
    eOsizecntnr_t remainingrxpkts = 1;    
    uint16_t processedpkts = 0;
    eObool_t processreception = eobool_true;
    eOresult_t res;
    
    // evaluate if we can enter in the reception loop
    if((0 == p->cfg.maxnumofRXpackets) || (eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runRX)))
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
           (eores_OK != resrx) || (eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runRX)))
        {
            processreception = eobool_false;
        }        
    
    }
 
       
}




__weak extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runRX))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;   
    
    // if itissafetoquit_asap then ... do nothing or very little.    
}

__weak extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
   eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runDO))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;    

    // if itissafetoquit_asap then ... do nothing or very little.
}

/*__weak*/ extern void eom_emsrunner_hid_userdef_taskTX_activity(EOMtheEMSrunner *p)
{
    p->numoftxrops = 0;
    p->numoftxpackets = 0;
    
    // A. perform a first activity
    eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(p);    
         
    // B. transmit a datagram
    eom_emsrunner_hid_userdef_taskTX_activity_datagramtransmission(p);
    
    // C. perform an user-defined function after datagram transmission
    eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(p);        
}



__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runTX))
    {
        itissafetoquit_asap =  eobool_true;
    }   

    itissafetoquit_asap = itissafetoquit_asap;
    
    // if itissafetoquit_asap then ... do nothing or very little.       
}


/*__weak*/ extern void eom_emsrunner_hid_userdef_taskTX_activity_datagramtransmission(EOMtheEMSrunner *p)
{
    EOpacket *txpkt = NULL;
    
    uint16_t numberoftxrops = 0;    
    uint16_t processedpkts = 0;
    eObool_t processtransmission = eobool_true;
    

    
    // evaluate if we can enter in the transmission loop
    //if((0 == p->cfg.maxnumofTXpackets) || (eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runTX)))
    if(0 == p->cfg.maxnumofTXpackets)
    {
        processtransmission = eobool_false;
    }     
    
        
    // the transmission section
    if(eobool_true == processtransmission)
    {
        eOresult_t resformer;
        eOresult_t restx;
        
        // 1. process one packet        
        processedpkts++;   
        
        // 1.1 call the former to retrieve a tx packet (even if it is an empty ropframe)        
        resformer = eom_emstransceiver_Form(eom_emstransceiver_GetHandle(), &txpkt, &numberoftxrops);
        
        // 1.2  send a packet back. but only if the former gave us a good one.
        if(eores_OK == resformer)
        {
            //restx = 
            restx = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt);
            if(eores_OK == restx)
            {
                p->numoftxrops += numberoftxrops;
                p->numoftxpackets++;
                p->numofpacketsinsidesocket++;
            }
            else
            {
                s_eom_emsrunner_update_dignosticsinfo_txfailure();
                eom_emsrunner_hid_userdef_onfailedtransmission(p);
            }
        }
        else
        {
            eom_emsrunner_hid_userdef_onemstransceivererror(eom_emstransceiver_GetHandle());
        }

        // 2. evaluate quit from the loop
        //if((processedpkts >= p->cfg.maxnumofTXpackets) || (eobool_true == s_theemsrunner.safeDurationExpired[eo_emsrunner_taskid_runTX]))
        //{
            processtransmission = eobool_false;
        //}   
        
    }    
           
}


__weak extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_hid_cansafelyexecute(p, eo_emsrunner_taskid_runTX))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;
    
    // if itissafetoquit_asap then ... do nothing or very little.       
}


__weak extern void eom_emsrunner_hid_userdef_onexecutionoverflow(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    const char * tskname[] = {"tskRX", "tskDO", "tskTX"};
    char str[48];
    eOerrmanErrorType_t errortype = (eo_emsrunner_mode_hardrealtime == p->mode) ? (eo_errortype_fatal) : (eo_errortype_warning);
    snprintf(str, sizeof(str)-1, "exec overflow of %s", tskname[taskid]); 
    eo_errman_Error(eo_errman_GetHandle(), errortype, s_eobj_ownname, str); 
}


__weak extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    char str[48];
    eOerrmanErrorType_t errortype = (eo_emsrunner_mode_hardrealtime == p->mode) ? (eo_errortype_fatal) : (eo_errortype_warning);
    snprintf(str, sizeof(str)-1, "failed to tx a packet, possibly because socket is full"); 
    eo_errman_Error(eo_errman_GetHandle(), errortype, s_eobj_ownname, str); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emsrunner_update_dignosticsinfo_timing(void) 
{
    static uint64_t prevtime = 0;
    uint64_t currtime = osal_system_abstime_get();
    int64_t error = 0;
    
    if(0 == prevtime)
    {
        prevtime = currtime;
        return;
    }
    
    error = (currtime - prevtime - s_theemsrunner.cfg.period); // period is 1000 microseconds
    if(error < 0) { error = -error; }
    
    
    
    s_eom_emsrunner_diagnosticsinfo.numberofperiods ++;
    s_eom_emsrunner_diagnosticsinfo.cumulativeabsoluteerrorinperiod += error;
    
    s_eom_emsrunner_diagnosticsinfo.meanofabsoluteerrorinperiod = s_eom_emsrunner_diagnosticsinfo.cumulativeabsoluteerrorinperiod / s_eom_emsrunner_diagnosticsinfo.numberofperiods;
    
    
    if(error > s_eom_emsrunner_diagnosticsinfo.maxabsoluteerrorinperiod)
    {
        s_eom_emsrunner_diagnosticsinfo.maxabsoluteerrorinperiod = error;       
    }
    
    if(error < s_eom_emsrunner_diagnosticsinfo.minabsoluteerrorinperiod)
    {
        s_eom_emsrunner_diagnosticsinfo.minabsoluteerrorinperiod = error;       
    }    
    
    {   // compute moving mean using 1024 samples
        static uint32_t mean_1k_n = 0;
        static uint32_t mean_n = 0;
        
        uint32_t mean_1k_nplus1 = 0;
        uint32_t mean_nplus1 = 0;
        
        mean_1k_nplus1 = mean_1k_n - mean_n + error;
        
        mean_nplus1 = mean_1k_nplus1 >> 10;
        
        // save for next time
        mean_1k_n = mean_1k_nplus1;
        mean_n = mean_nplus1;
        
        s_eom_emsrunner_diagnosticsinfo.movingmeanofabsoluteerrorinperiod =  mean_nplus1;      
    }
    
    
    // finally save the curr time 
    prevtime = currtime;   
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
    
}



static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.
    
    
    //eov_ipnet_Activate(eov_ipnet_GetHandle());
    
    // perform the rx activity    
    if(eobool_true == eom_runner_hid_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runRX))
    {
        eom_emsrunner_hid_userdef_taskRX_activity(&s_theemsrunner);
    }
      
    //eov_ipnet_Deactivate(eov_ipnet_GetHandle());
    

    // Y. if someone has stopped the cycle...
    if(eobool_false == s_theemsrunner.cycleisrunning)
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]);        
        hal_timer_stop(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runRX]);
    }
    
    if(eobool_true == eom_runner_hid_signaloverflow(&s_theemsrunner, eo_emsrunner_taskid_runRX))
    {
       s_eom_emsrunner_update_dignosticsinfo_exeoverflows(eo_emsrunner_taskid_runRX);
       eom_emsrunner_hid_userdef_onexecutionoverflow(&s_theemsrunner, eo_emsrunner_taskid_runRX); 
    }
    
    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runDO], osal_callerTSK);
}


static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t)
{
 
}



static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.

    if(eobool_true == eom_runner_hid_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runDO))
    {
        // perform the do activity
        eom_emsrunner_hid_userdef_taskDO_activity(&s_theemsrunner);
    }
       
    
    // Y. if someone has stopped the cycle...
    if(eobool_false == s_theemsrunner.cycleisrunning)
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]);        
        hal_timer_stop(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runDO]);
    }

    if(eobool_true == eom_runner_hid_signaloverflow(&s_theemsrunner, eo_emsrunner_taskid_runDO))
    {
       s_eom_emsrunner_update_dignosticsinfo_exeoverflows(eo_emsrunner_taskid_runDO);
       eom_emsrunner_hid_userdef_onexecutionoverflow(&s_theemsrunner, eo_emsrunner_taskid_runDO);     
    }
    
    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runTX], osal_callerTSK);
}


static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.
    
    s_theemsrunner.numoftxpackets = 0;
    
    //eov_ipnet_Activate(eov_ipnet_GetHandle());

#if 0    
//    if(eobool_true == eom_runner_hid_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runTX))
    if(1) // always execute the transmission
    {
        eom_emsrunner_hid_userdef_taskTX_activity(&s_theemsrunner);
        
        if(0 != s_theemsrunner.numofpacketsinsidesocket)
        {
            uint8_t i;            
            for(i=0; i<s_theemsrunner.numofpacketsinsidesocket; i++)
            {   // must decrement a number of times equal to the number of pkts given to the socket    
                osal_semaphore_decrement(s_theemsrunner.waitudptxisdone, osal_reltimeINFINITE);
            }
            s_theemsrunner.numofpacketsinsidesocket = 0;
        }
        
    }
#else

    eom_emsrunner_hid_userdef_taskTX_activity(&s_theemsrunner);
    
    for(;;)
    {
        if((0 == s_theemsrunner.numofpacketsinsidesocket) || (eobool_false == eom_runner_hid_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runTX)))
        {
            break;
        }
        
        osal_semaphore_decrement(s_theemsrunner.waitudptxisdone, osal_reltimeINFINITE);
        s_theemsrunner.numofpacketsinsidesocket--;
    }



#endif    
    //eov_ipnet_Deactivate(eov_ipnet_GetHandle());
    
    
    if(eobool_true == eom_runner_hid_signaloverflow(&s_theemsrunner, eo_emsrunner_taskid_runTX))
    {   // it is in this position so that ... it is still possible to send the EMS appl in ERROR state.
        s_eom_emsrunner_update_dignosticsinfo_exeoverflows(eo_emsrunner_taskid_runTX);
        eom_emsrunner_hid_userdef_onexecutionoverflow(&s_theemsrunner, eo_emsrunner_taskid_runTX);
    } 

    if(eobool_false == s_theemsrunner.cycleisrunning)
    {   
        // stop the timers ...
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]);        
        hal_timer_stop(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runTX]);
        
        // reset the event to the dummy value.
        eOsmEventsEMSappl_t ev = s_theemsrunner.event;
        s_theemsrunner.event = eo_sm_emsappl_EVdummy;
        // we process the event. it can be either eo_sm_emsappl_EVgo2err or eo_sm_emsappl_EVgo2cfg
        eom_emsappl_ProcessEvent(eom_emsappl_GetHandle(), ev); 
    }
    else
    {   
        
        // at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
        s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX], osal_callerTSK);
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





static void s_eom_emsrunner_6HALTIMERS_start_oneshotosalcbk_for_rxdotx_cycle(void* arg)
{
    osal_timer_timing_t timing;
    osal_timer_onexpiry_t onexpiry;
    
    timing.startat  = OSAL_abstimeNONE;
    timing.count    = osal_info_get_tick(); 
    timing.mode     = osal_tmrmodeONESHOT; 
    
    onexpiry.cbk    = s_eom_emsrunner_6HALTIMERS_start_rxdotx_cycle_ultrabasic;
    onexpiry.par    = arg;        
    
    s_theemsrunner.safeDurationExpired[eo_emsrunner_taskid_runRX] = eobool_false;
    s_theemsrunner.safeDurationExpired[eo_emsrunner_taskid_runDO] = eobool_false;
    s_theemsrunner.safeDurationExpired[eo_emsrunner_taskid_runTX] = eobool_false; 

    s_theemsrunner.overflownToNextTask[eo_emsrunner_taskid_runRX] = eobool_false;
    s_theemsrunner.overflownToNextTask[eo_emsrunner_taskid_runDO] = eobool_false;
    s_theemsrunner.overflownToNextTask[eo_emsrunner_taskid_runTX] = eobool_false; 
    
    osal_timer_start(s_theemsrunner.osaltimer, &timing, &onexpiry, osal_callerTSK);


    // now we also need to enable the first task: taskRX. the other enables shall be send at the end of the relevant tasks
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX], osal_callerTSK);
    
    s_theemsrunner.cycleisrunning = eobool_true;    
}




static void s_eom_emsrunner_6HALTIMERS_start_rxdotx_cycle_ultrabasic(osal_timer_t *tmr, void *arg)
{
    
    eOemsrunner_cfg_t* cfg = &s_theemsrunner.cfg;
    uint64_t startusec = 0;
    //uint64_t nowusec   = 0;
    
    hal_timer_cfg_t oneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    }; 
    
//     hal_timer_cfg_t periodic_cfg  = 
//     {
//         .prescaler          = hal_timer_prescalerAUTO,         
//         .countdown          = 0,
//         .priority           = hal_int_priority02,
//         .mode               = hal_timer_mode_periodic,
//         .callback_on_exp    = NULL,
//         .arg                = NULL
//     };     
    

    
    // start of task rx:
    oneshot_cfg.countdown         = cfg->period + cfg->execRXafter;
    oneshot_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_activate_taskrx_ultrabasic;
    oneshot_cfg.arg               = (void*)eo_emsrunner_taskid_runRX;
    hal_timer_init(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX], &oneshot_cfg, NULL); 
    startusec = osal_system_abstime_get(); // keep in here ...
    hal_timer_start(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]);
    // startusec = osal_system_abstime_get();
    
    // warn for task rx:
//    oneshot_cfg.countdown          = cfg->execDOafter - cfg->safetyGAP - (osal_system_abstime_get() - startusec); // - 0;
    oneshot_cfg.countdown          = cfg->execRXafter + cfg->safeRXexecutiontime - (osal_system_abstime_get() - startusec); // - 0;
    oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_activate_warn_ultrabasic; 
    oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runRX;
    hal_timer_init(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runRX], &oneshot_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runRX]);

    // start of task do:
//    oneshot_cfg.countdown          = cfg->execDOafter - (osal_system_abstime_get() - startusec); //- 5;
    oneshot_cfg.countdown          = cfg->execDOafter - (osal_system_abstime_get() - startusec); //- 5;
    oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_activate_task_ultrabasic;
    oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runDO;
    hal_timer_init(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO], &oneshot_cfg, NULL);     
    hal_timer_start(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]);  
    
    // warn for task do:
//    oneshot_cfg.countdown          = cfg->execTXafter - cfg->safetyGAP - (osal_system_abstime_get() - startusec);//- 10;
    oneshot_cfg.countdown          = (cfg->execDOafter + cfg->safeDOexecutiontime) - (osal_system_abstime_get() - startusec);//- 10;
    oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_activate_warn_ultrabasic;
    oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runDO;
    hal_timer_init(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runDO], &oneshot_cfg, NULL);     
    hal_timer_start(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runDO]);
    
    // start of task tx:
//    oneshot_cfg.countdown          = cfg->execTXafter - (osal_system_abstime_get() - startusec);//- 15;
    oneshot_cfg.countdown          = cfg->execTXafter - (osal_system_abstime_get() - startusec);//- 15;
    oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_activate_task_ultrabasic;
    oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runTX;
    hal_timer_init(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX], &oneshot_cfg, NULL);     
    hal_timer_start(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]);
    
    // warn for task tx:
//    oneshot_cfg.countdown          = cfg->period - cfg->safetyGAP - (osal_system_abstime_get() - startusec);//- 20;
    oneshot_cfg.countdown          = (cfg->execTXafter + cfg->safeTXexecutiontime) - (osal_system_abstime_get() - startusec);//- 20;
    oneshot_cfg.callback_on_exp    = s_eom_emsrunner_6HALTIMERS_activate_warn_ultrabasic;
    oneshot_cfg.arg                = (void*)eo_emsrunner_taskid_runTX;
    hal_timer_init(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runTX], &oneshot_cfg, NULL);  
    hal_timer_start(s_theemsrunner.haltimer_alert[eo_emsrunner_taskid_runTX]);

}






static void s_eom_emsrunner_6HALTIMERS_start_task_ultrabasic(void *arg)
{
    eOemsrunner_taskid_t currtaskid = (eOemsrunner_taskid_t) (int32_t)arg;
    eOemsrunner_taskid_t prevtaskid = (eo_emsrunner_taskid_runRX == currtaskid) ? (eo_emsrunner_taskid_runTX) : ((eOemsrunner_taskid_t)((uint8_t)currtaskid-1));
#if defined(EVIEWER_ENABLED)    
    evEntityId_t preventityid = eventviewer_switch_to(EVIEWER_IDofTSKstart(currtaskid));
#endif
    EOMtask *currtask = s_theemsrunner.task[currtaskid];
    EOMtask* prevtask = s_theemsrunner.task[prevtaskid];
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
//    static osal_task_t *osaltaskipnetexec = NULL;

    
    // set to false the safety gap touched for DO
    s_theemsrunner.safeDurationExpired[currtaskid] = eobool_false;
    s_theemsrunner.overflownToNextTask[currtaskid] = eobool_false;
    
 
    if(prevtask->osaltask == scheduledosaltask) 
    {
        s_theemsrunner.overflownToNextTask[prevtaskid] = eobool_true;
    }
    else if(eo_emsrunner_taskid_runTX == prevtaskid) 
    {
        // must also verify that task ipnet is not executing 
        
//        if(NULL == osaltaskipnetexec)
//        {   // compute it only once in life
//            EOMtask * taskipnetexec = eom_ipnet_GetTask(eom_ipnet_GetHandle(), eomipnet_task_proc);
//            osaltaskipnetexec = taskipnetexec->osaltask;
//        }
        
        if(s_theemsrunner.osaltaskipnetexec == scheduledosaltask)
        {
            s_theemsrunner.overflownToNextTask[prevtaskid] = eobool_true;
        }
    }    
      
    // send event to activate the task in argument
    eom_task_isrSetEvent(currtask, eo_emsrunner_evt_execute);
    

#if defined(EVIEWER_ENABLED)    
    eventviewer_switch_to(preventityid);
#endif
}


static void s_eom_emsrunner_6HALTIMERS_warn_task_ultrabasic(void *arg)
{
    eOemsrunner_taskid_t taskid = (eOemsrunner_taskid_t) (int32_t)arg;
#if defined(EVIEWER_ENABLED)    
    evEntityId_t prev = eventviewer_switch_to(EVIEWER_IDofTSKalert(taskid));
#endif    
    EOMtask *task = s_theemsrunner.task[taskid];
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
    
        
    if(task->osaltask == scheduledosaltask)
    {   // if execution is in here it means that the task has not finished its computing in the time it was given to it minus the guard
               
        // set the boolean variable of the given task to true.
        s_theemsrunner.safeDurationExpired[taskid] = eobool_true;        
    }
    else if(eo_emsrunner_taskid_runTX == taskid) 
    {
        // must also verify that task ipnet is not executing 
       
        if(s_theemsrunner.osaltaskipnetexec == scheduledosaltask)
        {
            s_theemsrunner.safeDurationExpired[taskid] = eobool_true;
        }
    }        

//     // i stop the timer ...
//     if(eobool_false == s_theemsrunner.cycleisrunning)
//     {   
//         hal_timer_stop(s_theemsrunner.haltimer_alert[taskid]);
//     }

#if defined(EVIEWER_ENABLED)    
    eventviewer_switch_to(prev);
#endif    
}


static void s_eom_emsrunner_6HALTIMERS_activate_warn_ultrabasic(void *arg)
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
    periodic_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_warn_task_ultrabasic;
    periodic_cfg.arg               = arg;
    hal_timer_init(s_theemsrunner.haltimer_alert[taskid], &periodic_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_alert[taskid]);    
}


static void s_eom_emsrunner_6HALTIMERS_activate_task_ultrabasic(void *arg)
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
    periodic_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_start_task_ultrabasic;
    periodic_cfg.arg               = arg;
    hal_timer_init(s_theemsrunner.haltimer_start[taskid], &periodic_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_start[taskid]);    
}

static void s_eom_emsrunner_6HALTIMERS_activate_taskrx_ultrabasic(void *arg)
{
    s_eom_emsrunner_6HALTIMERS_start_task_ultrabasic(arg);
    
    s_eom_emsrunner_6HALTIMERS_activate_task_ultrabasic(arg);    
}

static void s_eom_emsrunner_update_dignosticsinfo_exeoverflows(eOemsrunner_taskid_t taskid)
{
     s_eom_emsrunner_diagnosticsinfo.executionoverflows[taskid] ++;
}

static void s_eom_emsrunner_update_dignosticsinfo_txfailure(void)
{
     s_eom_emsrunner_diagnosticsinfo.datagrams_failed_to_go_in_txsocket ++;
}


#if defined(EVIEWER_ENABLED)
void evRXstart(void){}
void evRXalert(void){}
void evDOstart(void){}
void evDOalert(void){}
void evTXstart(void){}
void evTXalert(void){}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





