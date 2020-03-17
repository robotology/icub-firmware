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

#include "embot_os_Thread.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"


// - class InitThread

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::InitThread::Impl
{    
    osal_task_t *osaltask {nullptr};
    //Config config {};       
    Impl() = default;
    
    ~Impl() { }       
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::os::InitThread& embot::os::InitThread::getInstance()
{
    static InitThread* p = new InitThread();
    return *p;
}

embot::os::InitThread::InitThread()
{   
    pImpl = std::make_unique<Impl>();    
}



embot::os::InitThread::~InitThread() { }


embot::os::Thread::Type embot::os::InitThread::getType() const 
{
    return Type::Init;
}


embot::os::Priority embot::os::InitThread::getPriority() const
{   
    return embot::os::Priority::schedInit;
}


bool embot::os::InitThread::setPriority(embot::os::Priority priority)
{
    return false;
}

  
bool embot::os::InitThread::setEvent(embot::os::Event event)
{
    return false;
}  


bool embot::os::InitThread::setMessage(embot::os::Message message, core::relTime timeout)
{
    return false;
}

bool embot::os::InitThread::setCallback(const core::Callback &callback, core::relTime timeout)
{
    return false;
}

void embot::os::InitThread::synch()
{ 
    pImpl->osaltask = osal_task_get(osal_callerAUTOdetect);
    osal_task_extdata_set(pImpl->osaltask, this);   
}


// - class IdleThread

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::IdleThread::Impl
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


embot::os::IdleThread& embot::os::IdleThread::getInstance()
{
    static IdleThread* p = new IdleThread();
    return *p;
}

embot::os::IdleThread::IdleThread()
{   
    pImpl = std::make_unique<Impl>();    
}


embot::os::IdleThread::~IdleThread() { }


embot::os::Thread::Type embot::os::IdleThread::getType() const 
{
    return Type::Idle;
}


embot::os::Priority embot::os::IdleThread::getPriority() const
{   
    return embot::os::Priority::schedIdle;
}


bool embot::os::IdleThread::setPriority(embot::os::Priority priority)
{
    return false;
}

  
bool embot::os::IdleThread::setEvent(embot::os::Event event)
{
    if(!pImpl->started)
    {
        return false;
    }
    osal_result_t r = osal_eventflag_set(static_cast<osal_eventflag_t>(event), pImpl->osaltask, osal_callerAUTOdetect);
    return (osal_res_OK == r) ? (true) : (false);
}  


bool embot::os::IdleThread::setMessage(embot::os::Message message, core::relTime timeout)
{
    return false;
}

bool embot::os::IdleThread::setCallback(const core::Callback &callback, core::relTime timeout)
{
    return false;
}

void embot::os::IdleThread::synch()
{ 
    pImpl->osaltask = osal_task_get(osal_callerAUTOdetect);
    osal_task_extdata_set(pImpl->osaltask, this);   
    pImpl->started = true;
}


// - class EventThread

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::EventThread::Impl
{    
    EventThread * taskParent;
    osal_task_t *osaltask;

    Config config;
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(EventThread *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;
        
        config.startup = nullptr;
        config.onevent = dummyOnEvent; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_eventdriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.timeout = embot::core::reltimeWaitForever;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_eventdriven_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::core::tointegral(config.priority);
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
    
    static void dummyOnEvent(Thread *t, os::EventMask m, void *p) {}
    
    static void osal_eventdriven_loop(void *p) 
    {
        EventThread *t = reinterpret_cast<EventThread*>(p);
        const osal_reltime_t tout = static_cast<osal_reltime_t>(t->pImpl->config.timeout); // they are the same uint32_t type expressed in usec
        const Thread::fpStartup startup = t->pImpl->config.startup;
        const Thread::fpOnEvent onevent = t->pImpl->config.onevent;
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


embot::os::EventThread::EventThread()
: pImpl(new Impl(this))
{   
    
}


embot::os::EventThread::~EventThread()
{   
    delete pImpl;
}


embot::os::Thread::Type embot::os::EventThread::getType() const 
{
    return Type::eventTrigger;
}


embot::os::Priority embot::os::EventThread::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::os::EventThread::setPriority(embot::os::Priority priority)
{
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::core::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::os::EventThread::setEvent(embot::os::Event event)
{
    osal_result_t r = osal_eventflag_set(static_cast<osal_eventflag_t>(event), pImpl->osaltask, osal_callerAUTOdetect);
    return (osal_res_OK == r) ? (true) : (false);
}  


bool embot::os::EventThread::setMessage(embot::os::Message message, core::relTime timeout)
{
    return false;
}

bool embot::os::EventThread::setCallback(const core::Callback &callback, core::relTime timeout)
{
    return false;
}


bool embot::os::EventThread::start(const Config &cfg)
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
    pImpl->osaltaskproperties.priority = embot::core::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;    
        
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::os::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}


// - class MessageThread

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::MessageThread::Impl
{    
    MessageThread * taskParent;
    osal_task_t *osaltask;    
    osal_messagequeue_t *osalmessagequeue;
    
    Config config {};
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(MessageThread *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;
        osalmessagequeue = nullptr;

        config.startup = nullptr;
        config.onmessage = dummyOnMessage; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_messagedriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.timeout = embot::core::reltimeWaitForever;
        config.messagequeuesize = 2;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_messagedriven_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::core::tointegral(config.priority);
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
    
    static void dummyOnMessage(Thread *t, os::Message m, void *p) {}
    
    static void osal_messagedriven_loop(void *p) 
    {
        MessageThread *t = reinterpret_cast<MessageThread*>(p);
        const osal_reltime_t tout = static_cast<osal_reltime_t>(t->pImpl->config.timeout); // they are the same uint32_t type expressed in usec
        const Thread::fpStartup startup = t->pImpl->config.startup;
        const Thread::fpOnMessage onmessage = t->pImpl->config.onmessage;
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
            os::Message msg = reinterpret_cast<os::Message>(osal_messagequeue_getquick(mq, tout, osal_callerAUTOdetect));
            onmessage(t, msg, param);
        }        
    }
    
};



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

embot::os::MessageThread::MessageThread()
: pImpl(new Impl(this))
{   
    
}


embot::os::MessageThread::~MessageThread()
{   
    delete pImpl;
}


embot::os::Thread::Type embot::os::MessageThread::getType() const
{
    return Type::messageTrigger;
}


embot::os::Priority embot::os::MessageThread::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::os::MessageThread::setPriority(embot::os::Priority priority)
{   
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::core::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::os::MessageThread::setEvent(embot::os::Event event)
{
    return false;
}  


bool embot::os::MessageThread::setMessage(embot::os::Message message, core::relTime timeout)
{
    osal_result_t r = osal_messagequeue_put(pImpl->osalmessagequeue, reinterpret_cast<osal_message_t>(message), static_cast<osal_reltime_t>(timeout), osal_callerAUTOdetect);
    return (osal_res_OK == r) ? (true) : (false);
}

bool embot::os::MessageThread::setCallback(const core::Callback &callback, core::relTime timeout)
{
    return false;
}


bool embot::os::MessageThread::start(const Config &cfg)
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
    pImpl->osaltaskproperties.priority = embot::core::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;  
       
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::os::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}


// - class CallbackThread

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::CallbackThread::Impl
{    
    CallbackThread * taskParent;
    osal_task_t *osaltask;    
    osal_messagequeue_t *osalfunctionqueue;
    osal_messagequeue_t *osalargumentqueue;

    Config config;
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(CallbackThread *parent) 
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
        config.timeout = embot::core::reltimeWaitForever;
        config.queuesize = 2;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_callbackdriven_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::core::tointegral(config.priority);
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
    
    static void dummyAfter(Thread *t, core::Callback &m, void *p) {}
    
    static void osal_callbackdriven_loop(void *p) 
    {
        CallbackThread *t = reinterpret_cast<CallbackThread*>(p);
        const osal_reltime_t tout = static_cast<osal_reltime_t>(t->pImpl->config.timeout); // they are the same uint32_t type expressed in usec
        const Thread::fpStartup startup = t->pImpl->config.startup;
        const Thread::fpAfterCallback after = (nullptr != t->pImpl->config.aftercallback) ? (t->pImpl->config.aftercallback) : (dummyAfter);
        void * param = t->pImpl->config.param;
        osal_messagequeue_t *fQ = t->pImpl->osalfunctionqueue;
        osal_messagequeue_t *aQ = t->pImpl->osalargumentqueue;
        
        embot::core::Callback cbk;;
       

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
            cbk.call = reinterpret_cast<embot::core::fpCaller>(osal_messagequeue_getquick(fQ, tout, osal_callerAUTOdetect));
            // it executes only if it is valid 
            cbk.execute();
            after(t, cbk, param);
        }   

    }
    
};



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::os::CallbackThread::CallbackThread()
: pImpl(new Impl(this))
{   
    
}


embot::os::CallbackThread::~CallbackThread()
{   
    delete pImpl;
}


embot::os::Thread::Type embot::os::CallbackThread::getType() const
{
    return Type::callbackTrigger;
}


embot::os::Priority embot::os::CallbackThread::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::os::CallbackThread::setPriority(embot::os::Priority priority)
{
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::core::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::os::CallbackThread::setEvent(embot::os::Event event)
{
    return false;
}  


bool embot::os::CallbackThread::setMessage(embot::os::Message message, core::relTime timeout)
{
    return false;
}

bool embot::os::CallbackThread::setCallback(const embot::core::Callback &callback, core::relTime timeout)
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



bool embot::os::CallbackThread::start(const Config &cfg)
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
    pImpl->osaltaskproperties.priority = embot::core::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;  
       
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::os::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}


// - class PeriodicThread

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::PeriodicThread::Impl
{    
    PeriodicThread * taskParent;
    osal_task_t *osaltask;    

    Config config;
    
    osal_task_properties_t osaltaskproperties;
    
    Impl(PeriodicThread *parent) 
    {
        taskParent = parent;
        osaltask = nullptr;

        config.startup = nullptr;
        config.onperiod = dummyOnPeriod; // i prefer assigning a dummy value in here. just in case. because to speed things up i dont check pImpl->onevent inside osal_messagedriven_loop()
        config.param = nullptr;
        config.stacksize = 64;
        config.priority = Priority::minimum;
        config.period = embot::core::time1second;
        
        // to be used in osal_task_new1(). only priority and stacksize will change
        osaltaskproperties.function = osal_periodic_loop;
        osaltaskproperties.param = taskParent;
        osaltaskproperties.priority = embot::core::tointegral(config.priority);
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
    
    static void dummyOnPeriod(Thread *t, void *p) {}
    
    static void osal_periodic_loop(void *p) 
    {
        PeriodicThread *t = reinterpret_cast<PeriodicThread*>(p);
        const osal_reltime_t period = static_cast<osal_reltime_t>(t->pImpl->config.period); // they are the same uint32_t type expressed in usec
        const Thread::fpStartup startup = t->pImpl->config.startup;
        const Thread::fpOnPeriod onperiod = t->pImpl->config.onperiod;
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


embot::os::PeriodicThread::PeriodicThread()
: pImpl(new Impl(this))
{   
    
}


embot::os::PeriodicThread::~PeriodicThread()
{   
    delete pImpl;
}


embot::os::Thread::Type embot::os::PeriodicThread::getType() const
{
    return Type::periodicTrigger;
}


embot::os::Priority embot::os::PeriodicThread::getPriority() const
{   
    return pImpl->config.priority;
}


bool embot::os::PeriodicThread::setPriority(embot::os::Priority priority)
{
    pImpl->config.priority = priority;
    pImpl->osaltaskproperties.priority = embot::core::tointegral(priority);

    osal_result_t r = osal_task_priority_set(pImpl->osaltask, pImpl->osaltaskproperties.priority);
    return (osal_res_OK == r) ? (true) : (false);
}

  
bool embot::os::PeriodicThread::setEvent(embot::os::Event event)
{
    return false;
}  


bool embot::os::PeriodicThread::setMessage(embot::os::Message message, core::relTime timeout)
{
    return false;
}

bool embot::os::PeriodicThread::setCallback(const core::Callback &callback, core::relTime timeout)
{
    return false;
}


bool embot::os::PeriodicThread::start(const Config &cfg)
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
    pImpl->osaltaskproperties.priority = embot::core::tointegral(pImpl->config.priority);
    pImpl->osaltaskproperties.stacksize = pImpl->config.stacksize;  
       
    pImpl->osaltask = osal_task_new1(&pImpl->osaltaskproperties);
    // by the following i add a reference of this task to the osal object. it is used in embot::os::taskRunning() 
    osal_task_extdata_set(pImpl->osaltask, this);
    
    return true;    
}



    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

