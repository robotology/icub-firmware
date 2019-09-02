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
#include <memory>


namespace embot { namespace sys {
            
    class Task
    {
    public:
        
        // types used by all derived Task classes
    
        enum class Type : std::uint8_t { undefined = 0, eventTrigger = 1, messageTrigger = 3, callbackTrigger = 4, periodicTrigger = 5, Init = 100, Idle = 101 };
                                
        using fpStartup = void (*)(Task *, void *);
        using fpOnIdle = void (*)(Task *, void *);
        using fpOnEvent = void (*)(Task *, common::EventMask, void *);
        using fpOnMessage = void (*)(Task *, common::Message, void *);
        using fpOnPeriod = void (*)(Task *, void *);
        using fpAfterCallback = void (*)(Task *, common::Callback &, void *);
        
        struct BaseConfig
        {
            std::uint16_t stacksize {256};          // the stack used by the task. it cannot be 0. reasonable value is >= 256
            Priority priority {Priority::minimum};  // the priority with which the system executes the task. 
            Task::fpStartup startup {nullptr};      // this function, if not nullptr, is executed only once at start ot the task. its second argument is param
            void *param {nullptr};                  // the optional param passed to startup() and other functions derived from BaseConfig
            BaseConfig() = default;
            constexpr BaseConfig(std::uint16_t st, Priority pr, Task::fpStartup fpst, void* pa) : stacksize(st), priority(pr), startup(fpst), param(pa) {}
            bool isvalid() const 
            {   // startup and param can be nullptr
                if((0 == stacksize)) { return false; } 
                else { return true; }
            }
        };    
  
                                          
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


    class InitTask: public Task
    {
    public:
        
        struct Config : public Task::BaseConfig
        {            
            Config() = default;
            constexpr Config(std::uint16_t st, Task::fpStartup fpst, void* pa) : BaseConfig(st, embot::sys::Priority::schedInit, fpst, pa) {}            
            bool isvalid() const
            {   
                return BaseConfig::isvalid();
            }
        };
                             
        static InitTask& getInstance();        
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever);
        
        void synch(); // only the scheduler can call this method
                
    private:
        InitTask();    
        virtual ~InitTask();

    private:        
        struct Impl;
        std::unique_ptr<Impl> pImpl;           
    };

    
    class IdleTask: public Task
    {
    public:
        
        struct Config : public Task::BaseConfig
        {            
            Task::fpOnIdle onidle {nullptr}; // this function, must not be nullptr, is executed continuosuly when the system is idle. its second argument is param

            Config() = default;
            constexpr Config(std::uint16_t st, Task::fpStartup fpst, void* pa, Task::fpOnIdle fpon) : BaseConfig(st, embot::sys::Priority::schedIdle, fpst, pa), onidle(fpon) {}            
            bool isvalid() const
            {   // onidle can be nullptr
                return BaseConfig::isvalid();
            }
        };
                             
        static IdleTask& getInstance();        
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        
        virtual bool setEvent(common::Event event);  
        virtual bool setMessage(common::Message message, common::relTime timeout = common::timeWaitForever);
        virtual bool setCallback(const common::Callback &callback, common::relTime timeout = common::timeWaitForever);
        
        void synch(); // only the scheduler can call this method
        
    private:
        IdleTask();    
        virtual ~IdleTask();

    private:        
        struct Impl;
        std::unique_ptr<Impl> pImpl;           
    };
    
    
    
    class EventTask: public Task
    {
    public:
        
        struct Config : public Task::BaseConfig
        {
            common::relTime timeout {embot::common::timeWaitForever}; // the timeout with which the task waits an event. in case of timeout onevent() is called with event mask = 0.            
            Task::fpOnEvent onevent {nullptr}; // this function, must not be nullptr, is executed at every event or at expiry of timeout. its second argument is the event mask, its third is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Task::fpStartup fpst, void* pa, common::relTime ti, Task::fpOnEvent fpon) : BaseConfig(st, pr, fpst, pa), timeout(ti), onevent(fpon) {}            
            bool isvalid() const
            {   // onevent cannot be nullptr
                if((nullptr == onevent)) { return false; }  
                else { return BaseConfig::isvalid(); }
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
    
    
    // vedere come gestire il caso di messaggio = 0 e di timeout. ad esempio possiamo invalidare il messaggio 0. oppure ... 
    class MessageTask: public Task
    {
    public:  

        struct Config : public Task::BaseConfig
        {
            common::relTime timeout {embot::common::timeWaitForever}; // the timeout with which the task waits an event. in case of timeout onevent() is called with event mask = 0.            
            std::uint8_t messagequeuesize {4}; // the size of the message queue. if higher the task can accept more messages.
            Task::fpOnMessage onmessage {nullptr}; // this function, must not be nullptr, is executed at every received message or at expiry of timeout. its second argument is the received message, its third is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Task::fpStartup fpst, void* pa, common::relTime ti, uint8_t qs, Task::fpOnMessage fpon) : BaseConfig(st, pr, fpst, pa), timeout(ti), messagequeuesize(qs), onmessage(fpon) {}            
            bool isvalid() const
            {   // onmessage cannot be nullptr, and messagequeuesize cannot be 0
                if((nullptr == onmessage) || (0 == messagequeuesize)) { return false; } 
                else { return BaseConfig::isvalid(); }
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
        
    
        struct Config : public Task::BaseConfig
        {
            common::relTime timeout {embot::common::timeWaitForever}; // the timeout with which the task waits for a callback.            
            std::uint8_t queuesize {4};  // the size of the queue of callbacks. if higher the task can accept more callbacks at the same time.
            Task::fpAfterCallback aftercallback {nullptr}; // this function, if not nullptr is executed after every time a callback is executed. its third argument is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Task::fpStartup fpst, void* pa, common::relTime ti, uint8_t qs, Task::fpAfterCallback fpaf) : BaseConfig(st, pr, fpst, pa), timeout(ti), queuesize(qs), aftercallback(fpaf) {}            
            bool isvalid() const
            {   // queuesize cannot be 0
                if((0 == queuesize)) { return false; } 
                else { return BaseConfig::isvalid(); }
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
           
        struct Config : public Task::BaseConfig
        {
            common::relTime period {embot::common::time1second}; // the period.
            Task::fpOnPeriod onperiod {nullptr}; // this function, must not be nullptr, is executed at every period. its second argument is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Task::fpStartup fpst, void* pa, common::relTime ti, Task::fpOnPeriod fpon) : BaseConfig(st, pr, fpst, pa), period(ti), onperiod(fpon) {}            
            bool isvalid() const
            {   // onperiod cannot be nullptr, period cannot be 0
                if((nullptr == onperiod) || (0 == period)) { return false; }  
                else { return BaseConfig::isvalid(); }
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
