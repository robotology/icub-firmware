/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// marco.accame@iit.it: revised on sept 2020.
// - removed osal
// - substituted osal with embot::os

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_os.h"
#include "embot_os_Thread.h"

#include "EOVtask_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtask.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

using fpStartup = void (*)(EOMtask *tsk, uint32_t zero);
using fpRun = void (*)(EOMtask *tsk, uint32_t evtmsgper);
using fpName = void (*)(void *tsk);

struct Config
{
    eOmtaskType_t type {eom_mtask_UserDefined};
    uint8_t priority {1};
    uint16_t stacksize {512};
    fpStartup startup_fn {nullptr};
    fpRun run_fn {nullptr};
    uint32_t queuesizeORalleventsmask {0};
    eOreltime_t timeoutORperiod {0};
    void *extdata {nullptr};
    fpName nameofthetask_fn {nullptr};
    const char *name {nullptr};
    
    Config() = default;
    
    void load(  eOmtaskType_t _type, uint8_t _priority, uint16_t _stacksize,
                void (*_startup_fn)(EOMtask *tsk, uint32_t zero),
                void (*_run_fn)(EOMtask *tsk, uint32_t evtmsgper), 
                uint32_t _queuesizeORalleventsmask, eOreltime_t _timeoutORperiod,
                void *_extdata,
                void (*_nameofthetask_fn)(void *tsk),
                const char *_name)
                                           
   {
        type = _type;
        priority = _priority;
        stacksize = _stacksize;
        startup_fn = _startup_fn;
        run_fn = _run_fn;
        queuesizeORalleventsmask = _queuesizeORalleventsmask;
        timeoutORperiod = _timeoutORperiod;
        extdata = _extdata;
        nameofthetask_fn = _nameofthetask_fn;
        name = _name;      
   }
   
};

struct EOMtask_hid 
{
    // - base object
    EOVtask *base {nullptr};

    // - other stuff
    embot::os::Thread *thr {nullptr};    
    Config config {};
    
    EOMtask_hid() = default;
}; 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_prepare_base_object(EOMtask *p);

static void s_embot_common_start(embot::os::Thread *t, void *par);

static void s_embot_evt_run(embot::os::Thread *t, embot::os::EventMask m, void *par);

static void s_embot_msg_run(embot::os::Thread *t, embot::os::Message m, void *par);

static void s_embot_cbk_after(embot::os::Thread *t, embot::core::Callback &cbk, void *par);

static void s_embot_per_run(embot::os::Thread *t, void *par);

static void eom_task_START(EOMtask *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtask * eom_task_New1(void)
{   
    return nullptr ;
}

extern eOresult_t eom_task_Init1(EOMtask *p, eOmtaskType_t type, uint8_t priority, uint16_t stacksize,
                                       void (*startup_fn)(EOMtask *tsk, uint32_t zero),
                                       void (*run_fn)(EOMtask *tsk, uint32_t evtmsgper), 
                                       uint32_t queuesizeORalleventsmask, eOreltime_t timeoutORperiod,
                                       void *extdata,
                                       void (*nameofthetask_fn)(void *tsk),
                                       const char *name)
{
    
    return eores_NOK_generic;
}


extern void eom_task_Delete(EOMtask *p)
{
    if(nullptr == p)
    {
        return;
    }   

    if(nullptr != p->thr)
    {
        delete(p->thr);
    }    
    
    eov_task_hid_Delete(p->base);
    
    delete p;

}

extern EOMtask * eom_task_New(eOmtaskType_t type, uint8_t priority, uint16_t stacksize,
                                       void (*startup_fn)(EOMtask *tsk, uint32_t zero),
                                       void (*run_fn)(EOMtask *tsk, uint32_t evtmsgper), 
                                       uint32_t queuesizeORalleventsmask, eOreltime_t timeoutORperiod,
									   void *extdata,
                                       void (*nameofthetask_fn)(void *tsk),
                                       const char *name
									   )

{
    
    EOMtask *task = nullptr;
    
    switch(type)
    {
        case eom_mtask_EventDriven:
        {
            task = new EOMtask;
            task->thr = new embot::os::EventThread;            
        } break;
        
        case eom_mtask_MessageDriven:
        {
            task = new EOMtask;
            task->thr = new embot::os::MessageThread;        
        } break;

        case eom_mtask_CallbackDriven:
        {
            task = new EOMtask;
            task->thr = new embot::os::CallbackThread;       
        } break;   

        case eom_mtask_Periodic:
        {
            task = new EOMtask;         
            task->thr = new embot::os::PeriodicThread;         
        } break;   
                
        case eom_mtask_OnAllEventsDriven:
        default:
        {
            for(;;);
        } break;                   
    }
    
    if(nullptr != task)
    {
        task->config.load(  type, priority, stacksize,
                    startup_fn,
                    run_fn, 
                    queuesizeORalleventsmask, timeoutORperiod,
                    extdata,
                    nameofthetask_fn,
                    name);   
        
        s_prepare_base_object(task);
        eom_task_START(task);
    }
          
    return(task);
}


static void eom_task_START(EOMtask *p) 
{
    if(nullptr == p)
    {
        return;
    }
    
    bool res = false;
 
    switch(p->thr->getType())
    {
        case embot::os::Thread::Type::eventTrigger:
        {
            embot::os::EventThread *t = reinterpret_cast<embot::os::EventThread*>(p->thr);
            embot::os::EventThread::Config c { 
                        p->config.stacksize, 
                        embot::os::priority::convert(p->config.priority),
                        s_embot_common_start,
                        p,
                        p->config.timeoutORperiod,
                        s_embot_evt_run,
                        p->config.name              
                    };
            res = t->start(c, p->config.nameofthetask_fn);                   
        } break;
        
        case embot::os::Thread::Type::messageTrigger:
        {
            embot::os::MessageThread *t = reinterpret_cast<embot::os::MessageThread*>(p->thr);
            embot::os::MessageThread::Config c {
                        p->config.stacksize, 
                        embot::os::priority::convert(p->config.priority),
                        s_embot_common_start,
                        p,
                        p->config.timeoutORperiod,
                        static_cast<uint8_t>(p->config.queuesizeORalleventsmask),    
                        s_embot_msg_run,
                        p->config.name              
                    };
            res = t->start(c, p->config.nameofthetask_fn);                   
        } break; 

        case embot::os::Thread::Type::callbackTrigger:
        {
            embot::os::CallbackThread *t = reinterpret_cast<embot::os::CallbackThread*>(p->thr);
            embot::os::CallbackThread::Config c {
                        p->config.stacksize, 
                        embot::os::priority::convert(p->config.priority),
                        s_embot_common_start,
                        p,
                        p->config.timeoutORperiod, 
                        static_cast<uint8_t>(p->config.queuesizeORalleventsmask),
                        s_embot_cbk_after,
                        p->config.name              
                    };
            res = t->start(c, p->config.nameofthetask_fn);                   
        } break;   

        case embot::os::Thread::Type::periodicTrigger:
        {
            embot::os::PeriodicThread *t = reinterpret_cast<embot::os::PeriodicThread*>(p->thr);
            embot::os::PeriodicThread::Config c {
                        p->config.stacksize, 
                        embot::os::priority::convert(p->config.priority),
                        s_embot_common_start,
                        p,
                        p->config.timeoutORperiod, 
                        s_embot_per_run,
                        p->config.name              
                    };
            res = t->start(c, p->config.nameofthetask_fn);                   
        } break;   
        
        default: 
        {
        } break;
    } 

    if(false == res)
    {
        embot::core::print("eom_task_START(): invalid embot::os::EventThread::Config");
        for(;;);
    }    
}

extern void eom_task_Start(EOMtask *p) 
{
    if(nullptr == p)
    {
        return;
    }
    
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();                    
}

extern EOMtask * eom_task_Wrap(void *rtosthread)
{
    EOMtask *task = nullptr;
    embot::os::Thread *thread = reinterpret_cast<embot::os::Thread*>(rtosthread);
    
    if(nullptr == thread)
    {
        return nullptr;
    }
    
    switch(thread->getType())
    {
        case embot::os::Thread::Type::callbackTrigger:
        {
            task = new EOMtask;
            task->thr = thread;            
        } break;
        

        default:
        {
            for(;;);
        } break;                   
    }
    
    if(nullptr != task)
    {
        task->config.type = eom_mtask_CallbackDriven;
//        load(  type, priority, stacksize,
//                    startup_fn,
//                    run_fn, 
//                    queuesizeORalleventsmask, timeoutORperiod,
//                    extdata,
//                    nameofthetask_fn,
//                    name);   
        
        s_prepare_base_object(task);
    }
    
    return(task);
}

extern void * eom_task_GetRTOSthread(EOMtask *p)
{
    if(nullptr == p)
    {
        return nullptr;
    }
    
    return p->thr;    
}

extern void* eom_task_GetExternalData(EOMtask *p)
{
    if(nullptr == p) 
    {
        return(nullptr);
    }

    return(p->config.extdata);
}

extern eOresult_t eom_task_SetEvent(EOMtask *p, eOevent_t evt) 
{
    if(nullptr == p) 
    {
        return(eores_NOK_nullpointer);
    }
    return (true == p->thr->setEvent(evt)) ? eores_OK : eores_NOK_generic;
}


extern eOresult_t eom_task_isrSetEvent(EOMtask *p, eOevent_t evt) 
{
    return eom_task_SetEvent(p, evt);
}


extern eOresult_t eom_task_SendMessage(EOMtask *p, eOmessage_t msg, eOreltime_t tout) 
{
    if(nullptr == p) 
    {
        return(eores_NOK_nullpointer);
    }
    return (true == p->thr->setMessage(reinterpret_cast<embot::os::Message>(msg), tout)) ? eores_OK : eores_NOK_generic;
}


extern eOresult_t eom_task_isrSendMessage(EOMtask *p, eOmessage_t msg) 
{
    return(eom_task_SendMessage(p, msg, 0));
}


extern eOresult_t eom_task_ExecCallback(EOMtask *p, eOcallback_t cbk, void *arg, eOreltime_t tout)
{ 
    if(nullptr == p) 
    {
        return(eores_NOK_nullpointer);
    }    
    return (true == p->thr->setCallback({cbk, arg}, tout)) ? eores_OK : eores_NOK_generic;
}


extern eOresult_t eom_task_isrExecCallback(EOMtask *p, eOcallback_t cbk, void *arg)
{
    return eom_task_ExecCallback(p, cbk, arg, 0);
}


extern eOresult_t eom_task_PrioritySet(EOMtask *p, uint8_t prio)
{
    if(nullptr == p) 
    {
        return(eores_NOK_nullpointer);
    }

    p->config.priority = prio;

    return (true == p->thr->setPriority(embot::os::priority::convert(prio))) ? eores_OK : eores_NOK_generic;
}


extern eOresult_t eom_task_PriorityGet(EOMtask *p, uint8_t *prio)
{
    if(nullptr == p) 
    {
        return(eores_NOK_nullpointer);
    }

    *prio = p->config.priority;

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eom_virtual_tsk_set_event(EOVtaskDerived *t, eOevent_t evt) 
{
    return eom_task_SetEvent(reinterpret_cast<EOMtask*>(t), evt);
}

static eOresult_t s_eom_virtual_isr_send_message(EOVtaskDerived *t, eOmessage_t msg) 
{
    return eom_task_isrSendMessage(reinterpret_cast<EOMtask*>(t), msg);
}

static eOresult_t s_eom_virtual_tsk_send_message(EOVtaskDerived *t, eOmessage_t msg, eOreltime_t tout) 
{
    return eom_task_SendMessage(reinterpret_cast<EOMtask*>(t), msg, tout);
}

static eOresult_t s_eom_virtual_isr_exec_callback(EOVtaskDerived *t, eOcallback_t cbk, void *arg) 
{
    return eom_task_isrExecCallback(reinterpret_cast<EOMtask*>(t), cbk, arg);
}

static eOresult_t s_eom_virtual_tsk_exec_callback(EOVtaskDerived *t, eOcallback_t cbk, void *arg, eOreltime_t tout) 
{
    return eom_task_ExecCallback(reinterpret_cast<EOMtask*>(t), cbk, arg, tout);
}

extern eOid08_t s_eom_virtual_get_id(EOVtaskDerived *t) 
{     
    return(0);
}

static void s_prepare_base_object(EOMtask *p)
{
    
    p->base = eov_task_hid_New();
    
    switch(p->thr->getType())
    {
        case embot::os::Thread::Type::eventTrigger:
        {
             eov_task_hid_SetVTABLE( p->base, 
                                    (eOvoid_fp_voidp_uint32_t)p->config.startup_fn, (eOvoid_fp_voidp_uint32_t)p->config.run_fn,
                                    s_eom_virtual_tsk_set_event, s_eom_virtual_tsk_set_event,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_virtual_get_id
                                  );                 
        } break;
        
        case embot::os::Thread::Type::messageTrigger:
        {
            eov_task_hid_SetVTABLE( p->base, 
                                    (eOvoid_fp_voidp_uint32_t)p->config.startup_fn, (eOvoid_fp_voidp_uint32_t)p->config.run_fn,
                                    NULL, NULL,
                                    s_eom_virtual_isr_send_message, s_eom_virtual_tsk_send_message,
                                    NULL, NULL,
                                    s_eom_virtual_get_id
                                  );                 
        } break; 

        case embot::os::Thread::Type::callbackTrigger:
        {
            eov_task_hid_SetVTABLE( p->base, 
                                    (eOvoid_fp_voidp_uint32_t)p->config.startup_fn, (eOvoid_fp_voidp_uint32_t)p->config.run_fn,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_virtual_isr_exec_callback, s_eom_virtual_tsk_exec_callback,
                                    s_eom_virtual_get_id
                                  );                  
        } break;   

        case embot::os::Thread::Type::periodicTrigger:
        {
            eov_task_hid_SetVTABLE( p->base, 
                                    (eOvoid_fp_voidp_uint32_t)p->config.startup_fn, (eOvoid_fp_voidp_uint32_t)p->config.run_fn,
                                    NULL, NULL,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_virtual_get_id
                                  );                 
        } break;   
        
        default: 
        {
        } break;
    }            
    
}


static void s_embot_common_start(embot::os::Thread *t, void *par)
{
    EOMtask *p = reinterpret_cast<EOMtask*>(par);
    p->config.startup_fn(p, 0);
}

static void s_embot_evt_run(embot::os::Thread *t, embot::os::EventMask m, void *par)
{
    EOMtask *p = reinterpret_cast<EOMtask*>(par);
    p->config.run_fn(p, m);
}

static void s_embot_msg_run(embot::os::Thread *t, embot::os::Message m, void *par)
{
    EOMtask *p = reinterpret_cast<EOMtask*>(par);
    p->config.run_fn(p, reinterpret_cast<uint32_t>(m));
}

static void s_embot_cbk_after(embot::os::Thread *t, embot::core::Callback &cbk, void *par)
{
    EOMtask *p = reinterpret_cast<EOMtask*>(par);
    p->config.run_fn(p, reinterpret_cast<uint32_t>(cbk.call));
}

static void s_embot_per_run(embot::os::Thread *t, void *par)
{
    EOMtask *p = reinterpret_cast<EOMtask*>(par);
    p->config.run_fn(p, p->config.timeoutORperiod);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

