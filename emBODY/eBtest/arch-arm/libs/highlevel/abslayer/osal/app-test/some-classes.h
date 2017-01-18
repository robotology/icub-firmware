


/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _SOMECLASSES_H_
#define _SOMECLASSES_H_



#include <cstdint>


namespace embot 
{
    typedef std::uint64_t Time;     // expressed in usec. it expresses the time of the system since start of the embot::sys::Scheduler.
    
    using callbackFptr = void (*)(void *);
    using workerFptr = void (*)(void);

    typedef std::uint32_t relTime;  // expressed in usec. it is used to express relative time. 0 means: 0 usec from ...
    
    extern const relTime timeWaitNone;
    extern const relTime time1millisec;
    extern const relTime time1second;
    extern const relTime timeWaitForever;
   
}


namespace embot 
{
    namespace sys 
    {
             
    class Task;
        
                
    Time timeNow();    
    Task* taskRunning();
        
        
    class theScheduler
    {
    public:
        static theScheduler& getInstance()
        {
            static theScheduler* p = new theScheduler();
            return *p;
        }
        
    public:
        struct Config
        {
            std::uint32_t    clockfrequency;
            relTime     ticktime;
            workerFptr  launcher;
            std::uint16_t    launcherstacksize;
            workerFptr  onidle;
            std::uint16_t    onidlestacksize;
            workerFptr  onfatalerror;     
            Config() 
            {
                clockfrequency = 168000000; ticktime = 1000; 
                launcher = nullptr; launcherstacksize = 1024; 
                onidle = nullptr; onidlestacksize = 512;
                onfatalerror = nullptr;
            }
            void setTiming(std::uint32_t _clockfrequency, relTime _ticktime = 1000) { clockfrequency = _clockfrequency; ticktime = _ticktime; }
            void setLauncher(workerFptr _launcher, std::uint16_t _launcherstacksize = 1024) { launcher = _launcher; launcherstacksize = _launcherstacksize; }
            void setOnIdle(workerFptr _onidle, std::uint16_t _onidlestacksize = 512) { onidle = _onidle; onidlestacksize = _onidlestacksize; }
            void setOnFatal(workerFptr _onfatalerror) { onfatalerror = _onfatalerror; }
        }; 
        
        bool init(Config &config);  // can be called many times but not after start(). if so, it does nothing
        
        void start();    // it does not return ... unless the scheduler is already started
        
        bool isStarted();


    private:
        theScheduler();  

    public:
        // remove copy constructors
        theScheduler(const theScheduler&) = delete;
        theScheduler(theScheduler&) = delete;
        // remove copy assignment operator
        void operator=(const theScheduler&) = delete;
        void operator=(theScheduler&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };   
    
    
    class theTimerManager
    {
    public:
        static theTimerManager& getInstance()
        {
            static theTimerManager* p = new theTimerManager();
            return *p;
        }
        
    public:
        struct Config
        {
            std::uint8_t        priority;
            std::uint16_t       stacksize;
            std::uint16_t       capacityofhandler;          
            Config() 
            {
                priority = 240;
                stacksize = 1024;
                capacityofhandler = 8;
            }
        }; 
        
        bool init(Config &config);
        
        bool start();    

    private:
        theTimerManager();  

    public:
        // remove copy constructors
        theTimerManager(const theTimerManager&) = delete;
        theTimerManager(theTimerManager&) = delete;
        // remove copy assignment operator
        void operator=(const theTimerManager&) = delete;
        void operator=(theTimerManager&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };   
    

    
    class theCallbackManager
    {
    public:
        static theCallbackManager& getInstance()
        {
            static theCallbackManager* p = new theCallbackManager();
            return *p;
        }
        
    public:
        struct Config
        {
            std::uint8_t        priority;
            std::uint16_t       stacksize;
            std::uint16_t       capacityofhandler;          
            Config() 
            {
                priority = 240;
                stacksize = 1024;
                capacityofhandler = 8;
            }
        }; 
        
        bool init(Config &config);
        
        bool start();    

    private:
        theCallbackManager();  

    public:
        // remove copy constructors
        theCallbackManager(const theCallbackManager&) = delete;
        theCallbackManager(theCallbackManager&) = delete;
        // remove copy assignment operator
        void operator=(const theCallbackManager&) = delete;
        void operator=(theCallbackManager&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };  

    

    class Task
    {
    public:
        
        enum class Type { undefined = -1, eventTrigger = 1, messageTrigger = 3, periodicTrigger = 5 };
        typedef std::uint8_t Priority; // lower is ... lower 
        typedef std::uint32_t Event;
        typedef std::uint32_t Message;
        
        using startupFptr = void (*)(Task *, void *);                
        using oneventFptr = void (*)(Task *, Event, void *);
        using onmessageFptr = void (*)(Task *, Message, void *);
        using periodicFptr = void (*)(Task *, void *);
                             
        Task();
        virtual ~Task();
        
        virtual Type getType() = 0;        
        virtual Priority getPriority() = 0;
        virtual bool setPriority(Priority &priority) = 0;
        
        virtual bool setEvent(Event event) = 0;
        virtual bool setMessage(Message message, relTime timeout = timeWaitForever) = 0;
        
        void* getEOMtask();
        
        Time timeOfTrigger();
        
    protected:        
        struct Impl0;
        Impl0 *pImpl0;     
    };  

    class EventTask: public Task
    {
    public:
                    
        EventTask();
        EventTask(Task::startupFptr startup, Task::oneventFptr onevent, std::uint32_t stacksize, Priority priority, relTime timeout, void *param = nullptr);
        ~EventTask();
    
        bool init(Task::startupFptr startup, Task::oneventFptr onevent, std::uint32_t stacksize, Priority priority, relTime timeout, void *param = nullptr);
    
        virtual Type getType();
        virtual Priority getPriority();
        virtual bool setPriority(Priority &priority);
        
        virtual bool setEvent(Event event);  
        virtual bool setMessage(Message message, relTime timeout = timeWaitForever);
           
    };
        

    class MessageTask: public Task
    {
    public:
        
        MessageTask();
        MessageTask(Task::startupFptr startup, Task::onmessageFptr onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, relTime timeout, void *param = nullptr);
        ~MessageTask();
    
        bool init(Task::startupFptr startup, Task::onmessageFptr onmessage, std::uint32_t stacksize, Priority priority, std::uint8_t messagequeuecapacity, relTime timeout, void *param = nullptr);
    
        virtual Type getType();
        virtual Priority getPriority();
        virtual bool setPriority(Priority &priority);
        
        virtual bool setEvent(Event event);  
        virtual bool setMessage(Message message, relTime timeout = timeWaitForever);
           
    };

    class PeriodicTask: public Task
    {
    public:
        
        PeriodicTask();
        PeriodicTask(Task::startupFptr startup, Task::periodicFptr periodicactivity, std::uint32_t stacksize, Priority priority, relTime period, void *param = nullptr);
        ~PeriodicTask();
    
        bool init(Task::startupFptr startup, Task::periodicFptr periodicactivity, std::uint32_t stacksize, Priority priority, relTime period, void *param = nullptr);
    
        virtual Type getType();
        virtual Priority getPriority();
        virtual bool setPriority(Priority &priority);
        
        virtual bool setEvent(Event event);  
        virtual bool setMessage(Message message, relTime timeout = timeWaitForever);
           
    };
    
    
    struct Action
    {
        enum class Type { none = 0, event2task = 1, message2task = 2, executecallback = 3 };
        
        struct EventToTask
        {
            Task::Event     event;
            Task*           task;            
        };
        
        struct MessageToTask
        {
            Task::Message   message;
            Task*           task;            
        };
        
        struct ExecuteCallback
        {
            callbackFptr    callback;
            void*           arg;
            Task*           task;            
        };  
        
        Type            type;
        union
        {
            EventToTask     evt;
            MessageToTask   msg;
            ExecuteCallback cbk;
        };
        
        
        Action(){ clear(); } 
        
        void clear() { type = Type::none; }
        
        void set(EventToTask &e)
        {
            type = Type::event2task;
            evt = e;           
        }
        
        void set(MessageToTask &m)
        {
            type = Type::message2task;
            msg = m;           
        }
        
        void set(ExecuteCallback &c)
        {
            type = Type::executecallback;
            cbk = c;           
        }
    };
    
    
    class Timer
    {
    public:
        
        enum class Type { oneshot = 0, forever = 1 };

        enum class Status { idle = 0, counting = 1, oneshotcompleted = 2 };
        
        
        Timer();
        ~Timer();
        bool start(relTime countdown, Type type, Action &onexpiry); 
        bool stop(); 
        
        Status getStatus();
        
        Type getType();

    private:
        
        struct Impl;
        Impl *pImpl;    
    };

    }
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


