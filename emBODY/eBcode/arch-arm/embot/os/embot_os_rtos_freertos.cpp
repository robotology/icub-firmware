
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// embot::os::rtos on top of the original, unmodified FreeRTOS kernel built by the freertos lib (armclang).
//
// - it is compiled only if EMBOT_USE_rtos_freertos is defined (embot_os_rtos.cpp includes it).
// - it uses only the public API of the freertos lib: the single header freertos_api.h (FreeRTOS.h, task.h, queue.h,
//   semphr.h, timers.h and the extras of the lib). It does not see any internal structure of FreeRTOS.
// - as embot_os_rtos_cmsisrtx.cpp: the RTOS objects are created at run time with new and with the static API of FreeRTOS
//   (xTaskCreateStatic() etc.): the memory of the control blocks, of the stacks and of the queues is ours. The kernel has
//   no heap (configSUPPORT_DYNAMIC_ALLOCATION = 0).
// - the time in microseconds is the 64-bit extension of the tick of the kernel plus the counter of the SysTick.
// - the tick and the stacks of the idle and timer tasks are set by scheduler_init() at run time with freertos_config_set().
//
// The mapping:
//   thread         -> task (xTaskCreateStatic), priority = the value of embot::os::Priority (configMAX_PRIORITIES = 56)
//   event          -> the notification value of the task (32 bits), as the thread flags of CMSIS-RTOS2
//   messagequeue   -> queue of void*
//   callbackqueue  -> queue of embot::core::Callback
//   timer          -> software timer (it runs in the timer task, as the timers of RTX)
//   mutex          -> recursive mutex, with priority inheritance
//   semaphore      -> counting semaphore
//
// What is different from RTX (see also the README of the freertos lib):
// - an ISR which calls embot::os::rtos (e.g. setEvent() inside a callback of embot::hw) must have a NVIC priority >= 1:
//   the ISRs with priority 0 are never masked by FreeRTOS and must not call it. configASSERT() catches the case.
// - the commands to the timers go through the queue of the timer task. We do not wait inside the timer task itself nor
//   in an ISR. The memory of a deleted timer is released by the timer task after it has processed the deletion.
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"

#if defined(EMBOT_USE_rtos_freertos)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "freertos_api.h"

#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <atomic>
#include <new>
#include <string>
#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------
// - helpers
// --------------------------------------------------------------------------------------------------------------------

// the entry function of the init thread
extern "C" void sysINIT(void *p);

namespace embot { namespace os { namespace rtos {

    namespace freertos {

        constexpr UBaseType_t priorityInitThread {54};      // as osPriorityRealtime6 used by cmsisrtx (the timer task is 55)
        constexpr uint32_t stackminsize {FREERTOS_STACK_MINSIZE};

        // errors that we add for the heap. the ones of the lib are FREERTOS_ERROR_* in freertos_api.h
        constexpr uint32_t errNew {666};
        constexpr uint32_t errNewNothrow {667};

        // a stack size in bytes as we give it to FreeRTOS: a multiple of 8, not less than the minimum
        constexpr uint32_t stacksize(uint32_t requested)
        {
            return (requested < stackminsize) ? stackminsize : ((requested + 7U) & ~7U);
        }

        // zeroed memory: our operator new uses calloc(). it never returns nullptr: when the heap is full it calls
        // freertos_error_notify()
        inline void * alloc(size_t size)
        {
            return ::operator new(size);
        }

        inline void dealloc(void *p)
        {
            if(nullptr != p)
            {
                ::operator delete(p);
            }
        }

        // PRIMASK and IPSR without including the CMSIS core headers: they would clash with the ones of the stm32 hal
        inline uint32_t irqsave()
        {
            uint32_t primask {0};
            __asm volatile ("mrs %0, primask\n\tcpsid i" : "=r" (primask) :: "memory");
            return primask;
        }

        inline void irqrestore(uint32_t primask)
        {
            __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
        }

        inline bool isinisr()
        {
            uint32_t ipsr {0};
            __asm volatile ("mrs %0, ipsr" : "=r" (ipsr));
            return 0 != ipsr;
        }

        inline bool kernelstarted()
        {
            return taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState();
        }

        // true if we are running inside the timer task: the commands to the timers must not wait there
        inline bool isintimertask()
        {
            return kernelstarted() && (xTaskGetCurrentTaskHandle() == xTimerGetTimerDaemonTaskHandle());
        }

        // the wait for the commands to the timer task: forever from a normal task, no wait in the timer task itself
        inline TickType_t timercommandwait()
        {
            return isintimertask() ? 0 : portMAX_DELAY;
        }

        // microseconds per tick of the kernel. it is evaluated again after scheduler_init() has set the tick
        inline uint32_t usecpertick(bool refresh = false)
        {
            static uint32_t u {0};
            if((0 == u) || (true == refresh))
            {
                u = 1000000U / freertos_tick_rate_hz;
            }
            return u;
        }

        // in an ISR the only valid timeout is 0
        inline TickType_t reltime2tick(embot::core::relTime t)
        {
            if(embot::core::reltimeWaitForever == t)
            {
                return portMAX_DELAY;
            }
            if(isinisr() || (0 == t))
            {
                return 0;
            }
            // round up: a short but non-zero timeout must not become a no-wait
            const uint64_t u {usecpertick()};
            const uint64_t ticks {(static_cast<uint64_t>(t) + u - 1U) / u};
            return (ticks >= portMAX_DELAY) ? (portMAX_DELAY - 1U) : static_cast<TickType_t>(ticks);
        }

        // the period of a timer: as reltime2tick() but also inside an ISR (a timer can be started there), and never 0
        inline TickType_t period2tick(embot::core::relTime t)
        {
            if(embot::core::reltimeWaitForever == t)
            {
                return portMAX_DELAY - 1U;
            }
            const uint64_t u {usecpertick()};
            const uint64_t ticks {(static_cast<uint64_t>(t) + u - 1U) / u};
            return (ticks >= portMAX_DELAY) ? (portMAX_DELAY - 1U) : std::max<TickType_t>(1U, static_cast<TickType_t>(ticks));
        }

        // -- the time in microseconds since the start of the kernel

        // the tick of FreeRTOS is 32 bits (49 days at 1 kHz): we extend it to 64 bits. it must be called inside a critical
        // section and at least once every 2^32 ticks
        inline uint64_t tick64(uint32_t tick32)
        {
            static uint32_t last {0};
            static uint32_t high {0};
            if(tick32 < last)
            {
                high++;
            }
            last = tick32;
            return (static_cast<uint64_t>(high) << 32) | tick32;
        }

        // it is the offset applied by scheduler_timeset(). the time does not go backwards by itself: only timeset moves it.
        static int64_t timeoffset {0};

        inline uint64_t rawusec()
        {
            const uint32_t primask {irqsave()};

            // xTaskGetTickCount() is a plain read of a 32-bit variable on Cortex-M (portTICK_TYPE_IS_ATOMIC): it can be
            // called also from an ISR of any priority, which is not true for xTaskGetTickCountFromISR() with configASSERT()
            uint32_t tick {static_cast<uint32_t>(xTaskGetTickCount())};
            uint32_t count {freertos_tick_count()};          // counts elapsed in the current tick
            if(0U != freertos_tick_overflow())
            {   // the systick has wrapped but its handler has not incremented the tick yet
                count = freertos_tick_count();
                tick++;
            }
            const uint64_t t {tick64(tick)};

            irqrestore(primask);

            const uint64_t interval {freertos_tick_interval()};
            const uint64_t upt {usecpertick()};
            return (t * upt) + ((static_cast<uint64_t>(count) * upt) / interval);
        }

        // -- the events: the notification value of the task, used as the thread flags of CMSIS-RTOS2.
        // it waits for any (all = false) or all (all = true) of the bits of mask. it clears only the bits it returns and it
        // returns the value before the clear (as osThreadFlagsWait()), or 0 on timeout
        inline uint32_t waitflags(uint32_t mask, bool all, TickType_t ticks)
        {
            TimeOut_t timeout {};
            vTaskSetTimeOutState(&timeout);
            TickType_t remaining {ticks};

            for(;;)
            {
                // a read without clear. only this task clears its bits: what we see set stays set
                const uint32_t current {ulTaskNotifyValueClear(nullptr, 0)};
                const uint32_t got {current & mask};
                const bool satisfied {all ? (got == mask) : (0 != got)};
                if(satisfied)
                {
                    ulTaskNotifyValueClear(nullptr, all ? mask : got);
                    return current;
                }

                if(0 == remaining)
                {
                    return 0;
                }

                // it returns at the next notification (also of bits not in mask) or at the timeout. a notification arrived
                // after the read above is pending: the wait returns at once and we check again
                uint32_t value {0};
                xTaskNotifyWait(0, 0, &value, remaining);

                if(pdTRUE == xTaskCheckForTimeOut(&timeout, &remaining))
                {   // the time is over: one more check and then out
                    remaining = 0;
                }
            }
        }

        // the fast path of waitflags() for the wait of ANY of the bits of mask, as event_get() and EventThread use it.
        // the semantics is the same (it clears only the bits it returns, it returns the value before the clear or 0 on
        // timeout) but with one or two calls of the kernel instead of ~6: ulTaskNotifyValueClear() reads and clears the
        // bits of mask atomically, and xTaskNotifyWait() clears them on exit. Measured on the amcmj1 (CM4 @ 200 MHz) with
        // the ping-pong of freertos-blink: with waitflags() FreeRTOS cost +546 / +944 cycles more than RTX in the two directions.
        inline uint32_t waitany(uint32_t mask, TickType_t ticks)
        {
            const uint32_t before {ulTaskNotifyValueClear(nullptr, mask)};
            if(0 != (before & mask))
            {
                return before;
            }

            if(0 == ticks)
            {
                return 0;
            }

            if(portMAX_DELAY == ticks)
            {   // a wake up without bits of mask (other bits, or a notification already consumed by the read above): wait again
                for(;;)
                {
                    uint32_t value {0};
                    xTaskNotifyWait(0, mask, &value, portMAX_DELAY);
                    if(0 != (value & mask))
                    {
                        return value;
                    }
                }
            }

            TimeOut_t timeout {};
            vTaskSetTimeOutState(&timeout);
            TickType_t remaining {ticks};
            for(;;)
            {
                uint32_t value {0};
                if((pdTRUE == xTaskNotifyWait(0, mask, &value, remaining)) && (0 != (value & mask)))
                {
                    return value;
                }
                if(pdTRUE == xTaskCheckForTimeOut(&timeout, &remaining))
                {   // the time is over. a bit may have arrived just now: take it, as the waitflags() does with its last check
                    const uint32_t last {ulTaskNotifyValueClear(nullptr, mask)};
                    return (0 != (last & mask)) ? last : 0;
                }
            }
        }

    } // namespace freertos

}}} // namespace embot { namespace os { namespace rtos {


// --------------------------------------------------------------------------------------------------------------------
// - the memory of the objects. it is allocated with new and it is owned by us.
//   with the static API, FreeRTOS returns as handle of an object the address of its control block (StaticTask_t,
//   StaticQueue_t, ...): hence the control block is the first member of every struct and the handle of the object is
//   also the pointer to the struct.
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace os { namespace rtos {

    // -- the implemention of thread_props_t

    struct thread_props_Internals
    {
        TaskFunction_t func {nullptr};
        void *argument {nullptr};
        UBaseType_t priority {0};
        StaticTask_t *tcb {nullptr};
        StackType_t *stack {nullptr};
        uint32_t stacksize {0};             // bytes

        constexpr thread_props_Internals() = default;

        void deallocate()
        {
            freertos::dealloc(stack);
            freertos::dealloc(tcb);
            stack = nullptr;
            tcb = nullptr;
            stacksize = 0;
        }

        void allocate(uint16_t size)
        {
            if(nullptr != tcb)
            {
                deallocate();
            }
            stacksize = freertos::stacksize(size);
            // the stack first, as xTaskCreate() of FreeRTOS does: the stack grows down, so an overflow goes into the memory
            // below it and not into the TCB, which is allocated after (above) it. the TCB stays valid and the kernel can
            // report the overflow at the next context switch
            stack = static_cast<StackType_t*>(freertos::alloc(stacksize));
            tcb = static_cast<StaticTask_t*>(freertos::alloc(sizeof(StaticTask_t)));
        }

        void prepare(embot::core::fpCaller function, void* param, uint8_t prio, uint16_t size)
        {
            allocate(size);
            priority = prio;
            func = function;
            argument = param;
        }

        // the task. nullptr if the props are not valid
        TaskHandle_t create()
        {
            if((nullptr == func) || (nullptr == tcb) || (nullptr == stack))
            {
                return nullptr;
            }
            // the Event Viewer (freertos.debug.cm4.lib) must get the entry function of the task, which the TCB does not keep:
            // freertos_ev_name() tells it. The scheduler is suspended so that the new task cannot run before it has a name.
            // Before the start of the scheduler no task can run anyway.
            const bool suspend {taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()};
            if(suspend)
            {
                vTaskSuspendAll();
            }
            TaskHandle_t h {xTaskCreateStatic(func, nullptr, stacksize / sizeof(StackType_t), argument, priority, stack, tcb)};
            freertos_ev_name(h, func);
            if(suspend)
            {
                xTaskResumeAll();
            }
            return h;
        }

        ~thread_props_Internals()
        {
            deallocate();
        }
    };

    struct thread_props_t::Impl
    {
        thread_props_Internals _internals {};

        Impl() = default;
        ~Impl() = default;

        void prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize)
        {
            _internals.prepare(function, param, priority, stacksize);
        }

        bool release()
        {
            const bool had {nullptr != _internals.tcb};
            _internals.deallocate();
            return had;
        }

        bool isvalid() const
        {
            return (nullptr != _internals.func) && (nullptr != _internals.tcb);
        }

        void setprio(uint8_t priority)
        {
            _internals.priority = priority;
        }

        uint8_t getprio() const
        {
            return static_cast<uint8_t>(_internals.priority);
        }

        void * getstack(uint16_t &size) const
        {
            size = static_cast<uint16_t>(_internals.stacksize);
            return _internals.stack;
        }
    };

    thread_props_t::thread_props_t()
    : pImpl(new Impl())
    {
    }

    thread_props_t::~thread_props_t()
    {
        delete pImpl;
    }

    void thread_props_t::prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize)
    {
        pImpl->prepare(function, param, priority, stacksize);
    }

    void thread_props_t::setprio(uint8_t priority)
    {
        pImpl->setprio(priority);
    }

    uint8_t thread_props_t::getprio() const
    {
        return pImpl->getprio();
    }

    void * thread_props_t::getstack(uint16_t &size) const
    {
        return pImpl->getstack(size);
    }

    void * thread_props_t::getInternals()
    {
        return &pImpl->_internals;
    }

    bool thread_props_t::isvalid() const
    {
        return pImpl->isvalid();
    }

    bool thread_props_t::release()
    {
        return pImpl->release();
    }

}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a messagequeue (also the one of a callbackqueue)

namespace embot { namespace os { namespace rtos {

    struct osQueueMemory
    {
        StaticQueue_t cb {};                // keep it first
        uint8_t *storage {nullptr};
        UBaseType_t length {0};

        osQueueMemory() = default;
        ~osQueueMemory()
        {
            delete [] storage;
        }

        QueueHandle_t create(UBaseType_t len, UBaseType_t sizeofitem)
        {
            length = len;
            storage = new uint8_t[len * sizeofitem] {};
            return xQueueCreateStatic(len, sizeofitem, storage, &cb);
        }

        // an object of this struct from the handle
        static osQueueMemory * from(void *handle)
        {
            return reinterpret_cast<osQueueMemory*>(handle);
        }
    };
    static_assert(0 == offsetof(osQueueMemory, cb), "cb must be the first member");

    // -- the common code of messagequeue and callbackqueue

    namespace freertos {

        inline void * queuenew(uint8_t length, UBaseType_t sizeofitem)
        {
            if(0 == length)
            {
                return nullptr;
            }
            osQueueMemory *mem {new osQueueMemory};
            QueueHandle_t q {mem->create(length, sizeofitem)};
            if(nullptr == q)
            {
                delete mem;
            }
            return q;
        }

        inline void queuedelete(void *q)
        {
            if(nullptr == q)
            {
                return;
            }
            vQueueDelete(static_cast<QueueHandle_t>(q));
            delete osQueueMemory::from(q);
        }

        inline bool queueput(void *q, const void *item, embot::core::relTime timeout)
        {
            if(isinisr())
            {
                BaseType_t woken {pdFALSE};
                const BaseType_t r {xQueueSendToBackFromISR(static_cast<QueueHandle_t>(q), item, &woken)};
                portYIELD_FROM_ISR(woken);
                return pdPASS == r;
            }
            return pdPASS == xQueueSendToBack(static_cast<QueueHandle_t>(q), item, reltime2tick(timeout));
        }

        inline bool queueget(void *q, void *item, embot::core::relTime timeout)
        {
            if(isinisr())
            {
                BaseType_t woken {pdFALSE};
                const BaseType_t r {xQueueReceiveFromISR(static_cast<QueueHandle_t>(q), item, &woken)};
                portYIELD_FROM_ISR(woken);
                return pdPASS == r;
            }
            return pdPASS == xQueueReceive(static_cast<QueueHandle_t>(q), item, reltime2tick(timeout));
        }

        inline uint8_t queuesize(void *q)
        {
            if(isinisr())
            {
                return static_cast<uint8_t>(uxQueueMessagesWaitingFromISR(static_cast<QueueHandle_t>(q)));
            }
            return static_cast<uint8_t>(uxQueueMessagesWaiting(static_cast<QueueHandle_t>(q)));
        }

        inline uint8_t queueavailable(void *q)
        {
            if(isinisr())
            {   // uxQueueSpacesAvailable() has no ISR version
                return static_cast<uint8_t>(osQueueMemory::from(q)->length - uxQueueMessagesWaitingFromISR(static_cast<QueueHandle_t>(q)));
            }
            return static_cast<uint8_t>(uxQueueSpacesAvailable(static_cast<QueueHandle_t>(q)));
        }

    } // namespace freertos

}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a timer. the FreeRTOS timer is created by timer_new() and its type (once or periodic) and period are
//    changed at every start. we give FreeRTOS a trampoline and we keep here the user function and parameter: this way we can
//    change them at every start with no access to the internals of FreeRTOS.

namespace embot { namespace os { namespace rtos {

    struct osTimerMemory
    {
        StaticTimer_t cb {};                // keep it first
        TimerHandle_t id {nullptr};
        fpOnTimerExpiry func {nullptr};
        void *param {nullptr};

        osTimerMemory() = default;
        ~osTimerMemory() = default;

        // it runs inside the timer task
        static void trampoline(TimerHandle_t t)
        {
            osTimerMemory *m {static_cast<osTimerMemory*>(pvTimerGetTimerID(t))};
            if((nullptr != m) && (nullptr != m->func))
            {
                m->func(m->param);
            }
        }

        // it runs inside the timer task, after the deletion of the timer has been processed (the queue is FIFO)
        static void release(void *mem, uint32_t unused)
        {
            (void)unused;
            delete static_cast<osTimerMemory*>(mem);
        }
    };
    static_assert(0 == offsetof(osTimerMemory, cb), "cb must be the first member");

}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a mutex and of a semaphore

namespace embot { namespace os { namespace rtos {

    struct osMutexMemory
    {
        StaticSemaphore_t cb {};            // keep it first

        osMutexMemory() = default;
        ~osMutexMemory() = default;
    };
    static_assert(0 == offsetof(osMutexMemory, cb), "cb must be the first member");

    struct osSemaphoreMemory
    {
        StaticSemaphore_t cb {};            // keep it first

        osSemaphoreMemory() = default;
        ~osSemaphoreMemory() = default;
    };
    static_assert(0 == offsetof(osSemaphoreMemory, cb), "cb must be the first member");

}}} // namespace embot { namespace os { namespace rtos {


// --------------------------------------------------------------------------------------------------------------------
// - the implemention of scheduler_props_t
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace os { namespace rtos {

    struct scheduler_props_Internals
    {
        embot::core::Time ticktime {0};
        embot::core::fpWorker oninit {nullptr};
        embot::core::fpWorker onidle {nullptr};
        scheduler_props_t::fpOnOSerror onerror {nullptr};
        uint16_t initstacksize {0};
        uint16_t idlestacksize {0};
        uint16_t timerstacksize {0};
        thread_props_t init_props {};
        void *idlestack {nullptr};          // memory given to FreeRTOS by scheduler_init()
        void *timerstack {nullptr};
        bool initted {false};

        scheduler_props_Internals() = default;
    };

    struct scheduler_props_t::Impl
    {
        scheduler_props_t * owner {nullptr};
        scheduler_props_Internals _internals {};

        explicit Impl(scheduler_props_t *o) : owner(o) {}
        ~Impl() = default;

        void prepare(embot::core::Time ticktime, uint16_t initstacksize, uint16_t idlestacksize, uint16_t timerstacksize,
                     embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror)
        {
            _internals.ticktime = ticktime;
            _internals.initstacksize = initstacksize;
            _internals.idlestacksize = idlestacksize;
            _internals.timerstacksize = timerstacksize;
            _internals.onidle = onidle;
            _internals.oninit = oninit;
            _internals.onerror = onerror;

            _internals.init_props.prepare(sysINIT, owner, freertos::priorityInitThread, initstacksize);

            _internals.initted = true;
        }

        bool isvalid() const
        {
            return _internals.initted && _internals.init_props.isvalid();
        }

        void * getinitstack(uint16_t &size) const
        {
            return _internals.init_props.getstack(size);
        }

        // the stack of the idle thread: allocated by scheduler_init() and given to FreeRTOS (nullptr before it)
        void * getidlestack(uint16_t &size) const
        {
            size = _internals.idlestacksize;
            return _internals.idlestack;
        }

        bool releaseinit()
        {
            return _internals.init_props.release();
        }
    };

    scheduler_props_t::scheduler_props_t()
    : pImpl(new Impl(this))
    {
    }

    scheduler_props_t::~scheduler_props_t()
    {
        delete pImpl;
    }

    void scheduler_props_t::prepare(embot::core::Time ticktime, uint16_t initstacksize, uint16_t idlestacksize, uint16_t timerstacksize,
                                    embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror)
    {
        pImpl->prepare(ticktime, initstacksize, idlestacksize, timerstacksize, onidle, oninit, onerror);
    }

    void * scheduler_props_t::getInternals()
    {
        return &pImpl->_internals;
    }

    bool scheduler_props_t::isvalid() const
    {
        return pImpl->isvalid();
    }

    void * scheduler_props_t::getidlestack(uint16_t &size) const
    {
        return pImpl->getidlestack(size);
    }

    void * scheduler_props_t::getinitstack(uint16_t &size) const
    {
        return pImpl->getinitstack(size);
    }

    bool scheduler_props_t::release(Resource res)
    {
        // the init thread deletes itself: FreeRTOS keeps its TCB in the list of the tasks to be cleaned by the idle task.
        // embot::os::theScheduler does not call it so far (releaseINITatend is false), and the memory stays allocated
        return pImpl->releaseinit();
    }

}}} // namespace embot { namespace os { namespace rtos {


// --------------------------------------------------------------------------------------------------------------------
// - scheduler section
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace os { namespace rtos {

    // the props of the scheduler: they keep the parameters across scheduler_init() and scheduler_start()
    scheduler_props_t *the_scheduler_props {nullptr};

    bool scheduler_init(scheduler_props_t &props)
    {
        if(false == props.isvalid())
        {
            return false;
        }

        scheduler_props_Internals *i {reinterpret_cast<scheduler_props_Internals*>(props.getInternals())};
        if(0 == i->ticktime)
        {
            return false;
        }

        // the kernel reads the tick and the memory of the idle and timer tasks only inside vTaskStartScheduler(): we can
        // set them now. all the memory is given by us and it is never released.
        const uint32_t idlesize {freertos::stacksize(i->idlestacksize)};
        const uint32_t timersize {freertos::stacksize(i->timerstacksize)};
        void *idlestack {freertos::alloc(idlesize)};
        void *timerstack {freertos::alloc(timersize)};

        freertos_config_t cfg {};
        cfg.tick_freq = static_cast<uint32_t>(1000000U / i->ticktime);
        cfg.idle_stack_mem = idlestack;
        cfg.idle_stack_size = idlesize;
        cfg.timer_stack_mem = timerstack;
        cfg.timer_stack_size = timersize;

        if(FREERTOS_CONFIG_OK != freertos_config_set(&cfg))
        {
            freertos::dealloc(idlestack);
            freertos::dealloc(timerstack);
            return false;
        }

        i->idlestack = idlestack;
        i->timerstack = timerstack;
        i->idlestacksize = static_cast<uint16_t>(idlesize);
        i->timerstacksize = static_cast<uint16_t>(timersize);
        freertos::usecpertick(true);        // the tick may have changed

        the_scheduler_props = &props;
        return true;
    }

    void scheduler_start()
    {
        scheduler_start2();
    }

    void scheduler_start2()
    {
        if((nullptr == the_scheduler_props) || (false == the_scheduler_props->isvalid()))
        {
            return;
        }

        scheduler_props_Internals *i {reinterpret_cast<scheduler_props_Internals*>(the_scheduler_props->getInternals())};

        if(nullptr != i->oninit)
        {
            thread_props_Internals *ti {reinterpret_cast<thread_props_Internals*>(i->init_props.getInternals())};
            ti->create();
        }

        // it does not return
        vTaskStartScheduler();
    }

    bool scheduler_isrunning()
    {
        return taskSCHEDULER_RUNNING == xTaskGetSchedulerState();
    }

    embot::core::Time scheduler_timeget()
    {
        if(false == freertos::kernelstarted())
        {
            return 0;
        }

        const uint32_t primask {freertos::irqsave()};
        const int64_t offset {freertos::timeoffset};
        freertos::irqrestore(primask);

        return static_cast<embot::core::Time>(static_cast<int64_t>(freertos::rawusec()) + offset);
    }

    // the time is not changed in the RTOS: we keep the offset between the requested time and the one of the kernel
    bool scheduler_timeset(embot::core::Time t)
    {
        if(false == freertos::kernelstarted())
        {
            return false;
        }

        const int64_t raw {static_cast<int64_t>(freertos::rawusec())};
        const uint32_t primask {freertos::irqsave()};
        freertos::timeoffset = static_cast<int64_t>(t) - raw;
        freertos::irqrestore(primask);
        return true;
    }

    thread_t * scheduler_thread_running()
    {
        // before the start FreeRTOS points to the last created task: as osThreadGetId() of RTX we say none
        if(false == freertos::kernelstarted())
        {
            return nullptr;
        }
        return xTaskGetCurrentTaskHandle();
    }

    // -- the association of a FreeRTOS task with its embot::os::Thread, and of a thread with its periodic timer.
    // they are fixed tables, not std::map: no heap, no lock. A writer disables the interrupts for the few instructions of the
    // update, a reader (also an ISR, or freertos_error_notify()) does not lock anything and never sees a half written item.
    namespace freertos {

        constexpr uint32_t maxthreads {64};             // threads associated with an embot::os::Thread at the same time
        constexpr uint32_t maxperiodictimers {32};      // threads with a period set by thread_period_set()

        template<typename V, uint32_t N>
        class PtrTable
        {
        public:
            constexpr PtrTable() : slots() {}

            // the value of the key, nullptr if there is none
            V * get(const void *key) const
            {
                if(nullptr == key)
                {
                    return nullptr;
                }
                for(uint32_t i=0; i<N; i++)
                {
                    if(key == slots[i].key.load(std::memory_order_acquire))
                    {
                        V *v {slots[i].value.load(std::memory_order_acquire)};
                        // the item may have been removed while we read it
                        return (key == slots[i].key.load(std::memory_order_acquire)) ? v : nullptr;
                    }
                }
                return nullptr;
            }

            // false if the key is already there (the old value is kept, as std::map::insert) or the table is full
            bool add(const void *key, V *value)
            {
                if(nullptr == key)
                {
                    return false;
                }
                bool done {false};
                const uint32_t primask {irqsave()};
                int32_t freeslot {-1};
                for(uint32_t i=0; i<N; i++)
                {
                    const void *k {slots[i].key.load(std::memory_order_relaxed)};
                    if(key == k)
                    {
                        freeslot = -2;
                        break;
                    }
                    if((nullptr == k) && (freeslot < 0))
                    {
                        freeslot = static_cast<int32_t>(i);
                    }
                }
                if(freeslot >= 0)
                {   // the key is written last: a reader finds the value already there
                    slots[freeslot].value.store(value, std::memory_order_relaxed);
                    slots[freeslot].key.store(key, std::memory_order_release);
                    done = true;
                }
                irqrestore(primask);
                return done;
            }

            // it removes the key and returns its value, nullptr if there was none
            V * take(const void *key)
            {
                if(nullptr == key)
                {
                    return nullptr;
                }
                V *v {nullptr};
                const uint32_t primask {irqsave()};
                for(uint32_t i=0; i<N; i++)
                {
                    if(key == slots[i].key.load(std::memory_order_relaxed))
                    {
                        v = slots[i].value.load(std::memory_order_relaxed);
                        slots[i].key.store(nullptr, std::memory_order_release);
                        slots[i].value.store(nullptr, std::memory_order_relaxed);
                        break;
                    }
                }
                irqrestore(primask);
                return v;
            }

        private:
            struct Slot
            {
                std::atomic<const void*> key {nullptr};
                std::atomic<V*> value {nullptr};
            };
            Slot slots[N];
        };

        static PtrTable<embot::os::Thread, maxthreads> tableofthreads {};
    }

    void scheduler_associate(thread_t *osthread, embot::os::Thread *t)
    {
        if((false == freertos::tableofthreads.add(osthread, t)) && (nullptr == freertos::tableofthreads.get(osthread)))
        {   // not already there, so the table is full
            embot::core::print("beware: embot::os::rtos::scheduler_associate(): more than " + std::to_string(freertos::maxthreads) +
                               " threads, increase freertos::maxthreads");
        }
    }

    void scheduler_deassociate(thread_t *osthread, embot::os::Thread *t)
    {
        freertos::tableofthreads.take(osthread);
    }

    embot::os::Thread * scheduler_getassociated(thread_t *osthread)
    {
        return freertos::tableofthreads.get(osthread);
    }

    void scheduler_lock()
    {
        vTaskSuspendAll();
    }

    void scheduler_unlock()
    {
        xTaskResumeAll();
    }


    // -- thread section

    thread_t * thread_new(thread_props_t &props)
    {
        // every memory allocation is done internally to thread_props_t
        if(false == props.isvalid())
        {
            return nullptr;
        }

        thread_props_Internals *i {reinterpret_cast<thread_props_Internals*>(props.getInternals())};
        return i->create();
    }

    namespace freertos {
        // the periodic timers of thread_period_set(), one per thread. constexpr constructor: constant initialisation, no guard
        PtrTable<timer_t, maxperiodictimers> & periodictimers()
        {
            static PtrTable<timer_t, maxperiodictimers> tableoftimers {};
            return tableoftimers;
        }
    }

    void thread_delete(thread_t *osthread)
    {
        // the memory is in thread_props_t and it is the responsibility of the caller to release it.
        // a task which is not the running one is removed from the kernel at once: its memory can be released after this
        // call. a task which deletes itself never returns from here.
        if(nullptr != osthread)
        {
            // the periodic timer of the thread (thread_period_set()) must go first: else it keeps notifying the deleted task,
            // and xTaskNotify() on a deleted task that was waiting puts it back in the ready list (it runs again on memory
            // already released). The timer task has the highest priority: after timer_delete() no callback can run.
            timer_t *tmr {freertos::periodictimers().take(osthread)};
            if(nullptr != tmr)
            {
                timer_stop(tmr);
                timer_delete(tmr);
            }
            vTaskDelete(static_cast<TaskHandle_t>(osthread));
        }
    }

    bool thread_setpriority(thread_t *osthread, embot::os::Priority priority)
    {
        if(nullptr == osthread)
        {
            return false;
        }
        vTaskPrioritySet(static_cast<TaskHandle_t>(osthread), static_cast<UBaseType_t>(priority));
        return true;
    }

    void thread_period_set(thread_t *osthread, embot::core::relTime period)
    {
        // a periodic timer sets the event 0x1 of the thread. the timer of the thread is in a table, not in a std::map: no heap,
        // and two threads can call this at the same time. thread_delete() removes it
        freertos::PtrTable<timer_t, freertos::maxperiodictimers> &tableoftimers {freertos::periodictimers()};

        timer_t *previous {tableoftimers.take(osthread)};
        if(nullptr != previous)
        {   // we stop and remove the previous one
            timer_stop(previous);
            timer_delete(previous);
        }

        if(0 != period)
        {
            timer_t *tmr {timer_new()};
            timer_start(tmr, timerMode::forever, period, [](void *p){ event_set(p, 0x1); }, osthread);
            if(false == tableoftimers.add(osthread, tmr))
            {   // another call set the period of the same thread in the meantime, or the table is full: we keep the other one
                timer_stop(tmr);
                timer_delete(tmr);
                embot::core::print("beware: embot::os::rtos::thread_period_set(): period of the thread already set or more than " +
                                   std::to_string(freertos::maxperiodictimers) + " periodic threads");
            }
        }
    }

    void thread_period_wait(thread_t *osthread, embot::core::relTime period)
    {
        event_get(embot::core::reltimeWaitForever);
    }

    void thread_sleep(thread_t *osthread, embot::core::relTime delay)
    {
        vTaskDelay(freertos::reltime2tick(delay));
    }


    // -- event section

    bool event_set(thread_t *osthread, embot::os::Event event)
    {
        if(nullptr == osthread)
        {
            return false;
        }

        if(freertos::isinisr())
        {
            BaseType_t woken {pdFALSE};
            xTaskNotifyFromISR(static_cast<TaskHandle_t>(osthread), event, eSetBits, &woken);
            portYIELD_FROM_ISR(woken);
            return true;
        }

        // with eSetBits it always succeeds
        xTaskNotify(static_cast<TaskHandle_t>(osthread), event, eSetBits);
        return true;
    }

    embot::os::EventMask event_get(embot::core::relTime timeout)
    {
        return freertos::waitany(0x7fffffff, freertos::reltime2tick(timeout));
    }

    embot::os::EventMask event_wait(const embot::os::EventMask mask, embot::core::relTime timeout)
    {
        return event_wait(mask, embot::os::EventWaitMode::ALL, timeout);
    }

    embot::os::EventMask event_wait(const embot::os::EventMask mask, embot::os::EventWaitMode ewm, embot::core::relTime timeout)
    {
        const TickType_t ticks {freertos::reltime2tick(timeout)};
        return (embot::os::EventWaitMode::ANY == ewm) ? freertos::waitany(mask, ticks) : freertos::waitflags(mask, true, ticks);
    }


    // -- messagequeue section

    messagequeue_t * messagequeue_new(uint8_t length)
    {
        return freertos::queuenew(length, sizeof(embot::os::Message));
    }

    void messagequeue_delete(messagequeue_t *mq)
    {
        freertos::queuedelete(mq);
    }

    bool messagequeue_put(messagequeue_t *mq, embot::os::Message message, embot::core::relTime timeout)
    {
        return freertos::queueput(mq, &message, timeout);
    }

    embot::os::Message messagequeue_get(messagequeue_t *mq,  embot::core::relTime timeout)
    {
        embot::os::Message m {nullptr};
        if(false == freertos::queueget(mq, &m, timeout))
        {
            return nullptr;
        }
        return m;
    }

    uint8_t messagequeue_size(messagequeue_t *mq)
    {
        return freertos::queuesize(mq);
    }

    uint8_t messagequeue_available(messagequeue_t *mq)
    {
        return freertos::queueavailable(mq);
    }


    // -- callbackqueue section

    callbackqueue_t * callbackqueue_new(uint8_t length)
    {
        return freertos::queuenew(length, sizeof(embot::core::Callback));
    }

    void callbackqueue_delete(callbackqueue_t *cq)
    {
        freertos::queuedelete(cq);
    }

    bool callbackqueue_put(callbackqueue_t *cq, const embot::core::Callback &callback, embot::core::relTime timeout)
    {
        return freertos::queueput(cq, &callback, timeout);
    }

    embot::core::Callback callbackqueue_get(callbackqueue_t *cq,  embot::core::relTime timeout)
    {
        embot::core::Callback cbk {};
        if(false == freertos::queueget(cq, &cbk, timeout))
        {
            return embot::core::Callback {};
        }
        return cbk;
    }

    uint8_t callbackqueue_size(callbackqueue_t *cq)
    {
        return freertos::queuesize(cq);
    }

    uint8_t callbackqueue_available(callbackqueue_t *cq)
    {
        return freertos::queueavailable(cq);
    }


    // -- timer section

    timer_t * timer_new()
    {
        // the pointer of the timer_t is the one of our struct. the period (1 tick) and the type are set again at every start
        osTimerMemory *mem {new osTimerMemory};
        mem->id = xTimerCreateStatic(nullptr, 1, pdFALSE, mem, osTimerMemory::trampoline, &mem->cb);
        if(nullptr == mem->id)
        {
            delete mem;
            return nullptr;
        }
        return mem;
    }

    void timer_delete(timer_t *t)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        if(nullptr == mem)
        {
            return;
        }
        // the deletion is a command for the timer task, and so is the release of the memory: the queue is FIFO, so the memory
        // is released only after the timer task has removed the timer. we never release it here.
        const TickType_t wait {freertos::timercommandwait()};
        if(pdPASS != xTimerDelete(mem->id, wait))
        {   // it can happen only inside the timer task, which cannot wait: the timer stays alive, with no function to call
            mem->func = nullptr;
            embot::core::print("beware: embot::os::rtos::timer_delete(): the queue of the timer task is full, a timer is not deleted");
            return;
        }
        if(pdPASS != xTimerPendFunctionCall(osTimerMemory::release, mem, 0, wait))
        {   // the timer is deleted but its memory stays allocated
            embot::core::print("beware: embot::os::rtos::timer_delete(): the queue of the timer task is full, the memory of a timer is lost");
        }
    }

    bool timer_running(timer_t *t)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        return (nullptr != mem) && (pdFALSE != xTimerIsTimerActive(mem->id));
    }

    bool timer_start(timer_t *t, timerMode mode, embot::core::relTime countdown, fpOnTimerExpiry onexpiry, void* param)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        if(nullptr == mem)
        {
            return false;
        }

        // the period of a FreeRTOS timer cannot be 0
        const TickType_t ticks {freertos::period2tick(countdown)};
        const BaseType_t autoreload {(timerMode::forever == mode) ? pdTRUE : pdFALSE};

        if(freertos::isinisr())
        {
            BaseType_t woken {pdFALSE};
            xTimerStopFromISR(mem->id, &woken);
            mem->func = onexpiry;
            mem->param = param;
            // vTimerSetReloadMode() is not allowed in an ISR: the mode cannot be changed there
            const bool ok {(static_cast<UBaseType_t>(autoreload) == uxTimerGetReloadMode(mem->id)) && (pdPASS == xTimerChangePeriodFromISR(mem->id, ticks, &woken))};
            portYIELD_FROM_ISR(woken);
            return ok;
        }

        const TickType_t wait {freertos::timercommandwait()};
        // both osal and cmsisrtx allow to restart a running timer
        xTimerStop(mem->id, wait);
        mem->func = onexpiry;
        mem->param = param;
        vTimerSetReloadMode(mem->id, autoreload);
        // it also starts the timer
        return pdPASS == xTimerChangePeriod(mem->id, ticks, wait);
    }

    void timer_stop(timer_t *t)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        if(nullptr == mem)
        {
            return;
        }

        if(freertos::isinisr())
        {
            BaseType_t woken {pdFALSE};
            xTimerStopFromISR(mem->id, &woken);
            portYIELD_FROM_ISR(woken);
            return;
        }
        xTimerStop(mem->id, freertos::timercommandwait());
    }


    // -- mutex section

    mutex_t * mutex_new()
    {
        osMutexMemory *mem {new osMutexMemory};
        SemaphoreHandle_t id {xSemaphoreCreateRecursiveMutexStatic(&mem->cb)};
        if(nullptr == id)
        {
            delete mem;
        }
        return id;
    }

    bool mutex_take(mutex_t *m, embot::core::relTime timeout)
    {
        return pdTRUE == xSemaphoreTakeRecursive(static_cast<SemaphoreHandle_t>(m), freertos::reltime2tick(timeout));
    }

    void mutex_release(mutex_t *m)
    {
        xSemaphoreGiveRecursive(static_cast<SemaphoreHandle_t>(m));
    }

    void mutex_delete(mutex_t *m)
    {
        if(nullptr == m)
        {
            return;
        }
        vSemaphoreDelete(static_cast<SemaphoreHandle_t>(m));
        delete reinterpret_cast<osMutexMemory*>(m);
    }


    // -- semaphore section

    semaphore_t * semaphore_new(uint8_t max, uint8_t cur)
    {
        osSemaphoreMemory *mem {new osSemaphoreMemory};
        SemaphoreHandle_t id {xSemaphoreCreateCountingStatic(max, cur, &mem->cb)};
        if(nullptr == id)
        {
            delete mem;
        }
        return id;
    }

    bool semaphore_acquire(semaphore_t *s, embot::core::relTime timeout)
    {
        if(freertos::isinisr())
        {
            BaseType_t woken {pdFALSE};
            const BaseType_t r {xSemaphoreTakeFromISR(static_cast<SemaphoreHandle_t>(s), &woken)};
            portYIELD_FROM_ISR(woken);
            return pdTRUE == r;
        }
        return pdTRUE == xSemaphoreTake(static_cast<SemaphoreHandle_t>(s), freertos::reltime2tick(timeout));
    }

    void semaphore_release(semaphore_t *s)
    {
        if(freertos::isinisr())
        {
            BaseType_t woken {pdFALSE};
            xSemaphoreGiveFromISR(static_cast<SemaphoreHandle_t>(s), &woken);
            portYIELD_FROM_ISR(woken);
            return;
        }
        xSemaphoreGive(static_cast<SemaphoreHandle_t>(s));
    }

    void semaphore_delete(semaphore_t *s)
    {
        if(nullptr == s)
        {
            return;
        }
        vSemaphoreDelete(static_cast<SemaphoreHandle_t>(s));
        delete reinterpret_cast<osSemaphoreMemory*>(s);
    }


    // -- memory section
    //    the heap of the arm c library is protected by the mutexes that the freertos lib gives to the library
    //    (_mutex_acquire() etc. in cfg/freertos_lib.c). do not allocate from an ISR.

    void * memory_new(size_t size)
    {
        return calloc(size, 1);
    }

    void * memory_realloc(void *mem, size_t size)
    {
        return realloc(mem, size);
    }

    void memory_delete(void *mem)
    {
        free(mem);
    }

}}} // namespace embot { namespace os { namespace rtos {


// --------------------------------------------------------------------------------------------------------------------
// - the functions that the freertos lib (cfg/freertos_lib.c) finds: they override its weak ones
// --------------------------------------------------------------------------------------------------------------------

extern "C" {

// the init thread: it calls oninit() and deletes itself. It is named sysINIT as in cmsisrtx: the Event Viewer shows it so
// and keeps its name after it is gone (cfg/freertos_eventviewer.c). its memory is released by release(Resource::InitMemory)
void sysINIT(void *p)
{
    embot::os::rtos::scheduler_props_t *pp {reinterpret_cast<embot::os::rtos::scheduler_props_t*>(p)};
    embot::os::rtos::scheduler_props_Internals *i {reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(pp->getInternals())};
    if(nullptr != i->oninit)
    {
        i->oninit();
    }
    vTaskDelete(nullptr);
}

// the idle hook of FreeRTOS: it runs inside the idle task. The onidle() of embot::os::theScheduler contains its own forever
// loop and never returns: the idle task then never cleans the tasks which deleted themselves (only the init thread does it,
// and its memory is not released anyway). onidle() must never block.
void vApplicationIdleHook(void)
{
    embot::core::fpWorker onidle {nullptr};
    if(nullptr != embot::os::rtos::the_scheduler_props)
    {
        auto i = reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(embot::os::rtos::the_scheduler_props->getInternals());
        onidle = i->onidle;
    }

    if(nullptr != onidle)
    {
        onidle();
    }
}

// it overrides the weak one of the lib. It can be called from a task or from an exception (the stack overflow is detected
// at the context switch, inside PendSV): the text goes to embot::core::print() (ITM, safe there), then the user callback,
// then it stops here for the debugger.
uint32_t freertos_error_notify(uint32_t code, void *object)
{
    // indexed by the FREERTOS_ERROR_* codes (1..3), then by our errors of the heap; the others use "unknown"
    static const char * message[] =
    {
        "unknown",
        "freertos: stack overflow in thread",
        "freertos: assert failed",
        "freertos: queue of the timer task is full, timer",
        "freertos: no more heap for operator new",              // errNew (666)
        "freertos: no more heap for operator new(nothrow)"      // errNewNothrow (667)
    };

    uint32_t index {0};
    if((code >= FREERTOS_ERROR_STACK_OVERFLOW) && (code <= FREERTOS_ERROR_TIMERQUEUE))
    {
        index = code;
    }
    else if(embot::os::rtos::freertos::errNew == code)
    {
        index = 4;
    }
    else if(embot::os::rtos::freertos::errNewNothrow == code)
    {
        index = 5;
    }
    const char * errstr = message[index];
    void * thread = nullptr;

    // for this code object is the thread: show its name
    if(FREERTOS_ERROR_STACK_OVERFLOW == code)
    {
        thread = object;
    }

    std::string text {"freertos_error_notify(" + std::to_string(code) + "): " + errstr};
    if(nullptr != thread)
    {
        embot::os::Thread *t = embot::os::rtos::scheduler_getassociated(thread);
        const char *name = (nullptr != t) ? t->getName() : nullptr;
        text += " " + std::string((nullptr != name) ? name : "?");
    }
    embot::core::print(text);

    if(nullptr != embot::os::rtos::the_scheduler_props)
    {
        auto i = reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(embot::os::rtos::the_scheduler_props->getInternals());
        if(nullptr != i->onerror)
        {
            i->onerror(thread, static_cast<int>(code), errstr);
        }
    }

    for(;;);

    return 0;
}

// configASSERT() of FreeRTOSConfig.h calls it. It overrides the weak one of the lib
void freertos_assert_failed(const char *file, uint32_t line)
{
    embot::core::print(std::string("freertos: assert failed in ") + ((nullptr != file) ? file : "?") + " @ line " + std::to_string(line));
    freertos_error_notify(FREERTOS_ERROR_ASSERT, nullptr);
}

} // extern "C"


// --------------------------------------------------------------------------------------------------------------------
// - heap operators
//   new returns zeroed memory, as it did with osal, cmsisos2 and cmsisrtx. they use the heap of the arm c library, which
//   the freertos lib protects with the suspension of the scheduler (see _mutex_acquire() in cfg/freertos_lib.c).
// --------------------------------------------------------------------------------------------------------------------

void* operator new(std::size_t size) noexcept(false)
{
    void* ptr = calloc(size, 1);
    if(nullptr == ptr)
    {
        freertos_error_notify(embot::os::rtos::freertos::errNew, nullptr);
    }
    return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow_value) noexcept
{
    void* ptr = calloc(size, 1);
    if(nullptr == ptr)
    {
        freertos_error_notify(embot::os::rtos::freertos::errNewNothrow, nullptr);
    }
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    if(nullptr != ptr)
    {
        free(ptr);
    }
}

void operator delete(void* ptr, std::size_t size) noexcept
{
    (void)size;
    if(nullptr != ptr)
    {
        free(ptr);
    }
}

#endif // #if defined(EMBOT_USE_rtos_freertos)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

