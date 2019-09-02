/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_sys_Task.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"


// - class InitTask

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::InitTask::Impl
{    
    osal_task_t *osaltask {nullptr};
    //Config config {};       
    Impl() = default;
    
    ~Impl() { }       
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::InitTask& embot::sys::InitTask::getInstance()
{
    static InitTask* p = new InitTask();
    return *p;
}

embot::sys::InitTask::InitTask()
{   
    pImpl = std::make_unique<Impl>();    
}



embot::sys::InitTask::~InitTask() { }


embot::sys::Task::Type embot::sys::InitTask::getType() const 
{
    return Type::Init;
}


embot::sys::Priority embot::sys::InitTask::getPriority() const
{   
    return embot::sys::Priority::schedInit;
}


bool embot::sys::InitTask::setPriority(embot::sys::Priority priority)
{
    return false;
}

  
bool embot::sys::InitTask::setEvent(embot::common::Event event)
{
    return false;
}  


bool embot::sys::InitTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}

bool embot::sys::InitTask::setCallback(const common::Callback &callback, common::relTime timeout)
{
    return false;
}

void embot::sys::InitTask::synch()
{ 
    pImpl->osaltask = osal_task_get(osal_callerAUTOdetect);
    osal_task_extdata_set(pImpl->osaltask, this);   
}


// - class IdleTask

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::IdleTask::Impl
{    
    bool started {false};
    osal_task_t *osaltask {nullptr};
    //Config config {};
        
    Impl() = default; 
    
    ~Impl() { }    
   
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::IdleTask& embot::sys::IdleTask::getInstance()
{
    static IdleTask* p = new IdleTask();
    return *p;
}

embot::sys::IdleTask::IdleTask()
{   
    pImpl = std::make_unique<Impl>();    
}


embot::sys::IdleTask::~IdleTask() { }


embot::sys::Task::Type embot::sys::IdleTask::getType() const 
{
    return Type::Idle;
}


embot::sys::Priority embot::sys::IdleTask::getPriority() const
{   
    return embot::sys::Priority::schedIdle;
}


bool embot::sys::IdleTask::setPriority(embot::sys::Priority priority)
{
    return false;
}

  
bool embot::sys::IdleTask::setEvent(embot::common::Event event)
{
    if(!pImpl->started)
    {
        return false;
    }
    osal_result_t r = osal_eventflag_set(static_cast<osal_eventflag_t>(event), pImpl->osaltask, osal_callerAUTOdetect);
    return (osal_res_OK == r) ? (true) : (false);
}  


bool embot::sys::IdleTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}

bool embot::sys::IdleTask::setCallback(const common::Callback &callback, common::relTime timeout)
{
    return false;
}

void embot::sys::IdleTask::synch()
{ 
    pImpl->osaltask = osal_task_get(osal_callerAUTOdetect);
    osal_task_extdata_set(pImpl->osaltask, this);   
    pImpl->started = true;
}


// - class EventTask

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::EventTask::Impl
{    
    EventTask * taskParent;
    osal_task_t *osaltask;

    Config config;
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(EventTask *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;
        
        config.startup = nullptr;
        config.onevent = dummyOnEvent; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_eventdriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.timeout = embot::common::timeWaitForever;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_eventdriven_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::common::tointegral(config.priority);
        osaltaskproperties.stacksize = config.stacksize;
  
    }
    
    ~Impl()
    {
        if(nullptr != osaltask)
        {
            osal_task_delete(osaltask);
            osaltask = nullptr;
        }
    }
    
    static void dummyOnEvent(Task *t, common::EventMask m, void *p) {}
    
    static void osal_eventdriven_loop(void *p) 
    {
        EventTask *t = reinterpret_cast<EventTask*>(p);
        const osal_reltime_t tout = static_cast<osal_reltime_t>(t->pImpl->config.timeout); // they are the same uint32_t type expressed in usec
        const Task::fpStartup startup = t->pImpl->config.startup;
        const Task::fpOnEvent onevent = t->pImpl->config.onevent;
        void * param = t->pImpl->config.param;

        // exec the startup
        if(nullptr != startup)
        {
            startup(t, param);
        }

        
        // start the forever loop
        for(;;)
        {
            onevent(t, osal_eventflag_getany(tout), param);
        }        
    }
    
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::EventTask::EventTask()
: pImpl(new Impl(this))
{   
    
}


embot::sys::EventTask::~EventTask()
{   
    delete pImpl;
}


embot::sys::Task::Type embot::sys::EventTask::getType() const 
{
    return Type::eventTrigger;
}


embot::sys::Priority embot::sys::EventTask::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::sys::EventTask::setPriority(embot::sys::Priority priority)
{
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::common::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::sys::EventTask::setEvent(embot::common::Event event)
{
    osal_result_t r = osal_eventflag_set(static_cast<osal_eventflag_t>(event), pImpl->osaltask, osal_callerAUTOdetect);
    return (osal_res_OK == r) ? (true) : (false);
}  


bool embot::sys::EventTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}

bool embot::sys::EventTask::setCallback(const common::Callback &callback, common::relTime timeout)
{
    return false;
}


bool embot::sys::EventTask::start(const Config &cfg)
{    
    if(false == cfg.isvalid())
    {
        return false;
    }
    
    pImpl->config = cfg;
    
    pImpl->config.stacksize = (pImpl->config.stacksize+7)/8;
    pImpl->config.stacksize *= 8;
    
    pImpl->osaltaskproperties.function = pImpl->osal_eventdriven_loop;
    pImpl->osaltaskproperties.param = this;        
    pImpl->osaltaskproperties.priority = embot::common::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;    
        
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::sys::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}


// - class MessageTask

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::MessageTask::Impl
{    
    MessageTask * taskParent;
    osal_task_t *osaltask;    
    osal_messagequeue_t *osalmessagequeue;
    
    Config config {};
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(MessageTask *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;
        osalmessagequeue = nullptr;

        config.startup = nullptr;
        config.onmessage = dummyOnMessage; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_messagedriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.timeout = embot::common::timeWaitForever;
        config.messagequeuesize = 2;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_messagedriven_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::common::tointegral(config.priority);
        osaltaskproperties.stacksize = config.stacksize;
  
    }
    
    ~Impl()
    {
        if(nullptr != osaltask)
        {
            osal_task_delete(osaltask);
            osaltask = nullptr;
        }
        
        if(nullptr != osalmessagequeue)
        {
            osal_messagequeue_delete(osalmessagequeue);
            osalmessagequeue = nullptr;
        }
    }
    
    static void dummyOnMessage(Task *t, common::Message m, void *p) {}
    
    static void osal_messagedriven_loop(void *p) 
    {
        MessageTask *t = reinterpret_cast<MessageTask*>(p);
        const osal_reltime_t tout = static_cast<osal_reltime_t>(t->pImpl->config.timeout); // they are the same uint32_t type expressed in usec
        const Task::fpStartup startup = t->pImpl->config.startup;
        const Task::fpOnMessage onmessage = t->pImpl->config.onmessage;
        void * param = t->pImpl->config.param;
        osal_messagequeue_t *mq = t->pImpl->osalmessagequeue;
       

        // exec the startup
        if(nullptr != startup)
        {
            startup(t, param);
        }

        
        // start the forever loop
        for(;;)
        {
            // osal_messagequeue_getquick() returns a osal_message_t which is a uint32_t* which may hold a simple integer of a pointer to larger data
            // the use depends on situations ...
            common::Message msg = reinterpret_cast<common::Message>(osal_messagequeue_getquick(mq, tout, osal_callerAUTOdetect));
            onmessage(t, msg, param);
        }        
    }
    
};



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

embot::sys::MessageTask::MessageTask()
: pImpl(new Impl(this))
{   
    
}


embot::sys::MessageTask::~MessageTask()
{   
    delete pImpl;
}


embot::sys::Task::Type embot::sys::MessageTask::getType() const
{
    return Type::messageTrigger;
}


embot::sys::Priority embot::sys::MessageTask::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::sys::MessageTask::setPriority(embot::sys::Priority priority)
{   
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::common::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::sys::MessageTask::setEvent(embot::common::Event event)
{
    return false;
}  


bool embot::sys::MessageTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    osal_result_t r = osal_messagequeue_put(pImpl->osalmessagequeue, reinterpret_cast<osal_message_t>(message), static_cast<osal_reltime_t>(timeout), osal_callerAUTOdetect);
    return (osal_res_OK == r) ? (true) : (false);
}

bool embot::sys::MessageTask::setCallback(const common::Callback &callback, common::relTime timeout)
{
    return false;
}


bool embot::sys::MessageTask::start(const Config &cfg)
{    
    if(false == cfg.isvalid())
    {
        return false;
    }
    
    pImpl->config = cfg;
    
    pImpl->config.stacksize = (pImpl->config.stacksize+7)/8;
    pImpl->config.stacksize *= 8;
    
    pImpl->osalmessagequeue = osal_messagequeue_new(pImpl->config.messagequeuesize); 
    
    pImpl->osaltaskproperties.function = pImpl->osal_messagedriven_loop;
    pImpl->osaltaskproperties.param = this;        
    pImpl->osaltaskproperties.priority = embot::common::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;  
       
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::sys::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}


// - class CallbackTask

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::CallbackTask::Impl
{    
    CallbackTask * taskParent;
    osal_task_t *osaltask;    
    osal_messagequeue_t *osalfunctionqueue;
    osal_messagequeue_t *osalargumentqueue;

    Config config;
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(CallbackTask *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;
        osalfunctionqueue = nullptr;
        osalargumentqueue = nullptr;
        
        config.startup = nullptr;
        config.aftercallback = dummyAfter; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_messagedriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.timeout = embot::common::timeWaitForever;
        config.queuesize = 2;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_callbackdriven_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::common::tointegral(config.priority);
        osaltaskproperties.stacksize = config.stacksize;
  
    }
    
    ~Impl()
    {
        if(nullptr != osaltask)
        {
            osal_task_delete(osaltask);
            osaltask = nullptr;
        }
        
        if(nullptr != osalfunctionqueue)
        {
            osal_messagequeue_delete(osalfunctionqueue);
            osalfunctionqueue = nullptr;
        }
        
        if(nullptr != osalargumentqueue)
        {
            osal_messagequeue_delete(osalargumentqueue);
            osalargumentqueue = nullptr;
        }
    }
    
    static void dummyAfter(Task *t, common::Callback &m, void *p) {}
    
    static void osal_callbackdriven_loop(void *p) 
    {
        CallbackTask *t = reinterpret_cast<CallbackTask*>(p);
        const osal_reltime_t tout = static_cast<osal_reltime_t>(t->pImpl->config.timeout); // they are the same uint32_t type expressed in usec
        const Task::fpStartup startup = t->pImpl->config.startup;
        const Task::fpAfterCallback after = (nullptr != t->pImpl->config.aftercallback) ? (t->pImpl->config.aftercallback) : (dummyAfter);
        void * param = t->pImpl->config.param;
        osal_messagequeue_t *fQ = t->pImpl->osalfunctionqueue;
        osal_messagequeue_t *aQ = t->pImpl->osalargumentqueue;
        
        embot::common::Callback cbk;;
       

        // exec the startup
        if(nullptr != startup)
        {
            startup(t, param);
        }
        
        // start the forever loop
        for(;;)
        {
            // osal_messagequeue_getquick() returns a osal_message_t which is a uint32_t* which may hold a simple integer of a pointer to larger data
            // the order is importnt in here.... dont exchange the following two lines .....
            cbk.arg = reinterpret_cast<void*>(osal_messagequeue_getquick(aQ, tout, osal_callerAUTOdetect)); 
            cbk.call = reinterpret_cast<embot::common::fpCaller>(osal_messagequeue_getquick(fQ, tout, osal_callerAUTOdetect));
            // it executes only if it is valid 
            cbk.execute();
            after(t, cbk, param);
        }   

    }
    
};



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::CallbackTask::CallbackTask()
: pImpl(new Impl(this))
{   
    
}


embot::sys::CallbackTask::~CallbackTask()
{   
    delete pImpl;
}


embot::sys::Task::Type embot::sys::CallbackTask::getType() const
{
    return Type::callbackTrigger;
}


embot::sys::Priority embot::sys::CallbackTask::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::sys::CallbackTask::setPriority(embot::sys::Priority priority)
{
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::common::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::sys::CallbackTask::setEvent(embot::common::Event event)
{
    return false;
}  


bool embot::sys::CallbackTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}

bool embot::sys::CallbackTask::setCallback(const embot::common::Callback &callback, common::relTime timeout)
{
    if(false == callback.isvalid())
    {
        return false;
    }
    
    osal_result_t res = osal_res_NOK_generic;

    res = osal_messagequeue_put(pImpl->osalargumentqueue, reinterpret_cast<osal_message_t>(callback.arg), timeout, osal_callerAUTOdetect);
    if(osal_res_OK != res)
    {
        return false;
    }
    
    res = osal_messagequeue_put(pImpl->osalfunctionqueue, reinterpret_cast<osal_message_t>(callback.call), timeout, osal_callerAUTOdetect);
    if(osal_res_OK != res)
    {
        return false;
    }
 
    return true;
}



bool embot::sys::CallbackTask::start(const Config &cfg)
{    
    if(false == cfg.isvalid())
    {
        return false;
    }
    
    pImpl->config = cfg;    
    
    pImpl->config.stacksize = (pImpl->config.stacksize+7)/8;
    pImpl->config.stacksize *= 8;
    
    pImpl->osalargumentqueue = osal_messagequeue_new(pImpl->config.queuesize); 
    pImpl->osalfunctionqueue = osal_messagequeue_new(pImpl->config.queuesize); 
    
    pImpl->osaltaskproperties.function = pImpl->osal_callbackdriven_loop;
    pImpl->osaltaskproperties.param = this;        
    pImpl->osaltaskproperties.priority = embot::common::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;  
       
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::sys::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}


// - class PeriodicTask

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::PeriodicTask::Impl
{    
    PeriodicTask * taskParent;
    osal_task_t *osaltask;    

    Config config;
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(PeriodicTask *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;

        config.startup = nullptr;
        config.onperiod = dummyOnPeriod; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_messagedriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.period = embot::common::time1second;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_periodic_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::common::tointegral(config.priority);
        osaltaskproperties.stacksize = config.stacksize;
  
    }
    
    ~Impl()
    {
        if(nullptr != osaltask)
        {
            osal_task_delete(osaltask);
            osaltask = nullptr;
        }        
    }
    
    static void dummyOnPeriod(Task *t, void *p) {}
    
    static void osal_periodic_loop(void *p) 
    {
        PeriodicTask *t = reinterpret_cast<PeriodicTask*>(p);
        const osal_reltime_t period = static_cast<osal_reltime_t>(t->pImpl->config.period); // they are the same uint32_t type expressed in usec
        const Task::fpStartup startup = t->pImpl->config.startup;
        const Task::fpOnPeriod onperiod = t->pImpl->config.onperiod;
        void * param = t->pImpl->config.param;
       

        // exec the startup
        if(nullptr != startup)
        {
            startup(t, param);
        }

        osal_task_period_set(period);
        
        // start the forever loop
        for(;;)
        {
            osal_task_period_wait();
            onperiod(t, param);
        }        
    }
    
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::PeriodicTask::PeriodicTask()
: pImpl(new Impl(this))
{   
    
}


embot::sys::PeriodicTask::~PeriodicTask()
{   
    delete pImpl;
}


embot::sys::Task::Type embot::sys::PeriodicTask::getType() const
{
    return Type::periodicTrigger;
}


embot::sys::Priority embot::sys::PeriodicTask::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::sys::PeriodicTask::setPriority(embot::sys::Priority priority)
{
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::common::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::sys::PeriodicTask::setEvent(embot::common::Event event)
{
    return false;
}  


bool embot::sys::PeriodicTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}

bool embot::sys::PeriodicTask::setCallback(const common::Callback &callback, common::relTime timeout)
{
    return false;
}


bool embot::sys::PeriodicTask::start(const Config &cfg)
{    
    if(false == cfg.isvalid())
    {
        return false;
    }
    
    pImpl->config = cfg;
    
    pImpl->config.stacksize = (pImpl->config.stacksize+7)/8;
    pImpl->config.stacksize *= 8;
    
    
    pImpl->osaltaskproperties.function = pImpl->osal_periodic_loop;
    pImpl->osaltaskproperties.param = this;        
    pImpl->osaltaskproperties.priority = embot::common::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;  
       
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::sys::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}



    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

