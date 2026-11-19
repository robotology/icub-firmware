
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_OS_RTOS_H_
#define _EMBOT_OS_RTOS_H_

#include "embot_os.h"
#include "embot_os_common.h"
#include <atomic>
#include <type_traits>

// so far the default is for OSAL. the choices are: EMBOT_USE_rtos_osal, EMBOT_USE_rtos_cmsisos2 (patched RTX 5.5.3),
// EMBOT_USE_rtos_cmsisrtx (original latest RTX built by libs/highlevel/abslayer/cmsisrtx, in embot_os_rtos_cmsisrtx.cpp)
// and EMBOT_USE_rtos_freertos (original FreeRTOS built by libs/highlevel/abslayer/freertos, in embot_os_rtos_freertos.cpp)
#if !defined(EMBOT_USE_rtos_cmsisos2) & !defined(EMBOT_USE_rtos_osal) & !defined(EMBOT_USE_rtos_cmsisrtx) & !defined(EMBOT_USE_rtos_freertos)
    #define EMBOT_USE_rtos_osal
#endif    


namespace embot { namespace os { namespace rtos {
    
    using thread_t = void; 
    using messagequeue_t = void; 
    using timer_t = void;
    using mutex_t = void;
    using semaphore_t = void;
    using callbackqueue_t = void;
   
    // -- scheduler section
                
    struct scheduler_props_t
    {
        using fpOnOSerror = void (*) (thread_t* thread, int errorcode, const char * errormsg);
        
        scheduler_props_t(); 
        ~scheduler_props_t();  
                     
        void prepare(embot::core::Time ticktime, 
                     uint16_t initstacksize, uint16_t idlestacksize, uint16_t timerstacksize,
                     embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror);
        
        bool isvalid() const;
        
        void * getinitstack(uint16_t &size) const;
        void * getidlestack(uint16_t &size) const;
        
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
    
    // it sets the time of tje scheduler
    bool scheduler_timeset(embot::core::Time t);
    
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

    // 0 if called from a thread (or before the scheduler starts, in thread mode), else the id of the interrupt being
    // served, which depends on the cpu: on Cortex-M the exception number (IPSR), on RISC-V what the port of the chip
    // gives (see embot_os_rtos_cpu.h). it can be called also from an ISR
    uint16_t interruptid();
    
    // -- thread section
        
    struct thread_props_t
    {        
        thread_props_t();
         ~thread_props_t();
        
        void prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize);          
        bool isvalid() const;
        
        void setprio(uint8_t priority); 
        uint8_t getprio() const;         

        void * getstack(uint16_t &size) const;

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
    
    embot::os::EventMask event_wait(const embot::os::EventMask mask, embot::core::relTime timeout);

    embot::os::EventMask event_wait(const embot::os::EventMask mask, embot::os::EventWaitMode ewm, embot::core::relTime timeout);
    
    // -- messagequeue section

    messagequeue_t * messagequeue_new(uint8_t length);
    
    void messagequeue_delete(messagequeue_t *mq);
    
    embot::os::Message messagequeue_get(messagequeue_t *mq,  embot::core::relTime timeout);

    bool messagequeue_put(messagequeue_t *mq, embot::os::Message message, embot::core::relTime timeout);

    uint8_t messagequeue_size(messagequeue_t *mq);
    
    uint8_t messagequeue_available(messagequeue_t *mq);
    
    
    // -- callbackqueue section

    callbackqueue_t * callbackqueue_new(uint8_t length);
    
    void callbackqueue_delete(callbackqueue_t *cq);
    
    embot::core::Callback callbackqueue_get(callbackqueue_t *cq,  embot::core::relTime timeout);

    bool callbackqueue_put(callbackqueue_t *cq, const embot::core::Callback &callback, embot::core::relTime timeout);

    uint8_t callbackqueue_size(callbackqueue_t *cq);
    
    uint8_t callbackqueue_available(callbackqueue_t *cq);    

    // -- timer section
    
    timer_t * timer_new();
    
    void timer_delete(timer_t *t);
    
    enum class timerMode { oneshot = 0, forever = 1 };
    #if defined(EMBOT_USE_rtos_osal)
    using fpOnTimerExpiry = void (*) (timer_t* tmr, void* param);  
    #elif defined(EMBOT_USE_rtos_cmsisos2) || defined(EMBOT_USE_rtos_cmsisrtx) || defined(EMBOT_USE_rtos_freertos)
    using fpOnTimerExpiry = embot::core::fpCaller;
    #endif    
    
    bool timer_running(timer_t *t);
    
    bool timer_start(timer_t *t, timerMode mode, embot::core::relTime countdown, fpOnTimerExpiry onexpiry, void* param);

    void timer_stop(timer_t *t);
    
    
    
    // -- mutex section
    
    mutex_t * mutex_new();    
    bool mutex_take(mutex_t *m, embot::core::relTime timeout);    
    void mutex_release(mutex_t *m);   
    void mutex_delete(mutex_t *m);
    
    // -- raii lock on an embot::os::rtos mutex. it releases the mutex only if it took it (a finite timeout may expire)
    class Lock
    {
    public:
        explicit Lock(mutex_t *m, embot::core::relTime tout = embot::core::reltimeWaitForever)
            : m_(m), taken_(mutex_take(m_, tout)) {}
        ~Lock()
        {
            if(taken_)
            {
                mutex_release(m_);
            }
        }
        bool taken() const { return taken_; }
        Lock(const Lock&) = delete;
        Lock& operator=(const Lock&) = delete;
    private:
        mutex_t *m_ {nullptr};          // keep it before taken_: taken_(mutex_take(m_, ...)) uses it
        bool taken_ {false};
    };

    // -- as Lock, but it takes the mutex only if needed (e.g. when some paths are lock-free)
    class LockIf
    {
    public:
        explicit LockIf(mutex_t *m, bool needed, embot::core::relTime tout = embot::core::reltimeWaitForever)
            : m_(m), taken_(needed && mutex_take(m_, tout)) {}
        ~LockIf()
        {
            if(taken_)
            {
                mutex_release(m_);
            }
        }
        bool taken() const { return taken_; }
        LockIf(const LockIf&) = delete;
        LockIf& operator=(const LockIf&) = delete;
    private:
        mutex_t *m_ {nullptr};
        bool taken_ {false};
    };
    
    
    // -- semaphore section
    
    semaphore_t * semaphore_new(uint8_t max, uint8_t cur);    
    bool semaphore_acquire(semaphore_t *s, embot::core::relTime timeout);    
    void semaphore_release(semaphore_t *s);   
    void semaphore_delete(semaphore_t *s);
    
    
    // -- memory section
    
    void * memory_new(size_t size);    
    void * memory_realloc(void *mem, size_t size);    
    void memory_delete(void *mem);    
    
    
    // -- critical section: a very short section that no thread and no interrupt can enter (the interrupts are off).
    
    // it can be used in a thread and in an ISR, and it nests: critical_exit() restores the state that critical_enter()
    // returned. keep it to a few instructions: its length adds to the latency of every interrupt. on a single core it
    // makes atomic what the cpu cannot do w/ one instruction (e.g. a compare-and-swap on a RISC-V w/out the A extension)

    // functions
    uint32_t critical_enter();
    void critical_exit(uint32_t state);

    // RAII format (as Lock and LockIf)
    // CriticalSection: a scope guard (RAII) that turns the interrupts off for its lifetime and then restores their
    // previous state. it nests and it can be used also in an ISR. keep it to a few instructions
    class CriticalSection
    {
    public:
        explicit CriticalSection() : state_(critical_enter()) {}
        ~CriticalSection() { critical_exit(state_); }
        CriticalSection(const CriticalSection&) = delete;
        CriticalSection& operator=(const CriticalSection&) = delete;
    private:
        uint32_t state_ {0};
    };
    
    // -- atomic section

    // Atomic: a value of at most 4 bytes shared by threads and ISRs, w/ only the operations that work on every cpu of
    // embot. it can be used in a thread and in an ISR.
    // - load() and store() are plain accesses: an aligned access of up to 4 bytes is a single instruction on every cpu
    // - add(), exchange() and compare_exchange() read, modify and write in one atomic step:
    //   - on Cortex-M3 and up they are the ones of std::atomic: a short lock-free loop of ldrex / strex, w/ the
    //     interrupts on (an interrupt between the two makes the strex fail and the loop retry). on RISC-V w/ the A
    //     extension a single amo instruction (lr / sc for compare_exchange)
    //   - on a cpu w/out atomic instructions (Cortex-M0, RISC-V w/out A) a few instructions inside a CriticalSection
    //     (a single core is assumed)
    //   so the code that uses Atomic costs nothing more on the boards of today and moves as it is to such a cpu
    // - only these operations: fetch_add() and the others of std::atomic would need, on a cpu w/out atomic
    //   instructions, functions of a library that may take a lock, which must never happen in an ISR
    // - the constructor is constexpr: a static Atomic is constant-initialised, no code runs at startup
    template<typename T>
    class Atomic
    {
        static_assert(std::is_trivially_copyable_v<T> && (sizeof(T) <= 4), "Atomic holds a trivially copyable value of at most 4 bytes");
        static constexpr bool lockfree {std::atomic<T>::is_always_lock_free};

    public:
        constexpr explicit Atomic(T v) : value_(v) {}
        Atomic(const Atomic&) = delete;
        Atomic& operator=(const Atomic&) = delete;

        T load(std::memory_order order = std::memory_order_seq_cst) const
        {
            return value_.load(order);
        }

        void store(T v, std::memory_order order = std::memory_order_seq_cst)
        {
            value_.store(v, order);
        }

        // it adds v and returns the value before
        T add(T v, std::memory_order order = std::memory_order_seq_cst)
        {
            if constexpr (lockfree)
            {
                return value_.fetch_add(v, order);
            }
            else
            {
                const CriticalSection cs {};
                const T old {value_.load(std::memory_order_relaxed)};
                value_.store(static_cast<T>(old + v), std::memory_order_relaxed);
                return old;
            }
        }

        // it writes v and returns the value before
        T exchange(T v, std::memory_order order = std::memory_order_seq_cst)
        {
            if constexpr (lockfree)
            {
                return value_.exchange(v, order);
            }
            else
            {
                const CriticalSection cs {};
                const T old {value_.load(std::memory_order_relaxed)};
                value_.store(v, std::memory_order_relaxed);
                return old;
            }
        }

        // as std::atomic::compare_exchange_strong(): if it holds expected it writes desired and returns true, else it
        // copies into expected the value it holds and returns false
        bool compare_exchange(T &expected, T desired, std::memory_order order = std::memory_order_seq_cst)
        {
            if constexpr (lockfree)
            {
                return value_.compare_exchange_strong(expected, desired, order);
            }
            else
            {
                const CriticalSection cs {};
                const T current {value_.load(std::memory_order_relaxed)};
                if(current == expected)
                {
                    value_.store(desired, std::memory_order_relaxed);
                    return true;
                }
                expected = current;
                return false;
            }
        }

    private:
        std::atomic<T> value_;
    };

    // examples
    //
    // a counter incremented by an ISR and read by a thread
    //   embot::os::rtos::Atomic<uint32_t> lost {0};         // constant initialisation: no code runs at startup
    //   void onoverflow()                                   // ISR
    //   {
    //       lost.add(1);
    //   }
    //   const uint32_t n {lost.load()};                     // thread
    //
    // take the value and restart from zero in one step, e.g. for a snapshot of the counters
    //   const uint32_t since {lost.exchange(0)};            // thread (it works also in an ISR)
    //
    // keep the maximum seen: record() can be called from threads and from ISRs at the same time
    //   embot::os::rtos::Atomic<uint32_t> maxlatency {0};
    //   void record(uint32_t v)                             // thread or ISR
    //   {
    //       uint32_t m {maxlatency.load(std::memory_order_relaxed)};
    //       while((v > m) && !maxlatency.compare_exchange(m, v, std::memory_order_relaxed))
    //       {   // m now holds the value written by the other one: compare again
    //       }
    //   }
    //   const uint32_t worst {maxlatency.load()};           // thread
    
    
}}} // namespace embot { namespace os { namespace rtos {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

