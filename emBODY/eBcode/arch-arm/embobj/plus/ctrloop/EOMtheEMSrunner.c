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
#include "EOtheErrorManager.h"


#include "EOMtheEMStransceiver.h"

#include "EOtransceiver.h"
#include "EOtransmitter.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "EOaction.h"
#include "EOMtheCallbackManager.h"
#include "EoError.h"
#include "EOVtheSystem.h"

// marco.accame on 18jun25
// this macro must be kept defined. it shapes code for mode besteffort in such a way
// that the hw timer that starts the period activates only the rx phase and not rx-do-tx.
// if we do not define it, the hw timer activates all the phases in one shot. that usually
// work but ... if a phase goes beyond its budget it may be that the rx-do-tx realing in the
// wrong way ...
#define BESTEFFORT_ACTIVATE_RX_ONLY


#if defined(USE_EMBOT_theHandler)
//#warning EOMtheEMSrunner uses embot::app::eth::theHandler
#include "embot_app_eth_theHandler.h"
#include "embot_app_scope.h"
#else
#include "OPCprotocolManager_Cfg.h"
#include "EOMtheEMSappl.h"
#endif

#if !defined(EMBOBJ_USE_EMBOT)
#include "osal.h"
#else
#include "embot_os_rtos.h"
#endif // #if !defined(EMBOBJ_USE_EMBOT)

#if defined(USE_EMBOT_HW)

#include "embot_hw_sys.h"

#else
#include "hal_timer.h"
#endif

#include "embot_core.h"





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

// define it in case you experience negative delta times
//#define theRunner_USE_compensationofdurations


#define theRunner_USE_revised_algorithm
// NOTE: by undefining theRunner_USE_revised_algorithm we revert back as originally designed

// #define DEBUG_show_when_evt_execute_is_sent
// NOTE: if defined eventviewr will show when the evt_execute is emitted


// #define DEBUG_force_time_overflow
// NOTE: if defined the cycle sometimes will produce a phase with much longer time


// #define EOM_EMSRUNNER_EVIEW_MEASURES
// NOTE: if defined the eventviewer will show the timing of the phases. marco.accame: maybe it is not useful anymore.


#if defined(theRunner_USE_revised_algorithm)

// with USE_ACTIVATION_Mode02 defined the timer sends eo_emsrunner_evt_execute to its task in the following way. 
// - if the target task is NOT executing AND
// - if the enabling task (the enabling of RX is TX, of DO is RX and of TX is DO) is the latest executed task OR it is in execution
// if undefined it always sends eo_emsrunner_evt_execute 
#define USE_ACTIVATION_Mode02

// with USE_TIMINGMEASURE_effective we start measuring time on entry of thread. 
// if undefined we measure since emission of the event by the timer 
#define USE_TIMINGMEASURE_effective

// with USE_OVERFLOWCHECK_effectivetime we mark overflow by checking execution time > budget. 
// if undefined we mark overflow if the next timer triggers and thread is still in exec. 
#define USE_OVERFLOWCHECK_effectivetime

#endif



#if defined(DEBUG_show_when_evt_execute_is_sent)

#include "embot_app_scope.h"
#include <array>

void execRXemitted() {}
//embot::app::scope::SignalEViewer *sevrx {nullptr};
embot::app::scope::SignalEViewer::Config evcrx {execRXemitted, embot::app::scope::SignalEViewer::Config::LABEL::one};  

void execDOemitted() {}
//embot::app::scope::SignalEViewer *sevdo {nullptr};
embot::app::scope::SignalEViewer::Config evcdo {execDOemitted, embot::app::scope::SignalEViewer::Config::LABEL::two};  

void execTXemitted() {}
//embot::app::scope::SignalEViewer *sevtx {nullptr};
embot::app::scope::SignalEViewer::Config evctx {execTXemitted, embot::app::scope::SignalEViewer::Config::LABEL::three};  

std::array<embot::app::scope::SignalEViewer *, eo_emsrunner_task_numberof> ev {nullptr};

void eom_emsrunner_scope_init()
{
    ev[0] = new embot::app::scope::SignalEViewer(evcrx);
    ev[1] = new embot::app::scope::SignalEViewer(evcdo);
    ev[2] = new embot::app::scope::SignalEViewer(evctx);    
}

void eom_emsrunner_scope_tick(eOemsrunner_taskid_t t)
{
    if(t < eo_emsrunner_task_numberof)
    {        
        ev[t]->on();
        ev[t]->off();
    }
}

#endif



#if defined(DEBUG_force_time_overflow)

#if defined(USE_EMBOT_theHandler)
#include "embot_hw_sys.h"
#else
#include "embot_core.h"
#endif


void mydelay(uint64_t t)
{
#if defined(USE_EMBOT_theHandler)
    embot::hw::sys::delay(t);
#else
    embot::core::wait(t);
#endif    
    
}

void debug_delay_task(eOemsrunner_taskid_t t, uint64_t iterationnumber)
{
    static constexpr uint64_t period {1000};
    static constexpr std::array<uint64_t, 3> delays {700, 1900, 900};
    
    if(eo_emsrunner_taskid_runRX == t)
    {        
        if((iterationnumber % period) == 4)
        {            
            mydelay(delays[t]);
        }
    }
    else if(eo_emsrunner_taskid_runDO == t)
    {
        if((iterationnumber % period) == 5)
        {
            mydelay(delays[t]);
        }         
    }
    else if(eo_emsrunner_taskid_runTX == t)
    {
        if((iterationnumber % period) == 4)
        {
            mydelay(delays[t]);
        }        
    }
    
}
    

    
#endif    

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg = 
{
    EO_INIT(.taskpriority)              {250,   251,    252},  
    EO_INIT(.taskstacksize)             {2048,  1024,   2048},
    EO_INIT(.haltimerstart)             {hal_timer2, hal_timer3, hal_timer4},    
    EO_INIT(.haltimeralert)             {hal_timer5, hal_timer6, hal_timer7},  
    EO_INIT(.period)                    1000,  
    EO_INIT(.execRXafter)               0, 
    EO_INIT(.safeRXexecutiontime)       300,
    EO_INIT(.execDOafter)               400, 
    EO_INIT(.safeDOexecutiontime)       250,
    EO_INIT(.execTXafter)               700,
    EO_INIT(.safeTXexecutiontime)       250,    
    EO_INIT(.maxnumofRXpackets)         3,
    EO_INIT(.maxnumofTXpackets)         1,
    EO_INIT(.modeatstartup)             eo_emsrunner_mode_synchronized,
    EO_INIT(.defaultTXdecimationfactor) 1
};

constexpr uint8_t histoBINSnumber {7};  // must be 7 because we shall fit 7+1 U8 values in the par64
constexpr uint16_t histoBINSsize {250}; // that may be changed. a reasonable value can be period/4 ore period/5
// i use [0, 250) U [250, 500) U [500, 750) U [750, 1000) U [1000, 1250) U [1250, 1500) U [1500, 1750) 
// then when i call .probabilitydensityfunction(v) i shall have:
// v[0] with all the value below 0 -> none
// v[1] with all the values in the first bin [0, 250) .... v[histoBINSnumber] with all the values in the last bin [1500, 1750) 
// v[histoBINSnumber+1] with the values beyond 1750
constexpr embot::tools::Histogram::Config histoCFG {0, histoBINSnumber*histoBINSsize, histoBINSsize};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES)
#include "eventviewer.h"
#endif

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES)

    #define EOM_EMSRUNNER_EVIEW_MEASURES_START
    #define EOM_EMSRUNNER_EVIEW_MEASURES_STOP
//    #define EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS


    #if defined(EOM_EMSRUNNER_EVIEW_MEASURES_START)

    void meas_rx_start(void){}
    void meas_do_start(void){} 
    void meas_tx_start(void){}
      
    static const evEntityId_t ev_meas_rx_start = ev_ID_first_usrdef+15; 
    static const evEntityId_t ev_meas_do_start = ev_ID_first_usrdef+17;   
    static const evEntityId_t ev_meas_tx_start = ev_ID_first_usrdef+19;         
    static const evEntityId_t ev_meas_start[3] = {ev_meas_rx_start, ev_meas_do_start, ev_meas_tx_start}; 
    
    #endif

    #if defined(EOM_EMSRUNNER_EVIEW_MEASURES_STOP)
    
    void meas_rx_stop(void){}
    void meas_do_stop(void){}    
    void meas_tx_stop(void){}
        
    static const evEntityId_t ev_meas_rx_stop = ev_ID_first_usrdef+16;    
    static const evEntityId_t ev_meas_do_stop = ev_ID_first_usrdef+18;       
    static const evEntityId_t ev_meas_tx_stop = ev_ID_first_usrdef+20;   
    static const evEntityId_t ev_meas_stop[3] = {ev_meas_rx_stop, ev_meas_do_stop, ev_meas_tx_stop};
    
    #endif
    
    #if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)

    void tmr_all_oneshot_start(void){}
    void tmr_rx_oneshotexp(void){}    
    void tmr_do_oneshotexp(void){} 
    void tmr_tx_oneshotexp(void){} 
        
    void tmr_rx_exp(void){} 
    void tmr_do_exp(void){}   
    void tmr_tx_exp(void){}

    static const evEntityId_t ev_tmr_all_oneshot_start = ev_ID_first_usrdef+21; 
    static const evEntityId_t ev_tmr_rx_oneshotexp     = ev_ID_first_usrdef+22; 
    static const evEntityId_t ev_tmr_do_oneshotexp     = ev_ID_first_usrdef+23; 
    static const evEntityId_t ev_tmr_tx_oneshotexp     = ev_ID_first_usrdef+24; 
    static const evEntityId_t ev_tmr_rx_exp            = ev_ID_first_usrdef+25; 
    static const evEntityId_t ev_tmr_do_exp            = ev_ID_first_usrdef+26; 
    static const evEntityId_t ev_tmr_tx_exp            = ev_ID_first_usrdef+27;   

    static const evEntityId_t ev_tmr_oneshotexp[3] = {ev_tmr_rx_oneshotexp, ev_tmr_do_oneshotexp, ev_tmr_tx_oneshotexp};
    static const evEntityId_t ev_tmr_exp[3] = {ev_tmr_rx_exp, ev_tmr_do_exp, ev_tmr_tx_exp};
    
    #endif

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

static void s_eom_emsrunner_tasktiming_on_entry(eOemsrunner_taskid_t taskid, uint64_t tstart);
static void s_eom_emsrunner_tasktiming_on_exit(eOemsrunner_taskid_t taskid, uint64_t tstop);

static void s_eom_emsrunner_periodtiming_on_entry(uint64_t tstart);
static void s_eom_emsrunner_periodtiming_on_exit(uint64_t tstop);

static void s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eOemsrunner_taskid_t taskid);
static void s_eom_emsrunner_periodtiming_eval(void);


static void s_eom_runner_overflow_set(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);
static void s_eom_runner_overflow_clr(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);

static void s_eom_runner_period_overflow_set(EOMtheEMSrunner *p);
static void s_eom_runner_period_overflow_clr(EOMtheEMSrunner *p);

static eOemsrunner_taskid_t s_eom_emsrunner_prev(eOemsrunner_taskid_t n);

static void s_eom_emsrunner_trigger_next(eOemsrunner_taskid_t next);

static void e_eom_emsrunner_update_timebudget(void);



// functions for synchonized mode that need 3 (or 6) HW timers

static void s_eom_emsrunner_6HALTIMERS_start(EOMtheEMSrunner *p);
static void s_eom_emsrunner_6HALTIMERS_stop(void);
static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_execute_task(void *arg);
static void s_eom_emsrunner_6HALTIMERS_execute_task(void *arg);

#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else
static void s_eom_emsrunner_6HALTIMERS_safestop_check_task(void *arg);
static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check(void *arg);
#endif

// functions for best effort mode that requires one HW timer only

static void s_eom_emsrunner_1HWTIMER_start(EOMtheEMSrunner *p);
static void s_eom_emsrunner_1HWTIMER_stop(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOMtheEMSrunner";

 
static EOMtheEMSrunner s_theemsrunner = 
{
    EO_INIT(.cfg)                   {0},
    EO_INIT(.task)                  {NULL, NULL, NULL},
    EO_INIT(.event)                 eo_sm_emsappl_EVdummy,
    EO_INIT(.haltimer_start)        {hal_timer2, hal_timer3, hal_timer4},
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else    
    EO_INIT(.haltimer_safestop)     {hal_timer5, hal_timer6, hal_timer7},
#endif    
    EO_INIT(.numofrxpackets)        0,  
    EO_INIT(.numofrxrops)           0,
    EO_INIT(.numoftxpackets)        0,
    EO_INIT(.numoftxrops)           0,
    EO_INIT(.mode)                  eo_emsrunner_mode_besteffort,
    EO_INIT(.numofpacketsinsidesocket) 0,
    EO_INIT(.waitudptxisdone)       NULL,
    EO_INIT(.usedTXdecimationfactor) 1,
    EO_INIT(.txropsnumberincycle)   {0, 0, 0},
    EO_INIT(.txcan1frames)          0,
    EO_INIT(.txcan2frames)          0,
    EO_INIT(.cycletiming)           
    { 
        EO_INIT(.cycleisrunning) eobool_false, 
        
        EO_INIT(.taskinexecution) eo_emsrunner_taskid_NONE, 
        
        EO_INIT(.taskterminatedaslatest) eo_emsrunner_taskid_NONE, 
        
        EO_INIT(.iterationnumber) 0,
        
        EO_INIT(.tasktiming)
        { 
            {   //  eo_emsrunner_taskid_runRX
                EO_INIT(.timebudget) 0, 
                EO_INIT(.timestarted) 0, 
                EO_INIT(.timestopped) 0, 
                EO_INIT(.duration) {0, 0},
                EO_INIT(.isexecuting) eobool_false, 
                EO_INIT(.isabout2overflow) eobool_false, 
                EO_INIT(.isoverflown) eobool_false
            }, 
            {   //  eo_emsrunner_taskid_runDO
                EO_INIT(.timebudget) 0, 
                EO_INIT(.timestarted) 0, 
                EO_INIT(.timestopped) 0, 
                EO_INIT(.duration) {0, 0},
                EO_INIT(.isexecuting) eobool_false, 
                EO_INIT(.isabout2overflow) eobool_false, 
                EO_INIT(.isoverflown) eobool_false
            }, 
            {   //  eo_emsrunner_taskid_runTX
                EO_INIT(.timebudget) 0, 
                EO_INIT(.timestarted) 0, 
                EO_INIT(.timestopped) 0, 
                EO_INIT(.duration) {0, 0},
                EO_INIT(.isexecuting) eobool_false, 
                EO_INIT(.isabout2overflow) eobool_false, 
                EO_INIT(.isoverflown) eobool_false
            },             
        },
        EO_INIT(.periodtiming)
        {
            EO_INIT(.budget) 0, 
            EO_INIT(.started) 0, 
            EO_INIT(.stopped) 0, 
            EO_INIT(.duration) {0, 0},
            EO_INIT(.isexecuting) eobool_false, 
            EO_INIT(.isabout2overflow) eobool_false, 
            EO_INIT(.isoverflown) eobool_false,
            EO_INIT(.histogram) {}
        }    
    },

    EO_INIT(.isrunning) eobool_false,
    EO_INIT(.logging) {1, 0},
    EO_INIT(.timing) {1000, 0, 400, 700, 100}
    
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
    

    // copy into object the IDs of the hal timers required by the configuration   
    memcpy(&s_theemsrunner.haltimer_start, cfg->haltimerstart, sizeof(s_theemsrunner.haltimer_start));
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else     
    memcpy(&s_theemsrunner.haltimer_safestop, cfg->haltimeralert, sizeof(s_theemsrunner.haltimer_safestop));
#endif    
    // verify that they are all different
    //#warning TODO: in eom_emsrunner_Initialise() you may write code to check that all 6 hal timers are different
    
    s_theemsrunner.cycletiming.cycleisrunning = eobool_false; 
    
    eom_emsrunner_SetMode(&s_theemsrunner, cfg->modeatstartup);
    
    s_theemsrunner.event = eo_sm_emsappl_EVdummy;
       
    s_theemsrunner.numofpacketsinsidesocket = 0;

#if !defined(EMBOBJ_USE_EMBOT)    
    s_theemsrunner.waitudptxisdone = osal_semaphore_new(255, 0);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theemsrunner.waitudptxisdone), "eom_emsrunner_Initialise(): waitudptxisdone is NULL", s_eobj_ownname, NULL);
#else    
    s_theemsrunner.waitudptxisdone = embot::os::rtos::semaphore_new(255, 0);
#endif   
    
    
    s_theemsrunner.cycletiming.taskinexecution = s_theemsrunner.cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_NONE;
    s_theemsrunner.cycletiming.iterationnumber = 0;  

    e_eom_emsrunner_update_timebudget();

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
                                                                  tRX, "runRX");
 
    s_theemsrunner.task[eo_emsrunner_taskid_runDO] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runDO], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runDO], 
                                                                  s_eom_emsrunner_taskDO_startup, s_eom_emsrunner_taskDO_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) | (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tDO, "runDO"); 
                                                                  
    s_theemsrunner.task[eo_emsrunner_taskid_runTX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runTX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runTX], 
                                                                  s_eom_emsrunner_taskTX_startup, s_eom_emsrunner_taskTX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) | (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tTX, "runTX");                                                              
     
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_START)
    eventviewer_load(ev_meas_rx_start, meas_rx_start);
    eventviewer_load(ev_meas_do_start, meas_do_start);  
    eventviewer_load(ev_meas_tx_start, meas_tx_start);
#endif

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_STOP)
    eventviewer_load(ev_meas_rx_stop, meas_rx_stop);
    eventviewer_load(ev_meas_do_stop, meas_do_stop);      
    eventviewer_load(ev_meas_tx_stop, meas_tx_stop);
#endif

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)
    eventviewer_load(ev_tmr_all_oneshot_start, tmr_all_oneshot_start);
    eventviewer_load(ev_tmr_rx_oneshotexp, tmr_rx_oneshotexp);
    eventviewer_load(ev_tmr_do_oneshotexp, tmr_do_oneshotexp);
    eventviewer_load(ev_tmr_tx_oneshotexp, tmr_tx_oneshotexp);
    eventviewer_load(ev_tmr_rx_exp, tmr_rx_exp);
    eventviewer_load(ev_tmr_do_exp, tmr_do_exp);
    eventviewer_load(ev_tmr_tx_exp, tmr_tx_exp);
#endif
    

#if defined(DEBUG_show_when_evt_execute_is_sent)
    eom_emsrunner_scope_init();   
#endif

    s_theemsrunner.isrunning = eobool_false;
    
    s_theemsrunner.logging.flags = (0x0001 << eomn_appl_log_asynchro_exectime_rxdotx_overflow);

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

extern eObool_t eom_emsrunner_IsRunning(EOMtheEMSrunner *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }   

    return p->isrunning;    
}

//extern EOMtask * eom_emsrunner_GetTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t id)
//{
//    if(NULL == p)
//    {
//        return(NULL);
//    }
//    
//    return(s_theemsrunner.task[id]);
//}


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

#if defined(USE_EMBOT_theHandler)

#include "embot_hw_bsp_config.h"
#include "theApplication_Config.h"

#if defined(EMBOT_ENABLE_hw_timer_emulated)
        
    // 10: period (1ms) -> 10 ms, dostart (0.4ms) -> 4ms, safetx (0.25ms) -> 2.5 ms
    // 100: period (1ms) -> 100 ms, dostart (0.4ms) -> 40ms, safetx (0.25ms) -> 25 ms
constexpr uint32_t cs {(embot::app::eth::theApplication_Config.OStick == 500*embot::core::time1microsec) ? 10 : 100};

#else
    constexpr uint32_t cs {1};
#endif
    
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
    
    p->timing = *timing;
    
    p->cfg.execRXafter = cs*timing->rxstartafter;
    p->cfg.safeRXexecutiontime = cs*timing->dostartafter - cs*timing->rxstartafter - cs*timing->safetygap;
    p->cfg.execDOafter = cs*timing->dostartafter;
    p->cfg.safeDOexecutiontime = cs*timing->txstartafter - cs*timing->dostartafter - cs*timing->safetygap;
    p->cfg.execTXafter = cs*timing->txstartafter;
    p->cfg.safeTXexecutiontime = cs*timing->period - cs*timing->txstartafter + cs*timing->rxstartafter - cs*timing->safetygap;
    p->cfg.period = cs*timing->period;
    
    e_eom_emsrunner_update_timebudget();
    
    return(eores_OK);
}

#else

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
    
    p->timing = *timing;

    p->cfg.execRXafter = timing->rxstartafter;
    p->cfg.safeRXexecutiontime = timing->dostartafter - timing->rxstartafter - timing->safetygap;
    p->cfg.execDOafter = timing->dostartafter;
    p->cfg.safeDOexecutiontime = timing->txstartafter - timing->dostartafter - timing->safetygap;
    p->cfg.execTXafter = timing->txstartafter;
    p->cfg.safeTXexecutiontime = timing->period - timing->txstartafter + timing->rxstartafter - timing->safetygap;
    p->cfg.period = timing->period;
    
    e_eom_emsrunner_update_timebudget();
    
    return(eores_OK);
}

#endif

extern eOresult_t eom_emsrunner_SetReport(EOMtheEMSrunner *p, eOmn_appl_config_logging_t *logging)
{
    if((NULL == p) || (NULL == logging))
    {
        return(eores_NOK_nullpointer);
    } 
    

    s_theemsrunner.logging = *logging;
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
   
    p->numofpacketsinsidesocket = 0;      
 
#if !defined(EMBOBJ_USE_EMBOT)     
    osal_semaphore_set(s_theemsrunner.waitudptxisdone, 0);    
#else
    // already at zero, and embot::os::rtos does not have a semaphore_set(cur)
#endif    
    // set the ems-transceiver to have a given tx-rate.
    EOtransmitter * transmitter = eo_transceiver_GetTransmitter(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()));    
    // replies and occasionals are not decimated. only regulars are decimated. in this way we have prompt responses to ask<> rops and we can send up occasionals asap.
    eo_transmitter_TXdecimation_Set(transmitter, 1, s_theemsrunner.usedTXdecimationfactor, 1);

    bool ok {false};
    
    switch(p->mode)
    {
        case eo_emsrunner_mode_synchronized:
        {
            // now i start the timers for rx, do, tx and also for guards ...    
            s_eom_emsrunner_6HALTIMERS_start(p);
            ok = true;
        } break;
        
        case eo_emsrunner_mode_besteffort:
        {
            // i start one timer only for the period
            s_eom_emsrunner_1HWTIMER_start(p);
            ok = true;
        } break;
        
        default:
        {
            ok = false;
        } break;
    }
    
    p->isrunning = ok;
    
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
            
    switch(p->mode)
    {
        case eo_emsrunner_mode_synchronized:
        {
            s_eom_emsrunner_6HALTIMERS_stop();
        } break;
        
        case eo_emsrunner_mode_besteffort:
        {
            s_eom_emsrunner_1HWTIMER_stop();
        } break;
        
        default:
        {   // surely this one stops them all (if ever activated)
            s_eom_emsrunner_6HALTIMERS_stop();
        } break;
    }    
    
    p->isrunning = eobool_false; 
    
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
    
    if(eobool_true == s_theemsrunner.cycletiming.cycleisrunning)
    {
        return(eores_NOK_generic);        
    }
    
    s_theemsrunner.mode = mode;
    
    return(eores_OK);
}


extern void eom_emsrunner_OnUDPpacketTransmitted(EOMtheEMSrunner *p)
{
#if !defined(EMBOBJ_USE_EMBOT)    
    osal_semaphore_increment(s_theemsrunner.waitudptxisdone, osal_callerTSK);
#else    
    embot::os::rtos::semaphore_release(s_theemsrunner.waitudptxisdone);
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


void tRX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start((EOMtask*)p);
} 


void tDO(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start((EOMtask*)p);
} 


void tTX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start((EOMtask*)p);
} 



#if defined(USE_EMBOT_theHandler)
#else

EO_weak extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
{

}


EO_weak extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runRX))
    {
        itissafetoquit_asap =  eobool_true;
    } 
    
    itissafetoquit_asap = itissafetoquit_asap;

    // if itissafetoquit_asap then ... do nothing or very little.
}


EO_weak extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runRX))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;   
    
    // if itissafetoquit_asap then ... do nothing or very little.    
}


EO_weak extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runDO))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;    

    // if itissafetoquit_asap then ... do nothing or very little.
}



EO_weak extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runTX))
    {
        itissafetoquit_asap =  eobool_true;
    }   

    itissafetoquit_asap = itissafetoquit_asap;
    
    // if itissafetoquit_asap then ... do nothing or very little.       
}




EO_weak extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    eObool_t itissafetoquit_asap = eobool_false;
    
    if(eobool_false == eom_runner_cansafelyexecute(p, eo_emsrunner_taskid_runTX))
    {
        itissafetoquit_asap =  eobool_true;
    } 

    itissafetoquit_asap = itissafetoquit_asap;
    
    // if itissafetoquit_asap then ... do nothing or very little.       
}



EO_weak extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    char str[64];
    eOerrmanErrorType_t errortype = eo_errortype_warning;
    snprintf(str, sizeof(str), "failed tx of a packet"); 
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eo_errman_code_sys_ctrloop_udptxfailure;
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;      
    eo_errman_Error(eo_errman_GetHandle(), errortype, str, s_eobj_ownname, &errdes); 
}

#endif


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


static void s_eom_runner_period_overflow_set(EOMtheEMSrunner *p)
{
    p->cycletiming.periodtiming.isoverflown = true;
}


static void s_eom_runner_period_overflow_clr(EOMtheEMSrunner *p)
{
    p->cycletiming.periodtiming.isoverflown = false;
}

static bool s_eom_runner_period_overflow_check(EOMtheEMSrunner *p)
{
    return p->cycletiming.periodtiming.isoverflown;
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
    eo_errman_Trace(eo_errman_GetHandle(), "called _taskRX_startup()", s_eobj_ownname);
}


static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t)
{
    uint64_t tstart = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_runRX;
    
    s_theemsrunner.cycletiming.iterationnumber ++;
    
    s_eom_emsrunner_tasktiming_on_entry(eo_emsrunner_taskid_runRX, tstart);
    
    s_eom_emsrunner_periodtiming_on_entry(tstart);
                 
    // perform the rx activity    
    if(eobool_true == eom_runner_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runRX))
    {
#if defined(DEBUG_force_time_overflow)
        debug_delay_task(eo_emsrunner_taskid_runRX, s_theemsrunner.cycletiming.iterationnumber);
#endif        
        s_eom_emsrunner_userdef_taskRX_activity(&s_theemsrunner);
    }
          

    // Y. if someone has stopped the cycle...
    if(eobool_false == s_theemsrunner.cycletiming.cycleisrunning)
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]);  
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else         
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
#endif        
    }
    
   
    uint64_t tstop = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    s_eom_emsrunner_tasktiming_on_exit(eo_emsrunner_taskid_runRX, tstop);
    
    s_theemsrunner.cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_runRX;
    s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_NONE;

    // at the end trigger the next
    s_eom_emsrunner_trigger_next(eo_emsrunner_taskid_runDO);
    
//#if defined(BESTEFFORT_ACTIVATE_RX_ONLY)
//    if(eo_emsrunner_mode_besteffort == s_theemsrunner.mode) 
//    {        
//        // activate DO
//        eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runDO], eo_emsrunner_evt_execute);
//    }
//#endif        
//    
//    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
//    eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runDO], eo_emsrunner_evt_enable);
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
        eOresult_t resrx {eores_NOK_generic};
        
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
    eo_errman_Trace(eo_errman_GetHandle(), "called _taskDO_startup()", s_eobj_ownname);
}



static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t)
{
    uint64_t tstart = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_runDO;
    
    s_eom_emsrunner_tasktiming_on_entry(eo_emsrunner_taskid_runDO, tstart);
    
    
    if(eobool_true == eom_runner_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runDO))
    {
#if defined(DEBUG_force_time_overflow)
        debug_delay_task(eo_emsrunner_taskid_runDO, s_theemsrunner.cycletiming.iterationnumber);
#endif            
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
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else         
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]))
        {        
            hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
        }
#endif
        
        // then .. stop do timers
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]);    
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else         
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]);
#endif        
    }

    uint64_t tstop = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    s_eom_emsrunner_tasktiming_on_exit(eo_emsrunner_taskid_runDO, tstop);
       
	s_theemsrunner.cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_runDO;	
    s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_NONE;

    // at the end trigger the next
    s_eom_emsrunner_trigger_next(eo_emsrunner_taskid_runTX);    
    
//#if defined(BESTEFFORT_ACTIVATE_RX_ONLY)
//    if(eo_emsrunner_mode_besteffort == s_theemsrunner.mode) 
//    {
//        // activate TX
//        eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runTX], eo_emsrunner_evt_execute);
//    }
//#endif      
//    
//    // Z. at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
//    eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runTX], eo_emsrunner_evt_enable);
}


static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t)
{
    // nothing to do
    eo_errman_Trace(eo_errman_GetHandle(), "called _taskTX_startup()", s_eobj_ownname);
}


static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t)
{
    uint64_t tstart = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_runTX;
    
    s_eom_emsrunner_tasktiming_on_entry(eo_emsrunner_taskid_runTX, tstart);
    
        
    // clear the number of udp packets and rops that they must be transmitted.
    // it will be responsibility of s_eom_emsrunner_userdef_taskTX_activity() to fill their number
    // in case any udp packet is transmitted.
    // note: numofpacketsinsidesocket is the important value. numoftxpackets is redundant. numoftxrops may be useful
    
    s_theemsrunner.numofpacketsinsidesocket = 0;
    s_theemsrunner.numoftxpackets = 0;
    s_theemsrunner.numoftxrops = 0;

#if defined(DEBUG_force_time_overflow)
    debug_delay_task(eo_emsrunner_taskid_runTX, s_theemsrunner.cycletiming.iterationnumber);
#endif 
    
    s_eom_emsrunner_userdef_taskTX_activity(&s_theemsrunner);
    
    
    for(;;)
    {
        // after the function s_eom_emsrunner_userdef_taskTX_activity(), the variable numofpacketsinsidesocket is updated if any UDP packet is sent away
        // the function eom_runner_cansafelyexecute() always returns true
        // THUS: we activate the semaphore only if there are packets that are being transmitted
        if((0 == s_theemsrunner.numofpacketsinsidesocket) || (eobool_false == eom_runner_cansafelyexecute(&s_theemsrunner, eo_emsrunner_taskid_runTX)))
        {
            break;
        }
       
        // the semaphore starts w/ an initial count = 0, so when we acquire it (or decrement) we block until some other thread
        // increases the count. that is done by the thread IPnet that calls eom_emsrunner_OnUDPpacketTransmitted() when it gives 
        // the packet to the IPAL, which in turns directly writes into ETH peripheral. 
        // SO: in here we wait that the UDP frame has gone to the ETH peripheral
#if !defined(EMBOBJ_USE_EMBOT)     
        osal_semaphore_decrement(s_theemsrunner.waitudptxisdone, osal_reltimeINFINITE);
#else
        embot::os::rtos::semaphore_acquire(s_theemsrunner.waitudptxisdone, embot::core::reltimeWaitForever);
#endif 
        // when one UDP frame has gone we decrement numofpacketsinsidesocket. when it becomes zero the break exits te loop         
        s_theemsrunner.numofpacketsinsidesocket--; 
    }

   
    uint64_t tstop = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    s_eom_emsrunner_tasktiming_on_exit(eo_emsrunner_taskid_runTX, tstop);
    s_eom_emsrunner_periodtiming_on_exit(tstop);
    
    // we check if we have received order to exit the rx-do-tx cycle
    if(eobool_false == s_theemsrunner.cycletiming.cycleisrunning)
    {  

        // better to stop also the rx and do timers if they are not stopped yet. that may happen if something inside task-do/tx has asked for a stop of running
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]))
        {
            hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]); 
        } 
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]))
        {
            hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]); 
        }   
        // and now surely the TX timer
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]);
        
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else         
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]))
        {        
            hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
        }        
        if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]))
        {        
            hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]);
        }        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runTX]);
#endif
        
        s_theemsrunner.cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_NONE;
        s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_NONE;
        
        
        // and now ... finally we sent the state machine into the new state
        // reset the event to the dummy value.
        eOsmEventsEMSappl_t ev = s_theemsrunner.event;
        s_theemsrunner.event = eo_sm_emsappl_EVdummy;
        // we process the event. it can be either eo_sm_emsappl_EVgo2err or eo_sm_emsappl_EVgo2cfg
#if defined(USE_EMBOT_theHandler)
        embot::app::eth::theHandler::Command cmd = embot::app::eth::legacy::thehandler::tocommand(static_cast<embot::app::eth::legacy::thehandler::eoApplEVENT>(ev));
        embot::app::eth::theHandler::State st = embot::app::eth::theHandler::tostate(cmd);
        embot::app::eth::theHandler::getInstance().moveto(st);
#else        
        eom_emsappl_SM_ProcessEvent(eom_emsappl_GetHandle(), ev); 
#endif        
    }
    else
    {
        s_theemsrunner.cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_runTX;
        s_theemsrunner.cycletiming.taskinexecution = eo_emsrunner_taskid_NONE;     
        // at the end trigger the next
        s_eom_emsrunner_trigger_next(eo_emsrunner_taskid_runRX);        
//        eom_task_SetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_enable);
    }
}


static void s_eom_emsrunner_userdef_taskTX_activity(EOMtheEMSrunner *p)
{    
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
    if(0 == p->cfg.maxnumofTXpackets)
    {
        processtransmission = eobool_false;
    } 
    else
    {
        processtransmission = eobool_true;
    }
    
    // marco.accame on 18jun25: we always allow transmission of a UDP frame as long it has any ROP inside, even if we have usedTXdecimationfactor > 1
    // because it is the object EOtransmitter that adds or not the regular ROP depending on the value of usedTXdecimationfactor
    // but it may be that there are spontaneous or reply ROPs 
        
        
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
                eOresult_t restx = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt, 5*EOK_reltime1sec);
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



static void s_eom_emsrunner_tasktiming_on_entry(eOemsrunner_taskid_t taskid, uint64_t tstart)
{
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_START)
    evEntityId_t prev = eventviewer_switch_to(ev_meas_start[taskid]);      
    eventviewer_switch_to(prev); 
#endif
    
#if defined(USE_TIMINGMEASURE_effective)
    s_theemsrunner.cycletiming.tasktiming[taskid].timestarted = tstart;
#else        
#endif 
    
    s_theemsrunner.cycletiming.tasktiming[taskid].isexecuting = eobool_true;   
}

#if defined(theRunner_USE_compensationofdurations)
static void send_debug_message(uint16_t par16, uint64_t par64)
{
    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;
    errdes.par16            = par16;
    errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &errdes);

}
#endif

static void s_eom_emsrunner_tasktiming_on_exit(eOemsrunner_taskid_t taskid, uint64_t tstop)
{
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_STOP)
    evEntityId_t prev = eventviewer_switch_to(ev_meas_stop[taskid]);      
    eventviewer_switch_to(prev); 
#endif
    
    s_theemsrunner.cycletiming.tasktiming[taskid].timestopped = tstop;
    s_theemsrunner.cycletiming.tasktiming[taskid].duration[1] = s_theemsrunner.cycletiming.tasktiming[taskid].duration[0]; 
#if defined(USE_TIMINGMEASURE_effective)  
    int64_t delta = s_theemsrunner.cycletiming.tasktiming[taskid].timestopped - s_theemsrunner.cycletiming.tasktiming[taskid].timestarted;
#if defined(theRunner_USE_compensationofdurations)
    if(delta < 0)
    {
        // emit msg
        send_debug_message(taskid, delta);
        delta = 1;
    }
    else if(delta > 1000)
    {
        delta -= 1000;    
    } 
#endif // #if defined(theRunner_USE_compensationofdurations)           
    s_theemsrunner.cycletiming.tasktiming[taskid].duration[0] = delta;
#else
    int64_t delta = s_theemsrunner.cycletiming.tasktiming[taskid].timestopped - s_theemsrunner.cycletiming.tasktiming[taskid].timestarted;
#if defined(theRunner_USE_compensationofdurations)
    if(delta < 0)
    {
        // emit msg
        send_debug_message(taskid, delta);
        delta = 1;
    }
    else if(delta > 1000)
    {
        delta -= 1000;    
    }     
#endif // #if defined(theRunner_USE_compensationofdurations)           
    s_theemsrunner.cycletiming.tasktiming[taskid].duration[0] = delta;
#endif    
    s_theemsrunner.cycletiming.tasktiming[taskid].isexecuting = eobool_false;   

#if defined(USE_OVERFLOWCHECK_effectivetime)
        
    if(s_theemsrunner.cycletiming.tasktiming[taskid].duration[0] >= s_theemsrunner.cycletiming.tasktiming[taskid].timebudget)
    {
        // damn ... this thread has finished beyond budget i must mark an overflow.
        s_eom_runner_overflow_set(&s_theemsrunner, taskid);
    } 
    
#else    
#endif  


    // we eval the timings of the previous rx phase
    s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(taskid);  
}


static void s_eom_emsrunner_periodtiming_on_entry(uint64_t tstart)
{
    s_theemsrunner.cycletiming.periodtiming.started = tstart; 
}

static void s_eom_emsrunner_periodtiming_on_exit(uint64_t tstop)
{
    
    s_theemsrunner.cycletiming.periodtiming.stopped = tstop;
    
    s_theemsrunner.cycletiming.periodtiming.duration[1] = s_theemsrunner.cycletiming.periodtiming.duration[0]; 
#if defined(USE_TIMINGMEASURE_effective)  
    int64_t delta = s_theemsrunner.cycletiming.periodtiming.stopped - s_theemsrunner.cycletiming.periodtiming.started;
#if defined(theRunner_USE_compensationofdurations)
    if(delta < 0)
    {
        // emit msg
        send_debug_message(taskid, delta);
        delta = 1;
    }
    else if(delta > 1000)
    {
        delta -= 1000;    
    } 
#endif // #if defined(theRunner_USE_compensationofdurations)           
    s_theemsrunner.cycletiming.periodtiming.duration[0] = delta;
#else
    int64_t delta = s_theemsrunner.cycletiming.periodtiming.timestopped - s_theemsrunner.cycletiming.periodtiming.timestarted;
#if defined(theRunner_USE_compensationofdurations)
    if(delta < 0)
    {
        // emit msg
        send_debug_message(taskid, delta);
        delta = 1;
    }
    else if(delta > 1000)
    {
        delta -= 1000;    
    }     
#endif // #if defined(theRunner_USE_compensationofdurations)           
    s_theemsrunner.cycletiming.periodtiming.duration[0] = delta;
#endif    
    s_theemsrunner.cycletiming.periodtiming.isexecuting = false;   

#if defined(USE_OVERFLOWCHECK_effectivetime)
        
    if(s_theemsrunner.cycletiming.periodtiming.duration[0] >= s_theemsrunner.cycletiming.periodtiming.budget)
    {
        // damn ... this period has finished beyond budget i must mark an overflow.
        s_eom_runner_period_overflow_set(&s_theemsrunner);
    } 
    
#else    
#endif  
    
    s_eom_emsrunner_periodtiming_eval();

}

static void s_eom_emsrunner_periodtiming_eval(void)
{

    // --- stats: min, avg, max 
    
    constexpr uint16_t mask_logstatisticsPERIOD = 0x0001 << eomn_appl_log_periodic_exectime_period_minavgmax;
    bool logSTATsPERIOD = (0 != s_theemsrunner.logging.period10ms) && ((s_theemsrunner.logging.flags & mask_logstatisticsPERIOD) == mask_logstatisticsPERIOD);
    
    constexpr uint16_t mask_loghistogramPERIOD = 0x0001 << eomn_appl_log_periodic_exectime_period_histogram;
    bool logHistoPERIOD = (0 != s_theemsrunner.logging.period10ms) && ((s_theemsrunner.logging.flags & mask_loghistogramPERIOD) == mask_loghistogramPERIOD);    

    constexpr uint16_t mask_logoverflowsPERIOD = 0x0001 << eomn_appl_log_asynchro_exectime_period_overflow;
    bool logOVERFLOWsPERIOD = ((s_theemsrunner.logging.flags & mask_logoverflowsPERIOD) == mask_logoverflowsPERIOD);
    
    uint16_t currduration = s_theemsrunner.cycletiming.periodtiming.duration[0];

    // stat period
    if(true == logSTATsPERIOD)
    {      
        uint32_t offset2use = 0; // if > 0 the emission is shifted so that not every periodic emission is sent in the same cycle

        enum { stat_cnt = 0, stat_min = 1, stat_max = 2, stat_cum = 3, stats_numberof = 4 }; 
        static uint64_t periodstats[stats_numberof] = {0};        
        
        if(periodstats[stat_cnt] == 0)
        {
            periodstats[stat_max] = periodstats[stat_cum] = 0;
            periodstats[stat_min] = 1000000;  
            periodstats[stat_cnt] = offset2use;            
        }
        
        periodstats[stat_cum] += currduration;
        periodstats[stat_cnt]++;

        if(currduration < periodstats[stat_min])
        {
            periodstats[stat_min] = currduration;
        }
        
        if(currduration > periodstats[stat_max])
        {
            periodstats[stat_max] = currduration;
        }
        
        // and now we may emit
        
        uint16_t periodreport = 10*s_theemsrunner.logging.period10ms; // logging.period is in 10*ms. we want ms
                
        if(periodstats[stat_cnt] >= (periodreport+offset2use))
        {
            uint64_t divider = periodstats[stat_cnt]-offset2use;
            uint64_t average = periodstats[stat_cum] / divider;
            
            uint64_t par64 = ((periodstats[stat_min] & 0xffff) << 48) | ((average & 0xffff) << 32) | ((periodstats[stat_max] & 0xffff) << 16) | ((periodreport/10) & 0xffff); 
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = 0;
            eOerrmanErrorType_t errortype = eo_errortype_warning;
            errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_exec_time_stats);
            errdes.par16            = 3;
            errdes.par64            = par64; 
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);                     
            
            // as last
            periodstats[stat_cnt] = 0;
        }          
        
    }           
    
    // histo period

    if(true == logHistoPERIOD)
    {
        
        s_theemsrunner.cycletiming.periodtiming.histogram.add(currduration);
        
        // and now we check if we may emit
        
        uint32_t periodreport = 10*s_theemsrunner.logging.period10ms; // logging.period is in 10*ms. we want ms
        
       
        if(s_theemsrunner.cycletiming.periodtiming.histogram.getvalues()->total >= periodreport)
        {
            std::vector<uint32_t> v {};
            s_theemsrunner.cycletiming.periodtiming.histogram.probabilitydensityfunction(v, 255.0);
//            for(size_t i=0; i<v.size(); i++)
//            {
//                embot::core::print("v[" + std::to_string(i) + "] = " + std::to_string(v[i]));
//            }
            s_theemsrunner.cycletiming.periodtiming.histogram.reset();      

            uint64_t p64 {0};
            // v[0] holds whats below the bins, v[1] until v[size-2] holds the bins, v[size-1] holds what beyond
            // and size is ... 8+2
            // there is nothing lower because we start from 0, so we just 
            for(uint8_t i=0; i<=histoBINSnumber; i++)
            {
                uint64_t x = v[i+1];
                p64 |= (x<<(8*i));
                // embot::core::print("x " + std::to_string(i) + " = " + std::to_string(x));
            }
          
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = 0;
            eOerrmanErrorType_t errortype = eo_errortype_warning;
            errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_ctrloop_histogramPERIOD);
            errdes.par16            = histoBINSsize;
            errdes.par64            = p64; 
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);                  
        }           
                
    }  
    
    // --- overflow period
    
    if((true == logOVERFLOWsPERIOD) && (true == s_eom_runner_period_overflow_check(&s_theemsrunner)))
    {
        s_eom_runner_period_overflow_clr(&s_theemsrunner);
        
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        eOerrmanErrorType_t errortype = eo_errortype_warning;
        
        uint64_t times {0};
        uint16_t budget = s_theemsrunner.cycletiming.periodtiming.budget & 0xffff;
        uint64_t currduration = s_theemsrunner.cycletiming.periodtiming.duration[0];
        uint64_t rxtime {s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runRX].duration[0]};
        uint64_t dotime {s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runDO].duration[0]};
        uint64_t txtime {s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runTX].duration[0]};        
        uint64_t others = ((currduration & 0xffff) << 48) | ((rxtime & 0xffff) << 32) | ((dotime & 0xffff) << 16) | (txtime & 0xffff);
                   
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_ctrloop_execoverflowPERIOD);
        errdes.par16            = budget;
        errdes.par64            = others;
        eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
    }  
}



static void s_eom_emsrunner_update_diagnosticsinfo_check_overflows2(eOemsrunner_taskid_t taskid)
{    
    static const eOerror_value_SYS_t errorvalue[3] = { eoerror_value_SYS_ctrloop_execoverflowRX, eoerror_value_SYS_ctrloop_execoverflowDO, eoerror_value_SYS_ctrloop_execoverflowTX };
    static uint64_t last4durations = 0;
    
    static bool first[3] =  {true, true, true};
    
    uint64_t currduration = s_theemsrunner.cycletiming.tasktiming[taskid].duration[0];
    

    uint32_t periodreport = 0;
    constexpr uint32_t offsetreport[3] = {0, 0, 0}; //{0, 1000, 2000};
    uint32_t offset2use[] = {0, 0, 0};
    

    enum { stat_cnt = 0, stat_min = 1, stat_max = 2, stat_cum = 3, stats_numberof = 4}; 
    static uint64_t stats[eo_emsrunner_task_numberof][stats_numberof] = {0};
    
    constexpr uint16_t mask_logoverflowsRXDOTX = 0x0001 << eomn_appl_log_asynchro_exectime_rxdotx_overflow;
    bool logOVERFLOWsRXDOTX = ((s_theemsrunner.logging.flags & mask_logoverflowsRXDOTX) == mask_logoverflowsRXDOTX);
    
    constexpr uint16_t mask_logstatisticsRXDOTX = 0x0001 << eomn_appl_log_periodic_exectime_rxdotx_minavgmax;
    bool logSTATsRXDOTX = (0 != s_theemsrunner.logging.period10ms) && ((s_theemsrunner.logging.flags & mask_logstatisticsRXDOTX) == mask_logstatisticsRXDOTX);

    if(true == logSTATsRXDOTX)
    {
        periodreport = 10*s_theemsrunner.logging.period10ms; // logging.period is in 10*ms. we want ms    
        
        if(stats[taskid][stat_cnt] == 0)
        {
            offset2use[taskid] = (true == first[taskid]) ? offsetreport[taskid] : 0;
            first[taskid] = false;
            
            stats[taskid][stat_max] = stats[taskid][stat_cum] = 0;
            stats[taskid][stat_min] = 1000000;  
            stats[taskid][stat_cnt] = offset2use[taskid];            
        }
        
        stats[taskid][stat_cum] += currduration;
        stats[taskid][stat_cnt]++;

        if(currduration < stats[taskid][stat_min])
        {
            stats[taskid][stat_min] = currduration;
        }
        
        if(currduration > stats[taskid][stat_max])
        {
            stats[taskid][stat_max] = currduration;
        } 
    }        
    
    
	eOerrmanDescriptor_t errdes = {0};
	errdes.sourcedevice = eo_errman_sourcedevice_localboard;
  	errdes.sourceaddress    = 0;
	eOerrmanErrorType_t errortype = eo_errortype_warning;
    
    
	switch(taskid)
	{
		case eo_emsrunner_taskid_runRX:
        {
                        
			if((true == logOVERFLOWsRXDOTX) && (eobool_true == s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown))
			{
				errdes.code             = eoerror_code_get(eoerror_category_System, errorvalue[taskid]);
				errdes.par16            = currduration & 0xffff;
                errdes.par64            = last4durations; 
				eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
			}
            
            if(true == logSTATsRXDOTX)
            {
                if(stats[taskid][stat_cnt] >= (periodreport+offset2use[taskid]))
                {
                    uint64_t divider = stats[taskid][stat_cnt] - offset2use[taskid];
                    uint64_t average = stats[taskid][stat_cum] / divider;
                    
                    uint64_t par64 = ((stats[taskid][stat_min] & 0xffff) << 48) | ((average & 0xffff) << 32) | ((stats[taskid][stat_max] & 0xffff) << 16) | ((periodreport/10) & 0xffff); 
                    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_exec_time_stats);
                    errdes.par16            = taskid;
                    errdes.par64            = par64; 
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);                     
                    
                    // as last
                    stats[taskid][stat_cnt] = 0;
                }                               
            }    
        } break;
            
		case eo_emsrunner_taskid_runDO:
        {
                        
			if((true == logOVERFLOWsRXDOTX) && (eobool_true == s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown))
			{
                static int dont_stress = 0;
#if defined(USE_EMBOT_theHandler)
                static const int stresstolerance  = 0;
#else
                static const int stresstolerance  = 5000;
#endif                
                if (++dont_stress > stresstolerance)
                {
                    dont_stress = 0;
                    
                    errdes.code             = eoerror_code_get(eoerror_category_System, errorvalue[taskid]);
                    errdes.par16            = currduration & 0xffff;
                    errdes.par64            = last4durations; 
                    eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
                }
			}
            
            if(true == logSTATsRXDOTX)
            {
                if(stats[taskid][stat_cnt] >= (periodreport+offset2use[taskid]))
                {
                    uint64_t divider = stats[taskid][stat_cnt] - offset2use[taskid];
                    uint64_t average = stats[taskid][stat_cum] / divider;
                    
                    uint64_t par64 = ((stats[taskid][stat_min] & 0xffff) << 48) | ((average & 0xffff) << 32) | ((stats[taskid][stat_max] & 0xffff) << 16) | ((periodreport/10) & 0xffff); 
                    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_exec_time_stats);
                    errdes.par16            = taskid;
                    errdes.par64            = par64; 
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);  

                    // as last
                    stats[taskid][stat_cnt] = 0;                    
                }                               
            }            

        } break;
            
		case eo_emsrunner_taskid_runTX:
        {
                        
			if((true == logOVERFLOWsRXDOTX) && (eobool_true == s_theemsrunner.cycletiming.tasktiming[taskid].isoverflown))
			{
				errdes.code             = eoerror_code_get(eoerror_category_System, errorvalue[taskid]);
				errdes.par16            = currduration & 0xffff;
                uint64_t canframes = s_theemsrunner.txcan1frames | (s_theemsrunner.txcan2frames << 8);
                errdes.par64            = (last4durations & 0x0000ffffffffffff) | (canframes << 48); 
				eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &errdes);
			}
            
            if(true == logSTATsRXDOTX)
            {
                if(stats[taskid][stat_cnt] >= (periodreport+offset2use[taskid]))
                {
                    uint64_t divider = stats[taskid][stat_cnt] - offset2use[taskid];
                    uint64_t average = stats[taskid][stat_cum] / divider;   

                    uint64_t par64 = ((stats[taskid][stat_min] & 0xffff) << 48) | ((average & 0xffff) << 32) | ((stats[taskid][stat_max] & 0xffff) << 16) | ((periodreport/10) & 0xffff); 
                    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_exec_time_stats);
                    errdes.par16            = taskid;
                    errdes.par64            = par64; 
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);  
                    
                    // as last
                    stats[taskid][stat_cnt] = 0;
                }                               
            }
          
        } break;
        
        default: {} break;
	}
    
    // must clear
    s_eom_runner_overflow_clr(&s_theemsrunner, taskid);
    
    last4durations = last4durations << 16;
    last4durations = last4durations & 0xffffffffffff0000;
    last4durations = last4durations | (currduration & 0xffff);    
}


// marco.accame on 18jun25: this function was originally introduced to allow early termination of a processing
// in case the time budget has expired. hoever, w/ the modes that we have had so (eo_emsrunner_mode_synchronized 
// and eo_emsrunner_mode_besteffort) it has never returned false. and i think that we shall never implement a
// eo_emsrunner_mode_hardrealtime that will force weraly termination of some processing.
// SO: i will comment out its body and force it to return always true 
extern eObool_t eom_runner_cansafelyexecute(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
#if 1
    return eobool_true;
#else    
    eObool_t ret = eobool_false;

    switch(p->mode)
    {
        case eo_emsrunner_mode_synchronized:
        case eo_emsrunner_mode_besteffort:
        {
            ret = eobool_true;
        } break;
        
        default:
        {
            ret = eobool_true;
        } break;
        
    }   
    
    return(ret);    
#endif    
}

static eOemsrunner_taskid_t s_eom_emsrunner_prev(eOemsrunner_taskid_t n)
{
    return (eo_emsrunner_taskid_runRX == n) ? (eo_emsrunner_taskid_runTX) : ((eOemsrunner_taskid_t)((uint8_t)n-1));
}

static void s_eom_emsrunner_trigger_next(eOemsrunner_taskid_t next)
{
#if defined(BESTEFFORT_ACTIVATE_RX_ONLY)
    if((eo_emsrunner_mode_besteffort == s_theemsrunner.mode) && (eo_emsrunner_taskid_runRX != next)) 
    {        
        // activate next
        eom_task_SetEvent(s_theemsrunner.task[next], eo_emsrunner_evt_execute);
    }
#endif        
    
    // enable next in the chain by sending to it a eo_emsrunner_evt_enable
    eom_task_SetEvent(s_theemsrunner.task[next], eo_emsrunner_evt_enable);    
}

static void e_eom_emsrunner_update_timebudget(void)
{
    s_theemsrunner.cycletiming.periodtiming.budget = s_theemsrunner.cfg.period - s_theemsrunner.timing.safetygap;
    s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runRX].timebudget = s_theemsrunner.cfg.execDOafter;
    s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runDO].timebudget = s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.execDOafter;
    s_theemsrunner.cycletiming.tasktiming[eo_emsrunner_taskid_runTX].timebudget = s_theemsrunner.cfg.period - s_theemsrunner.cfg.execTXafter;    
}


// ------------------------------------------------------------------------------------------------------------------------------------
// part in which we manages the 3 (or 6) HW timers for the synhcronized mode, with all its variants
// ------------------------------------------------------------------------------------------------------------------------------------
// start


static void s_eom_emsrunner_6HALTIMERS_start(EOMtheEMSrunner *p)
{    
    // now i start the timers for rx, do, tx and also for guards ...
    // i start the hal timers in one-shot mode with countdown = 1 period + the shift for {rx, do, tx, guards}.
    // at their expiry i restart each timer to be periodic with countdown = 1 period. 
    // the periodic expiries will send the relevant eo_emsrunner_evt_execute to {rx, do, tx} and will set flags for the guards 
 
    // reset all flags for period
    p->cycletiming.periodtiming.reset();
    p->cycletiming.periodtiming.histogram.init(histoCFG);
    p->cycletiming.periodtiming.histogram.reset();
    
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
        
        hal_timer_cfg_t oneshot_cfg = 
        {
            EO_INIT(.prescaler)         hal_timer_prescalerAUTO,         
            EO_INIT(.countdown)         0,
            EO_INIT(.priority)          hal_int_priority02,
            EO_INIT(.mode)              hal_timer_mode_oneshot,
            EO_INIT(.callback_on_exp)   s_eom_emsrunner_6HALTIMERS_start_periodic_timer_execute_task,
            EO_INIT(.arg)               NULL
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
        

#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else                 
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
#endif

    }
   

    // we also need to enable the first task: taskRX. the other tasks will be enabled at the end of the previous tasks
    
    eom_task_SetEvent(p->task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_enable);
    
    // we impose that the cycle is running    
    p->cycletiming.cycleisrunning = eobool_true;
    p->cycletiming.taskinexecution = p->cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_NONE;
    p->cycletiming.iterationnumber = 0;
    
    memset(&s_theemsrunner.cycletiming.tasktiming, 0, sizeof(s_theemsrunner.cycletiming.tasktiming));
    // but also reassign the time budgets inside tasktiming[]
    e_eom_emsrunner_update_timebudget();

    // finally: we activate the hal timers. 

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)
    evEntityId_t prev = eventviewer_switch_to(ev_tmr_all_oneshot_start );      
    eventviewer_switch_to(prev); 
#endif
    
    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runRX]);
    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runDO]);
    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runTX]);    

#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else  
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
#endif

}

static void s_eom_emsrunner_6HALTIMERS_stop(void)
{
    // rx timer
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]); 
    } 
    // do timer
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runDO]); 
    }     
    // tx timer
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runTX]); 
    }
    
#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else      
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]))
    {        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runRX]);
    }
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]))
    {        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runDO]);
    }    
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runTX])) 
    {        
        hal_timer_stop(s_theemsrunner.haltimer_safestop[eo_emsrunner_taskid_runTX]);
    }
#endif
        
}


#if defined(USE_ACTIVATION_Mode02)

static void s_eom_emsrunner_6HALTIMERS_execute_task(void *arg)
{   
    eOemsrunner_taskid_t taskID2execute = (eOemsrunner_taskid_t) (int32_t)arg;
    eOemsrunner_taskid_t taskIDprevious = s_eom_emsrunner_prev(taskID2execute);    
//    eOemsrunner_taskid_t taskIDprevprev = s_eom_emsrunner_prev(taskIDprevious);
    
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)
    evEntityId_t prev = eventviewer_switch_to(ev_tmr_exp[taskID2execute]);      
    eventviewer_switch_to(prev); 
#endif  

    
    bool sendEVENTexecute = false;
    
    // we send the or of these conditions:
    // 1. it is the first cycle
    // 2. it the previous executing 
    // 3. the previous was the latest to execute and we are not executing now
    
    bool IStheFIRSTcycle = (0 == s_theemsrunner.cycletiming.tasktiming[taskID2execute].timestarted) ? true : false;
    bool ISthepreviousEXECUTING = s_theemsrunner.cycletiming.tasktiming[taskIDprevious].isexecuting;
    bool HASthepreviousJUSTexecuted = s_theemsrunner.cycletiming.taskterminatedaslatest == taskIDprevious; 
    bool ISthecurrentEXECUTING = s_theemsrunner.cycletiming.tasktiming[taskID2execute].isexecuting;
    
    sendEVENTexecute = IStheFIRSTcycle || ISthepreviousEXECUTING || (HASthepreviousJUSTexecuted && !ISthecurrentEXECUTING);

  

#if defined(USE_OVERFLOWCHECK_effectivetime)
#else    
    if(eobool_true == ISthepreviousEXECUTING)
    {
        // damn ... the previous task has not finished yet .... i must mark an overflow for that task.
        s_eom_runner_overflow_set(&s_theemsrunner, taskIDprevious);
    }
#endif
    
    if(true == sendEVENTexecute)
    {

#if defined(DEBUG_show_when_evt_execute_is_sent)        
        eom_emsrunner_scope_tick(taskID2execute);
#endif
        
        // send event to execute the task in argument
        EOMtask *task2execute = s_theemsrunner.task[taskID2execute];
        eom_task_isrSetEvent(task2execute, eo_emsrunner_evt_execute);
        
#if defined(USE_TIMINGMEASURE_effective)
#else   
        eOabstime_t now = eov_sys_LifeTimeGet(eov_sys_GetHandle());        
        s_theemsrunner.cycletiming.tasktiming[taskID2execute].timestarted = now;
#endif
    }

}


#elif !defined(USE_ACTIVATION_Mode02)

static void s_eom_emsrunner_6HALTIMERS_execute_task(void *arg)
{
    eOemsrunner_taskid_t taskID2execute = (eOemsrunner_taskid_t) (int32_t)arg;
#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)
    evEntityId_t prev = eventviewer_switch_to(ev_tmr_exp[taskID2execute]);      
    eventviewer_switch_to(prev); 
#endif    
    
    EOMtask *task2execute = s_theemsrunner.task[taskID2execute];
           
    eOemsrunner_taskid_t taskIDprevious = (eo_emsrunner_taskid_runRX == taskID2execute) ? (eo_emsrunner_taskid_runTX) : ((eOemsrunner_taskid_t)((uint8_t)taskID2execute-1));
//    EOMtask* taskprevious = s_theemsrunner.task[taskIDprevious];


#if defined(USE_OVERFLOWCHECK_effectivetime)
#else    
    if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskIDprevious].isexecuting)
    {
        // damn ... the previous task has not finished yet .... i must mark an overflow for that task.
        s_eom_runner_overflow_set(&s_theemsrunner, taskIDprevious);
    }
#endif    


		 
    // send event to activate the task in argument
    eom_task_isrSetEvent(task2execute, eo_emsrunner_evt_execute);


#if defined(USE_TIMINGMEASURE_effective)
#else        
    s_theemsrunner.cycletiming.tasktiming[taskID2execute].timestarted = eov_sys_LifeTimeGet(eov_sys_GetHandle());
#endif    
    
}

#endif // #elif !defined(USE_ACTIVATION_Mode02)

#if defined(EOMTHERUNNER_DONT_USE_safestop)
#else 
static void s_eom_emsrunner_6HALTIMERS_safestop_check_task(void *arg)
{
    eOemsrunner_taskid_t taskID2check = (eOemsrunner_taskid_t) (int32_t)arg;
    //EOMtask *task2check = s_theemsrunner.task[taskID2check];
           
    if(eobool_true == s_theemsrunner.cycletiming.tasktiming[taskID2check].isexecuting)
    {
        // damn ... the previous task has not finished yet .... i must mark an overflow for that task.
        s_theemsrunner.cycletiming.tasktiming[taskID2check].isabout2overflow = eobool_true;
    }
    // marco.accame on 18jun25: isabout2overflow is set true in here BUT ... someone must check it,
    // so something and then also to ... clear it. 
    // that however must be done for a value of eOemsrunner_mode_t that we dont have. 
    // for instance for a eo_emsrunner_mode_realtime for which we dont execute something of we emit messages if ...
    // BUT we dont do it
    // SO: better remove the code
    #warning TODO: remove code for the extra 3 HW timers 
}

static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_safestop_check(void *arg)
{
    eOemsrunner_taskid_t taskid = (eOemsrunner_taskid_t) (int32_t)arg;
    
    hal_timer_cfg_t periodic_cfg  = 
    {
        EO_INIT(.prescaler)         hal_timer_prescalerAUTO,         
        EO_INIT(.countdown)         0,
        EO_INIT(.priority)          hal_int_priority02,
        EO_INIT(.mode)              hal_timer_mode_periodic,
        EO_INIT(.callback_on_exp)   NULL,
        EO_INIT(.arg)               NULL
    }; 

    periodic_cfg.countdown         = s_theemsrunner.cfg.period;
    periodic_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_safestop_check_task;
    periodic_cfg.arg               = arg;
    hal_timer_init(s_theemsrunner.haltimer_safestop[taskid], &periodic_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_safestop[taskid]);    
}
#endif

static void s_eom_emsrunner_6HALTIMERS_start_periodic_timer_execute_task(void *arg)
{
    eOemsrunner_taskid_t taskid = (eOemsrunner_taskid_t) (int32_t)arg;

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)
    evEntityId_t prev = eventviewer_switch_to(ev_tmr_oneshotexp[taskid]);      
    eventviewer_switch_to(prev); 
#endif
    
    hal_timer_cfg_t periodic_cfg  = 
    {
        EO_INIT(.prescaler)         hal_timer_prescalerAUTO,         
        EO_INIT(.countdown)         0,
        EO_INIT(.priority)          hal_int_priority02,
        EO_INIT(.mode)              hal_timer_mode_periodic,
        EO_INIT(.callback_on_exp)   NULL,
        EO_INIT(.arg)               NULL
    }; 

    periodic_cfg.countdown         = s_theemsrunner.cfg.period;
    periodic_cfg.callback_on_exp   = s_eom_emsrunner_6HALTIMERS_execute_task;
    periodic_cfg.arg               = arg;
    hal_timer_init(s_theemsrunner.haltimer_start[taskid], &periodic_cfg, NULL); 
    hal_timer_start(s_theemsrunner.haltimer_start[taskid]);    
}

// end of
// ------------------------------------------------------------------------------------------------------------------------------------
// part in which we manages the 3 (or 6) HW timers for the synhcronized mode, with all its variants
// ------------------------------------------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------------------------------------------
// part in which we manages the 1 HW timer for the best effort mode
// ------------------------------------------------------------------------------------------------------------------------------------
// start


static void s_eom_emsrunner_1HWTIMER_trigger(void *arg)
{   

#if defined(BESTEFFORT_ACTIVATE_RX_ONLY)
    // activate RX only. the other activations will be at end of the thread
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_execute);
#else
    // send events to all three threads
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_execute);
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runDO], eo_emsrunner_evt_execute);
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runTX], eo_emsrunner_evt_execute);
#endif    

    // and nothing else
    return;        
}


static void s_eom_emsrunner_1HWTIMER_start(EOMtheEMSrunner *p)
{   
    if(eo_emsrunner_mode_besteffort != s_theemsrunner.mode) 
    {
        return;
    }
    
    // now i start the timer for rx only already in periodic mode with countdown = 1 period. 
    // its periodic expiry will send the three eo_emsrunner_evt_execute to {rx, do, tx} 
 
    // reset all flags for period
    p->cycletiming.periodtiming.reset();
    p->cycletiming.periodtiming.histogram.init(histoCFG);
    p->cycletiming.periodtiming.histogram.reset();
    
    // reset all flags telling about overflow of a task into the safety gap or into the next task.
    // marco.accame on 11 june 2025: i keep them but they seems useless for this case
    
    p->cycletiming.tasktiming[eo_emsrunner_taskid_runRX].isabout2overflow = eobool_false;
    p->cycletiming.tasktiming[eo_emsrunner_taskid_runDO].isabout2overflow = eobool_false;
    p->cycletiming.tasktiming[eo_emsrunner_taskid_runTX].isabout2overflow = eobool_false;

    s_eom_runner_overflow_clr(&s_theemsrunner, eo_emsrunner_taskid_runRX);
    s_eom_runner_overflow_clr(&s_theemsrunner, eo_emsrunner_taskid_runDO);
    s_eom_runner_overflow_clr(&s_theemsrunner, eo_emsrunner_taskid_runTX);
    
    // init the hal timer for rx
    
    {
        eOemsrunner_cfg_t* cfg = &p->cfg;
        
        hal_timer_cfg_t oneshot_cfg = 
        {
            EO_INIT(.prescaler)         hal_timer_prescalerAUTO,         
            EO_INIT(.countdown)         0,
            EO_INIT(.priority)          hal_int_priority02,
            EO_INIT(.mode)              hal_timer_mode_periodic,
            EO_INIT(.callback_on_exp)   s_eom_emsrunner_1HWTIMER_trigger,
            EO_INIT(.arg)               NULL
        }; 
                      
        
        // the timer for task rx (the only one)
        oneshot_cfg.countdown         = cfg->period; 
        oneshot_cfg.arg               = nullptr;
        hal_timer_init(p->haltimer_start[eo_emsrunner_taskid_runRX], &oneshot_cfg, NULL); 
                      
    }
   

    // we also need to enable the first task: taskRX. the other tasks will be enabled at the end of the previous task
    
    eom_task_SetEvent(p->task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_enable);
    
    // we impose that the cycle is running    
    p->cycletiming.cycleisrunning = eobool_true;
    p->cycletiming.taskinexecution = p->cycletiming.taskterminatedaslatest = eo_emsrunner_taskid_NONE;
    p->cycletiming.iterationnumber = 0;
    
    memset(&s_theemsrunner.cycletiming.tasktiming, 0, sizeof(s_theemsrunner.cycletiming.tasktiming));
    // but also reassign the time budgets inside tasktiming[]
    e_eom_emsrunner_update_timebudget();

    // finally: we activate the hal timer. it is best to avoid any interruption in here, thus we disable scheduling

#if defined(EOM_EMSRUNNER_EVIEW_MEASURES_TIMERS)
    evEntityId_t prev = eventviewer_switch_to(ev_tmr_all_oneshot_start );      
    eventviewer_switch_to(prev); 
#endif
    

    hal_timer_start(p->haltimer_start[eo_emsrunner_taskid_runRX]);
          
}


static void s_eom_emsrunner_1HWTIMER_stop(void)
{
    // rx timer only
    if(hal_timer_status_running == hal_timer_status_get(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]))
    {
        hal_timer_stop(s_theemsrunner.haltimer_start[eo_emsrunner_taskid_runRX]); 
    }     
}


// end of
// ------------------------------------------------------------------------------------------------------------------------------------
// part in which we manages the 3 (or 6) HW timers for the synhcronized mode, with all its variants
// ------------------------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

