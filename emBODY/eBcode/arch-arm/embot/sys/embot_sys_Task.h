/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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
        
    
    class Task
    {
    public:
        
        // common types
        enum class Type { undefined = -1, eventTrigger = 1, messageTrigger = 3, callbackTrigger = 4, periodicTrigger = 5 };
        
        struct Priority
        {
            std::uint8_t value;
            static const std::uint8_t minimum = 2;
            static const std::uint8_t maximum = 251;
            Priority(std::uint8_t v) { value = v; }
            Priority() { value = Priority::minimum; }
            bool isvalid() const { if((value < Priority::minimum) || (value > Priority::maximum)) { return false; } else { return true; } }
        };
        
        using fpStartup = void (*)(Task *, void *);
        using fpOnEvent = void (*)(Task *, common::EventMask, void *);
        using fpOnMessage = void (*)(Task *, common::Message, void *);
        using fpActivity = void (*)(Task *, void *);
        using fpAfterCallback = void (*)(Task *, common::Callback &, void *);
  
                                          
        // interface
        virtual Type getType() const = 0;        
        virtual Priority getPriority() const = 0;
        virtual bool setPriority(Priority priority) = 0;
        
        virtual bool setEvent(common::Event event) = 0;
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever) = 0;
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever) = 0;
        
    public:
        virtual ~Task() {};                        
    };  

    
    class EventTask: public Task
    {
    public:
               
        struct Config
        {
            std::uint16_t stacksize;    // the stack used by the task. it cannot be 0. reasonable value is >= 256
            Priority priority;          // the priority which which the system executes the task. 
            common::relTime timeout;    // the timeout with which the task waits an event. in case of timeout onevent() is called with event mask = 0.
            
            Task::fpStartup startup;    // this function, if not nullptr, is executed only one at start ot the task. its second argument is param
            Task::fpOnEvent onevent;    // this function, must not be nullptr, is executed at every event or at expiry of timeout. its second argument is the event mask, its third is param
            void *param;                // the optional param passed to startup() and onevent()
  
            Config() { stacksize = 256; priority = Priority::minimum; timeout = embot::common::timeWaitForever; startup = nullptr; onevent = nullptr; param = nullptr; }
            bool isvalid() const
            { 
                if((nullptr == onevent) || (0 == stacksize) || (false == priority.isvalid())) { return false; }  
                else { return true; }
            }
        };
                    
        EventTask();
        virtual ~EventTask();
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever);
        
        
        bool start(const Config &cfg);    

    private:        
        struct Impl;
        Impl *pImpl;           
    };
    
    
    // vedere come gestire il caso di messaggio = 0 e di timeout. ad esempio possimao invalidare il messaggio 0. oppure ... 
    class MessageTask: public Task
    {
    public:                
        
        struct Config
        {
            std::uint16_t stacksize;        // the stack used by the task. it cannot be 0. reasonable value is >= 256
            Priority priority;              // the priority which which the system executes the task. 
            common::relTime timeout;        // the timeout with which the task waits a message. in case of timeout onmessage() is called with message = 0.
            std::uint8_t messagequeuesize;  // the size of the message queue. if higher the task can accepr more message.     
            
            Task::fpStartup startup;        // this function, if not nullptr, is executed only one at start ot the task. its second argument is param
            Task::fpOnMessage onmessage;    // this function, must not be nullptr, is executed at every received message or at expiry of timeout. its second argument is the received message, its third is param
            void *param;                    // the optional param passed to startup() and onmessage()

            Config() { stacksize = 256; priority = Priority::minimum; timeout = embot::common::timeWaitForever; messagequeuesize = 4; startup = nullptr; onmessage = nullptr; param = nullptr; }
            bool isvalid() const
            { 
                if((nullptr == onmessage) || (0 == stacksize) || (false == priority.isvalid()) || (0 == messagequeuesize)) { return false; }  
                else { return true; }
            }
        };
                    
        MessageTask();
        virtual ~MessageTask();
          
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever);
        
        
        bool start(const Config &cfg);
    
    private:        
        struct Impl;
        Impl *pImpl;           
    };
    
    class CallbackTask: public Task
    {
    public:
        
        struct Config
        {
            std::uint16_t stacksize;            // the stack used by the task. it cannot be 0. reasonable value is >= 256
            Priority priority;                  // the priority which which the system executes the task. 
            common::relTime timeout;            // the timeout with which the task waits for a callback. 
            std::uint8_t queuesize;             // the size of the queue of callbacks. if higher the task can accept more callbacks at the same time.     
            
            Task::fpStartup startup;            // this function, if not nullptr, is executed only one at start ot the task. its second argument is param
            Task::fpAfterCallback aftercallback;// this funtion, if not nullptr is executed after every time a callback is executed. its third argument is param 
            void *param;                        // the optional param passed to startup() and xxx()

            Config() { stacksize = 256; priority = Priority::minimum; timeout = embot::common::timeWaitForever; queuesize = 4; startup = nullptr; aftercallback =  nullptr; param = nullptr; }
            bool isvalid() const
            { 
                if((0 == stacksize) || (false == priority.isvalid()) || (0 == queuesize)) { return false; }  
                else { return true; }
            }
        };
                    
        CallbackTask();
        virtual ~CallbackTask();
        
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);        
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever);
        
                
        bool start(const Config &cfg);
    
    private:        
        struct Impl;
        Impl *pImpl;           
    };    
        
    class PeriodicTask: public Task
    {
    public:
                
        struct Config
        {
            std::uint16_t stacksize;    // the stack used by the task. it cannot be 0. reasonable value is >= 256
            Priority priority;          // the priority which which the system executes the task. 
            common::relTime period;     // the period  
            
            Task::fpStartup startup;    // this function, if not nullptr, is executed only one at start ot the task. its second argument is param
            Task::fpActivity activity;  // this function, must not be nullptr, is executed at every period. its second argument is param
            void *param;                // the optional param passed to startup() and activity()

            Config() { stacksize = 256; priority = Priority::minimum; period = embot::common::time1second; startup = nullptr; activity = nullptr; param = nullptr; }
            bool isvalid() const
            { 
                if((nullptr == activity) || (0 == stacksize) || (false == priority.isvalid()) || (0 == period)) { return false; }  
                else { return true; }
            }
        };
                    
        PeriodicTask();
        virtual ~PeriodicTask();
       
        bool start(const Config &cfg);
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever);

    private:        
        struct Impl;
        Impl *pImpl;           
    };
    
    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
