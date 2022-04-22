
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_eom.h"

#if !defined(EMBOBJ_USE_EMBOT)
#error EMBOBJ_USE_EMBOT is not defined: EOMtheIPnet may not compile / work
#endif

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"

#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EOVtheSystem_hid.h" 
#include "EOVtheTimerManager_hid.h" 
#include "EOVtheCallbackManager_hid.h" 
#include "EOMmutex.h" 
#include "EOVmutex.h"
#include "EOtimer_hid.h"
#include "EOMtask.h"


// --------------------------------------------------------------------------------------------------------------------
// - the code
// --------------------------------------------------------------------------------------------------------------------


namespace embot { namespace os { namespace EOM {
    

void init_EOVtheSystem();
void init_EOVtheTimerManager();
void init_EOVtheCallbackManager();

bool _initted {false};
Config _config {};

void initialise(const Config &cfg)
{
    if(_initted)
    {
        return;
    }
    
    _config = cfg;
    
    init_EOVtheSystem();
    init_EOVtheTimerManager();
    init_EOVtheCallbackManager();
    
    
    _initted = true;
}


// - EOVtheSystem

constexpr eOmempool_alloc_config_t s_alloc_cfg =
{
    .pool =
    {
        .size08 = 0, .data08 = nullptr, .size16 = 0, .data16 = nullptr, 
        .size32 = 0, .data32 = nullptr, .size64 = 0, .data64 = nullptr
    },
    .heap =
    {
        .allocate = embot::os::rtos::memory_new,
        .reallocate = embot::os::rtos::memory_realloc,
        .release = embot::os::rtos::memory_delete        
    }
};

constexpr eOmempool_cfg_t s_mempool_cfg = 
{
    .mode = eo_mempool_alloc_dynamic, 
    .conf = &s_alloc_cfg    
};

eOresult_t s_systemstart(eOvoid_fp_void_t userinit_fn);
EOVtaskDerived* s_getrunningtask(void);
uint64_t s_now_get();
void s_now_set(uint64_t t);
uint64_t s_nano_get();
void s_irq_disable();

void init_EOVtheSystem()
{    
    // in here we init EOVtheSystem 
    eov_sys_hid_Initialise(   
                            &s_mempool_cfg, 
                            &_config.erromancfg, 
                            s_systemstart,
                            s_getrunningtask,
                            s_now_get,
                            s_now_set,
                            s_nano_get,
                            s_irq_disable
                          );    
}

eOresult_t s_systemstart(eOvoid_fp_void_t userinit_fn)
{
    // dummy ... never used but required
    return eores_OK;
}

EOVtaskDerived* s_getrunningtask(void)
{
//    osal_task_t *p;
//    
//    p = osal_task_get(osal_callerTSK);

//    if(NULL == p)
//    {
//        return(NULL);
//    }

//    return(osal_task_extdata_get(p));
    #warning TOBESOLVED
    return nullptr;
}

uint64_t s_now_get()
{
    return embot::core::now();
}

void s_now_set(uint64_t t)
{
  #warning TOBESOLVED  
}


uint64_t s_nano_get()
{
    return 1000*embot::core::now();
}

void s_irq_disable()
{
    //embot::hw::sys::irq_disable();
    embot::os::rtos::scheduler_lock();
}

// - EOVtheTimerManager

eOresult_t s_eom_timerman_OnNewTimer(EOVtheTimerManager* tm, EOtimer *t);
eOresult_t s_eom_timerman_OnDelTimer(EOVtheTimerManager* tm, EOtimer *t);
eOresult_t s_eom_timerman_AddTimer(EOVtheTimerManager* tm, EOtimer *t);
eOresult_t s_eom_timerman_RemTimer(EOVtheTimerManager* tm, EOtimer *t);

void init_EOVtheTimerManager()
{
   eov_timerman_hid_Initialise(
                                s_eom_timerman_OnNewTimer, 
                                s_eom_timerman_OnDelTimer, 
                                s_eom_timerman_AddTimer, 
                                s_eom_timerman_RemTimer, 
                                eom_mutex_New()
                               );     
    
    
    
}
    
eOresult_t s_eom_timerman_OnNewTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == tm) || (NULL == t)) 
    {
        return(eores_NOK_nullpointer);    
    }
    
    if(NULL != t->envir.osaltimer)
    {
        // this timer has already been created ..... return error
        return(eores_NOK_generic);
    }

// no need to protect data structures of the timer manager    
//    if(eores_OK != eom_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
//    {
//        // cannot lock it ... bye bye
//         return(eores_NOK_generic);
//    }
    
    // create the embot timer
    t->envir.osaltimer = new embot::os::Timer();
    t->status = EOTIMER_STATUS_IDLE;
    
    // it may be that osal does not have any more timers available.
    if(NULL != t->envir.osaltimer)
    {
        res = eores_OK;
    }
    else
    {
        res = eores_NOK_generic;
    }
    
    
//    // unlock the manager
//    eom_mutex_Release((EOMmutex*)tm->mutex);

    return(res);  
   
}


eOresult_t s_eom_timerman_OnDelTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    //eOresult_t res = eores_NOK_generic;
    
    if((NULL == tm) || (NULL == t)) 
    {
        return(eores_NOK_nullpointer);    
    }
    
    if(NULL == t->envir.osaltimer)
    {
        // this timer has already been deleted 
        return(eores_OK);
    }

// no need to protect data structures of the timer manager    
//    if(eores_OK != eom_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
//    {
//        // cannot lock it ... bye bye
//         return(eores_NOK_generic);
//    }
    
    // delete the osal timer
    delete reinterpret_cast<embot::os::Timer*>(t->envir.osaltimer);
    t->status = EOTIMER_STATUS_IDLE;
    t->envir.osaltimer = nullptr;
        
    
//    // unlock the manager
//    eom_mutex_Release((EOMmutex*)tm->mutex);

    return(eores_OK);  
   
}


void to_embot_action(const EOaction_hid *act, embot::os::Action &action)
{    
    EOMtask* task = nullptr;
    embot::os::Thread *thread = nullptr;
    
    switch(act->actiontype)
    {
        case eo_actypeEvent:
        {
            task = reinterpret_cast<EOMtask*>(act->data.evt.totask);
            thread = reinterpret_cast<embot::os::Thread *>(eom_task_GetRTOSthread(task));
            action.load(embot::os::EventToThread(act->data.evt.event, thread));
        } break;
 
        case eo_actypeMessage:
        {
            task = reinterpret_cast<EOMtask*>(act->data.msg.totask);
            thread = reinterpret_cast<embot::os::Thread *>(eom_task_GetRTOSthread(task));
            action.load(embot::os::MessageToThread(reinterpret_cast<embot::os::Message>(act->data.msg.message), thread));
        } break;

        case eo_actypeCallback:
        {
            task = reinterpret_cast<EOMtask*>(act->data.cbk.exectask);
            thread = reinterpret_cast<embot::os::Thread *>(eom_task_GetRTOSthread(task));
            action.load(embot::os::CallbackToThread(act->data.cbk.callback, act->data.cbk.argument, thread));
        } break;
        
        default:
        {
            action.clear();
        } break;        
    }    
}

eOresult_t s_eom_timerman_AddTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    eOresult_t res = eores_NOK_generic;
   
    if((NULL == tm) || (NULL == t)) 
    {
        return(eores_NOK_nullpointer);    
    }
    
    if(EOTIMER_STATUS_RUNNING == t->status)
    {
        // this timer is already running ..... return error
        return(eores_NOK_generic);
    }

    if(NULL == t->envir.osaltimer)
    {
        // this timer does not have a osal timer ..... return error
        return(eores_NOK_generic);
    }

    
//    if(eores_OK != eov_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE)) 
    if(eores_OK != eov_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
    {
        // cannot lock it ... bye bye
         return(eores_NOK_generic);
    }

    // start the osal timer
//    timing.startat  = t->startat; 
//    timing.count    = t->expirytime; 
//    timing.mode     = ( (EOTIMER_MODE_FOREVER == t->mode) ? (osal_tmrmodeFOREVER) : (osal_tmrmodeONESHOT) );
//    onexpi.cbk      = s_eom_timerman_OnExpiry;
//    onexpi.par      = t;

//    res = (eOresult_t)osal_timer_start((osal_timer_t*)t->envir.osaltimer, &timing, &onexpi, osal_callerTSK);
    
    embot::os::Timer* tmr = reinterpret_cast<embot::os::Timer*>(t->envir.osaltimer);    
    embot::os::Action act;
    to_embot_action(&t->onexpiry, act);        
    embot::os::Timer::Mode mode = (EOTIMER_MODE_FOREVER == t->mode) ? (embot::os::Timer::Mode::forever) : (embot::os::Timer::Mode::oneshot);
    embot::os::Timer::Config cfg{t->expirytime, act, mode, 0};
    bool b = tmr->start(cfg);
    
    if(true == b)
    {
        t->status   = EOTIMER_STATUS_RUNNING;
    }
    else
    {
        t->status   = EOTIMER_STATUS_IDLE;
    }
        
    // unlock the manager
    eov_mutex_Release((EOMmutex*)tm->mutex);

    return(res);  
   
}


eOresult_t s_eom_timerman_RemTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    //osal_timer_t osaltimer = NULL;

    if((NULL == tm) || (NULL == t)) 
    {
         return(eores_NOK_nullpointer);
    }
  
  
    //if(eores_OK != eov_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE)) 
    if(eores_OK != eov_mutex_Take((EOMmutex*)tm->mutex, eok_reltimeINFINITE))
    {
        return(eores_NOK_generic);
    }

    // store it ... for later
    //osaltimer = t->envir.osaltimer;
    
    // stop the osal timer. operation is null safe.
//    osal_timer_stop((osal_timer_t*)t->envir.osaltimer, osal_callerTSK);
    
    embot::os::Timer* tmr = reinterpret_cast<embot::os::Timer*>(t->envir.osaltimer); 
    if(nullptr != tmr)
    {
        tmr->stop(); 
    }        

        
    // reset the values of the timer but do not set envir.osaltimer to NULL
    eo_timer_hid_Reset_but_not_osaltime(t, eo_tmrstat_Idle);
    // thus reassign the value of osaltimer
    //t->osaltimer = osaltimer;

    // unlock the manager
    // eov_mutex_Release((EOMmutex*)tm->mutex);
    eov_mutex_Release((EOMmutex*)tm->mutex);

    return(eores_OK);
}

// - EOVtheCallbackManager

eOresult_t s_eom_callbackman_execute(EOVtheCallbackManager *v, eOcallback_t cbk, void *arg, eOreltime_t tout);

EOMtask *cbkmanagertsk {nullptr};

void init_EOVtheCallbackManager()
{
    cbkmanagertsk = eom_task_Wrap(embot::os::theCallbackManager::getInstance().thread());

    // i initialise the base callback manager
    eov_callbackman_hid_Initialise(s_eom_callbackman_execute, cbkmanagertsk);
    
}

eOresult_t s_eom_callbackman_execute(EOVtheCallbackManager *v, eOcallback_t cbk, void *arg, eOreltime_t tout)
{
    if(NULL == v) 
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eov_task_tskExecCallback(cbkmanagertsk, cbk, arg, tout));
}

}}} // namespace embot { namespace os { namespace EOM {
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

