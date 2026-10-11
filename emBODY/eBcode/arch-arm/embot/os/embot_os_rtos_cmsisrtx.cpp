
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// embot::os::rtos on top of the original, unmodified CMSIS-RTX (RTX5) built by the cmsisrtx project (armclang).
//
// - it is compiled only if EMBOT_USE_rtos_cmsisrtx is defined. embot_os_rtos.cpp (osal and cmsisos2) is not used then.
// - it uses only the public API of cmsisrtx: the single header cmsisrtx.h (CMSIS-RTOS2 API, OS Tick API and the sizes
//   of the control blocks). It does not see any internal structure of RTX.
// - the RTOS objects are created at run time with new: the memory of the control blocks, of the stacks and of the
//   queues is provided by us (cb_mem, stack_mem, mq_mem), not by the pool of RTX_Config.h.
// - the time in microseconds is the 64-bit extension of the tick of the kernel plus the counter of the SysTick.
//
// what the original RTX fixes at build time (RTX_Config.h) is set by scheduler_init() at run time with
// cmsisrtx_config_set(): the tick, the stacks of the idle and timer threads, the timer queue and the ISR queue, before the
// kernel starts. If the lib is built with CMSISRTX_RUNTIME_CONFIG=0 that is not possible: scheduler_init() then checks
// that the tick of RTX_Config.h (OS_TICK_FREQ) is the requested one and returns false otherwise.
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"

#if defined(EMBOT_USE_rtos_cmsisrtx)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "cmsisrtx.h"

#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <atomic>
#include <new>
#include <string>

// --------------------------------------------------------------------------------------------------------------------
// - helpers
// --------------------------------------------------------------------------------------------------------------------

// the entry function of the init thread. it is a C function with this name because the Event Viewer of uVision
// (see cmsisrtx/src/extra/cmsisrtx_eventviewer.c) names a thread after the symbol of its entry function
extern "C" void tINIT(void *p);

namespace embot { namespace os { namespace rtos {

    namespace cmsisrtx {

        // the length of the queues given to RTX by scheduler_init()
        constexpr uint32_t timerqueuelength {32};       // timer callbacks which can be pending for the timer thread
        constexpr uint32_t isrqueuelength {128};        // items of the ISR post processing queue

        // a stack size as RTX wants it: a multiple of 8, not less than the minimum
        constexpr uint32_t stacksize(uint32_t requested)
        {
            return (requested < CMSISRTX_STACK_MINSIZE) ? CMSISRTX_STACK_MINSIZE : ((requested + 7U) & ~7U);
        }

        // zeroed memory, 8-byte aligned by the C library
        inline void * alloc(size_t size)
        {
            return calloc(1, size);
        }

        // OS error codes (rtx_os.h): 2 isr queue overflow, 3 timer queue overflow, 4 libspace not available, 5 library mutex failure.
        // the ones we use and the ones we add for the heap
        constexpr uint32_t errStackOverflow {1};
        constexpr uint32_t errClibSpace {4};
        constexpr uint32_t errNew {666};
        constexpr uint32_t errNewNothrow {667};

        constexpr uint32_t priorityInitThread {osPriorityRealtime6};

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

        // -- the fast ram (cm7: DTCM, 0x20000000)
        // with EMBOT_OS_RTOS_CMSISRTX_FASTRAM the memory of the hottest objects of RTX (control block and stack of the threads,
        // idle and timer stacks, timer queue, ISR queue) comes from an area that the scatter file puts in DTCM, which is not
        // cached and has no wait states: it is what the context switch reads and writes. When the area is full, or without the
        // macro, the memory is the one of the heap, as before. The area is the section .bss.cmsisrtx_fastram of the scatter
        // file (see cfg/amcmj1.1cm7-lwip-0offset.fastram.sct in the application): its size must be the same.
        // It is not used for buffers of DMA: DTCM is not reachable by the DMA of the peripherals.
#if defined(EMBOT_OS_RTOS_CMSISRTX_FASTRAM)

#if !defined(EMBOT_OS_RTOS_CMSISRTX_FASTRAM_SIZE)
#define EMBOT_OS_RTOS_CMSISRTX_FASTRAM_SIZE (64U*1024U)
#endif

        // first fit allocator with a list of free blocks sorted by address. a block is a header of 8 bytes (its size, header
        // included) and the payload, so that the payload is 8-byte aligned. interrupts are disabled while the list is changed.
        class FastRam
        {
        public:
            constexpr FastRam(uint8_t *a, uint32_t s) : area(a), size(s), freelist(nullptr), inited(false) {}

            // zeroed memory, nullptr if there is no room
            void * alloc(size_t n)
            {
                if((0 == n) || (n > size))
                {
                    return nullptr;
                }
                uint32_t need {(static_cast<uint32_t>(n) + 7U) & ~7U};
                need += headersize;
                if(need < minblock)
                {
                    need = minblock;
                }
                uint8_t *payload {nullptr};
                const uint32_t primask {irqsave()};
                init();
                Block *prev {nullptr};
                for(Block *b = freelist; nullptr != b; prev = b, b = b->next)
                {
                    if(b->size < need)
                    {
                        continue;
                    }
                    Block *next {b->next};
                    if((b->size - need) >= minblock)
                    {   // we split it: the rest stays free
                        Block *rest {reinterpret_cast<Block*>(reinterpret_cast<uint8_t*>(b) + need)};
                        rest->size = b->size - need;
                        rest->next = next;
                        next = rest;
                        b->size = need;
                    }
                    (nullptr == prev) ? (freelist = next) : (prev->next = next);
                    payload = reinterpret_cast<uint8_t*>(b) + headersize;
                    break;
                }
                irqrestore(primask);
                if(nullptr != payload)
                {
                    memset(payload, 0, need - headersize);
                }
                return payload;
            }

            bool owns(const void *p) const
            {
                return (p >= area) && (p < (area + size));
            }

            void free(void *p)
            {
                Block *b {reinterpret_cast<Block*>(static_cast<uint8_t*>(p) - headersize)};
                const uint32_t primask {irqsave()};
                Block *prev {nullptr};
                Block *next {freelist};
                while((nullptr != next) && (next < b))
                {
                    prev = next;
                    next = next->next;
                }
                // we merge it with the following block, then with the previous one
                if((nullptr != next) && ((reinterpret_cast<uint8_t*>(b) + b->size) == reinterpret_cast<uint8_t*>(next)))
                {
                    b->size += next->size;
                    b->next = next->next;
                }
                else
                {
                    b->next = next;
                }
                if((nullptr != prev) && ((reinterpret_cast<uint8_t*>(prev) + prev->size) == reinterpret_cast<uint8_t*>(b)))
                {
                    prev->size += b->size;
                    prev->next = b->next;
                }
                else if(nullptr != prev)
                {
                    prev->next = b;
                }
                else
                {
                    freelist = b;
                }
                irqrestore(primask);
            }

            // bytes not allocated (in the free blocks, headers included): to be seen in the debugger
            uint32_t freebytes() const
            {
                uint32_t n {0};
                for(const Block *b = freelist; nullptr != b; b = b->next)
                {
                    n += b->size;
                }
                return n;
            }

        private:
            static constexpr uint32_t headersize {8};
            static constexpr uint32_t minblock {16};
            struct Block
            {
                uint32_t size;      // of the block, header included
                uint32_t pad;
                Block *next;        // used only when the block is free: it overlaps the payload
            };
            void init()
            {
                if(false == inited)
                {
                    freelist = reinterpret_cast<Block*>(area);
                    freelist->size = size;
                    freelist->next = nullptr;
                    inited = true;
                }
            }
            uint8_t *area;
            uint32_t size;
            Block *freelist;
            bool inited;
        };

        alignas(8) static uint8_t fastramarea[EMBOT_OS_RTOS_CMSISRTX_FASTRAM_SIZE] __attribute__((section(".bss.cmsisrtx_fastram")));
        static FastRam fastram {fastramarea, EMBOT_OS_RTOS_CMSISRTX_FASTRAM_SIZE};

#endif // EMBOT_OS_RTOS_CMSISRTX_FASTRAM

        // zeroed memory for the objects above. it never returns nullptr: when the heap is full operator new calls osRtxErrorNotify()
        inline void * fastalloc(size_t size)
        {
#if defined(EMBOT_OS_RTOS_CMSISRTX_FASTRAM)
            void *p {fastram.alloc(size)};
            if(nullptr != p)
            {
                return p;
            }
#endif
            return ::operator new(size);
        }

        inline void fastfree(void *p)
        {
            if(nullptr == p)
            {
                return;
            }
#if defined(EMBOT_OS_RTOS_CMSISRTX_FASTRAM)
            if(fastram.owns(p))
            {
                fastram.free(p);
                return;
            }
#endif
            ::operator delete(p);
        }

        inline bool isinisr()
        {
            uint32_t ipsr {0};
            __asm volatile ("mrs %0, ipsr" : "=r" (ipsr));
            return 0 != ipsr;
        }

        // microseconds per tick of the kernel. it is evaluated once: it is a build time constant of RTX
        inline uint32_t usecpertick(bool refresh = false)       // refresh: after the tick was configured
        {
            static uint32_t u {0};
            if((0 == u) || (true == refresh))
            {
                u = 1000000U / osKernelGetTickFreq();
            }
            return u;
        }

        // RTX allows a timeout only in thread mode: in an ISR the only valid value is 0
        inline uint32_t reltime2tick(embot::core::relTime t)
        {
            if(embot::core::reltimeWaitForever == t)
            {
                return osWaitForever;
            }
            if(isinisr() || (0 == t))
            {
                return 0;
            }
            // round up: a short but non-zero timeout must not become a no-wait
            const uint64_t u {usecpertick()};
            const uint64_t ticks {(static_cast<uint64_t>(t) + u - 1U) / u};
            return (ticks >= osWaitForever) ? (osWaitForever - 1U) : static_cast<uint32_t>(ticks);
        }

        // -- the time in microseconds since the start of the kernel

        // the tick of RTX is 32 bits (49 days at 1 kHz): we extend it to 64 bits. it must be called inside a critical
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

            uint32_t tick {osKernelGetTickCount()};
            uint32_t count {OS_Tick_GetCount()};            // counts elapsed in the current tick
            if(0U != OS_Tick_GetOverflow())
            {   // the systick has wrapped but its handler has not incremented the tick yet
                count = OS_Tick_GetCount();
                tick++;
            }
            const uint64_t t {tick64(tick)};

            irqrestore(primask);

            const uint64_t interval {OS_Tick_GetInterval()};
            const uint64_t upt {usecpertick()};
            return (t * upt) + ((static_cast<uint64_t>(count) * upt) / interval);
        }

        // the scheduler is not running until the kernel is started: the time stays at 0
        inline bool kernelstarted()
        {
            const osKernelState_t s {osKernelGetState()};
            return (osKernelInactive != s) && (osKernelReady != s);
        }

    } // namespace cmsisrtx

}}} // namespace embot { namespace os { namespace rtos {


// --------------------------------------------------------------------------------------------------------------------
// - the memory of the objects. it is allocated with new and it is owned by us.
//   RTX returns as id of an object the address of its cb_mem: hence cb_mem is the first member of every struct and
//   the id of the object is also the pointer to the struct.
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace os { namespace rtos {

    // -- the implemention of thread_props_t

    struct thread_props_Internals
    {
        osThreadFunc_t func {nullptr};
        void *argument {nullptr};
        osThreadAttr_t *attr {nullptr};

        constexpr thread_props_Internals() = default;

        void deallocate()
        {
            if(nullptr != attr)
            {
                cmsisrtx::fastfree(attr->stack_mem);
                cmsisrtx::fastfree(attr->cb_mem);
                delete attr;
                attr = nullptr;
            }
        }

        void allocate(uint16_t stacksize)
        {
            if(nullptr != attr)
            {
                deallocate();
            }
            // the stack of RTX must be 8-byte aligned and its size a multiple of 8
            const uint32_t size {((static_cast<uint32_t>(stacksize) + 7U) / 8U) * 8U};

            attr = new osThreadAttr_t {};
            attr->name = nullptr;
            attr->attr_bits = osThreadDetached;
            attr->cb_mem = cmsisrtx::fastalloc(CMSISRTX_THREAD_CB_SIZE);
            attr->cb_size = CMSISRTX_THREAD_CB_SIZE;
            attr->stack_mem = cmsisrtx::fastalloc(size);
            attr->stack_size = size;
            attr->priority = osPriorityNone;
            attr->tz_module = 0;
        }

        void prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize)
        {
            allocate(stacksize);
            attr->priority = static_cast<osPriority_t>(priority);
            func = function;
            argument = param;
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
            const bool had {nullptr != _internals.attr};
            _internals.deallocate();
            return had;
        }

        bool isvalid() const
        {
            return (nullptr != _internals.func) && (nullptr != _internals.attr);
        }

        void setprio(uint8_t priority)
        {
            if(nullptr != _internals.attr)
            {
                _internals.attr->priority = static_cast<osPriority_t>(priority);
            }
        }

        uint8_t getprio() const
        {
            return (nullptr != _internals.attr) ? static_cast<uint8_t>(_internals.attr->priority) : 0;
        }

        void * getstack(uint16_t &size) const
        {
            if(nullptr == _internals.attr)
            {
                size = 0;
                return nullptr;
            }
            size = static_cast<uint16_t>(_internals.attr->stack_size);
            return _internals.attr->stack_mem;
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

    struct osMessageQueueMemory
    {
        uint32_t cb_mem[CMSISRTX_MESSAGEQUEUE_CB_SIZE / sizeof(uint32_t)] {};    // keep it first
        osMessageQueueAttr_t attr {};
        uint32_t *mq {nullptr};

        osMessageQueueMemory() = default;
        ~osMessageQueueMemory()
        {
            delete [] mq;
        }

        void prepare(size_t length, size_t sizeofmsg)
        {
            const uint32_t mqsize {CMSISRTX_MESSAGEQUEUE_MEM_SIZE(static_cast<uint32_t>(length), static_cast<uint32_t>(sizeofmsg))};
            mq = new uint32_t[mqsize / sizeof(uint32_t)] {};
            attr.name = nullptr;
            attr.attr_bits = 0;
            attr.cb_mem = cb_mem;
            attr.cb_size = sizeof(cb_mem);
            attr.mq_mem = mq;
            attr.mq_size = mqsize;
        }

        osMessageQueueAttr_t * attribute() { return &attr; }
    };
    static_assert(0 == offsetof(osMessageQueueMemory, cb_mem), "cb_mem must be the first member");

}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a timer. the RTX timer is created at the first start, because RTX fixes at creation both the type
//    (once or periodic) and the function. we give RTX a trampoline and we keep here the user function and parameter:
//    this way we can change them at every start with no access to the internals of RTX.

namespace embot { namespace os { namespace rtos {

    struct osTimerMemory
    {
        uint32_t cb_mem[CMSISRTX_TIMER_CB_SIZE / sizeof(uint32_t)] {};           // keep it first
        osTimerAttr_t attr {};
        osTimerId_t id {nullptr};
        osTimerType_t type {osTimerPeriodic};
        fpOnTimerExpiry func {nullptr};
        void *param {nullptr};

        osTimerMemory()
        {
            attr.name = nullptr;
            attr.attr_bits = 0;
            attr.cb_mem = cb_mem;
            attr.cb_size = sizeof(cb_mem);
        }
        ~osTimerMemory() = default;

        // it runs inside the timer thread of RTX
        static void trampoline(void *arg)
        {
            osTimerMemory *m {static_cast<osTimerMemory*>(arg)};
            if((nullptr != m) && (nullptr != m->func))
            {
                m->func(m->param);
            }
        }
    };
    static_assert(0 == offsetof(osTimerMemory, cb_mem), "cb_mem must be the first member");

}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a mutex and of a semaphore

namespace embot { namespace os { namespace rtos {

    struct osMutexMemory
    {
        uint32_t cb_mem[CMSISRTX_MUTEX_CB_SIZE / sizeof(uint32_t)] {};           // keep it first
        osMutexAttr_t attr {};

        osMutexMemory()
        {
            attr.name = nullptr;
            attr.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
            attr.cb_mem = cb_mem;
            attr.cb_size = sizeof(cb_mem);
        }
        ~osMutexMemory() = default;
    };
    static_assert(0 == offsetof(osMutexMemory, cb_mem), "cb_mem must be the first member");

    struct osSemaphoreMemory
    {
        uint32_t cb_mem[CMSISRTX_SEMAPHORE_CB_SIZE / sizeof(uint32_t)] {};       // keep it first
        osSemaphoreAttr_t attr {};

        osSemaphoreMemory()
        {
            attr.name = nullptr;
            attr.attr_bits = 0;
            attr.cb_mem = cb_mem;
            attr.cb_size = sizeof(cb_mem);
        }
        ~osSemaphoreMemory() = default;
    };
    static_assert(0 == offsetof(osSemaphoreMemory, cb_mem), "cb_mem must be the first member");

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
        void *idlestack {nullptr};          // memory given to RTX by scheduler_init()
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
            _internals.idlestacksize = idlestacksize;       // idle and timer stacks are those of RTX_Config.h
            _internals.timerstacksize = timerstacksize;
            _internals.onidle = onidle;
            _internals.oninit = oninit;
            _internals.onerror = onerror;

            _internals.init_props.prepare(tINIT, owner, cmsisrtx::priorityInitThread, initstacksize);

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

        // the stack of the idle thread: allocated by scheduler_init() and given to RTX. (nullptr before it, or if the lib
        // does not support the run-time configuration: the stack is then the static one of RTX_Config.h)
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

        // the kernel was initialised before main() (the C library needs its mutexes): what it reads only at start can be
        // changed now. all the memory is given by us and it is never released.
        cmsisrtx_config_t cfg {};
        cfg.tick_freq = static_cast<uint32_t>(1000000U / i->ticktime);

        const uint32_t idlesize {cmsisrtx::stacksize(i->idlestacksize)};
        const uint32_t timersize {cmsisrtx::stacksize(i->timerstacksize)};
        void *idlestack {cmsisrtx::fastalloc(idlesize)};
        void *timerstack {cmsisrtx::fastalloc(timersize)};
        void *timermq {cmsisrtx::fastalloc(CMSISRTX_MESSAGEQUEUE_MEM_SIZE(cmsisrtx::timerqueuelength, CMSISRTX_TIMER_MSG_SIZE))};
        void **isrqueue {reinterpret_cast<void**>(cmsisrtx::fastalloc(cmsisrtx::isrqueuelength * sizeof(void*)))};
        if((nullptr == idlestack) || (nullptr == timerstack) || (nullptr == timermq) || (nullptr == isrqueue))
        {
            return false;
        }

        cfg.idle_stack_mem = idlestack;
        cfg.idle_stack_size = idlesize;
        cfg.timer_stack_mem = timerstack;
        cfg.timer_stack_size = timersize;
        cfg.timer_priority = 0;     // as OS_TIMER_THREAD_PRIO of RTX_Config.h
        cfg.timer_mq_mem = timermq;
        cfg.timer_mq_mem_size = CMSISRTX_MESSAGEQUEUE_MEM_SIZE(cmsisrtx::timerqueuelength, CMSISRTX_TIMER_MSG_SIZE);
        cfg.timer_mq_msgs = cmsisrtx::timerqueuelength;
        cfg.isr_queue_mem = isrqueue;
        cfg.isr_queue_size = cmsisrtx::isrqueuelength;

        const int32_t r {cmsisrtx_config_set(&cfg)};
        if(CMSISRTX_CONFIG_UNSUPPORTED == r)
        {   // lib built with CMSISRTX_RUNTIME_CONFIG=0: only the tick can be verified
            if((1000000U / i->ticktime) != osKernelGetTickFreq())
            {
                return false;
            }
        }
        else if(CMSISRTX_CONFIG_OK != r)
        {
            return false;
        }
        else
        {
            i->idlestack = idlestack;
            i->timerstack = timerstack;
            i->idlestacksize = static_cast<uint16_t>(idlesize);
            i->timerstacksize = static_cast<uint16_t>(timersize);
        }
        cmsisrtx::usecpertick(true);        // the tick may have changed

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

        // RTX initialises the kernel before main() inside _platform_post_stackheap_init() of rtx_lib.c, but if it
        // was not done (e.g. other C library) we do it now
        if(osKernelInactive == osKernelGetState())
        {
            osKernelInitialize();
        }

        if(nullptr != i->oninit)
        {
            thread_props_Internals *ti {reinterpret_cast<thread_props_Internals*>(i->init_props.getInternals())};
            osThreadNew(ti->func, ti->argument, ti->attr);
        }

        osKernelStart();
    }

    bool scheduler_isrunning()
    {
        return osKernelRunning == osKernelGetState();
    }

    embot::core::Time scheduler_timeget()
    {
        if(false == cmsisrtx::kernelstarted())
        {
            return 0;
        }

        const uint32_t primask {cmsisrtx::irqsave()};
        const int64_t offset {cmsisrtx::timeoffset};
        cmsisrtx::irqrestore(primask);

        return static_cast<embot::core::Time>(static_cast<int64_t>(cmsisrtx::rawusec()) + offset);
    }

    // the time is not changed in the RTOS: we keep the offset between the requested time and the one of the kernel
    bool scheduler_timeset(embot::core::Time t)
    {
        if(false == cmsisrtx::kernelstarted())
        {
            return false;
        }

        const int64_t raw {static_cast<int64_t>(cmsisrtx::rawusec())};
        const uint32_t primask {cmsisrtx::irqsave()};
        cmsisrtx::timeoffset = static_cast<int64_t>(t) - raw;
        cmsisrtx::irqrestore(primask);
        return true;
    }

    thread_t * scheduler_thread_running()
    {
        return osThreadGetId();
    }

    // -- the association of an RTX thread with its embot::os::Thread, and of a thread with its periodic timer.
    // they are fixed tables, not std::map: no heap, no lock. A writer disables the interrupts for the few instructions of the
    // update, a reader (also an ISR, or osRtxErrorNotify()) does not lock anything and never sees a half written item.
    namespace cmsisrtx {

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
        if((false == cmsisrtx::tableofthreads.add(osthread, t)) && (nullptr == cmsisrtx::tableofthreads.get(osthread)))
        {   // not already there, so the table is full
            embot::core::print("beware: embot::os::rtos::scheduler_associate(): more than " + std::to_string(cmsisrtx::maxthreads) +
                               " threads, increase cmsisrtx::maxthreads");
        }
    }

    void scheduler_deassociate(thread_t *osthread, embot::os::Thread *t)
    {
        cmsisrtx::tableofthreads.take(osthread);
    }

    embot::os::Thread * scheduler_getassociated(thread_t *osthread)
    {
        return cmsisrtx::tableofthreads.get(osthread);
    }

    void scheduler_lock()
    {
        osKernelLock();
    }

    void scheduler_unlock()
    {
        osKernelUnlock();
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
        return osThreadNew(i->func, i->argument, i->attr);
    }

    void thread_delete(thread_t *osthread)
    {
        // the memory is in thread_props_t and it is the responsibility of the caller to release it
        osThreadTerminate(osthread);
    }

    bool thread_setpriority(thread_t *osthread, embot::os::Priority priority)
    {
        return osOK == osThreadSetPriority(osthread, static_cast<osPriority_t>(priority));
    }

    void thread_period_set(thread_t *osthread, embot::core::relTime period)
    {
        // a periodic timer sets the event 0x1 of the thread. the timer of the thread is in a table, not in a std::map: no heap,
        // and two threads can call this at the same time
        static cmsisrtx::PtrTable<timer_t, cmsisrtx::maxperiodictimers> tableoftimers {};

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
                                   std::to_string(cmsisrtx::maxperiodictimers) + " periodic threads");
            }
        }
    }

    void thread_period_wait(thread_t *osthread, embot::core::relTime period)
    {
        event_get(embot::core::reltimeWaitForever);
    }

    void thread_sleep(thread_t *osthread, embot::core::relTime delay)
    {
        osDelay(cmsisrtx::reltime2tick(delay));
    }


    // -- event section

    bool event_set(thread_t *osthread, embot::os::Event event)
    {
        const uint32_t r {osThreadFlagsSet(osthread, event)};
        return 0 == (r & osFlagsError);
    }

    embot::os::EventMask event_get(embot::core::relTime timeout)
    {
        const uint32_t r {osThreadFlagsWait(0x7fffffff, osFlagsWaitAny, cmsisrtx::reltime2tick(timeout))};
        return (0 != (r & osFlagsError)) ? 0 : r;
    }

    embot::os::EventMask event_wait(const embot::os::EventMask mask, embot::core::relTime timeout)
    {
        return event_wait(mask, embot::os::EventWaitMode::ALL, timeout);
    }

    embot::os::EventMask event_wait(const embot::os::EventMask mask, embot::os::EventWaitMode ewm, embot::core::relTime timeout)
    {
        const uint32_t options {(embot::os::EventWaitMode::ALL == ewm) ? osFlagsWaitAll : osFlagsWaitAny};
        const uint32_t r {osThreadFlagsWait(mask, options, cmsisrtx::reltime2tick(timeout))};
        return (0 != (r & osFlagsError)) ? 0 : r;
    }


    // -- messagequeue section

    messagequeue_t * messagequeue_new(uint8_t length)
    {
        constexpr uint32_t sizeofmsg {sizeof(void*)};

        if(0 == length)
        {
            return nullptr;
        }
        osMessageQueueMemory *mem {new osMessageQueueMemory};
        mem->prepare(length, sizeofmsg);
        osMessageQueueId_t id {osMessageQueueNew(length, sizeofmsg, mem->attribute())};
        if(nullptr == id)
        {
            delete mem;
        }
        return id;
    }

    void messagequeue_delete(messagequeue_t *mq)
    {
        osMessageQueueMemory *mem {reinterpret_cast<osMessageQueueMemory*>(mq)};
        osMessageQueueDelete(mq);
        delete mem;
    }

    bool messagequeue_put(messagequeue_t *mq, embot::os::Message message, embot::core::relTime timeout)
    {
        return osOK == osMessageQueuePut(mq, &message, 0, cmsisrtx::reltime2tick(timeout));
    }

    embot::os::Message messagequeue_get(messagequeue_t *mq,  embot::core::relTime timeout)
    {
        embot::os::Message m {nullptr};
        if(osOK != osMessageQueueGet(mq, &m, nullptr, cmsisrtx::reltime2tick(timeout)))
        {
            return nullptr;
        }
        return m;
    }

    uint8_t messagequeue_size(messagequeue_t *mq)
    {
        return osMessageQueueGetCount(mq);
    }

    uint8_t messagequeue_available(messagequeue_t *mq)
    {
        return osMessageQueueGetSpace(mq);
    }


    // -- callbackqueue section

    callbackqueue_t * callbackqueue_new(uint8_t length)
    {
        constexpr uint32_t sizeofcbk {sizeof(embot::core::Callback)};

        if(0 == length)
        {
            return nullptr;
        }
        osMessageQueueMemory *mem {new osMessageQueueMemory};
        mem->prepare(length, sizeofcbk);
        osMessageQueueId_t id {osMessageQueueNew(length, sizeofcbk, mem->attribute())};
        if(nullptr == id)
        {
            delete mem;
        }
        return id;
    }

    void callbackqueue_delete(callbackqueue_t *cq)
    {
        osMessageQueueMemory *mem {reinterpret_cast<osMessageQueueMemory*>(cq)};
        osMessageQueueDelete(cq);
        delete mem;
    }

    bool callbackqueue_put(callbackqueue_t *cq, const embot::core::Callback &callback, embot::core::relTime timeout)
    {
        return osOK == osMessageQueuePut(cq, &callback, 0, cmsisrtx::reltime2tick(timeout));
    }

    embot::core::Callback callbackqueue_get(callbackqueue_t *cq,  embot::core::relTime timeout)
    {
        embot::core::Callback cbk {};
        if(osOK != osMessageQueueGet(cq, &cbk, nullptr, cmsisrtx::reltime2tick(timeout)))
        {
            return embot::core::Callback {};
        }
        return cbk;
    }

    uint8_t callbackqueue_size(callbackqueue_t *cq)
    {
        return osMessageQueueGetCount(cq);
    }

    uint8_t callbackqueue_available(callbackqueue_t *cq)
    {
        return osMessageQueueGetSpace(cq);
    }


    // -- timer section

    timer_t * timer_new()
    {
        // the pointer of the timer_t is the one of our struct. the RTX timer is created at the first start
        return new osTimerMemory;
    }

    void timer_delete(timer_t *t)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        if(nullptr == mem)
        {
            return;
        }
        if(nullptr != mem->id)
        {
            osTimerDelete(mem->id);
        }
        delete mem;
    }

    bool timer_running(timer_t *t)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        return (nullptr != mem->id) && (1 == osTimerIsRunning(mem->id));
    }

    bool timer_start(timer_t *t, timerMode mode, embot::core::relTime countdown, fpOnTimerExpiry onexpiry, void* param)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        const osTimerType_t type {(timerMode::oneshot == mode) ? osTimerOnce : osTimerPeriodic};

        if(nullptr != mem->id)
        {
            if(type != mem->type)
            {   // RTX fixes the type at creation: we recreate the timer on the same memory
                osTimerDelete(mem->id);
                mem->id = nullptr;
            }
            else if(1 == osTimerIsRunning(mem->id))
            {   // both osal and cmsisos2 allow to restart a running timer
                osTimerStop(mem->id);
            }
        }

        mem->func = onexpiry;
        mem->param = param;
        mem->type = type;

        if(nullptr == mem->id)
        {
            mem->id = osTimerNew(osTimerMemory::trampoline, type, mem, &mem->attr);
            if(nullptr == mem->id)
            {
                return false;
            }
        }

        return osOK == osTimerStart(mem->id, cmsisrtx::reltime2tick(countdown));
    }

    void timer_stop(timer_t *t)
    {
        osTimerMemory *mem {reinterpret_cast<osTimerMemory*>(t)};
        if(nullptr != mem->id)
        {
            osTimerStop(mem->id);
        }
    }


    // -- mutex section

    mutex_t * mutex_new()
    {
        osMutexMemory *mem {new osMutexMemory};
        osMutexId_t id {osMutexNew(&mem->attr)};
        if(nullptr == id)
        {
            delete mem;
        }
        return id;
    }

    bool mutex_take(mutex_t *m, embot::core::relTime timeout)
    {
        return osOK == osMutexAcquire(m, cmsisrtx::reltime2tick(timeout));
    }

    void mutex_release(mutex_t *m)
    {
        osMutexRelease(m);
    }

    void mutex_delete(mutex_t *m)
    {
        osMutexMemory *mem {reinterpret_cast<osMutexMemory*>(m)};
        osMutexDelete(m);
        delete mem;
    }


    // -- semaphore section

    semaphore_t * semaphore_new(uint8_t max, uint8_t cur)
    {
        osSemaphoreMemory *mem {new osSemaphoreMemory};
        osSemaphoreId_t id {osSemaphoreNew(max, cur, &mem->attr)};
        if(nullptr == id)
        {
            delete mem;
        }
        return id;
    }

    bool semaphore_acquire(semaphore_t *s, embot::core::relTime timeout)
    {
        return osOK == osSemaphoreAcquire(s, cmsisrtx::reltime2tick(timeout));
    }

    void semaphore_release(semaphore_t *s)
    {
        osSemaphoreRelease(s);
    }

    void semaphore_delete(semaphore_t *s)
    {
        osSemaphoreMemory *mem {reinterpret_cast<osSemaphoreMemory*>(s)};
        osSemaphoreDelete(s);
        delete mem;
    }


    // -- memory section
    //    the heap of the arm c library is protected by the mutexes that rtx_lib.c gives to the library
    //    (_mutex_acquire() etc). they do nothing inside an ISR: do not allocate from an ISR.

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
// - the functions that RTX (cfg/RTX_Config.c) finds: they override its weak ones
// --------------------------------------------------------------------------------------------------------------------

extern "C" {

// the init thread: it calls oninit() and exits. its memory is released by release(Resource::InitMemory)
void tINIT(void *p)
{
    embot::os::rtos::scheduler_props_t *pp {reinterpret_cast<embot::os::rtos::scheduler_props_t*>(p)};
    embot::os::rtos::scheduler_props_Internals *i {reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(pp->getInternals())};
    if(nullptr != i->oninit)
    {
        i->oninit();
    }
    osThreadExit();
}

// OS Idle Thread
[[noreturn]] void osRtxIdleThread(void *argument)
{
    (void)argument;

    embot::core::fpWorker onidle {nullptr};
    if(nullptr != embot::os::rtos::the_scheduler_props)
    {
        auto i = reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(embot::os::rtos::the_scheduler_props->getInternals());
        onidle = i->onidle;
    }

    for(;;)
    {
        if(nullptr != onidle)
        {
            onidle();
        }
    }
}

// OS Error Callback function. It overrides the weak one of the lib (cfg/RTX_Config.c). It can be called from a thread or from
// an exception: the text goes to embot::core::print() (ITM, safe there), then the user callback, then it stops here for the debugger.
uint32_t osRtxErrorNotify(uint32_t code, void *object_id)
{
    // indexed by the osRtxError* codes (1..5); 0 and the others (e.g. osRtxErrorSVC = 6) use "unknown"
    static const char * message[] =
    {
        "unknown",
        "os2: stack overflow in thread",
        "os2: isr queue overflow, object",
        "os2: timer queue overflow, timer",
        "os2: BEWARE! no libspace left: more than OS_THREAD_LIBSPACE_NUM (16) threads use the C library, thread",
        "os2: BEWARE! C library mutex failure: OS_DYNAMIC_MEM_SIZE too small"
    };

    const char * errstr = (code < 6) ? message[code] : message[0];
    void * thread = nullptr;

    // for these two codes object_id is the thread: show its name
    if((embot::os::rtos::cmsisrtx::errStackOverflow == code) || (embot::os::rtos::cmsisrtx::errClibSpace == code))
    {
        thread = object_id;
    }

    std::string text {"osRtxErrorNotify(" + std::to_string(code) + "): " + errstr};
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

} // extern "C"


// --------------------------------------------------------------------------------------------------------------------
// - heap operators
//   new returns zeroed memory, as it did with osal and cmsisos2. they use the heap of the arm c library, which RTX
//   protects with mutexes (see rtx_lib.c in cmsisrtx/cfg).
// --------------------------------------------------------------------------------------------------------------------

void* operator new(std::size_t size) noexcept(false)
{
    void* ptr = calloc(size, 1);
    if(nullptr == ptr)
    {
        osRtxErrorNotify(embot::os::rtos::cmsisrtx::errNew, nullptr);
    }
    return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow_value) noexcept
{
    void* ptr = calloc(size, 1);
    if(nullptr == ptr)
    {
        osRtxErrorNotify(embot::os::rtos::cmsisrtx::errNewNothrow, nullptr);
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

#endif // #if defined(EMBOT_USE_rtos_cmsisrtx)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

