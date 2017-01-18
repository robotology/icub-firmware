
// start of file

#if 0

#include "some-classes.h"


#include "EoCommon.h"
#include "EOtimer.h"
#include "EOaction.h"
#include "EOMtask.h"

#include "EOVtheCallbackManager.h"

#include "EOMtheTimerManager.h"

#include "EOMtheCallbackManager.h"


#include "osal.h"

//namespace embot { namespace common {
//    
//    const relTime timeWaitNone = 0;
//    const relTime time1millisec = 1000;
//    const relTime time1second = 1000000;
//    const relTime timeWaitForever = 0xffffffff;
//    
//}} // namespace embot { namespace common {

namespace embot {   
namespace sys {
    
   

    
//    common::Time timeNow()
//    {
//        return osal_system_abstime_get();
//    }
//    
//    Task* taskRunning()
//    {
//        osal_task_t *p = osal_task_get(osal_callerAUTOdetect);

//        if(nullptr == p)
//        {
//            return(NULL);
//        }

//        EOMtask *eomtask = static_cast<EOMtask*> (osal_task_extdata_get(p));    

//        return static_cast<Task*>(eom_task_GetExternalData(eomtask));        
//    }
//    

//struct embot::sys::theScheduler::Impl
//{ 
//  
//    osal_cfg_t osalConfig; 
//    osal_cfg_t osalDefaultConfig;

//    void set_default(osal_cfg_t &cfg)
//    {
//        memset(&cfg, 0, sizeof(osal_cfg_t));
//        
//        cfg.rtostype = osal_rtostype_oosiit;
//        cfg.memorymodel = osal_memmode_dynamic;
//        cfg.prio = 15;
//        cfg.cpufam = osal_cpufam_armcm4;  
//        cfg.cpufreq = 168000000;
//        cfg.tick = 1000;
//        cfg.launcherstacksize = 2048;
//        cfg.idlestacksize = 512;
//        cfg.roundrobin = osal_false;
//        cfg.roundrobintick = 0;
//        cfg.tasknum = 0;
//        // etc.
//        cfg.extfn.usr_on_fatal_error = osalOnFatalError;
//        cfg.extfn.usr_on_idle = osalIdleTask;
//    }

//    
//    bool started;
//    
//    static void osalIdleTask(void)
//    {
//        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();        
//        common::fpWorker onidle = thesystem.pImpl->osalIdleActivity;
//        
//        for(;;)
//        {
//            if(nullptr != onidle)
//            {
//                onidle();
//            }
//        }
//    } 
//    
//    static void osalOnFatalError(void* task, osal_fatalerror_t errorcode, const char * errormsg)
//    {
//        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();        
//        common::fpWorker onfatalerror = thesystem.pImpl->osalFatalErrorActivity;
//        
//        if(nullptr != onfatalerror)
//        {
//            onfatalerror();
//        }
//            
//        // we stop in here
//        for(;;);  
//    }

//    common::fpWorker osalLauncher;
//    common::fpWorker osalIdleActivity;   
//    common::fpWorker osalFatalErrorActivity;
//    
//             
//    static void osalDefaultLauncher(void) 
//    {    
//        // does nothing ...
//    }

//    void osalInit(void)
//    {
//        std::uint32_t ram08size = 0;
//        uint64_t *ram08data = NULL;
//        
//        osal_base_memory_getsize(&osalConfig, &ram08size);
//        
//        if(0 != ram08size)
//        {
//            ram08data = (uint64_t*)(uint64_t*)osal_base_memory_new(ram08size);
//        }
//        osal_base_initialise(&osalConfig, ram08data);    
//        

//        if(nullptr == osalLauncher)
//        {
//            osalLauncher = osalDefaultLauncher;
//        }        
//    }
//    
//    Impl() 
//    {
//        
//        set_default(osalConfig);
//        
//        started = false;
//        
//        osalIdleActivity = nullptr;
//        osalFatalErrorActivity = nullptr;
//        osalLauncher = nullptr;

//    }
//};




//embot::sys::theScheduler::theScheduler()
//: pImpl(new Impl)
//{   

//}


//bool embot::sys::theScheduler::init(Config &config)
//{   
//    if(isStarted())
//    {
//        return false;
//    }
//    
//    if((0 == config.clockfrequency) || (0 == config.ticktime))
//    {
//        return false;
//    }
//    
//    if((nullptr == config.launcher) || (0 == config.launcherstacksize))
//    {
//        return false;
//    }
//    
//    if(0 == config.onidlestacksize)
//    {
//        return false;
//    }
//    
//    pImpl->osalConfig.cpufreq = config.clockfrequency;
//    pImpl->osalConfig.tick = config.ticktime;    
//    
//    pImpl->osalLauncher = config.launcher;
//    pImpl->osalConfig.launcherstacksize = config.launcherstacksize;
//    
//    pImpl->osalIdleActivity = config.onidle;
//    pImpl->osalConfig.idlestacksize = config.onidlestacksize;
//    
//    pImpl->osalFatalErrorActivity = config.onfatalerror;
//    
//    
//    return true;    
//}



//void embot::sys::theScheduler::start()
//{   
//    if(isStarted())
//    {
//        return;
//    }
//    
//    pImpl->osalInit();
//        
//    // 1. init rtos in standard way:
//    
//    pImpl->started = true;
//    osal_system_start(pImpl->osalLauncher);
//}


//bool embot::sys::theScheduler::isStarted()
//{
//    return pImpl->started;
//}


//Task * embot::sys::theScheduler::getTask()
//{
//    if(false == pImpl->started)
//    {
//        return nullptr;
//    }
//    
//    osal_task_t *p = osal_task_get(osal_callerAUTOdetect);

//    if(nullptr == p)
//    {
//        return(NULL);
//    }

//    EOMtask *eomtask = static_cast<EOMtask*> (osal_task_extdata_get(p));    

//    return static_cast<Task*>(eom_task_GetExternalData(eomtask));
//}


//Time embot::sys::theScheduler::getTime()
//{   
//    return osal_system_abstime_get();
//}


// - class theTimerManager

//struct embot::sys::theTimerManager::Impl
//{    
//    bool started;
//    Config config;
//    
//    Impl() 
//    {              
//        started = false;
//        config.capacityofhandler =8;
//        config.priority = 240;
//        config.stacksize = 1024;
//    }
//};




//embot::sys::theTimerManager::theTimerManager()
//: pImpl(new Impl)
//{   

//}


//bool embot::sys::theTimerManager::init(Config &config)
//{   
//    if(true == pImpl->started)
//    {
//        return false;
//    }
//        
//    pImpl->config = config;
//    
//    return true;    
//}


//bool embot::sys::theTimerManager::start()
//{       
//    if(true == pImpl->started)
//    {
//        return false;
//    }
//    
//    pImpl->started = true;
//    
//    eOmtimerman_cfg_t cfg = {0};
//    cfg.messagequeuesize = pImpl->config.capacityofhandler;
//    cfg.priority = pImpl->config.priority;
//    cfg.stacksize = pImpl->config.stacksize;
//    
//    eom_timerman_Initialise(&cfg);
//    
//    return true;
//    
//}



// - class theCallbackManager

//struct embot::sys::theCallbackManager::Impl
//{    
//    bool started;
//    Config config;
//    
//    Impl() 
//    {              
//        started = false;
//        config.capacityofhandler = 8;
//        config.priority = 202;
//        config.stacksize = 1024;
//    }
//};




//embot::sys::theCallbackManager::theCallbackManager()
//: pImpl(new Impl)
//{   

//}


//bool embot::sys::theCallbackManager::init(Config &config)
//{   
//    if(true == pImpl->started)
//    {
//        return false;
//    }
//        
//    pImpl->config = config;
//    
//    return true;    
//}


//bool embot::sys::theCallbackManager::start()
//{       
//    if(true == pImpl->started)
//    {
//        return false;
//    }
//    
//    pImpl->started = true;
//    
//    eOmcallbackman_cfg_t cfg = {0};
//    cfg.queuesize = pImpl->config.capacityofhandler;
//    cfg.priority = pImpl->config.priority;
//    cfg.stacksize = pImpl->config.stacksize;
//    
//    eom_callbackman_Initialise(&cfg);
//    
//    return true;
//    
//}



//// - class Timer

//struct embot::sys::Timer::Impl
//{
//    EOtimer *tmr;
//    Impl() 
//    {
//        tmr = eo_timer_New();
//    }
//    ~Impl()
//    {
//        eo_timer_Delete(tmr);
//    }
//};

//embot::sys::Timer::Timer()
//: pImpl(new Impl)
//{   

//}

//embot::sys::Timer::~Timer()
//{   
//    delete pImpl;
//}


//bool embot::sys::Timer::start(common::relTime countdown, Type type, Action &onexpiry)
//{
//    EOaction_strg strg = {0};
//    EOaction* action = (EOaction*)&strg;
//    EOVtaskDerived *totask = NULL;
//    
//    switch(onexpiry.type)
//    {
//        case Action::Type::event2task:
//        {            
//            if(NULL != onexpiry.evt.task)
//            {
//                totask = (EOVtaskDerived*) onexpiry.evt.task->getEOMtask();
//            }
//            eo_action_SetEvent(action, onexpiry.evt.event, totask);
//        } break;
//        case Action::Type::message2task:
//        {
//            if(NULL != onexpiry.msg.task)
//            {
//                totask = (EOVtaskDerived*) onexpiry.msg.task->getEOMtask();
//            }
//            eo_action_SetMessage(action, onexpiry.msg.message, totask);
//        } break;
//        
//        case Action::Type::executecallback:
//        {
//            totask = eov_callbackman_GetTask(eov_callbackman_GetHandle());
//            eo_action_SetCallback(action, onexpiry.cbk.callback, onexpiry.cbk.arg, totask);
//        } break;  
//        
//        default:
//        {
//            eo_action_Clear(action);
//        } break;
//    } 
//    
//    eOresult_t r = eo_timer_Start(pImpl->tmr, eok_abstimeNOW, countdown, static_cast<eOtimerMode_t>(type), action);    
//    return (eores_OK == r) ? true : false;
//}

//bool embot::sys::Timer::stop()
//{
//    eOresult_t r = eo_timer_Stop(pImpl->tmr);    
//    return (eores_OK == r) ? true : false;
//}

//Timer::Status embot::sys::Timer::getStatus()
//{
//    eOtimerStatus_t st = eo_timer_GetStatus(pImpl->tmr);    
//    return static_cast<Timer::Status>(st);   
//}

//Timer::Type embot::sys::Timer::getType()
//{
//    eOtimerMode_t mode = eo_timer_GetMode(pImpl->tmr);    
//    return static_cast<Timer::Type>(mode);   
//}


// - class Task

//struct Task::Impl0
//{    
//    Task *taskParent;    
//    EOMtask *eomtask0;
//    void *param;
//    Task::fpStartup startupFP0;
//    Task::fpOnEvent oneventFP0; 
//    Task::fpOnMessage onmessageFP0; 
//    Task::fpPeriodicActivity periodicFP0;
//    common::Time timeOfLatestTrigger;

//    
//    Impl0() 
//    {
//        taskParent = nullptr;
//        eomtask0 = eom_task_New1();
//        param = nullptr;    
//        startupFP0 = nullptr;
//        oneventFP0 = nullptr; 
//        onmessageFP0 = nullptr;  
//        periodicFP0 = nullptr;
//        timeOfLatestTrigger = 0;      
//    }
//    
//    ~Impl0()
//    {
//        eom_task_Delete(eomtask0);
//    }
//    
//    static void startup0(EOMtask *tsk, std::uint32_t zero)
//    {
//        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
//        if(nullptr == t)
//        {
//            return;
//        }
//        
//        t->pImpl0->timeOfLatestTrigger = timeNow();
//        
//        if(nullptr != t->pImpl0->startupFP0)
//        {
//            t->pImpl0->startupFP0(t, t->pImpl0->param); 
//        }  
//    }
//    
//    static void eventrun0(EOMtask *tsk, std::uint32_t evtmsgper)
//    {
//        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
//        if(nullptr == t)
//        {
//            return;
//        }
//        
//        t->pImpl0->timeOfLatestTrigger = timeNow();
//        
//        if(nullptr != t->pImpl0->oneventFP0)
//        {
//            t->pImpl0->oneventFP0(t, static_cast<embot::common::Event>(evtmsgper), t->pImpl0->param); 
//        }    
//    }  

//    static void messagerun0(EOMtask *tsk, std::uint32_t evtmsgper)
//    {
//        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
//        if(nullptr == t)
//        {
//            return;
//        }
//        
//        t->pImpl0->timeOfLatestTrigger = timeNow();
//        
//        if(nullptr != t->pImpl0->onmessageFP0)
//        {
//            t->pImpl0->onmessageFP0(t, static_cast<embot::common::Message>(evtmsgper), t->pImpl0->param); 
//        }    
//    }    

//    static void periodicrun0(EOMtask *tsk, std::uint32_t evtmsgper)
//    {
//        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
//        if(nullptr == t)
//        {
//            return;
//        }
//        
//        t->pImpl0->timeOfLatestTrigger = timeNow();
//        
//        if(nullptr != t->pImpl0->periodicFP0)
//        {
//            t->pImpl0->periodicFP0(t, t->pImpl0->param); 
//        }    
//    }       
//};


//Task::Task()
//: pImpl0(new Impl0)
//{   
//    pImpl0->taskParent = nullptr;
//}

//Task::~Task()
//{   
//    delete pImpl0;
//}

//common::Time Task::timeOfTrigger()
//{
//    return pImpl0->timeOfLatestTrigger;
//}

//void * Task::getEOMtask()
//{
//    return pImpl0->eomtask0;
//}

//// - class EventTask

//EventTask::EventTask()
//{
//    pImpl0->taskParent = this;    
//}


//EventTask::EventTask(Task::fpStartup startup, Task::fpOnEvent onevent, std::uint32_t stacksize, Priority priority, common::relTime timeout, void *param)
//{
//    pImpl0->taskParent = this;    
//    init(startup, onevent, stacksize, priority, timeout, param);  
//}

//EventTask::~EventTask()
//{
//}

//Task::Type EventTask::getType()
//{
//    return Type::eventTrigger;
//}

//Task::Priority EventTask::getPriority()
//{
//    std::uint8_t prio = 0;
//    eom_task_PriorityGet(pImpl0->eomtask0, &prio);
//    
//    return prio;
//}

//bool EventTask::setPriority(Task::Priority &priority)
//{    
//    eom_task_PrioritySet(pImpl0->eomtask0, priority);
//    
//    return true;
//}
//    
//bool EventTask::setEvent(embot::common::Event event)
//{
//    eom_task_SetEvent(pImpl0->eomtask0, event);
//    return true;
//}  


//bool EventTask::setMessage(embot::common::Message message, common::relTime timeout)
//{
//    return false;
//}



//bool EventTask::init(Task::fpStartup startup, Task::fpOnEvent onevent, std::uint32_t stacksize, Priority priority, common::relTime timeout, void *param)
//{
//    pImpl0->startupFP0 = startup;
//    pImpl0->oneventFP0 = onevent;
//    pImpl0->param = param;
//    eom_task_Init1(pImpl0->eomtask0, eom_mtask_EventDriven, priority, stacksize, pImpl0->startup0, pImpl0->eventrun0, 0, timeout, this, NULL, NULL);
//    
//    return true;    
//}


////int EventTask::getPP()
////{   // just a test. you can remove it.    
////    return getPriority();
////}




//// - class MessageTask

//MessageTask::MessageTask()
//{
//    pImpl0->taskParent = this;    
//}


//MessageTask::MessageTask(Task::fpStartup startup, Task::fpOnMessage onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, common::relTime timeout, void *param)
//{
//    pImpl0->taskParent = this;     
//    init(startup, onmessage, stacksize, priority, messagequeuecapacity, timeout, param); 
//}

//MessageTask::~MessageTask()
//{
//}

//Task::Type MessageTask::getType()
//{
//    return Type::messageTrigger;
//}

//Task::Priority MessageTask::getPriority()
//{
//    std::uint8_t prio = 0;
//    eom_task_PriorityGet(pImpl0->eomtask0, &prio);    
//    return prio;
//}

//bool MessageTask::setPriority(Task::Priority &priority)
//{    
//    eom_task_PrioritySet(pImpl0->eomtask0, priority);    
//    return true;
//}
//    
//bool MessageTask::setEvent(embot::common::Event event)
//{
//    return false;
//}  


//bool MessageTask::setMessage(embot::common::Message message, common::relTime timeout)
//{
//    eom_task_SendMessage(pImpl0->eomtask0, message, timeout);
//    return true;
//}


//bool MessageTask::init(Task::fpStartup startup, Task::fpOnMessage onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, common::relTime timeout, void *param)
//{
//    pImpl0->startupFP0 = startup;
//    pImpl0->onmessageFP0 = onmessage;
//    pImpl0->param = param;
//    eom_task_Init1(pImpl0->eomtask0, eom_mtask_MessageDriven, priority, stacksize, pImpl0->startup0, pImpl0->messagerun0, messagequeuecapacity, timeout, this, NULL, NULL);        
//    return true;    
//}


//// - class PeriodicTask

//PeriodicTask::PeriodicTask()
//{
//    pImpl0->taskParent = this;    
//}


//PeriodicTask::PeriodicTask(Task::fpStartup startup, Task::fpPeriodicActivity periodicactivity, std::uint32_t stacksize, Priority priority, common::relTime period, void *param)
//{
//    pImpl0->taskParent = this;     
//    init(startup, periodicactivity, stacksize, priority, period, param); 
//}

//PeriodicTask::~PeriodicTask()
//{
//}

//Task::Type PeriodicTask::getType()
//{
//    return Type::periodicTrigger;
//}

//Task::Priority PeriodicTask::getPriority()
//{
//    std::uint8_t prio = 0;
//    eom_task_PriorityGet(pImpl0->eomtask0, &prio);    
//    return prio;
//}

//bool PeriodicTask::setPriority(Task::Priority &priority)
//{    
//    eom_task_PrioritySet(pImpl0->eomtask0, priority);    
//    return true;
//}
//    
//bool PeriodicTask::setEvent(embot::common::Event event)
//{
//    return false;
//}  

//bool PeriodicTask::setMessage(embot::common::Message message, common::relTime timeout)
//{
//    return false;
//}


//bool PeriodicTask::init(Task::fpStartup startup, Task::fpPeriodicActivity periodicactivity, std::uint32_t stacksize, Priority priority, common::relTime period, void *param)
//{
//    pImpl0->startupFP0 = startup;
//    pImpl0->periodicFP0 = periodicactivity;
//    pImpl0->param = param;
//    eom_task_Init1(pImpl0->eomtask0, eom_mtask_Periodic, priority, stacksize, pImpl0->startup0, pImpl0->periodicrun0, 0, period, this, NULL, NULL);        
//    return true;    
//}


}
}

#endif // #if 0

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






