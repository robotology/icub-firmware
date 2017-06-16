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

#include "EOMtask.h"

#include "osal.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::Task::Impl0
{    
    Task *taskParent;    
    EOMtask *eomtask0;
    void *param;
    Task::fpStartup startupFP0;
    Task::fpOnEvent oneventFP0; 
    Task::fpOnMessage onmessageFP0; 
    Task::fpPeriodicActivity periodicFP0;
    common::Time timeOfLatestTrigger;

    
    Impl0() 
    {
        taskParent = nullptr;
        eomtask0 = eom_task_New1();
        param = nullptr;    
        startupFP0 = nullptr;
        oneventFP0 = nullptr; 
        onmessageFP0 = nullptr;  
        periodicFP0 = nullptr;
        timeOfLatestTrigger = 0;      
    }
    
    ~Impl0()
    {
        eom_task_Delete(eomtask0);
    }
    
    static void startup0(EOMtask *tsk, std::uint32_t zero)
    {
        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
        if(nullptr == t)
        {
            return;
        }
        
        t->pImpl0->timeOfLatestTrigger = timeNow();
        
        if(nullptr != t->pImpl0->startupFP0)
        {
            t->pImpl0->startupFP0(t, t->pImpl0->param); 
        }  
    }
    
    static void eventrun0(EOMtask *tsk, std::uint32_t evtmsgper)
    {
        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
        if(nullptr == t)
        {
            return;
        }
        
        t->pImpl0->timeOfLatestTrigger = timeNow();
        
        if(nullptr != t->pImpl0->oneventFP0)
        {
            t->pImpl0->oneventFP0(t, static_cast<embot::common::EventMask>(evtmsgper), t->pImpl0->param); 
        }    
    }  

    static void messagerun0(EOMtask *tsk, std::uint32_t evtmsgper)
    {
        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
        if(nullptr == t)
        {
            return;
        }
        
        t->pImpl0->timeOfLatestTrigger = timeNow();
        
        if(nullptr != t->pImpl0->onmessageFP0)
        {
            t->pImpl0->onmessageFP0(t, static_cast<embot::common::Message>(evtmsgper), t->pImpl0->param); 
        }    
    }    

    static void periodicrun0(EOMtask *tsk, std::uint32_t evtmsgper)
    {
        Task * t =  static_cast<Task*> (eom_task_GetExternalData(tsk));
        if(nullptr == t)
        {
            return;
        }
        
        t->pImpl0->timeOfLatestTrigger = timeNow();
        
        if(nullptr != t->pImpl0->periodicFP0)
        {
            t->pImpl0->periodicFP0(t, t->pImpl0->param); 
        }    
    }       
};



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

//namespace embot { namespace sys {

// - class Task

embot::sys::Task::Task()
: pImpl0(new Impl0)
{   
    pImpl0->taskParent = nullptr;
}


embot::sys::Task::~Task()
{   
    delete pImpl0;
}


embot::common::Time embot::sys::Task::timeOfTrigger()
{
    return pImpl0->timeOfLatestTrigger;
}


void * embot::sys::Task::getEOMtask()
{
    return pImpl0->eomtask0;
}

void embot::sys::registerNameOfTask(void *p)
{
    eom_task_Start(reinterpret_cast<EOMtask*>(p));    
}


// - class EventTask


embot::sys::EventTask::EventTask()
{
    pImpl0->taskParent = this;    
}


embot::sys::EventTask::EventTask(Task::fpStartup startup, Task::fpOnEvent onevent, std::uint32_t stacksize, Priority priority, common::relTime timeout, void *param, Task::fpNameOfTask nameoftask)
{
    pImpl0->taskParent = this;    
    init(startup, onevent, stacksize, priority, timeout, param, nameoftask);  
}


embot::sys::EventTask::~EventTask()
{
}


embot::sys::Task::Type embot::sys::EventTask::getType()
{
    return Type::eventTrigger;
}


embot::sys::Task::Priority embot::sys::EventTask::getPriority()
{
    std::uint8_t prio = 0;
    eom_task_PriorityGet(pImpl0->eomtask0, &prio);
    
    return prio;
}


bool embot::sys::EventTask::setPriority(Task::Priority &priority)
{    
    eom_task_PrioritySet(pImpl0->eomtask0, priority);
    
    return true;
}

  
bool embot::sys::EventTask::setEvent(embot::common::Event event)
{
    eom_task_SetEvent(pImpl0->eomtask0, event);
    return true;
}  


bool embot::sys::EventTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}


bool embot::sys::EventTask::init(Task::fpStartup startup, Task::fpOnEvent onevent, std::uint32_t stacksize, Priority priority, common::relTime timeout, void *param, Task::fpNameOfTask nameoftask)
{
    pImpl0->startupFP0 = startup;
    pImpl0->oneventFP0 = onevent;
    pImpl0->param = param;
    eom_task_Init1(pImpl0->eomtask0, 
                    eom_mtask_EventDriven, 
                    static_cast<uint8_t>(priority), 
                    static_cast<uint16_t>(stacksize),
                    static_cast<void (*) (EOMtask*, uint32_t)>(pImpl0->startup0),
                    static_cast<void (*) (EOMtask*, uint32_t)>(pImpl0->eventrun0),
                    static_cast<uint32_t>(0), 
                    static_cast<eOreltime_t>(timeout),
                    static_cast<void*>(this), 
                    static_cast<void (*) (void*)>(nameoftask),
                    static_cast<const char *>(NULL)
                   );
    
    return true;    
}



//int EventTask::getPP()
//{   // just a test. you can remove it.    
//    return getPriority();
//}




// - class MessageTask


embot::sys::MessageTask::MessageTask()
{
    pImpl0->taskParent = this;    
}


embot::sys::MessageTask::MessageTask(Task::fpStartup startup, Task::fpOnMessage onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, common::relTime timeout, void *param, Task::fpNameOfTask nameoftask)
{
    pImpl0->taskParent = this;     
    init(startup, onmessage, stacksize, priority, messagequeuecapacity, timeout, param, nameoftask); 
}


embot::sys::MessageTask::~MessageTask()
{
}


embot::sys::Task::Type embot::sys::MessageTask::getType()
{
    return Type::messageTrigger;
}


embot::sys::Task::Priority embot::sys::MessageTask::getPriority()
{
    std::uint8_t prio = 0;
    eom_task_PriorityGet(pImpl0->eomtask0, &prio);    
    return prio;
}


bool embot::sys::MessageTask::setPriority(Task::Priority &priority)
{    
    eom_task_PrioritySet(pImpl0->eomtask0, priority);    
    return true;
}
  
  
bool embot::sys::MessageTask::setEvent(embot::common::Event event)
{
    return false;
}  


bool embot::sys::MessageTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    eom_task_SendMessage(pImpl0->eomtask0, message, timeout);
    return true;
}


bool embot::sys::MessageTask::init(Task::fpStartup startup, Task::fpOnMessage onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, common::relTime timeout, void *param, Task::fpNameOfTask nameoftask)
{
    pImpl0->startupFP0 = startup;
    pImpl0->onmessageFP0 = onmessage;
    pImpl0->param = param;
    //eom_task_Init1(pImpl0->eomtask0, eom_mtask_MessageDriven, priority, stacksize, pImpl0->startup0, pImpl0->messagerun0, messagequeuecapacity, timeout, this, NULL, NULL);        
    eom_task_Init1(pImpl0->eomtask0, 
                    eom_mtask_MessageDriven, 
                    static_cast<uint8_t>(priority), 
                    static_cast<uint16_t>(stacksize),
                    static_cast<void (*) (EOMtask*, uint32_t)>(pImpl0->startup0),
                    static_cast<void (*) (EOMtask*, uint32_t)>(pImpl0->messagerun0),
                    static_cast<uint32_t>(messagequeuecapacity), 
                    static_cast<eOreltime_t>(timeout),
                    static_cast<void*>(this), 
                    static_cast<void (*) (void*)>(nameoftask),
                    static_cast<const char *>(NULL)
                   );
    
    return true;    
}



// - class PeriodicTask

embot::sys::PeriodicTask::PeriodicTask()
{
    pImpl0->taskParent = this;    
}


embot::sys::PeriodicTask::PeriodicTask(Task::fpStartup startup, Task::fpPeriodicActivity periodicactivity, std::uint32_t stacksize, Priority priority, common::relTime period, void *param, Task::fpNameOfTask nameoftask)
{
    pImpl0->taskParent = this;     
    init(startup, periodicactivity, stacksize, priority, period, param, nameoftask); 
}


embot::sys::PeriodicTask::~PeriodicTask()
{
}


embot::sys::Task::Type embot::sys::PeriodicTask::getType()
{
    return Type::periodicTrigger;
}


embot::sys::Task::Priority embot::sys::PeriodicTask::getPriority()
{
    std::uint8_t prio = 0;
    eom_task_PriorityGet(pImpl0->eomtask0, &prio);    
    return prio;
}


bool embot::sys::PeriodicTask::setPriority(Task::Priority &priority)
{    
    eom_task_PrioritySet(pImpl0->eomtask0, priority);    
    return true;
}
  
  
bool embot::sys::PeriodicTask::setEvent(embot::common::Event event)
{
    return false;
}  


bool embot::sys::PeriodicTask::setMessage(embot::common::Message message, common::relTime timeout)
{
    return false;
}


bool embot::sys::PeriodicTask::init(Task::fpStartup startup, Task::fpPeriodicActivity periodicactivity, std::uint32_t stacksize, Priority priority, common::relTime period, void *param, Task::fpNameOfTask nameoftask)
{
    pImpl0->startupFP0 = startup;
    pImpl0->periodicFP0 = periodicactivity;
    pImpl0->param = param;
    //eom_task_Init1(pImpl0->eomtask0, eom_mtask_Periodic, priority, stacksize, pImpl0->startup0, pImpl0->periodicrun0, 0, period, this, NULL, NULL);        
    eom_task_Init1(pImpl0->eomtask0, 
                    eom_mtask_Periodic, 
                    static_cast<uint8_t>(priority), 
                    static_cast<uint16_t>(stacksize),
                    static_cast<void (*) (EOMtask*, uint32_t)>(pImpl0->startup0),
                    static_cast<void (*) (EOMtask*, uint32_t)>(pImpl0->periodicrun0),
                    static_cast<uint32_t>(0), 
                    static_cast<eOreltime_t>(period),
                    static_cast<void*>(this), 
                    static_cast<void (*) (void*)>(nameoftask),
                    static_cast<const char *>(NULL)
                   );
    
    return true;    
}

//}} // namespace embot { namespace sys {
    
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

