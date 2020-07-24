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

#ifndef _EMBOT_OS_THREAD_H_
#define _EMBOT_OS_THREAD_H_

#include "embot_core.h"
#include "embot_os_common.h"
#include <memory>


namespace embot { namespace os {
            
    class Thread
    {
    public:
        
        // types used by all derived Thread classes
    
        enum class Type : std::uint8_t { undefined = 0, eventTrigger = 1, messageTrigger = 3, callbackTrigger = 4, periodicTrigger = 5, Init = 100, Idle = 101 };
                                
        using fpStartup = void (*)(Thread *, void *);
        using fpOnIdle = void (*)(Thread *, void *);
        using fpOnEvent = void (*)(Thread *, os::EventMask, void *);
        using fpOnMessage = void (*)(Thread *, os::Message, void *);
        using fpOnPeriod = void (*)(Thread *, void *);
        using fpAfterCallback = void (*)(Thread *, core::Callback &, void *);
        
        struct BaseConfig
        {
            std::uint16_t stacksize {256};          // the stack used by the task. it cannot be 0. reasonable value is >= 256
            Priority priority {Priority::minimum};  // the priority with which the system executes the task. 
            Thread::fpStartup startup {nullptr};    // this function, if not nullptr, is executed only once at start ot the task. its second argument is param
            void *param {nullptr};                  // the optional param passed to startup() and other functions derived from BaseConfig
            const char * name {nullptr};
            BaseConfig() = default;
            constexpr BaseConfig(std::uint16_t st, Priority pr, Thread::fpStartup fpst, void* pa, const char * n = nullptr) : stacksize(st), priority(pr), startup(fpst), param(pa), name(n) {}
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
        virtual const char * getName() const = 0;
        virtual void run() = 0;
        
        virtual bool setEvent(os::Event event) = 0;
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever) = 0;
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever) = 0;
        
    public:
        virtual ~Thread() {};                        
    };  


    class InitThread: public Thread
    {
    public:
        
        struct Config : public Thread::BaseConfig
        {            
            Config() = default;
            constexpr Config(std::uint16_t st, Thread::fpStartup fpst, void* pa) : BaseConfig(st, embot::os::Priority::schedInit, fpst, pa) {}            
            bool isvalid() const
            {   
                return BaseConfig::isvalid();
            }
        };
                             
        static InitThread& getInstance();        
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        virtual const char * getName() const;
        virtual void run();
        
        virtual bool setEvent(os::Event event);  
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever);
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever);
        
        void synch(); // only the scheduler can call this method
        
        // 
        void terminate();
        bool isterminated() const;
        
        void releaseresources();
        bool resourcesarereleased() const;
                
    private:
        InitThread();    
        virtual ~InitThread();

    private:        
        struct Impl;
        std::unique_ptr<Impl> pImpl;           
    };

    
    class IdleThread: public Thread
    {
    public:
        
        struct Config : public Thread::BaseConfig
        {            
            Thread::fpOnIdle onidle {nullptr}; // this function, must not be nullptr, is executed continuosuly when the system is idle. its second argument is param

            Config() = default;
            constexpr Config(std::uint16_t st, Thread::fpStartup fpst, void* pa, Thread::fpOnIdle fpon) : BaseConfig(st, embot::os::Priority::schedIdle, fpst, pa), onidle(fpon) {}            
            bool isvalid() const
            {   // onidle can be nullptr
                return BaseConfig::isvalid();
            }
        };
                             
        static IdleThread& getInstance();        
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        virtual const char * getName() const;
        virtual void run();
        
        virtual bool setEvent(os::Event event);  
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever);
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever);
        
        void synch(); // only the scheduler can call this method
        
    private:
        IdleThread();    
        virtual ~IdleThread();

    private:        
        struct Impl;
        std::unique_ptr<Impl> pImpl;           
    };
    
    
    
    class EventThread: public Thread
    {
    public:
        
        struct Config : public Thread::BaseConfig
        {
            core::relTime timeout {embot::core::reltimeWaitForever}; // the timeout with which the task waits an event. in case of timeout onevent() is called with event mask = 0.            
            Thread::fpOnEvent onevent {nullptr}; // this function, must not be nullptr, is executed at every event or at expiry of timeout. its second argument is the event mask, its third is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Thread::fpStartup fpst, void* pa, core::relTime ti, Thread::fpOnEvent fpon, const char * name = nullptr) : BaseConfig(st, pr, fpst, pa, name), timeout(ti), onevent(fpon) {}            
            bool isvalid() const
            {   // onevent cannot be nullptr
                if((nullptr == onevent)) { return false; }  
                else { return BaseConfig::isvalid(); }
            }
        };
                    
        EventThread();
        virtual ~EventThread();
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        virtual const char * getName() const;
        
        virtual bool setEvent(os::Event event);  
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever);
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever);
                 
        bool start(const Config &cfg, embot::core::fpCaller eviewername = nullptr);
        virtual void run();    

    private:        
        struct Impl;
        Impl *pImpl;           
    };
    
    
    // vedere come gestire il caso di messaggio = 0 e di timeout. ad esempio possiamo invalidare il messaggio 0. oppure ... 
    class MessageThread: public Thread
    {
    public:  

        struct Config : public Thread::BaseConfig
        {
            core::relTime timeout {embot::core::reltimeWaitForever}; // the timeout with which the task waits an event. in case of timeout onevent() is called with event mask = 0.            
            std::uint8_t messagequeuesize {4}; // the size of the message queue. if higher the task can accept more messages.
            Thread::fpOnMessage onmessage {nullptr}; // this function, must not be nullptr, is executed at every received message or at expiry of timeout. its second argument is the received message, its third is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Thread::fpStartup fpst, void* pa, core::relTime ti, uint8_t qs, Thread::fpOnMessage fpon, const char * name = nullptr) : BaseConfig(st, pr, fpst, pa, name), timeout(ti), messagequeuesize(qs), onmessage(fpon) {}            
            bool isvalid() const
            {   // onmessage cannot be nullptr, and messagequeuesize cannot be 0
                if((nullptr == onmessage) || (0 == messagequeuesize)) { return false; } 
                else { return BaseConfig::isvalid(); }
            }
        };    
                            
        MessageThread();
        virtual ~MessageThread();
          
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        virtual const char * getName() const;
        
        virtual bool setEvent(os::Event event);  
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever);
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever);
         
        bool start(const Config &cfg, embot::core::fpCaller eviewername = nullptr);
        virtual void run();
    
    private:        
        struct Impl;
        Impl *pImpl;           
    };
 
    
    class CallbackThread: public Thread
    {
    public:
        
    
        struct Config : public Thread::BaseConfig
        {
            core::relTime timeout {embot::core::reltimeWaitForever}; // the timeout with which the task waits for a callback.            
            std::uint8_t queuesize {4};  // the size of the queue of callbacks. if higher the task can accept more callbacks at the same time.
            Thread::fpAfterCallback aftercallback {nullptr}; // this function, if not nullptr is executed after every time a callback is executed. its third argument is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Thread::fpStartup fpst, void* pa, core::relTime ti, uint8_t qs, Thread::fpAfterCallback fpaf, const char * name = nullptr) : BaseConfig(st, pr, fpst, pa, name), timeout(ti), queuesize(qs), aftercallback(fpaf) {}            
            bool isvalid() const
            {   // queuesize cannot be 0
                if((0 == queuesize)) { return false; } 
                else { return BaseConfig::isvalid(); }
            }
        };  
                    
        CallbackThread();
        virtual ~CallbackThread();
        
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        virtual const char * getName() const;
        
        virtual bool setEvent(os::Event event);  
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever);        
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever);
        

        bool start(const Config &cfg, embot::core::fpCaller eviewername = nullptr);
        virtual void run();
    
    private:        
        struct Impl;
        Impl *pImpl;           
    };    
 
    
    class PeriodicThread: public Thread
    {
    public:
           
        struct Config : public Thread::BaseConfig
        {
            core::relTime period {embot::core::time1second}; // the period.
            Thread::fpOnPeriod onperiod {nullptr}; // this function, must not be nullptr, is executed at every period. its second argument is param

            Config() = default;
            constexpr Config(std::uint16_t st, Priority pr, Thread::fpStartup fpst, void* pa, core::relTime ti, Thread::fpOnPeriod fpon, const char * name = nullptr) : BaseConfig(st, pr, fpst, pa, name), period(ti), onperiod(fpon) {}            
            bool isvalid() const
            {   // onperiod cannot be nullptr, period cannot be 0
                if((nullptr == onperiod) || (0 == period)) { return false; }  
                else { return BaseConfig::isvalid(); }
            }
        };      
                                    
        PeriodicThread();
        virtual ~PeriodicThread();
         
        bool start(const Config &cfg, embot::core::fpCaller eviewername = nullptr);
        virtual void run();
    
        virtual Type getType() const;
        virtual Priority getPriority() const;
        virtual bool setPriority(Priority priority);
        virtual const char * getName() const;
        
        virtual bool setEvent(os::Event event);  
        virtual bool setMessage(os::Message message, core::relTime timeout = core::reltimeWaitForever);
        virtual bool setCallback(const core::Callback &callback, core::relTime timeout = core::reltimeWaitForever);

    private:        
        struct Impl;
        Impl *pImpl;           
    };
    
    
}} // namespace embot { namespace os {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
