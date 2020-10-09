
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#if defined(EMBOT_USE_rtos_cmsisos2)
#include "cmsisos2.h"
#elif defined(EMBOT_USE_rtos_osal)
#include "osal.h"
extern uint32_t SystemCoreClock; // osal needs a cpu speed
#define EMBOT_USE_rtos_osal_map_of_threads
#else
#error embot::os::rtos -> pls specify an RTOS xxxx to use by definition of EMBOT_USE_rtos_xxxx
#endif

#include "stdlib.h"

#include "embot_core_binary.h"

#include <map>


// --------------------------------------------------------------------------------------------------------------------
// - the code
// --------------------------------------------------------------------------------------------------------------------



// -- the implemention of thread_props_t


namespace embot { namespace os { namespace rtos {
    
#if defined(EMBOT_USE_rtos_cmsisos2)
       
    struct thread_props_Internals
    {
        osThreadFunc_t func {nullptr};
        void *argument {nullptr};
        osThreadAttr_t *attr {nullptr};  
        
        void deallocate()
        {
            if(nullptr != attr)
            {
                delete reinterpret_cast<uint64_t*>(attr->stack_mem);
                delete reinterpret_cast<uint32_t*>(attr->cb_mem);
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
            attr = new osThreadAttr_t;
            attr->name = nullptr;
            attr->attr_bits = osThreadDetached;
            attr->cb_mem = new uint32_t[osRtxThreadCbSize/sizeof(uint32_t)];
            attr->cb_size = osRtxThreadCbSize;
            attr->stack_size = stacksize; 
            attr->stack_mem = new uint64_t[attr->stack_size/sizeof(uint64_t)];
            attr->priority = osPriorityNone;
            attr->tz_module = 0;
            attr->reserved = 0;                                   
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

        
        ~Impl()
        {
            // we must deallocate the memory inside _internals.            
        }
               
        void prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize)
        {
            _internals.prepare(function, param, priority, stacksize);         
        }
        
        void release()
        {
            _internals.deallocate();         
        }
        
        bool isvalid() const
        {
            if((nullptr == _internals.func) || (nullptr == _internals.attr))
            {   // the other _internals.attr.cb_mem etc are surely non-nullptr
                return false;
            }
            return true;
        }
        
        void setprio(uint8_t priority)
        {
            _internals.attr->priority = static_cast<osPriority_t>(priority);
        }
    }; 

#elif defined(EMBOT_USE_rtos_osal)

    struct thread_props_Internals
    {
        osal_task_properties_t props {};                
    };
    
    struct thread_props_t::Impl
    {
        thread_props_Internals _internals {};
        
        Impl() 
        {
            _internals.props.function = nullptr;
            _internals.props.param = nullptr;
            _internals.props.priority = 1;
            _internals.props.stacksize = 1;
        }
        
        ~Impl(){}
               
        void prepare(embot::core::fpCaller function, void* param, uint8_t priority, uint16_t stacksize)
        {
           _internals.props.function = function; 
           _internals.props.param = param;
           _internals.props.priority = priority;
           _internals.props.stacksize = stacksize;    
        }
        
        void setprio(uint8_t priority)
        {
           _internals.props.priority = priority; 
        }
        
        bool isvalid() const
        {
            return true;
        }
        
        void release()
        {
            // we do nothing because osal own the memory
        }
    }; 
#endif    

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
        pImpl->release();
        return false;
    }        

}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of an mq

namespace embot { namespace os { namespace rtos {    
#if defined(EMBOT_USE_rtos_cmsisos2)    
    struct osMessageQueueMemory
    {
        osRtxMessageQueue_t mq {};
        osMessageQueueAttr_t attr {};
        osMessageQueueMemory() = default;
        ~osMessageQueueMemory()
        {
            if(nullptr != attr.mq_mem) 
            {
                delete reinterpret_cast<uint32_t*>(attr.mq_mem);    
            }                
        }
            
        void prepare(size_t length, size_t sizeofmsg)
        {
            attr.name = nullptr;
            attr.attr_bits = 0;
            attr.cb_size = sizeof(osRtxMessageQueue_t);
            attr.cb_mem = &mq;
            attr.mq_size = osRtxMessageQueueMemSize(length, sizeofmsg);
            attr.mq_mem = new uint32_t[attr.mq_size/sizeof(uint32_t)];            
        }
        
        osMessageQueueAttr_t * attribute() { return &attr; }
    };    
#endif
}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a timer

namespace embot { namespace os { namespace rtos {
#if defined(EMBOT_USE_rtos_cmsisos2)    
    struct osTimerMemory
    {
        static constexpr uint32_t cb_size {osRtxTimerCbSize};
        uint32_t cb_mem[cb_size/sizeof(uint32_t)] {0};
        osTimerAttr_t attr {};
            
        osTimerMemory() = default;
        ~osTimerMemory() {}
            
        void prepare()
        {        
            attr.name = nullptr;
            attr.attr_bits = 0;
            attr.cb_size = cb_size;
            attr.cb_mem = cb_mem;                        
        }
        
        osTimerAttr_t * attribute() { return &attr; }
    }; 
#endif
}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a mutex

namespace embot { namespace os { namespace rtos {
#if defined(EMBOT_USE_rtos_cmsisos2)    
    struct osMutexMemory
    {
        static constexpr uint32_t cb_size {osRtxMutexCbSize};
        uint32_t cb_mem[cb_size/sizeof(uint32_t)] {0};
        osMutexAttr_t attr {};
            
        osMutexMemory() = default;
        ~osMutexMemory() {}
            
        void prepare()
        {        
            attr.name = nullptr;
            attr.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
            attr.cb_size = cb_size;
            attr.cb_mem = cb_mem;                        
        }
        
        osMutexAttr_t * attribute() { return &attr; }
    }; 
#endif
}}} // namespace embot { namespace os { namespace rtos {


// -- the memory of a semaphore

namespace embot { namespace os { namespace rtos {
#if defined(EMBOT_USE_rtos_cmsisos2)    
    struct osSemaphoreMemory
    {
        static constexpr uint32_t cb_size {osRtxSemaphoreCbSize};
        uint32_t cb_mem[cb_size/sizeof(uint32_t)] {0};
        osSemaphoreAttr_t attr {};
            
        osSemaphoreMemory() = default;
        ~osSemaphoreMemory() {}
            
        void prepare()
        {        
            attr.name = nullptr;
            attr.attr_bits = 0;
            attr.cb_size = cb_size;
            attr.cb_mem = cb_mem;                        
        }
        
        osSemaphoreAttr_t * attribute() { return &attr; }
    }; 
#endif
}}} // namespace embot { namespace os { namespace rtos {


// -- the implemention of scheduler_props_t

namespace embot { namespace os { namespace rtos {
        
#if defined(EMBOT_USE_rtos_cmsisos2)
        
    struct scheduler_props_Internals
    {
        osRtxConfig_t* cfg {&osRtxConfig};        
        embot::core::fpWorker oninit {nullptr};  
        embot::core::fpWorker onidle {nullptr};
        scheduler_props_t::fpOnOSerror onerror {nullptr};  
        uint16_t initstacksize {0};
        thread_props_t init_props {};
        bool initted {false};
    };
    
    struct scheduler_props_t::Impl    
    { 
        scheduler_props_t * owner {nullptr};      
        // it wraps osRtxConfig plus oninit(), onidle(), onerror() ...
        scheduler_props_Internals _internals {};
                                                   
        // memory for the idle thread
        thread_props_t _thr_idle_props {};
        // memory for the timer thread and its messagequeue
        thread_props_t _thr_timer_props {};  
        osMessageQueueMemory _thr_timer_mq_mem {};            
               
        Impl(scheduler_props_t *o) { owner = o; }        
        ~Impl() {}
                                
        void clear()
        {
            static constexpr uint32_t osrtx_flags = 0 | osRtxConfigPrivilegedMode | osRtxConfigStackWatermark | osRtxConfigStackCheck; 
            
            std::memset(_internals.cfg, 0, sizeof(osRtxConfig_t));    
            
            _internals.cfg->flags = osrtx_flags;   
            _internals.cfg->tick_freq = 1000;            // tbc (to be changed later on in the code)   
            _internals.cfg->robin_timeout = 0; 
            _internals.cfg->isr_queue = {nullptr, 0, 0}; // to be allocated  
            _internals.cfg->mem = { nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0};                           // dont change it. we use external mem  
            _internals.cfg->mpi = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};   // dont change it. we use external mem 
            _internals.cfg->thread_stack_size = 0;
            _internals.cfg->idle_thread_attr = nullptr;  // to be allocated 
            _internals.cfg->timer_thread_attr = nullptr; // to be allocated 
            _internals.cfg->timer_mq_attr = nullptr;     // to be allocated 
            _internals.cfg->timer_mq_mcnt = 0;           // tbc
            
        }
                 
        void prepare(embot::core::Time ticktime, uint16_t initstacksize, uint16_t idlestacksize, 
                     embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror)
        {
            
            clear();
            
            // cpufreq is not required by cmsisos2
            _internals.cfg->tick_freq = (1000000/ticktime); // tick_freq is in hz, ticktime is in usec
                
            // isr queue: prepare memory and assign it to cfg (aka osRtxConfig)
            static constexpr uint32_t os2_isr_queue_size = 128;
            _internals.cfg->isr_queue = {new void*[os2_isr_queue_size], os2_isr_queue_size, 0};
            
            // idle thread: prepare memory and assign it to cfg (aka osRtxConfig)                 
            _thr_idle_props.prepare([](void*p){}, nullptr, osPriorityIdle, idlestacksize);
            thread_props_Internals* i_id = reinterpret_cast<thread_props_Internals*>(_thr_idle_props.getInternals());
            _internals.cfg->idle_thread_attr = i_id->attr;
            
            // timer thread: prepare memory and assign it to cfg (aka osRtxConfig)
            constexpr uint16_t timerstacksize = 1024;
            _thr_timer_props.prepare([](void*p){}, nullptr, osPriorityRealtime7, timerstacksize);
            thread_props_Internals* i_tm = reinterpret_cast<thread_props_Internals*>(_thr_timer_props.getInternals());
            _internals.cfg->timer_thread_attr = i_tm->attr;
                                               
            // timer mq: prepare memory and assign it to cfg (aka osRtxConfig)
            constexpr size_t timerqueuelength = 32; // or 32, 34, etc...
            constexpr size_t timerqueuesizeofmsg = 8; // it is the sizeof the structs that holds the funtion pointer plus its void* argument
            _thr_timer_mq_mem.prepare(timerqueuelength, timerqueuesizeofmsg);    
            _internals.cfg->timer_mq_attr = _thr_timer_mq_mem.attribute();   
            _internals.cfg->timer_mq_mcnt = timerqueuelength;
                
                
            // init thread
                
            auto cmsisos2init = [](void *p) 
            { 
                scheduler_props_t * pp= reinterpret_cast<scheduler_props_t*>(p);
                scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(pp->getInternals());
                if(nullptr != i->oninit)
                {
                   i->oninit(); 
                }
                
                // however, in here the thread stops but its memory is not released ... we should ask another thread to do it. maybe the idle ...
                // we could: inside i->oninit()  we call a method to mark the threaadinit stopped. then we stop it in here. 
                // when the idle thread exeutes, it checks if init is stopped. if so, it releases its resources.
                // we shall do it later on ...
                
                osThreadExit();            
            };    
                
            _internals.init_props.prepare(cmsisos2init, owner, osPriorityRealtime6, initstacksize);

            // other user-defined stuff                
            _internals.initstacksize = initstacksize;
            _internals.onidle = onidle;
            _internals.onerror = onerror;   
            _internals.oninit = oninit;   

            _internals.initted = true;
        }

        bool isvalid() const
        {
            return _internals.initted;
        }  

        bool releaseinit()
        {
            _internals.init_props.release();
            return true;
        }            
    }; 


#elif defined(EMBOT_USE_rtos_osal)
    
    struct scheduler_props_Internals
    {
        osal_cfg_t cfg {};        
        embot::core::fpWorker oninit {nullptr};            
    };
    
    struct scheduler_props_t::Impl    
    {
        scheduler_props_t *owner {nullptr};
        
        using fpOnOSALerror = void (*) (void* thread, osal_fatalerror_t errorcode, const char * errormsg);
        
        scheduler_props_Internals _internals {};
        
        
        Impl(scheduler_props_t *o) { owner = o; }
        
        ~Impl() {}
        
        void reset()
        {
            std::memset(&_internals.cfg, 0, sizeof(osal_cfg_t));
            
            _internals.cfg.rtostype = osal_rtostype_oosiit;
            _internals.cfg.memorymodel = osal_memmode_dynamic;
            _internals.cfg.prio = 15;
            _internals.cfg.cpufam = osal_cpufam_armcm4;  
            _internals.cfg.cpufreq = 0; 
            _internals.cfg.tick = embot::core::time1millisec;
            _internals.cfg.launcherstacksize = 2048;
            _internals.cfg.idlestacksize = 512;
            _internals.cfg.roundrobin = osal_false;
            _internals.cfg.roundrobintick = 0;
            _internals.cfg.tasknum = 0;
            // etc.
            _internals.cfg.extfn.usr_on_fatal_error = nullptr;
            _internals.cfg.extfn.usr_on_idle = nullptr;    
        }
                
        void prepare(embot::core::Time ticktime, uint16_t initstacksize, uint16_t idlestacksize, 
                 embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror)
        {
        
            reset();
            _internals.cfg.cpufreq = SystemCoreClock;
            _internals.cfg.tick = ticktime;            
            _internals.cfg.launcherstacksize = initstacksize;        
            _internals.cfg.idlestacksize = idlestacksize;  
            _internals.cfg.extfn.usr_on_fatal_error = reinterpret_cast<fpOnOSALerror>(onerror);
            _internals.cfg.extfn.usr_on_idle = onidle; 
            _internals.oninit = oninit;
        }  

        bool isvalid() const
        {
            return true;
        }
        
        bool releaseinit()
        {
            return true;
        }
    };  

#endif


    scheduler_props_t::scheduler_props_t()
    : pImpl(new Impl(this))
    {           
    }

    scheduler_props_t::~scheduler_props_t()
    {   
        delete pImpl;
    }

    
    void scheduler_props_t::prepare(embot::core::Time ticktime, uint16_t initstacksize, uint16_t idlestacksize, 
                 embot::core::fpWorker onidle, embot::core::fpWorker oninit, fpOnOSerror onerror)
    {
        pImpl->prepare(ticktime, initstacksize, idlestacksize, onidle, oninit, onerror);
    }  

    void * scheduler_props_t::getInternals()
    {
        return &pImpl->_internals;
    }
   
    bool scheduler_props_t::isvalid() const
    {
        return true;
    }
    
    bool scheduler_props_t::release(Resource res)
    {
        return pImpl->releaseinit();
    }
    

}}} // namespace embot { namespace os { namespace rtos {





namespace embot { namespace os { namespace rtos {
    
    
    // -- scheduler section
    
    // pointer to scheduler props: it helps keeping parameters across scheduler_init() and scheduler_start()
    scheduler_props_t *the_scheduler_props {nullptr};          

    
    bool scheduler_init(scheduler_props_t &props)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        
        if(false == props.isvalid())
        {
            return false;
        }        
        
#elif defined(EMBOT_USE_rtos_osal)
        
        if(false == props.isvalid())
        {
            return false;
        }
        
        std::uint32_t ram08size = 0;
        uint64_t *ram08data = NULL;
        
        scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(props.getInternals());
        osal_base_memory_getsize(&i->cfg, &ram08size);
        
        if(0 != ram08size)
        {
            ram08data = (uint64_t*)(uint64_t*)osal_base_memory_new(ram08size);
        }
        
        osal_base_initialise(&i->cfg, ram08data);   
               
#endif  

        // we save the props in here 
        the_scheduler_props = &props;
        
        return true;
    }

    
    void scheduler_start()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        
        if((nullptr == the_scheduler_props) || (false == the_scheduler_props->isvalid()))
        {
            return;
        }

        scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(the_scheduler_props->getInternals());
        
        osRtxInfo.kernel.state = osRtxKernelInactive;
        osKernelInitialize();
        
        if(nullptr != i->oninit)
        {
            
            auto cmsisos2init = [](void *p) 
            { 
                scheduler_props_t * pp= reinterpret_cast<scheduler_props_t*>(p);
                scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(pp->getInternals());
                if(nullptr != i->oninit)
                {
                   i->oninit(); 
                }
                
                // however, in here the thread stops but its resources are not made available. ... we should ask another thread to do it. maybe the idle ...
                // we shall do it later on ...
                osThreadExit();            
            };
                    
            thread_props_t *props = new thread_props_t;
            props->prepare(cmsisos2init, the_scheduler_props, osPriorityRealtime6, i->initstacksize);
            thread_props_Internals* i = reinterpret_cast<thread_props_Internals*>(props->getInternals());        
            osThreadNew(i->func, i->argument, i->attr);            
        }
        
        osKernelStart();

#elif defined(EMBOT_USE_rtos_osal)        

        if((nullptr == the_scheduler_props) || (false == the_scheduler_props->isvalid()))
        {
            return;
        }
        
        scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(the_scheduler_props->getInternals());        
        osal_system_start(i->oninit);  
#endif        
    }
    
    
    void scheduler_start2()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        
        if((nullptr == the_scheduler_props) || (false == the_scheduler_props->isvalid()))
        {
            return;
        }

        scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(the_scheduler_props->getInternals());
        
        osRtxInfo.kernel.state = osRtxKernelInactive;
        osKernelInitialize();
        
        if(nullptr != i->oninit)
        {
            thread_props_Internals *ti = reinterpret_cast<thread_props_Internals*>(i->init_props.getInternals());       
            osThreadNew(ti->func, ti->argument, ti->attr);            
        }
        
        osKernelStart();

#elif defined(EMBOT_USE_rtos_osal)        

        if((nullptr == the_scheduler_props) || (false == the_scheduler_props->isvalid()))
        {
            return;
        }
        
        scheduler_props_Internals* i = reinterpret_cast<scheduler_props_Internals*>(the_scheduler_props->getInternals());        
        osal_system_start(i->oninit);  
#endif        
    }

    
    bool scheduler_isrunning()
    { 
#if defined(EMBOT_USE_rtos_cmsisos2)
        return osKernelRunning == osKernelGetState(); 
#elif defined(EMBOT_USE_rtos_osal)
        return osal_info_status_running == osal_info_get_status(); 
#endif            
    }

// needs stm32hal.h w/ all its stuff to see the __NOP() ....    
//    static core::Time _fakenow()
//    {
//        // if kernel is not running wait approximately 1 us then increment 
//        // and return auxiliary tick counter value
//        static volatile core::Time tt = 0;
//        //for(int i = (SystemCoreClock >> 14U); i > 0U; i--) { // that is 1 ms
//        for(int i = (SystemCoreClock >> 4U); i > 0U; i--) {
//            __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
//            __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
//        }        
//        return ++tt;        
//    }
    
    static core::Time _fakenow() { return 0; }
    
    embot::core::Time scheduler_timeget()
    {
        if(!scheduler_isrunning())
        {
            return _fakenow();
        }
        
#if defined(EMBOT_USE_rtos_cmsisos2)
        return cmsisos2_sys_abstime();
#elif defined(EMBOT_USE_rtos_osal)
        return osal_system_abstime_get();
#endif        
    } 
    
    
    thread_t * scheduler_thread_running()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        return osThreadGetId();
#elif defined(EMBOT_USE_rtos_osal)        
        return osal_task_get(osal_callerAUTOdetect);
#endif        
    }    

    std::map<thread_t*, embot::os::Thread*> mapofthreads;
    
    void scheduler_associate(thread_t *osthread, embot::os::Thread *t)
    {
#if defined(EMBOT_USE_rtos_cmsisos2) || (defined(EMBOT_USE_rtos_osal) && defined(EMBOT_USE_rtos_osal_map_of_threads))
    mapofthreads.insert(std::pair<thread_t*, embot::os::Thread*>(osthread, t));        
#elif defined(EMBOT_USE_rtos_osal)        
        osal_task_extdata_set(reinterpret_cast<osal_task_t*>(osthread), t); 
#endif        
    }
    
    void scheduler_deassociate(thread_t *osthread, embot::os::Thread *t)
    {
#if defined(EMBOT_USE_rtos_cmsisos2) || (defined(EMBOT_USE_rtos_osal) && defined(EMBOT_USE_rtos_osal_map_of_threads))
    mapofthreads.erase(osthread);        
#elif defined(EMBOT_USE_rtos_osal)        
        osal_task_extdata_set(reinterpret_cast<osal_task_t*>(osthread), t); 
#endif        
    }    
    
    embot::os::Thread * scheduler_getassociated(thread_t *osthread)
    {
        if(nullptr == osthread)
        {
            return nullptr;
        }
        
#if defined(EMBOT_USE_rtos_cmsisos2) || (defined(EMBOT_USE_rtos_osal) && defined(EMBOT_USE_rtos_osal_map_of_threads))
        auto a = mapofthreads.find(osthread);
        if(a == mapofthreads.end())
        {
            return nullptr;
        }
        return a->second;          
#elif defined(EMBOT_USE_rtos_osal)
        return reinterpret_cast<Thread*>(osal_task_extdata_get(reinterpret_cast<osal_task_t*>(osthread)));
#endif   
        
    }

    void scheduler_lock()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osKernelLock();
#elif defined(EMBOT_USE_rtos_osal)
        osal_system_scheduling_suspend();
#endif          
    }
    

    void scheduler_unlock()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osKernelUnlock();
#elif defined(EMBOT_USE_rtos_osal)
        osal_system_scheduling_restart();
#endif  
    }        
    // -- thread section
    

    thread_t * thread_new(thread_props_t &props)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        // every memory allocation is done internally to thread_props_t
        if(false == props.isvalid())
        {
            return nullptr;
        }
        
        thread_props_Internals* i = reinterpret_cast<thread_props_Internals*>(props.getInternals());        
        return osThreadNew(i->func, i->argument, i->attr);
#elif defined(EMBOT_USE_rtos_osal)
        thread_props_Internals* i = reinterpret_cast<thread_props_Internals*>(props.getInternals());        
        return osal_task_new1(&i->props);
#endif        
    }  

    void thread_delete(thread_t *osthread)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        // every memory allocation was done internally to thread_props_t, hence is is responibility of the caller to deallocate
        // in here we just terminate the thread
        osThreadTerminate(osthread);
#elif defined(EMBOT_USE_rtos_osal)       
        osal_task_delete(reinterpret_cast<osal_task_t*>(osthread));
#endif        
    }    
    
    bool thread_setpriority(thread_t *osthread, embot::os::Priority priority)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osStatus_t r = osThreadSetPriority(osthread, static_cast<osPriority_t>(priority));
        return (osOK == r) ? (true) : (false);
#elif defined(EMBOT_USE_rtos_osal)     
        osal_result_t r = osal_task_priority_set(reinterpret_cast<osal_task_t*>(osthread), embot::core::tointegral(priority));
        return (osal_res_OK == r) ? (true) : (false); 
#endif        
    }
    
    void thread_period_set(thread_t *osthread, embot::core::relTime period)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        // i could ... associate a timer to the thread. the timer sends an evt. the
        static std::map<thread_t*, timer_t *> mapoftimers;
       
        if(0 != period)
        {
            timer_t *tmr = timer_new();
            timer_start(tmr, timerMode::forever, period, [](void *p){ event_set(p, 0x1); }, osthread);
            
            mapoftimers.insert(std::pair<thread_t*, timer_t *>(osthread, tmr)); 
        }
        else
        {
            auto a = mapoftimers.find(osthread);
            if(a == mapoftimers.end())
            {
                return;
            }
            timer_t *tmr = a->second;               
            
            timer_stop(tmr);
            timer_delete(tmr);    
            mapoftimers.erase(osthread);            
        }
#elif defined(EMBOT_USE_rtos_osal)  
        if(0 != period)
        {
            osal_task_period_set(static_cast<osal_reltime_t>(period));
        }
#endif        
    }
    

    void thread_period_wait(thread_t *osthread, embot::core::relTime period)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        event_get(cmsisos_reltimeforever);       
#elif defined(EMBOT_USE_rtos_osal)      
        osal_task_period_wait();
#endif        
    }

    void thread_sleep(thread_t *osthread, embot::core::relTime delay)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osDelay(cmsisos2_sys_reltime2tick(timeout));            
#elif defined(EMBOT_USE_rtos_osal)      
        osal_task_wait(static_cast<osal_reltime_t>(delay));
#endif        
    }    

        
    
    // -- event section
    
    bool event_set(thread_t *osthread, embot::os::Event event)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osThreadFlagsSet(osthread, event);
        return true;
#elif defined(EMBOT_USE_rtos_osal)      
        osal_result_t r = osal_eventflag_set(static_cast<osal_eventflag_t>(event), reinterpret_cast<osal_task_t*>(osthread), osal_callerAUTOdetect);
        return (osal_res_OK == r) ? (true) : (false);
#endif        
    }

    embot::os::EventMask event_get(embot::core::relTime timeout)
    { 
#if defined(EMBOT_USE_rtos_cmsisos2)
        uint32_t r =  osThreadFlagsWait(0x7fffffff, osFlagsWaitAny, cmsisos2_sys_reltime2tick(timeout));
        if(osFlagsErrorTimeout == r)
        {
            return 0;
        }
        return r;
#elif defined(EMBOT_USE_rtos_osal)      
        return osal_eventflag_getany(static_cast<osal_reltime_t>(timeout)); 
#endif        
    }  

    
    // -- messagequeue section
        
    messagequeue_t * messagequeue_new(uint8_t length)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)        
        constexpr uint32_t sizeofmsg = sizeof(void*);

        if(0 == length)
        {
            return nullptr;
        }
        osMessageQueueMemory *mem = new osMessageQueueMemory;
        mem->prepare(length, sizeofmsg);
        return osMessageQueueNew(length, sizeofmsg, mem->attribute());
        
#elif defined(EMBOT_USE_rtos_osal)       
        return osal_messagequeue_new(length);
#endif       
    }
    
    void messagequeue_delete(messagequeue_t *mq)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osMessageQueueMemory *mem = reinterpret_cast<osMessageQueueMemory*>(mq);
        osMessageQueueDelete(mq);
        delete mem;
#elif defined(EMBOT_USE_rtos_osal)       
        osal_messagequeue_delete(reinterpret_cast<osal_messagequeue_t*>(mq));
#endif        
    }

    bool messagequeue_put(messagequeue_t *mq, embot::os::Message message, embot::core::relTime timeout)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)        
        osStatus_t r = osMessageQueuePut(mq, &message, 0, cmsisos2_sys_reltime2tick(timeout));
        return (osOK == r) ? (true) : (false);
#elif defined(EMBOT_USE_rtos_osal)       
        osal_result_t r = osal_messagequeue_put(reinterpret_cast<osal_messagequeue_t*>(mq), reinterpret_cast<osal_message_t>(message), static_cast<osal_reltime_t>(timeout), osal_callerAUTOdetect);
        return (osal_res_OK == r) ? (true) : (false);
#endif        
    }

    embot::os::Message messagequeue_get(messagequeue_t *mq,  embot::core::relTime timeout)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        embot::os::Message m {0};
        osStatus_t r = osMessageQueueGet(mq, &m, nullptr, cmsisos2_sys_reltime2tick(timeout));  
        if(osErrorTimeout == r)
        {
            return 0;
        }
        return m;
#elif defined(EMBOT_USE_rtos_osal)     
        return reinterpret_cast<embot::os::Message>(osal_messagequeue_getquick(reinterpret_cast<osal_messagequeue_t*>(mq), static_cast<osal_reltime_t>(timeout), osal_callerAUTOdetect));
#endif    
    }
    
    uint8_t messagequeue_size(messagequeue_t *mq)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        return osMessageQueueGetCount(mq);
#elif defined(EMBOT_USE_rtos_osal)     
        return osal_messagequeue_size(reinterpret_cast<osal_messagequeue_t*>(mq), osal_callerAUTOdetect);
#endif    
    }
    
    // -- timer section
    
    
    timer_t * timer_new()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     

        osTimerMemory *mem = new osTimerMemory; 
        mem->prepare();
        constexpr auto cmsisos2timerdummy = [] (void *p) { p = p; };
        return osTimerNew(cmsisos2timerdummy, osTimerPeriodic, nullptr, mem->attribute());
                
#else        
        return osal_timer_new();
#endif
    }
    
    void timer_delete(timer_t *t)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osTimerMemory *mem = reinterpret_cast<osTimerMemory*>(t);
        osTimerDelete(t);
        delete mem;
#elif defined(EMBOT_USE_rtos_osal)       
        osal_timer_delete(reinterpret_cast<osal_timer_t*>(t));
#endif        
    }


    bool timer_start(timer_t *t, timerMode mode, embot::core::relTime countdown, fpOnTimerExpiry onexpiry, void* param)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)

        if(1 == osTimerIsRunning(t))
        {
            osTimerStop(t);
        }
            
        osRtxTimer_t *ost = reinterpret_cast<osRtxTimer_t*>(t);
        ost->finfo.func = onexpiry;
        ost->finfo.arg = param;
        ost->type = ( (timerMode::oneshot == mode) ? (osTimerOnce) : (osTimerPeriodic) );
        
        osStatus_t r = osTimerStart(t, cmsisos2_sys_reltime2tick(countdown));
        return (osOK == r) ? (true) : (false);
    
#elif defined(EMBOT_USE_rtos_osal) 
        
        using osal_fpOnTimerExpiry = void (*)(osal_timer_t *, void *);
    
        osal_timer_timing_t timing { 
            .startat = OSAL_abstimeNONE, 
            .count = countdown, 
            .mode = ( (timerMode::oneshot == mode) ? (osal_tmrmodeONESHOT) : (osal_tmrmodeFOREVER) )
        };
        // osal_timer_onexpiry_t onexpi {.cbk = function, .par = param };
        osal_timer_onexpiry_t onexpi {.cbk = reinterpret_cast<osal_fpOnTimerExpiry>(onexpiry), .par = param };
        osal_result_t r = osal_timer_start(reinterpret_cast<osal_timer_t*>(t), &timing, &onexpi, osal_callerAUTOdetect);    

        return (osal_res_OK == r) ? true : false; 
#endif        
    }
     
    
    void timer_stop(timer_t *t)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osTimerStop(t);
#elif defined(EMBOT_USE_rtos_osal)      
        osal_timer_stop(reinterpret_cast<osal_timer_t*>(t), osal_callerAUTOdetect);
#endif        
    }

    
    // -- mutex section
    
    mutex_t * mutex_new()
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     
        osMutexMemory *mem = new osMutexMemory; 
        mem->prepare();
        return osMutexNew(mem->attribute());                
#else        
        return osal_mutex_new();
#endif
    } 
    
    bool mutex_take(mutex_t *m, embot::core::relTime timeout)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     
        return (osOK == osMutexAcquire(m, cmsisos2_sys_reltime2tick(timeout))) ? true : false;                 
#else           
        return (osal_res_OK == osal_mutex_take(reinterpret_cast<osal_mutex_t*>(m), timeout)) ? true : false; 
#endif
    }
    
    void mutex_release(mutex_t *m)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     
        osMutexRelease(m);                 
#else          
        osal_mutex_release(reinterpret_cast<osal_mutex_t*>(m));
#endif
    } 
    
    void mutex_delete(mutex_t *m)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osMutexMemory *mem = reinterpret_cast<osMutexMemory*>(m);
        osMutexDelete(m);
        delete mem;
#elif defined(EMBOT_USE_rtos_osal)       
        osal_mutex_delete(reinterpret_cast<osal_mutex_t*>(m));
#endif         
    }
    
    // -- semaphore section
    
    semaphore_t * semaphore_new(uint8_t max, uint8_t cur)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     
        osSemaphoreMemory *mem = new osSemaphoreMemory; 
        mem->prepare();
        return osSemaphoreNew(max, cur, mem->attribute());                
#else        
        return osal_semaphore_new(max, cur);
#endif
    } 
    
    bool semaphore_acquire(semaphore_t *s, embot::core::relTime timeout)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     
        return (osOK == osSemaphoreAcquire(s, cmsisos2_sys_reltime2tick(timeout))) ? true : false;                 
#else           
        return (osal_res_OK == osal_semaphore_decrement(reinterpret_cast<osal_semaphore_t*>(s), timeout)) ? true : false; 
#endif
    }
    
    void semaphore_release(semaphore_t *s)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)     
        osSemaphoreRelease(s);                 
#else          
        osal_semaphore_increment(reinterpret_cast<osal_semaphore_t*>(s), osal_callerAUTOdetect);
#endif
    } 
    
    void semaphore_delete(semaphore_t *s)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        osSemaphoreMemory *mem = reinterpret_cast<osSemaphoreMemory*>(s);
        osSemaphoreDelete(s);
        delete mem;
#elif defined(EMBOT_USE_rtos_osal)       
        osal_semaphore_delete(reinterpret_cast<osal_semaphore_t*>(s));
#endif         
    }
    
    void * memory_new(size_t size)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
        return cmsisos2_memory_new(size);
#elif defined(EMBOT_USE_rtos_osal)       
        return osal_base_memory_new(size);
#endif  
    } 
    
    void * memory_realloc(void *mem, size_t size)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
//        return cmsisos2_memory_realloc(mem, size);
        return realloc(mem, size);
#elif defined(EMBOT_USE_rtos_osal)       
        return osal_base_memory_realloc(mem, size);
#endif 
    }  
    
    void memory_delete(void *mem)
    {
#if defined(EMBOT_USE_rtos_cmsisos2)
 //       cmsisos2_memory_delete(mem);
        free(mem);
#elif defined(EMBOT_USE_rtos_osal)       
        osal_base_memory_del(mem);
#endif 
    }        
    
    
}}} // namespace embot { namespace os { namespace rtos {



#if defined(EMBOT_USE_rtos_cmsisos2)

// -- cmsisos2 adaptation layer
//    it is C code which cmsisos2 must find

osRtxConfig_t osRtxConfig {};
                 

// OS Idle Thread
__NO_RETURN void osRtxIdleThread (void *argument) 
{
    (void)argument;

    embot::os::rtos::scheduler_props_Internals* i = reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(embot::os::rtos::the_scheduler_props->getInternals());

    if(nullptr != i->onidle)
    {
        i->onidle();
    }

    for(;;);
}
    
// OS Error Callback function
uint32_t osRtxErrorNotify (uint32_t code, void *object_id) 
{
//    (void)object_id;
    static const char * message[] = 
    {
        "unknown",
        "os2: stack undeflow in ...",
        "os2: isr queue overflow in ...",
        "os2: tmr queue overflow",
        "os2: libspace not available",
        "os2: library mutex initialization failure"        
    };

    if(nullptr != embot::os::rtos::the_scheduler_props)
    {
        embot::os::rtos::scheduler_props_Internals* i = reinterpret_cast<embot::os::rtos::scheduler_props_Internals*>(embot::os::rtos::the_scheduler_props->getInternals());

        const char * errstr = (code < 6) ? message[code] : message[0];
        void * thread = nullptr;
        
        if(osRtxErrorStackUnderflow == code)
        {
            thread = object_id;
            embot::os::Thread *t = embot::os::rtos::scheduler_getassociated(thread);
            const char *name = t->getName();
            if(nullptr != name)
            {
                embot::core::print(std::string(errstr) + " " + name); 
            }
        }
                      
        if(nullptr != i->onerror)
        {
            i->onerror(thread, code, errstr);
        }
    }
    
    for(;;);
    
    return 0;
    
//// OS Error Codes
//#define osRtxErrorStackUnderflow        1U  ///< Stack overflow, i.e. stack pointer below its lower memory limit for descending stacks. (thread_id=object_id)
//#define osRtxErrorISRQueueOverflow      2U  ///< ISR Queue overflow detected when inserting object. (object_id)
//#define osRtxErrorTimerQueueOverflow    3U  ///< User Timer Callback Queue overflow detected for timer. (timer_id=object_id)
//#define osRtxErrorClibSpace             4U  ///< Standard C/C++ library libspace not available: increase OS_THREAD_LIBSPACE_NUM. (thread_id=object_id)
//#define osRtxErrorClibMutex             5U  ///< Standard C/C++ library mutex initialization failed.

}    


#endif





#if defined(EMBOT_USE_rtos_osal)
    
    // osal is able to protect heap allocator/disallocator vs multithread use
    // in two equally effective modes: 
    // - the arm c libraries multithread support as described by
    //   https://developer.arm.com/documentation/dui0475/m/the-arm-c-and-c---libraries/multithreaded-support-in-arm-c-libraries?lang=en  
    // - use of custom heap manager functions (allocator and disallocator) protected by svc calls.
    //   these functions are: osal_base_memory_new() and osal_base_memory_del()
    // on eth boards we have used both modes, even if this double protection is somehow redundant. 
    // on the embot::os can boards we have used only the custom heap manager functions called inside redefined new and delete operators.

    // hence for the embot::os::rtos with osal we keep on using the same strategy. 
    // it has worked so far and we dont want to change its behaviour.
    // use macros ....
    
    // #define EMBOT_HEAP_redefine_heapoperators

    #define EMBOT_HEAP_PROTECTION_use_OSsupport
    //#define EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport
        
    #if defined(EMBOT_HEAP_PROTECTION_use_OSsupport) & !defined(EMBOT_HEAP_redefine_heapoperators)
        #define EMBOT_HEAP_redefine_heapoperators
    #endif  

    #if defined(EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport) & !defined(EMBOT_HEAP_redefine_heapoperators)
        #define EMBOT_HEAP_redefine_heapoperators
    #endif

#elif defined(EMBOT_USE_rtos_cmsisos2)

    // cmsisos2 is able to protects heap allocator/disallocator vs multithread use
    // with the same mechanisms as osal.
    
    // the same mechanism of osal ( ... ) does not work on all applications.
    // for this reason we swithc using only the EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport

    // i have experienced problems with EMBOT_HEAP_PROTECTION_use_OSsupport
    // when running the application of a strain2 (but on the stm32g4eval it is ok).
    // hence i prefer to disable EMBOT_HEAP_PROTECTION_use_OSsupport.
    // and use only EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport.
    // this latter is also the only mode which RTX V5 use
    

    //#define EMBOT_HEAP_redefine_heapoperators
    
    //#define EMBOT_HEAP_PROTECTION_use_OSsupport
    #define EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport  
    
    
    #if defined(EMBOT_HEAP_PROTECTION_use_OSsupport) & !defined(EMBOT_HEAP_redefine_heapoperators)
        #define EMBOT_HEAP_redefine_heapoperators
    #endif  
    
    #if defined(EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport) & !defined(EMBOT_HEAP_redefine_heapoperators)
        #define EMBOT_HEAP_redefine_heapoperators
    #endif     

#endif


// -- heap protection
//    in here we redefine new and delete so that they may be thread safe. 

#if defined(EMBOT_HEAP_redefine_heapoperators)

// here is what i need with armclang and -std=c++17
void* operator new(std::size_t size) noexcept(false)
{
#if !defined(EMBOT_HEAP_PROTECTION_use_OSsupport)  
    void* ptr = calloc(size, 1);
    if(nullptr == ptr)
    {
    #if defined(EMBOT_USE_rtos_cmsisos2)
        osRtxErrorNotify(666, nullptr);
    #elif defined(EMBOT_USE_rtos_osal)
        for(;;);
    #endif
    }
#else    
#if defined(EMBOT_USE_rtos_cmsisos2)
    void* ptr = cmsisos2_memory_new(size);
    if(nullptr == ptr)
    {
        osRtxErrorNotify(666, nullptr);
    }
#elif defined(EMBOT_USE_rtos_osal)
    void* ptr = osal_base_memory_new(size);
#endif

#endif
    return ptr;
}

void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) noexcept
{
#if !defined(EMBOT_HEAP_PROTECTION_use_OSsupport)  
    void* ptr = calloc(size, 1);
    #if defined(EMBOT_USE_rtos_cmsisos2)
        osRtxErrorNotify(667, nullptr);
    #elif defined(EMBOT_USE_rtos_osal)
        for(;;);
    #endif
#else   
#if defined(EMBOT_USE_rtos_cmsisos2)
    void* ptr = cmsisos2_memory_new(size);
    if(nullptr == ptr)
    {
        osRtxErrorNotify(667, nullptr);
    }    
#elif defined(EMBOT_USE_rtos_osal)    
    void* ptr = osal_base_memory_new(size);
#endif
#endif    
    return ptr;    
}

void operator delete (void* ptr) noexcept
{
    if(nullptr == ptr)
    {
        return;
    }
#if !defined(EMBOT_HEAP_PROTECTION_use_OSsupport)     
    free(ptr);
#else    
#if defined(EMBOT_USE_rtos_cmsisos2)
    cmsisos2_memory_delete(ptr);
#elif defined(EMBOT_USE_rtos_osal)  
    osal_base_memory_del(ptr);
#endif
#endif    
}

#endif //#if defined(EMBOT_HEAP_PROTECTION_use_OSsupport)


// -- C/C++ Standard Library Multithreading Interface
//    see https://developer.arm.com/documentation/dui0475/m/the-arm-c-and-c---libraries/multithreaded-support-in-arm-c-libraries?lang=en

#if defined(EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport)


#if defined(EMBOT_USE_rtos_osal)

    // it is implemneted inside the osal
    #include "osal_arch_arm.h"

    extern "C" {

    // required by the arm c stdlib: gives a different memory space for the stdlib to each thread in the arm compiler
    __attribute__((used)) void * __user_perthread_libspace(void) ;
    void * __user_perthread_libspace(void) 
    { 
        static volatile uint8_t fc = 1; 
        void *ret = osal_arch_arm_armc99stdlib_getlibspace(fc);
        fc = 0; 
        return(ret);
    }

    // required by the arm c stdlib: initialises a mutex
    __attribute__((used)) int _mutex_initialize(void *m); 
    int _mutex_initialize(void *m) 
    { 
        return(osal_arch_arm_armc99stdlib_mutex_initialize(m)); 
    }

    // required by the arm c stdlib: takes a mutex
    __attribute__((used)) void _mutex_acquire(void *m);
    void _mutex_acquire(void *m) 
    { 
        osal_arch_arm_armc99stdlib_mutex_acquire(m); 
    } 

    // required by the arm c stdlib: releases a mutex
    __attribute__((used)) void _mutex_release(void *m);
    void _mutex_release(void *m) 
    { 
        osal_arch_arm_armc99stdlib_mutex_release(m); 
    }  

    } //  extern "C"

    
#elif defined(EMBOT_USE_rtos_cmsisos2)
    
    // it is implemented in here.
    // we get it from rtx_lib.c of cmsis os2

    // static mutext allocator
    void * mutex_new_static()
    {
        constexpr uint8_t nsysmtx = 4;
        static embot::os::rtos::osMutexMemory sysmtx[nsysmtx]; 
        static uint8_t syscnt = 0;
        
        if(syscnt >= nsysmtx)
        {
            return nullptr;
        }
        embot::os::rtos::osMutexMemory *mem = &sysmtx[syscnt++];
        mem->prepare();
        return osMutexNew(mem->attribute());                
    } 

    extern "C" {      
    
        #if ( !defined(RTX_NO_MULTITHREAD_CLIB) && \
         ( defined(__CC_ARM) || \
          (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && \
          !defined(__MICROLIB))
          
        #define OS_THREAD_LIBSPACE_NUM 4
        #define LIBSPACE_SIZE 96
      
        // Memory for libspace
        static uint32_t os_libspace[OS_THREAD_LIBSPACE_NUM+1][LIBSPACE_SIZE/4];

        // Thread IDs for libspace
        static osThreadId_t os_libspace_id[OS_THREAD_LIBSPACE_NUM];

        // Check if Kernel has been started
        static uint32_t os_kernel_is_active (void) {
          static uint8_t os_kernel_active = 0U;

          if (os_kernel_active == 0U) {
            if (osKernelGetState() > osKernelReady) {
              os_kernel_active = 1U;
            }
          }
          return (uint32_t)os_kernel_active;
        }

        // Provide libspace for current thread
        __attribute__((used))
        void *__user_perthread_libspace (void);
        void *__user_perthread_libspace (void) {
          osThreadId_t id;
          uint32_t     n;

          if (os_kernel_is_active() != 0U) {
            id = osThreadGetId();
            for (n = 0U; n < (uint32_t)OS_THREAD_LIBSPACE_NUM; n++) {
              if (os_libspace_id[n] == NULL) {
                os_libspace_id[n] = id;
              }
              if (os_libspace_id[n] == id) {
                break;
              }
            }
            if (n == (uint32_t)OS_THREAD_LIBSPACE_NUM) {
              (void)osRtxErrorNotify(osRtxErrorClibSpace, id);
            }
          } else {
            n = OS_THREAD_LIBSPACE_NUM;
          }

          //lint -e{9087} "cast between pointers to different object types"
          return (void *)&os_libspace[n][0];
        }

        // Mutex identifier
        typedef void *mutex;

        //lint -save "Function prototypes defined in C library"
        //lint -e970 "Use of 'int' outside of a typedef"
        //lint -e818 "Pointer 'm' could be declared as pointing to const"


        // Initialize mutex
        __attribute__((used))
        int _mutex_initialize(mutex *m);
        int _mutex_initialize(mutex *m) {
          volatile int result = 0;

        //  *m = osMutexNew(NULL);
            *m = mutex_new_static();
          if (*m != NULL) {
            result = 1;
          } else {
            result = 0;
            (void)osRtxErrorNotify(osRtxErrorClibMutex, m);
          }
          return result;
        }

        // Acquire mutex
        __attribute__((used))
        void _mutex_acquire(mutex *m);
        void _mutex_acquire(mutex *m) {
          if (os_kernel_is_active() != 0U) {
            (void)osMutexAcquire(*m, osWaitForever);
          }
        }

        // Release mutex
        __attribute__((used))
        void _mutex_release(mutex *m);
        void _mutex_release(mutex *m) {
          if (os_kernel_is_active() != 0U) {
            (void)osMutexRelease(*m);
          }
        }

        // Free mutex
        __attribute__((used))
        void _mutex_free(mutex *m);
        void _mutex_free(mutex *m) {
          (void)osMutexDelete(*m);
        }
        
        #endif // #if ( !defined(RTX_NO_MULTITHREAD_CLIB) ...
    } //extern "C"

#endif // #elif defined(EMBOT_USE_rtos_cmsisos2)

#endif // #if defined(EMBOT_HEAP_PROTECTION_use_armCmultithreadsupport)




// -- extra configuration for osal

#if defined(EMBOT_USE_rtos_osal)



struct osal_init_pre_main_t
{
    osal_init_pre_main_t() 
    {
        osal_set_errorhandler(on_fatal);
    }
    
    static void on_fatal(void* task, osal_fatalerror_t errorcode, const char * errormsg)
    {
        volatile uint32_t t {0};
        
        embot::core::print("osal_init_pre_main_t::on_fatal() -> " + std::string(errormsg));
        
        for(;;)
        {
            t++;
        }    
    }
    
};

// we cannot guarantee that this variable is initted as the first one.
// BUT it is initted before call of main(). hence osal_init_pre_main_t::on_fatal() is active at main() and before any override 
static osal_init_pre_main_t osal_init_pre_main {};

    #if 0
    the osal approach.
    osal follows this approach:
    - it uses osal_base_memory_new() and osal_base_memory_del() which are thread safe because executed under svc
    - requires external allocator / reallocator / deleter to get the ram; they are:
      osal_ext_calloc(), osal_ext_realloc(), osal_ext_free().
    - as in embot we use new / delete, we need to redefine them to use osal_base_memory_new() and osal_base_memory_del().
    #endif    

extern "C" void* osal_ext_calloc(uint32_t s, uint32_t n)
{
    void* ret = calloc(s, n);
    return(ret);
}

extern "C" void* osal_ext_realloc(void* m, uint32_t s)
{
    void* ret = realloc(m, s);
    return(ret);
}

extern "C" void osal_ext_free(void* m)
{
    free(m);
}

#endif // #if defined(EMBOT_USE_rtos_osal)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

