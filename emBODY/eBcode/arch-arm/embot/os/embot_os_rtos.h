
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_OS_RTOS_H_
#define _EMBOT_OS_RTOS_H_

#include "embot_os.h"

// so far the default is for OSAL
#if !defined(EMBOT_USE_rtos_cmsisos2) & !defined(EMBOT_USE_rtos_osal)
    #define EMBOT_USE_rtos_osal
#endif    


namespace embot { namespace os { namespace rtos {
    
    using thread_t = void; 
    using messagequeue_t = void; 
    using timer_t = void;
    using mutex_t = void;
    using semaphore_t = void;
   
    // -- scheduler section
                
    struct scheduler_props_t
    {
        using fpOnOSerror = void (*) (thread_t* thread, int errorcode, const char * errormsg);
        
        scheduler_props_t(); 
        ~scheduler_props_t();  
                     
        void prepare(embot::core::Time ticktime, 
                     uint16_t initstacksize, uint16_t idlestacksize, 
                     embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror);
        
        bool isvalid() const;
        
        enum class Resource { InitMemory = 0 };
        bool release(Resource res = Resource::InitMemory);
        
        // this is used internally by those who know how to do a reinterpret_cast<>
        void * getInternals();
                
    private:        
        struct Impl;
        Impl *pImpl; 
    };
    
    
    // it initialises the scheduler according to scheduler_props_t. it returns false if the props are not valid 
    bool scheduler_init(scheduler_props_t &props);
    
    // it starts the scheduler. it executes the user-defined function oninit() specified by props 
    // and when it returns, it schedules all other threads that oninit() may have started
    void scheduler_start();
    void scheduler_start2();
        
    // it tells if the scheduler is running
    bool scheduler_isrunning();
    
    // it retrieves the time of the scheduler.
    embot::core::Time scheduler_timeget();   
    

    // it tells which thread is actually running   
    thread_t * scheduler_thread_running();    
    
    // it associates the rtos thread to the embot::os::Thread
    void scheduler_associate(thread_t *osthread, embot::os::Thread *t);   
    // and it deassociate it.
    void scheduler_deassociate(thread_t *osthread, embot::os::Thread *t);
    // it gets the embot::os::Thread from the rtos thread
    embot::os::Thread * scheduler_getassociated(thread_t *osthread);
    
    void scheduler_lock();
    void scheduler_unlock();
    
    // -- thread section
        
    struct thread_props_t
    {        
        thread_props_t();
         ~thread_props_t();
        
        void prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize);          
        bool isvalid() const;
        
        void setprio(uint8_t priority);     

        bool release();        
        
        // this is used internally by those who know how to do a reinterpret_cast<>
        void * getInternals();
                
    private:        
        struct Impl;
        Impl *pImpl;         
    };  
    

    thread_t * thread_new(thread_props_t &props);
    
    void thread_delete(thread_t *osthread);
    
    bool thread_setpriority(thread_t *osthread, embot::os::Priority priority);      

    void thread_period_set(thread_t *osthread, embot::core::relTime period);

    void thread_period_wait(thread_t *osthread, embot::core::relTime period);  
    
    void thread_sleep(thread_t *osthread, embot::core::relTime delay);
       
    
    
    // -- event section
    
    bool event_set(thread_t *osthread, embot::os::Event event);    

    embot::os::EventMask event_get(embot::core::relTime timeout);
    
    
    // -- messagequeue section

    messagequeue_t * messagequeue_new(uint8_t length);
    
    void messagequeue_delete(messagequeue_t *mq);
    
    embot::os::Message messagequeue_get(messagequeue_t *mq,  embot::core::relTime timeout);

    bool messagequeue_put(messagequeue_t *mq, embot::os::Message message, embot::core::relTime timeout);

    uint8_t messagequeue_size(messagequeue_t *mq);

    // -- timer section
    
    timer_t * timer_new();
    
    void timer_delete(timer_t *t);
    
    enum class timerMode { oneshot = 0, forever = 1 };
    #if defined(EMBOT_USE_rtos_osal)
    using fpOnTimerExpiry = void (*) (timer_t* tmr, void* param);  
    #elif defined(EMBOT_USE_rtos_cmsisos2)
    using fpOnTimerExpiry = embot::core::fpCaller;
    #endif    
    
    bool timer_start(timer_t *t, timerMode mode, embot::core::relTime countdown, fpOnTimerExpiry onexpiry, void* param);

    void timer_stop(timer_t *t);
    
    
    
    // -- mutex section
    
    mutex_t * mutex_new();    
    bool mutex_take(mutex_t *m, embot::core::relTime timeout);    
    void mutex_release(mutex_t *m);   
    void mutex_delete(mutex_t *m);

    // -- semaphore section
    
    semaphore_t * semaphore_new(uint8_t max, uint8_t cur);    
    bool semaphore_acquire(semaphore_t *s, embot::core::relTime timeout);    
    void semaphore_release(semaphore_t *s);   
    void semaphore_delete(semaphore_t *s);
    
    
    // -- memory section
    
    void * memory_new(size_t size);    
    void * memory_realloc(void *mem, size_t size);    
    void memory_delete(void *mem);    
    
}}} // namespace embot { namespace os { namespace rtos {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

