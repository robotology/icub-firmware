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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_SYS_TASK_H_
#define _EMBOT_SYS_TASK_H_

#include "embot_common.h"

#include "embot_sys.h"

namespace embot { namespace sys {
        

    void registerNameOfTask(void *p); 
    
    class Task
    {
    public:
        
        enum class Type { undefined = -1, eventTrigger = 1, messageTrigger = 3, periodicTrigger = 5 };
        typedef std::uint8_t Priority; // lower is ... lower 
        
        using fpNameOfTask = void (*)(void *);
        using fpStartup = void (*)(Task *, void *);                
        using fpOnEvent = void (*)(Task *, common::EventMask, void *);
        using fpOnMessage = void (*)(Task *, common::Message, void *);
        using fpPeriodicActivity = void (*)(Task *, void *);
                             
        Task();
        virtual ~Task();
        
        virtual Type getType() = 0;        
        virtual Priority getPriority() = 0;
        virtual bool setPriority(Priority &priority) = 0;
        
        virtual bool setEvent(common::Event event) = 0;
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever) = 0;
                
        void* getEOMtask();        
        common::Time timeOfTrigger();
        
    protected:        
        struct Impl0;
        Impl0 *pImpl0;     
    };  

    
    class EventTask: public Task
    {
    public:
                    
        EventTask();
        EventTask(Task::fpStartup startup, Task::fpOnEvent onevent, std::uint32_t stacksize, Priority priority, common::relTime timeout, void *param = nullptr, fpNameOfTask nameoftask = nullptr);
        ~EventTask();
    
        bool init(Task::fpStartup startup, Task::fpOnEvent onevent, std::uint32_t stacksize, Priority priority, common::relTime timeout, void *param = nullptr, fpNameOfTask nameoftask = nullptr);
    
        virtual Type getType();
        virtual Priority getPriority();
        virtual bool setPriority(Priority &priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);

           
    };
        

    class MessageTask: public Task
    {
    public:
        
        MessageTask();
        MessageTask(Task::fpStartup startup, Task::fpOnMessage onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, common::relTime timeout, void *param = nullptr, fpNameOfTask nameoftask = nullptr);
        ~MessageTask();
    
        bool init(Task::fpStartup startup, Task::fpOnMessage onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, common::relTime timeout, void *param = nullptr, fpNameOfTask nameoftask = nullptr);
    
        virtual Type getType();
        virtual Priority getPriority();
        virtual bool setPriority(Priority &priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);

           
    };

    
    class PeriodicTask: public Task
    {
    public:
        
        PeriodicTask();
        PeriodicTask(Task::fpStartup startup, Task::fpPeriodicActivity periodicactivity, std::uint32_t stacksize, Priority priority, common::relTime period, void *param = nullptr, fpNameOfTask nameoftask = nullptr);
        ~PeriodicTask();
    
        bool init(Task::fpStartup startup, Task::fpPeriodicActivity periodicactivity, std::uint32_t stacksize, Priority priority, common::relTime period, void *param = nullptr, fpNameOfTask nameoftask = nullptr);
    
        virtual Type getType();
        virtual Priority getPriority();
        virtual bool setPriority(Priority &priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);

           
    };
    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
