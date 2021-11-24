/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#warning: in here i attempt to use the embot::os::rtos instead of osiit. read the note
// embot::os::rtos is the wrapper to cmsisos2 which works fine in the embot::os applications
// it may have reduced features but that is because cmsisos2 dont have all these features
// so, here we are.
// i already know that teh periodic threads have different APIs... 

#if !defined(OSAL_USE_rtos_cmsisos2)
#error this file implements osal over cmsisos2: pls define OSAL_USE_rtos_cmsisos2 
#endif


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "embot_os_rtos.h"
#include "cmsisos2.h"


#include "stdio.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

// marco.accame on 04nov2021: see later if i can remove it
extern void osal_hid_reset_static_ram(void);

// i keep it extern to allow its name in microvision
void osal_launcher(void* p);



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


extern const osal_reltime_t osal_reltimeZERO        = OSAL_reltimeZERO;
extern const osal_reltime_t osal_reltime1ms         = OSAL_reltime1ms;
extern const osal_reltime_t osal_reltime1sec        = OSAL_reltime1sec;
extern const osal_reltime_t osal_reltimeMAX         = OSAL_reltimeMAX;
extern const osal_reltime_t osal_reltimeINFINITE    = OSAL_reltimeINFINITE;

extern const osal_abstime_t osal_abstimeNONE        = OSAL_abstimeNONE;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


//enum {osal_task_signature = 0x33};
struct osal_task_opaque_t 
{   // 4+1+1+2+4+4 = 16 bytes
    embot::os::rtos::thread_t*          rtostsk;
    embot::os::rtos::thread_props_t*    props;
    void                                *ext;
    osal_task_id_t                      id;
    uint8_t                             prio;
    uint8_t                             dummy[2];
    
    bool isvalid() const
    {
        return (nullptr != rtostsk) && (nullptr != props);
    }
};

#warning TODO add a embot::os::rtos::thread_props_t*

//enum {osal_timer_signature = 0x01234544};
struct osal_timer_opaque_t
{   // 16 bytes
    embot::os::rtos::timer_t* rtostimer;
    uint32_t dummy;
    osal_timer_onexpiry_t onexpiry;   
};

//enum {osal_messagequeue_signature = 0x01234555};
struct osal_messagequeue_opaque_t
{   // 4 bytes
    uint32_t dummy;  
};



//enum {osal_mutex_signature = 0x01234566};
struct osal_mutex_opaque_t
{   // 4 bytes
    uint32_t dummy; 
};

//enum {osal_semaphore_signature = 0x01234577};
struct osal_semaphore_opaque_t
{   // 4 bytes
    uint32_t dummy;
};



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_osal_on_idle(void *p);
static void s_osal_error(osal_fatalerror_t err_code, const char *err_msg);
static void s_osal_on_fatalerror(osal_task_t *task, osal_fatalerror_t errorcode, const char * errormsg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const osal_cpufamily_t s_osal_cpufam = 
#if defined(OSAL_CPUFAM_CM3)
    osal_cpufam_armcm3;
#elif defined(OSAL_CPUFAM_CM4)
    osal_cpufam_armcm4;
#else
    255;
    #error -> must define an OSAL_CPUFAM_xxx
#endif

// mutex used to guarantee atomic access at user space in the osal layer
static osal_mutex_t *s_osal_mutex_api_protection = NULL;

#if defined(OSAL_USE_rtos_cmsisos2)
#warning fill it
#else
// configuration of osiit. the default is w/ oosiit_memmode_dynamic 
static oosiit_cfg_t s_osal_oosiit_cfg = 
{
    .memorymode = oosiit_memmode_dynamic
};
#endif

// configuration of osal. the default is w/ osal_memmode_dynamic and osal_rtostype_oosiit
static osal_cfg_t s_osal_osal_cfg = 
{
    .rtostype = osal_rtostype_oosiit,
    .memorymodel = osal_memmode_dynamic    
};

// stack for launcher task
//static uint64_t *s_osal_stacklauncher_data = NULL;
//static uint16_t  s_osal_stacklauncher_size = 0;

// stack for idle task
//static uint64_t *s_osal_stackidle_data = NULL;
//static uint16_t  s_osal_stackidle_size = 0;


// memory used to store osal_task data.
//static osal_task_t *s_osal_task_data = NULL; // its size is number of tasks+2 (idle, launcher, tasks)
//static uint8_t s_osal_task_maxnum = 0;
//static uint8_t s_osal_task_next = 0;

// special system tasks ...
static osal_task_t s_osal_task_launcher = {};
static osal_task_t s_osal_task_idle = {};

// indicates how many usec in a tick. used for time conversion
static uint32_t s_osal_usec_in_tick = 0;

// pointer to the launcher function
static void (*s_osal_launcher_fn)(void) = NULL;

// status of the system
static osal_info_status_t s_osal_info_status = osal_info_status_zero;

// tables of resources
static uint16_t s_resources_free[osal_info_entity_numberof] = {0, 0, 0, 0, 0, 0, 0};
static uint16_t s_resources_used[osal_info_entity_numberof] = {0, 0, 0, 0, 0, 0, 0};

// number of mutexes required by the armcc99.
//static uint8_t s_osal_arch_arm_armc99_sysmutex_number = 0;

static osal_task_id_t s_id_counter = 2; // 0 is idle, 1 is init
constexpr osal_task_id_t s_id_task_idle = 0;
constexpr osal_task_id_t s_id_task_launcher = 1;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//extern void osal_debug_enter(uint8_t par)
//{
//    embot::os::rtos::enter(par);    
//}


extern void osal_set_errorhandler( void(*on_fatal_error)(void* task, osal_fatalerror_t errorcode, const char * errormsg))
{
    s_osal_osal_cfg.extfn.usr_on_fatal_error = on_fatal_error;
}


extern uint32_t osal_base_memory_getsize(const osal_cfg_t *cfg, uint32_t *size08aligned)
{
    if(osal_memmode_static == cfg->memorymodel)
    {   
        s_osal_error(osal_error_incorrectparameter, "osal_base_memory_getsize(): invalid osal_memmode_static");          
    }
    
    // we need zero memory from external sources
    return 0;
}


extern void* osal_base_memory_new(uint32_t size)
{
    return embot::os::rtos::memory_new(size);
}


extern void* osal_base_memory_realloc(void* mem, uint32_t size)
{
    return embot::os::rtos::memory_realloc(mem, size);   
}

extern void osal_base_memory_del(void* mem)
{
    embot::os::rtos::memory_delete(mem);
}


extern osal_result_t osal_base_initialise(const osal_cfg_t *cfg, uint64_t *data08aligned)
{
    uint16_t size08 = 0;
    uint16_t size04 = 0;

    if(NULL == cfg)
    {
        s_osal_error(osal_error_missingconfiguration, "osal: NULL cfg in osal_initialise()");
        return(osal_res_NOK_nullpointer);    
    }    
        
    if(osal_memmode_static == cfg->memorymodel)
    {   
        s_osal_error(osal_error_incorrectparameter, "osal_initialise(): invalid osal_memmode_static"); 
        // return(osal_res_NOK_generic);        
    }    

#if defined(OSAL_USE_rtos_cmsisos2)
    #warning ... we may add a osal_rtostype_cmsisos2
    if(osal_rtostype_oosiit != cfg->rtostype)
    {
        s_osal_error(osal_error_unsupportedbehaviour, "osal: unsupported rtos");
        return(osal_res_NOK_generic);
    }
#endif
    
    if(s_osal_cpufam != cfg->cpufam)
    {
        s_osal_error(osal_error_unsupportedbehaviour, "osal: cfg has a wrong cpu family");
        return(osal_res_NOK_generic);        
    }

    if((osal_info_status_running == s_osal_info_status) || (osal_info_status_suspended == s_osal_info_status))
    {
        s_osal_error(osal_error_generic, "osal: called osal_initialise() after osal_start()");
        return(osal_res_NOK_generic);
    }

    if((NULL != cfg->extfn.hal_sys_irqn_priority_set) && (cfg->prio > 15))
    {
        s_osal_error(osal_error_generic, "osal: if used, prio must be between 0 and 15");
        return(osal_res_NOK_generic);
    }

    memcpy(&s_osal_osal_cfg, cfg, sizeof(osal_cfg_t));

    s_osal_usec_in_tick = cfg->tick;

    s_osal_info_status = osal_info_status_initialised;

    return(osal_res_OK);
}


extern osal_info_status_t osal_info_get_status(void)
{
    return(s_osal_info_status);
}


extern osal_reltime_t osal_info_get_tick(void)
{
    return(s_osal_usec_in_tick);
}


extern const osal_cfg_t* osal_info_get_config(void)
{
    if(osal_info_status_zero == s_osal_info_status)
    {
        return(NULL);
    }
    else
    {
        return(&s_osal_osal_cfg);
    }    
}


static void s_osIdleThread(void)
{
    s_osal_on_idle(NULL);   
}

//#define MODE1

static void s_launcher(void)
{
#if defined(MODE1)
    // IN HERE we start a further THREAD WITH PRIO MAX-1 which executes osal_launcher(). 
    // THEN we quit quit it.
    embot::os::rtos::thread_props_t props {};
    props.prepare(osal_launcher, NULL, 53, s_osal_osal_cfg.launcherstacksize);
    embot::os::rtos::thread_t *t = embot::os::rtos::thread_new(props);

    embot::os::rtos::thread_t *r = embot::os::rtos::scheduler_thread_running();
        
//    embot::os::rtos::scheduler_deassociate(s_osal_task_launcher.rtostsk, reinterpret_cast<embot::os::Thread*>(&s_osal_task_launcher));    
    // delete the thread styrted by the scheduler.   
    embot::os::rtos::thread_setpriority(r, embot::os::Priority::low08);
    embot::os::rtos::thread_delete(r);   // in here it is the scheduler which starts all     
#else        
    osal_launcher(NULL);   
#endif        
}


static void s_osOnError(embot::os::rtos::thread_t* thread, int errorcode, const char * errormsg)
{
    if(nullptr != thread)
    {
        embot::os::Thread *t = embot::os::rtos::scheduler_getassociated(thread);  
        const char *tname = ((nullptr == t) || (nullptr == t->getName())) ? "none" : t->getName();             
        embot::core::print(std::string("from os::Thread ") + tname + " -> " + errormsg);  
    }            
    std::string err = std::string("from rtos: error #") + std::to_string(errorcode) + " -> " + errormsg;
    s_osal_error(osal_error_unknownfromrtos, err.c_str());    
}

//extern "C" {
//extern uint32_t ITM_SendChar (uint32_t ch);
//}

//static int s_puts(const char* str) 
//{     
//    if(nullptr == str)
//    {
//        return(0);
//    }

//    std::uint32_t ch;
//    int num = 0;
//    while('\0' != (ch = *str))
//    {
//        ITM_SendChar(ch);
//        str++;
//        num++;
//    }
//     
//    ITM_SendChar('\n');
//    return(++num);    
//}
//  

extern "C" {
    extern int hal_trace_puts(const char * str);
}
    
static int s_traceprint(const std::string &str)
{
    return hal_trace_puts(str.c_str());
}

embot::os::rtos::scheduler_props_t sch_props {};
    
extern void osal_system_start(void (*launcher_fn)(void)) 
{
    
#if defined(OSAL_USE_rtos_cmsisos2)
    
    #warning osal_system_start(): verify code for the case of OSAL_USE_rtos_cmsisos2
        
    
    constexpr uint16_t timerstacksize {2*1024};
    
    sch_props.prepare(  s_osal_osal_cfg.tick, //  or also: s_osal_usec_in_tick, //config.timing.ticktime, 
        #if defined(MODE1)
                    2048, 
        #else
                    s_osal_osal_cfg.launcherstacksize, //behaviour.initconfig.stacksize, 
        #endif
                    s_osal_osal_cfg.idlestacksize, //.behaviour.idleconfig.stacksize, 
                    timerstacksize,
                    s_osIdleThread, 
                    s_launcher, 
                    s_osOnError);
                
    embot::os::rtos::scheduler_init(sch_props);
              
    s_osal_info_status = osal_info_status_running;
        
    s_osal_launcher_fn = launcher_fn; 
     
    if(NULL == s_osal_launcher_fn)
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incorrect param in osal_start()");
        for(;;);
    }   

    constexpr embot::core::PrintConfig pc {s_traceprint};
    embot::core::init({{}, pc}); 

    // prepare idle and init threads
    
    s_osal_task_idle.rtostsk = NULL;
    s_osal_task_idle.id = s_id_task_idle;
//    s_osal_task_idle.prio = embot::core::tointegral(embot::os::Priority::schedIdle);
//    s_osal_task_idle.stksize = s_osal_osal_cfg.idlestacksize;
//    s_osal_task_idle.stkdata = reinterpret_cast<uint64_t*>(props.getidlestack());
    s_osal_task_idle.ext = NULL;

    s_osal_task_launcher.rtostsk = NULL;
    s_osal_task_launcher.id = s_id_task_launcher;
//    s_osal_task_launcher.prio = embot::core::tointegral(embot::os::Priority::schedInit);
//    s_osal_task_launcher.stksize = s_osal_osal_cfg.launcherstacksize;
//    s_osal_task_launcher.stkdata = reinterpret_cast<uint64_t*>(props.getinitstack());
    s_osal_task_launcher.ext = NULL;

    
    #warning THINK of s_osal_task_idle external data and s_osal_task_launcher  
#if 0        
    s_osal_task_idle        = s_osal_taskobj_new();
    s_osal_task_launcher    = s_osal_taskobj_new();
    and now... fill the internals as in the following code
    // as we dont call an explicit oosiit_tsk_create() which gives a oosiit_tskptr_t, 
    // the task idle itself inside s_osal_on_idle() must call: s_osal_task_idle->rtostsk = oosiit_tsk_self()  
    s_osal_task_idle->rtostsk       = NULL;
    s_osal_task_idle->signtsk       = osal_task_signature;
    s_osal_task_idle->prio          = osal_prio_systsk_idle;
    s_osal_task_idle->stksize       = s_osal_stackidle_size;
    s_osal_task_idle->stkdata       = s_osal_stackidle_data;
    s_osal_task_idle->ext           = NULL;
    
    tskpidle.function   = s_osal_on_idle;
    tskpidle.param      = NULL;
    tskpidle.priority   = osal_prio_systsk_idle;
    tskpidle.stacksize  = s_osal_stackidle_size;
    tskpidle.stackdata  = s_osal_stackidle_data; 
    tskpidle.extdata    = s_osal_task_idle;    
    

    // as we dont call an explicit oosiit_tsk_create() which gives a oosiit_tskptr_t, 
    // the task launcher itself inside osal_launcher() must call: s_osal_task_launcher->rtostsk = oosiit_tsk_self()
    s_osal_task_launcher->rtostsk   = NULL;
    s_osal_task_launcher->signtsk   = osal_task_signature;
    s_osal_task_launcher->prio      = osal_prio_systsk_launcher;
    s_osal_task_launcher->stksize   = s_osal_stacklauncher_size; 
    s_osal_task_launcher->stkdata   = &s_osal_stacklauncher_data[0]; 
    s_osal_task_launcher->ext       = NULL;    
    
    tskpinit.function   = osal_launcher;
    tskpinit.param      = NULL;
    tskpinit.priority   = osal_prio_systsk_launcher;
    tskpinit.stacksize  = s_osal_stacklauncher_size;
    tskpinit.stackdata  = &s_osal_stacklauncher_data[0];
    tskpinit.extdata    = s_osal_task_launcher;    
#endif

    embot::os::rtos::scheduler_start2(); 

    s_osal_error(osal_error_unknownfromrtos, "scheduler cannot start");
    for(;;);    

#else
//    oosiit_task_properties_t tskpinit;
//    oosiit_task_properties_t tskpidle;

//    if(s_osal_info_status != osal_info_status_initialised)
//    {
//        if(s_osal_info_status == osal_info_status_zero)
//        {
//            s_osal_error(osal_error_generic, "osal: called osal_start() w/out prior osal_initialise()"); 
//        }
//        else
//        {
//            s_osal_error(osal_error_generic, "osal: called osal_start() again");  
//        }
//           
//    }

//    s_osal_info_status = osal_info_status_running;

//    s_osal_launcher_fn = launcher_fn; 
//     
//    if(NULL == s_osal_launcher_fn)
//    {
//        s_osal_error(osal_error_incorrectparameter, "osal: incorrect param in osal_start()");
//    }
//    
//    // as we dont call an explicit oosiit_tsk_create() which gives a oosiit_tskptr_t, 
//    // the task idle itself inside s_osal_on_idle() must call: s_osal_task_idle->rtostsk = oosiit_tsk_self()  
//    s_osal_task_idle->rtostsk       = NULL;
//    s_osal_task_idle->signtsk       = osal_task_signature;
//    s_osal_task_idle->prio          = osal_prio_systsk_idle;
//    s_osal_task_idle->stksize       = s_osal_stackidle_size;
//    s_osal_task_idle->stkdata       = s_osal_stackidle_data;
//    s_osal_task_idle->ext           = NULL;
//    
//    tskpidle.function   = s_osal_on_idle;
//    tskpidle.param      = NULL;
//    tskpidle.priority   = osal_prio_systsk_idle;
//    tskpidle.stacksize  = s_osal_stackidle_size;
//    tskpidle.stackdata  = s_osal_stackidle_data; 
//    tskpidle.extdata    = s_osal_task_idle;    
//    

//    // as we dont call an explicit oosiit_tsk_create() which gives a oosiit_tskptr_t, 
//    // the task launcher itself inside osal_launcher() must call: s_osal_task_launcher->rtostsk = oosiit_tsk_self()
//    s_osal_task_launcher->rtostsk   = NULL;
//    s_osal_task_launcher->signtsk   = osal_task_signature;
//    s_osal_task_launcher->prio      = osal_prio_systsk_launcher;
//    s_osal_task_launcher->stksize   = s_osal_stacklauncher_size; 
//    s_osal_task_launcher->stkdata   = &s_osal_stacklauncher_data[0]; 
//    s_osal_task_launcher->ext       = NULL;    
//    
//    tskpinit.function   = osal_launcher;
//    tskpinit.param      = NULL;
//    tskpinit.priority   = osal_prio_systsk_launcher;
//    tskpinit.stacksize  = s_osal_stacklauncher_size;
//    tskpinit.stackdata  = &s_osal_stacklauncher_data[0];
//    tskpinit.extdata    = s_osal_task_launcher;



//           
//    // start the system
//    oosiit_sys_start(&tskpinit, &tskpidle);
#endif    
}


#warning REVIEW: osal_info_entities_get_stats()
extern void osal_info_entities_get_stats(const uint16_t **used, const uint16_t **free)
{
    if((s_osal_info_status == osal_info_status_zero) || (s_osal_info_status == osal_info_status_initialised))
    {
        *used = NULL;
        *free = NULL;
        return;
    }
    
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);

    s_resources_free[osal_info_entity_globalstack]     =  s_osal_osal_cfg.globalstacksize  - s_resources_used[osal_info_entity_globalstack];  
    s_resources_free[osal_info_entity_task]            =  s_osal_osal_cfg.tasknum          - s_resources_used[osal_info_entity_task];
    s_resources_free[osal_info_entity_timer]           =  s_osal_osal_cfg.timernum         - s_resources_used[osal_info_entity_timer];
    s_resources_free[osal_info_entity_mutex]           =  s_osal_osal_cfg.mutexnum         - s_resources_used[osal_info_entity_mutex];
    s_resources_free[osal_info_entity_semaphore]       =  s_osal_osal_cfg.semaphorenum     - s_resources_used[osal_info_entity_semaphore];
    s_resources_free[osal_info_entity_messagequeue]    =  s_osal_osal_cfg.mqueuenum        - s_resources_used[osal_info_entity_messagequeue];
    s_resources_free[osal_info_entity_message]         =  s_osal_osal_cfg.mqueueelemnum    - s_resources_used[osal_info_entity_message];

    *used = s_resources_used;
    *free = s_resources_free;

    osal_mutex_release(s_osal_mutex_api_protection);
}

#warning REVIEW: osal_info_idletime_get()
extern osal_abstime_t osal_info_idletime_get(void)
{
    return(0);
}


#warning TODO: osal_system_ticks_abstime_set()
extern void osal_system_ticks_abstime_set(osal_abstime_t tot)
{
    //oosiit_time_set(s_osal_abstime2tick(tot)); 
}


extern osal_abstime_t osal_system_ticks_abstime_get(void)
{
    return cmsisos2_sys_abstime_milliresolution();
//    return embot::os::rtos::scheduler_timeget();
 //   return((uint64_t)s_osal_usec_in_tick * oosiit_time_get());     
}


extern osal_abstime_t osal_system_abstime_get(void)
{
    return embot::os::rtos::scheduler_timeget();
//    return(oosiit_microtime_get());     
}


//#warning NOTE: suboptimal implemenattion 
extern osal_nanotime_t osal_system_nanotime_get(void)
{
    return 1000*embot::os::rtos::scheduler_timeget();
//    return(oosiit_nanotime_get());    
}


extern void osal_system_scheduling_suspend(void)
{
    if(osal_info_status_running == s_osal_info_status)
    {   // order is important in here
        embot::os::rtos::scheduler_lock();
        //oosiit_sys_suspend(NULL);              
        s_osal_info_status = osal_info_status_suspended;
    }
}


extern void osal_system_scheduling_restart(void)
{
    if(osal_info_status_suspended == s_osal_info_status)
    {   // order is important in here
        s_osal_info_status = osal_info_status_running;
        embot::os::rtos::scheduler_unlock();
        //oosiit_sys_resume(0);        
    }
}

#warning NOT IMPLEMENTED: osal_system_timeofnextevent()
extern osal_reltime_t osal_system_timeofnextevent(void)
{
    osal_reltime_t nextevent = 0;
    if(osal_info_status_running == s_osal_info_status)
    {
        nextevent = 0;
        //nextevent = s_osal_usec_in_tick*oosiit_sys_timeofnextevent();              
    }
    
    return(nextevent);    
}


#warning NOTE: different behaviour. it returns 0
extern osal_reltime_t osal_system_suspend(void)
{
    osal_system_scheduling_suspend();
    return 0;
//    osal_reltime_t nextevent = 0;
//    if(osal_info_status_running == s_osal_info_status)
//    {   // order is important in here
//#if defined(OSAL_USE_rtos_cmsisos2)
//    #warning add code for OSAL_USE_rtos_cmsisos2
//#else
//        oosiit_sys_suspend(&nextevent);
//#endif           
//        s_osal_info_status = osal_info_status_suspended;
//    }
//    
//    return(s_osal_usec_in_tick*nextevent);
}


#warning NOTE: different behaviour.
extern void osal_system_resume(osal_reltime_t timeslept)
{
    osal_system_scheduling_restart();
//    if(osal_info_status_suspended == s_osal_info_status)
//    {   // order is important in here
//        s_osal_info_status = osal_info_status_running;
//#if defined(OSAL_USE_rtos_cmsisos2)
//    #warning add code for OSAL_USE_rtos_cmsisos2
//#else
//        oosiit_sys_resume(timeslept/s_osal_usec_in_tick);
//#endif                  
//    }
}


extern osal_task_t * osal_task_new1(osal_task_properties_t *tskprop)
{
    if(NULL == tskprop)
    {
        return(NULL);
    }
    return(osal_task_new(tskprop->function, tskprop->param, tskprop->priority, tskprop->stacksize));
}

//#include <map>

//static const std::map<uint8_t, embot::os::Priority> map_prio_osal_to_embotos = {
//    {osal_prio_systsk_idle,     embot::os::Priority::schedIdle},    // IDLE     000->001
//    {10,                        embot::os::Priority::low10}         // ETHMON   010->010
//};

//const std::map<uint8_t, uint8_t> map_prio_osal_to_embotos = {
//    {osal_prio_systsk_idle,     1},    // IDLE     000->001
//    {10,                        7}         // ETHMON   010->010
//};

//#warning TODO: priority mapping from osal to cmsisos2 and back
//    

//#include <array>
//constexpr std::array<embot::os::Priority, 255> s_map_prio_osal2embotos =
//{
//    embot::os::Priority::schedIdle,         // IDLE     000->001    osal_prio_systsk_idle
//    embot::os::Priority::none,              // xxxx     001->000    osal_prio_systsk_usrwhencreated
//    embot::os::Priority::none,              // xxxx     002->zzz    osal_prio_usrtsk_min
//    
//    
//    
//    
//    embot::os::Priority::low10,             // ETHMON   010-> 
//    
//    embot::os::Priority::none,              // xxxx     251->zzz    osal_prio_usrtsk_max

//    embot::os::Priority::schedInit          // LAUNCHER 254->055    osal_prio_systsk_launcher
//    embot::os::Priority::schedInit          // LAUNCHER 255->055    osal_prio_systsk_scheduler
//};

static uint8_t s_priority_osal_to_cmsisos2(uint8_t p)
{
    // there is not a 1 to 1 relation ....
    return p;
    
//    auto a = map_prio_osal_to_embotos.find(p);
//    if(a == map_prio_osal_to_embotos.end())
//    {
//        return 0;
//    }
//    return embot::core::tointegral(a->second); 
}


static uint8_t s_priority_cmsisos2_to_osal(uint8_t p)
{
    // there is not a 1 to 1 relation ....
    return p;
}

static uint8_t s_priority_transform(uint8_t prio)
{
    uint8_t r = prio;
    switch(prio)
    {
        case  10: r = 12; break;
        case  11: r = 13; break;
        case  39: r = 27; break;
        case  40: r = 28; break;
        case  41: r = 29; break;
        case  50: r = 30; break;
        case 202: r = 36; break;
        case 219: r = 37; break;
        case 220: r = 38; break;
        case 240: r = 39; break;
        case 247: r = 44; break;
        case 248: r = 45; break;
        case 249: r = 46; break;
        
        default:          break;
    }
    return r;
}


extern osal_task_t * osal_task_new(void (*run_fn)(void*), void *run_arg, uint8_t prio, uint16_t stksize)
{
    osal_task_t* retval = NULL;   

    if((0 == stksize) || (NULL == run_fn))
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incorr param in osal_task_new()");
    }
    
    prio = s_priority_transform(prio);
    constexpr uint8_t initialprio {2};

    if(false == embot::os::priority::isvalid(prio))
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incompatible embot::os::Priority in osal_task_new()");
    }    

    // protect vs concurrent call
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    
    retval = reinterpret_cast<osal_task_t*>(osal_base_memory_new(sizeof(osal_task_t)));
    if(NULL == retval)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for osal_task_t in osal_task_new()");
        osal_mutex_release(s_osal_mutex_api_protection);
        return(NULL);        
    }
       
    uint32_t tmp = (stksize+7)/8;
    stksize = 8*tmp;
//    uint8_t cmsisos2prio = s_priority_osal_to_cmsisos2(prio);
    retval->props = new embot::os::rtos::thread_props_t;
    retval->props->prepare(run_fn, run_arg, initialprio, stksize);        
    retval->rtostsk = embot::os::rtos::thread_new(*retval->props);
        
    
    // reinterpret_cast<uint64_t*>(props->getstack())
    
    if(NULL == retval->rtostsk)
    {
        // cannot get a task
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos task in osal_task_new()");
        osal_mutex_release(s_osal_mutex_api_protection);
        return(NULL);
    }
    
    // ok. i can increment the tasks.
//    s_resources_used[osal_info_entity_task] ++;
    
    
    // init the returning osal task
    retval->id         = s_id_counter++;
//    retval->signtsk    = osal_task_signature;
    #warning TODO: decide if we want cmsisos2prio or prio in here
    retval->prio       = prio;
    retval->ext        = NULL;
    
    // associate the retval to the rtos thread. we need that because ... 
    // we can have the pointer of osal_task_t w/ calls to osal_task_get()
    embot::os::rtos::scheduler_associate(retval->rtostsk, reinterpret_cast<embot::os::Thread*>(retval)); 
    
    // can release now
    osal_mutex_release(s_osal_mutex_api_protection); 
    
    
    // the destiny of the task is now given to the scheduler. it can start now if prio is higher than the priority of the caller  
    retval->props->setprio(prio);
    embot::os::rtos::thread_setpriority(retval->rtostsk, embot::os::priority::convert(prio));    
    
    return(retval);
}


extern osal_result_t osal_task_wait(osal_reltime_t time)
{
    constexpr embot::os::rtos::thread_t * running = nullptr;
    // nullptr is fine in here because thread_sleep does not use this argument
    embot::os::rtos::thread_sleep(running, time);
    return(osal_res_OK);
}


extern osal_result_t osal_task_pass(void)
{
    osThreadYield();    
    return(osal_res_OK);
}


extern osal_result_t osal_task_period_set(osal_reltime_t period)
{
    embot::os::rtos::thread_t *t = embot::os::rtos::scheduler_thread_running();
    embot::os::rtos::thread_period_set(t, period);    
    return(osal_res_OK);
}


extern osal_result_t osal_task_period_wait(void)
{
    constexpr embot::core::relTime period = 0; // it is ok like that. thread_period_wait() does not use the value
    embot::os::rtos::thread_t *t = embot::os::rtos::scheduler_thread_running();
    embot::os::rtos::thread_period_wait(t, period);
    return(osal_res_OK);
}


extern osal_result_t osal_task_delete(osal_task_t *tsk)
{   
    if(NULL == tsk)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    if(false == tsk->isvalid())
    {
        return(osal_res_NOK_generic);
    }  
    
    // some tasks could call it at the same time ... better to protect
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);   
            
    embot::os::rtos::thread_t* rtostsk = tsk->rtostsk;
    embot::os::rtos::thread_props_t* props = tsk->props;
    
    embot::os::rtos::scheduler_deassociate(rtostsk, reinterpret_cast<embot::os::Thread*>(tsk));
    
    
    // reset the data pointed by tsk so that another task calling osal_tsk_* functions with the same tsk
    // exits gracefully ... until the same memory is used for something else....    
    tsk->rtostsk        = NULL;
    tsk->id             = 0;
//    tsk->signtsk        = 0;
    tsk->prio           = 0;
    tsk->ext            = NULL;   

    osal_base_memory_del(tsk);     
    
    osal_mutex_release(s_osal_mutex_api_protection);
    
    embot::os::rtos::thread_delete(rtostsk);
    delete props;
    
    return(osal_res_OK);    
}


extern osal_result_t osal_task_priority_get(osal_task_t *tsk, uint8_t *prio)
{
    if((NULL == tsk) || (NULL == prio))
    {
        return(osal_res_NOK_nullpointer);
    }
    
    if(false == tsk->isvalid())
    {
        return(osal_res_NOK_generic);
    }
    
    *prio = tsk->props->getprio();

    return(osal_res_OK);
}


extern osal_result_t osal_task_priority_set(osal_task_t *tsk, uint8_t prio)
{    
    if(NULL == tsk)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    if(false == tsk->isvalid())
    {
        return(osal_res_NOK_generic);
    }

//    if((prio<osal_prio_usrtsk_min) || (prio>osal_prio_usrtsk_max))
//    {
//        return(osal_res_NOK_generic);
//    }  

    prio = s_priority_transform(prio);    

    if(false == embot::os::priority::isvalid(prio))
    {
        return(osal_res_NOK_generic);
    }  
    
    // some tasks could call it at the same time ... better to protect
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
        
//    uint8_t cmsisos2prio = s_priority_osal_to_cmsisos2(prio);
    tsk->props->setprio(prio);
    embot::os::rtos::thread_setpriority(tsk->rtostsk, embot::os::priority::convert(prio));
    
    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);
}


extern osal_result_t osal_task_id_get(osal_task_t *tsk, osal_task_id_t *id)
{
    if((NULL == tsk) || (NULL == id))
    {
        return(osal_res_NOK_nullpointer);
    }
 
    if(false == tsk->isvalid())
    {
        return(osal_res_NOK_generic);
    } 
    
    *id = tsk->id;
         
    return(osal_res_OK);
}


extern osal_result_t osal_task_extdata_set(osal_task_t *tsk, void *ext)
{
    if(NULL == tsk)
    {
        return(osal_res_NOK_nullpointer);
    }

    if(false == tsk->isvalid())
    {
        return(osal_res_NOK_nullpointer);
    }
    
    // some tasks could call it at the same time ... better to protect
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE); 

    tsk->ext = ext;

    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);
}


extern void* osal_task_extdata_get(osal_task_t *tsk)
{
    if(NULL == tsk)
    {
        return(NULL);
    }
    
    if(false == tsk->isvalid())
    {
        return(NULL);
    }
    
    return(tsk->ext);
}


extern osal_task_t * osal_task_get(osal_caller_t caller)
{    
    embot::os::rtos::thread_t *rt = embot::os::rtos::scheduler_thread_running();
    embot::os::Thread* t = embot::os::rtos::scheduler_getassociated(rt);
    return reinterpret_cast<osal_task_t*>(t);
}


extern void * osal_task_stack_get1(osal_task_t *tsk, uint16_t *size)
{
    if(NULL == tsk)
    {
        return(NULL);
    }
    
    if(false == tsk->isvalid())
    {
        return(NULL);
    }
    
    uint16_t s = 0;
    void * ret = tsk->props->getstack(s);
    if(nullptr != size)
    {
        *size = s;
    }
    return ret;
}

// messagequeue

extern osal_messagequeue_t * osal_messagequeue_new(uint16_t maxmsg)
{
    if(maxmsg == 0)
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incorr param in osal_messagequeue_new()");
        return(NULL);
    }
    
    embot::os::rtos::messagequeue_t * mq = embot::os::rtos::messagequeue_new(maxmsg);   
  
    if(NULL == mq)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for osal_messagequeue_t in osal_messagequeue_new()");
        return(NULL);
    }
    

    // protect here because the osiit_* calls are already protected as they are executes by svc
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);

    s_resources_used[osal_info_entity_messagequeue] ++;
    s_resources_used[osal_info_entity_message] += maxmsg;

    osal_mutex_release(s_osal_mutex_api_protection);

    return reinterpret_cast<osal_messagequeue_t*>(mq);
}

#warning see osal_messagequeue_get() 

extern osal_result_t osal_messagequeue_get(osal_messagequeue_t *mq, osal_message_t *pmsg, osal_reltime_t tout, osal_caller_t caller)
{
    osal_result_t res = osal_res_OK; 

    if((NULL == mq) || (NULL == pmsg))
    {
        return(osal_res_NOK_nullpointer);
    }
    
    embot::os::Message msg = embot::os::rtos::messagequeue_get(reinterpret_cast<embot::os::rtos::messagequeue_t*>(mq), tout);
    
    *pmsg = (osal_message_t)msg;

    return(res);
}


extern osal_message_t osal_messagequeue_getquick(osal_messagequeue_t *mq, osal_reltime_t tout, osal_caller_t caller)
{
    void *p = NULL;

    if(NULL == mq)
    {
        return((osal_message_t)0);
    }
    
    embot::os::Message msg = embot::os::rtos::messagequeue_get(reinterpret_cast<embot::os::rtos::messagequeue_t*>(mq), tout); 
    
    return((osal_message_t)msg);
}

extern uint16_t osal_messagequeue_available(osal_messagequeue_t *mq, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(0);
    }
    
    return embot::os::rtos::messagequeue_available(reinterpret_cast<embot::os::rtos::messagequeue_t*>(mq));
}


extern uint16_t osal_messagequeue_size(osal_messagequeue_t *mq, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(0);
    }
    
    return embot::os::rtos::messagequeue_size(reinterpret_cast<embot::os::rtos::messagequeue_t*>(mq));
        
}


extern osal_result_t osal_messagequeue_put(osal_messagequeue_t *mq, osal_message_t msg, osal_reltime_t tout, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    bool rr = embot::os::rtos::messagequeue_put(reinterpret_cast<embot::os::rtos::messagequeue_t*>(mq), msg, tout);
    
    return (rr) ? osal_res_OK : osal_res_NOK_timeout;

}

extern osal_result_t osal_messagequeue_delete(osal_messagequeue_t *mq)
{
    if(NULL == mq)
    {
        return(osal_res_NOK_nullpointer); 
    }

    embot::os::rtos::messagequeue_delete(reinterpret_cast<embot::os::rtos::messagequeue_t*>(mq));
        
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    s_resources_used[osal_info_entity_messagequeue] --;
    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);
}


extern osal_result_t osal_eventflag_set(osal_eventflag_t flag, osal_task_t * totask, osal_caller_t caller)
{
    if(NULL == totask)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    bool rr = embot::os::rtos::event_set(totask->rtostsk, flag);

    return(osal_res_OK);
}


extern osal_result_t osal_eventflag_get(osal_eventflag_t waitmsk, osal_eventflag_waitmode_t waitmode, osal_eventflag_t *rxmsk, osal_reltime_t tout)
{
    osal_result_t res = osal_res_NOK_timeout; 

    if(NULL == rxmsk)
    {
        return(osal_res_NOK_nullpointer);
    }

    *rxmsk = 0;
    
    // osal_waitANYflag or osal_waitALLflags
    uint32_t options = (osal_waitANYflag == waitmode) ? osFlagsWaitAny : osFlagsWaitAll;
    uint32_t flags = (osal_waitANYflag == waitmode) ? 0x7fffffff : waitmsk;  
    
    uint32_t r =  osThreadFlagsWait(flags, options, cmsisos2_sys_reltime2tick(tout));
    if(osFlagsErrorTimeout == r)
    {
        return osal_res_NOK_timeout;
    }
    
    *rxmsk = r;
    return osal_res_OK;    
    
}


extern osal_eventflag_t osal_eventflag_getany(osal_reltime_t tout)
{   
    return embot::os::rtos::event_get(tout);
}


extern osal_mutex_t * osal_mutex_new(void) 
{
    embot::os::rtos::mutex_t* m = embot::os::rtos::mutex_new();
    return reinterpret_cast<osal_mutex_t*>(m);
} 


extern osal_result_t osal_mutex_take(osal_mutex_t *mutex, osal_reltime_t tout) 
{   
    if(NULL == mutex)
    {
        return(osal_res_NOK_nullpointer);
    }
    bool v = embot::os::rtos::mutex_take(reinterpret_cast<embot::os::rtos::mutex_t*>(mutex), tout);    
    return (v) ? osal_res_OK : osal_res_NOK_timeout;
}


extern osal_result_t osal_mutex_release(osal_mutex_t *mutex) 
{
    if(NULL == mutex)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    embot::os::rtos::mutex_release(reinterpret_cast<embot::os::rtos::mutex_t*>(mutex));    
    return osal_res_OK;    
}


extern osal_result_t osal_mutex_delete(osal_mutex_t *mutex)
{
    if(NULL == mutex)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    embot::os::rtos::mutex_delete(reinterpret_cast<embot::os::rtos::mutex_t*>(mutex));
    return(osal_res_OK);
}


extern osal_semaphore_t * osal_semaphore_new(uint8_t maxtokens, uint8_t tokens)
{
    if((0 == maxtokens) || (tokens > maxtokens))
    {
        s_osal_error(osal_error_incorrectparameter, "osal: value of maxtokens or tokens are incorrect in osal_semaphore_new()");
        return(NULL);
    }

    embot::os::rtos::semaphore_t* s = embot::os::rtos::semaphore_new(maxtokens, tokens);
    return reinterpret_cast<osal_semaphore_t*>(s);
}

  
extern osal_result_t osal_semaphore_set(osal_semaphore_t *sem, uint8_t tokens)
{
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }
        
    osRtxSemaphore_t *ss = (osRtxSemaphore_t*)sem;    
    
    if(tokens > ss->max_tokens)
    {
        return(osal_res_NOK_generic);
    }

    if(tokens == ss->tokens)
    {
        return(osal_res_OK);
    }
    
    if(NULL == ss->thread_list)
    {
        ss->tokens = tokens;
        return(osal_res_OK);
    }
    
    // problems: we have a thread waiting ... for now i invalidate that
    
    s_osal_error(osal_error_incorrectparameter, "osal: osal_semaphore_set() cannot set because a thread is waiting");
    return(osal_res_NOK_generic);
}


extern osal_result_t osal_semaphore_decrement(osal_semaphore_t *sem, osal_reltime_t tout)
{   
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }

    bool v = embot::os::rtos::semaphore_acquire(reinterpret_cast<embot::os::rtos::semaphore_t*>(sem), tout);    
    return (v) ? osal_res_OK : osal_res_NOK_timeout;
}


extern osal_result_t osal_semaphore_increment(osal_semaphore_t *sem, osal_caller_t caller)
{
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }
    embot::os::rtos::semaphore_release(reinterpret_cast<embot::os::rtos::semaphore_t*>(sem));    
    return osal_res_OK;
}


extern osal_result_t osal_semaphore_delete(osal_semaphore_t *sem)
{
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }

    embot::os::rtos::semaphore_delete(reinterpret_cast<embot::os::rtos::semaphore_t*>(sem));
    return(osal_res_OK);
}


extern osal_timer_t * osal_timer_new(void)
{
    osal_timer_t * retval = reinterpret_cast<osal_timer_t*>(osal_base_memory_new(sizeof(osal_timer_t)));
    if(NULL == retval)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for osal_timer_t in osal_timer_new()");
//        osal_mutex_release(s_osal_mutex_api_protection);
        return(NULL);        
    }
    
    retval->rtostimer = embot::os::rtos::timer_new();
    retval->dummy = 0;
    retval->onexpiry.cbk = nullptr;
    retval->onexpiry.par = nullptr;

//    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
//    s_resources_used[osal_info_entity_timer] ++;
//    osal_mutex_release(s_osal_mutex_api_protection);

    return(retval);
}


void s_timer_adapter_onexpiry(void *p)
{
    osal_timer_t *t = reinterpret_cast<osal_timer_t*>(p);
    if(nullptr != t->onexpiry.cbk)
    {
        t->onexpiry.cbk(t, t->onexpiry.par);
    }
}

extern osal_result_t osal_timer_start(osal_timer_t *timer, osal_timer_timing_t *timing, osal_timer_onexpiry_t *onexpiry, osal_caller_t caller)
{
    if((NULL == timer) || (NULL == timer->rtostimer) || (NULL == timing) || (NULL == onexpiry))
    { 
        return(osal_res_NOK_nullpointer);
    }
    
    if(true == embot::os::rtos::timer_running(timer->rtostimer))
    {
        embot::os::rtos::timer_stop(timer->rtostimer);
    }

    // note: in cmsisos2 timing->startat is not supported
    embot::os::rtos::timerMode mode = (osal_tmrmodeONESHOT == timing->mode) ? embot::os::rtos::timerMode::oneshot : embot::os::rtos::timerMode::forever;
    embot::core::relTime countdown = timing->count;
    embot::os::rtos::fpOnTimerExpiry cmsisos2onexpiry = s_timer_adapter_onexpiry;
    timer->onexpiry.cbk = onexpiry->cbk;
    timer->onexpiry.par = onexpiry->par;    

    bool r = embot::os::rtos::timer_start(timer->rtostimer, mode, countdown, cmsisos2onexpiry, timer);
    
    return (true == r) ? osal_res_OK : osal_res_NOK_generic;
}


extern osal_result_t osal_timer_stop(osal_timer_t *timer, osal_caller_t caller)
{   
    if((NULL == timer) || (NULL == timer->rtostimer))
    { 
        return(osal_res_NOK_nullpointer);
    } 
    
    embot::os::rtos::timer_stop(timer->rtostimer);
    
    return osal_res_OK;    
}


extern osal_result_t osal_timer_isactive(osal_timer_t *timer, osal_caller_t caller)
{
    if((NULL == timer) || (NULL == timer->rtostimer))
    { 
        return(osal_res_NOK_nullpointer);
    } 
    
    bool r = embot::os::rtos::timer_running(timer->rtostimer);
    
    return (true == r) ? osal_res_OK : osal_res_NOK_generic;    
}

extern osal_result_t osal_timer_delete(osal_timer_t *timer)
{
    if((NULL == timer) || (NULL == timer->rtostimer))
    { 
        return(osal_res_NOK_nullpointer);
    } 

    if(true == embot::os::rtos::timer_running(timer->rtostimer))
    {
        embot::os::rtos::timer_stop(timer->rtostimer);
    }
    
    embot::os::rtos::timer_delete(timer->rtostimer);
    
    timer->rtostimer = nullptr;
    
    osal_base_memory_del(timer);
        
//    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
//    s_resources_used[osal_info_entity_timer] --;
//    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);
}

#warning NOTE: what about osal_ext_calloc(), osal_ext_realloc(), osal_ext_free() ??? 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void osal_hid_reset_static_ram(void)
{
    // init static ram. it is necessary to do that only if we use a shalib.   
    memset(&s_osal_osal_cfg, 0, sizeof(osal_cfg_t));

    s_osal_mutex_api_protection     = NULL;

    s_osal_info_status              = osal_info_status_zero;

    s_osal_usec_in_tick             = 0;
    s_osal_launcher_fn              = NULL;

    memset(s_resources_free, 0, osal_info_entity_numberof);
    memset(s_resources_used, 0, osal_info_entity_numberof);

    s_osal_launcher_fn = NULL;
    
    memset(&s_osal_task_launcher, 0, sizeof(s_osal_task_launcher));
    memset(&s_osal_task_idle, 0, sizeof(s_osal_task_idle));
    
    s_id_counter = 0;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_osal_on_fatalerror(osal_task_t *task, osal_fatalerror_t errorcode, const char * errormsg)
{
    if(NULL != s_osal_osal_cfg.extfn.usr_on_fatal_error)
    {
        s_osal_osal_cfg.extfn.usr_on_fatal_error(task, errorcode, errormsg);
        for(;;);
    }
    else
    {
        for(;;)
        {
            errorcode = errorcode;
        }
    }
}


static void s_osal_on_idle(void* p)
{
    p = p;
    s_osal_task_idle.rtostsk = embot::os::rtos::scheduler_thread_running();
    embot::os::rtos::scheduler_associate(s_osal_task_idle.rtostsk, reinterpret_cast<embot::os::Thread*>(&s_osal_task_idle));
    
    if(NULL != s_osal_osal_cfg.extfn.usr_on_idle)
    {
        s_osal_osal_cfg.extfn.usr_on_idle();
        for(;;);
    }
    else
    {
        for(;;);
    }
}


static void s_osal_error(osal_fatalerror_t err_code, const char *err_msg) 
{
    osal_task_t *task = NULL;

    osal_system_scheduling_suspend();
    
    if((osal_info_status_running == s_osal_info_status) || (osal_info_status_suspended == s_osal_info_status))
    { 
        task = osal_task_get(osal_callerAUTOdetect);
    }
    else
    {
        task = NULL;
    }

    if(NULL == err_msg)
    {   // use default message
        s_osal_on_fatalerror(task, err_code, "unspecified message");
    }
    else
    {   // use specific error
        s_osal_on_fatalerror(task, err_code, err_msg);   
    }
}


void osal_launcher(void* p)
{
#if defined(OSAL_USE_rtos_cmsisos2)
    #warning osal_launcher(): verify code for OSAL_USE_rtos_cmsisos2
        
    const int32_t s_SVCall_IRQn  = -5;
    //const int32_t s_PendSV_IRQn  = -2;
    //const int32_t s_SysTick_IRQn = -1;
    
    p = p;

//    s_osal_mutex_api_protection = osal_mutex_new();
//    if(NULL == s_osal_mutex_api_protection)
//    {
//        s_osal_error(osal_error_missingmemory, "osal: cannot create the s_osal_mutex_api_protection");
//    }
    
    // must retrieve the rtostsk in here because we did not call a oosiit_tsk_create(). the system did
//    s_osal_task_launcher->rtostsk = oosiit_tsk_self();

    // change the priority of the svc only. the pendsv and systick have fixed value. 
	// the default in osiit for svc, pendsv and systick is 14, 15, 15. 
    if(NULL != s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set)
    {
        s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set(s_SVCall_IRQn, s_osal_osal_cfg.prio);
        // the others must stay at priority 15....
        //s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set(s_PendSV_IRQn, s_osal_osal_cfg.prio);
        //s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set(s_SysTick_IRQn, s_osal_osal_cfg.prio);
    }
   

    s_osal_task_launcher.rtostsk = embot::os::rtos::scheduler_thread_running();
    embot::os::rtos::scheduler_associate(s_osal_task_launcher.rtostsk, reinterpret_cast<embot::os::Thread*>(&s_osal_task_launcher));
    
    // execute the user-defined function. it may never return ...
    if(NULL != s_osal_launcher_fn)
    {
        s_osal_launcher_fn();
    }
    
    embot::os::rtos::scheduler_deassociate(s_osal_task_launcher.rtostsk, reinterpret_cast<embot::os::Thread*>(&s_osal_task_launcher));
    
    // delete the launcher task    
    embot::os::rtos::thread_delete(embot::os::rtos::scheduler_thread_running());
    
#else
     
    const int32_t s_SVCall_IRQn  = -5;
    //const int32_t s_PendSV_IRQn  = -2;
    //const int32_t s_SysTick_IRQn = -1;
    
    p = p;

    s_osal_mutex_api_protection = osal_mutex_new();

    if(NULL == s_osal_mutex_api_protection)
    {
        s_osal_error(osal_error_missingmemory, "osal: cannot create the s_osal_mutex_api_protection");
    }
    
    // must retrieve the rtostsk in here because we did not call a oosiit_tsk_create(). the system did
    s_osal_task_launcher->rtostsk = oosiit_tsk_self();

    // change the priority of the svc only. the pendsv and systick have fixed value. 
	// the default in osiit for svc, pendsv and systick is 14, 15, 15. 
    if(NULL != s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set)
    {
        s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set(s_SVCall_IRQn, s_osal_osal_cfg.prio);
        // the others must stay at priority 15....
        //s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set(s_PendSV_IRQn, s_osal_osal_cfg.prio);
        //s_osal_osal_cfg.extfn.hal_sys_irqn_priority_set(s_SysTick_IRQn, s_osal_osal_cfg.prio);
    }
   

    // execute the user-defined function. it may never return ...
    s_osal_launcher_fn();

    // but if we are in here it means that the user wants to terminate the launcher task ...
    // thus i mark its stack as available by resetting its signature.
    //s_osal_stacklauncher_data[0] = (uint64_t)0;
    
    
    // delete the task ...

    // delete the launcher task
    oosiit_tsk_delete(oosiit_tsk_self());
#endif    
}

#if 0
extern void  *__libspace_start;
extern uint32_t (*std_libspace)[96/4];

// as in http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0475c/Chdicbfj.html
// you must call that inside void *__user_perthread_libspace (void)
// { static uint8_t firstcall = 1; void * ret; ret = osal_threadsafe_stdlib_mutex_initialize(firstcall); 
//   if(1 == firstcall) { firstcall = 0;} 
//   return(ret); 
// }
extern void * osal_arch_arm_armc99stdlib_getlibspace(uint8_t firstcall) 
{
    // provide a separate libspace for each task
//    osThreadId_t tp = NULL;

    if(1 == firstcall)
    {
        osal_hid_reset_static_ram();
    }


    if(s_osal_info_status < osal_info_status_running)
    {
        // osal not running yet
        return(&__libspace_start);
    }

    // we have osal running, thus also cmsisos2. we get the task id and we give it its memory
//    tp = osThreadGetId();
    osal_task_id_t tid = 0;    
    osal_task_id_get(osal_task_get(osal_callerAUTOdetect), &tid);
    
    static uint64_t s_d[20][96/8] = {0};
    return &s_d[tid][0];
    //return(oosiit_tsk_get_perthread_libspace(tp));
    
}

extern void rt_mut_init(void* mutex);

// as in http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0475c/Chdfjddj.html ...
// one should use it inside a void _mutex_acquire(void *m) function
extern int osal_arch_arm_armc99stdlib_mutex_initialize(void *m) 
{
    // this function accepts a pointer to a 32-bit word and initializes it as a valid mutex

    #define OSAL_SYSMUTEXCNT_HIDDEN  8
    
    static OOSIIT_MUT s_osal_sysmutexes_hidden[OSAL_SYSMUTEXCNT_HIDDEN];
    void *mutex = NULL;

   
    if(s_osal_arch_arm_armc99_sysmutex_number >= OSAL_SYSMUTEXCNT_HIDDEN) 
    {
        // if in here, you need to increase OSIIT_SYSMUTEXCNT_HIDDEN
         s_osal_error(osal_error_internal3, "armc99stdlib: not enough sys mutex");
    }

    // i get a pointer to a mutex
    mutex = (void*) &s_osal_sysmutexes_hidden[s_osal_arch_arm_armc99_sysmutex_number++];
    // i initialise the ram pointed by the mutex. i use rt_mut_init because it does not use the svc handler
    rt_mut_init(mutex);

    // now i write into *mutex the address of the initted mutex
    *((uint32_t*)m) = (uint32_t)mutex;


    // must return a nonzero value on success so that, at runtime, 
    // the library knows that it is being used in a multithreaded environment 
    return(1);
}


// as in http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0475c/Chdfjddj.html ...
// one should use it inside a void _mutex_acquire(mutex *m) function
extern void osal_arch_arm_armc99stdlib_mutex_acquire(void *m)
{
    // acquire a system mutex to lock stdlib resources only if osal is running
    if(s_osal_info_status >= osal_info_status_running) 
    {
        // the 32 bit pointed by m are a valid osal mutex
        oosiit_objptr_t *mutex = (oosiit_objptr_t*)m;
        // OOSIIT running, acquire a mutex
        oosiit_mut_wait(*mutex, OOSIIT_NOTIMEOUT);
    }
}

// as in http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0475c/Chdfjddj.html ...
// one should use it inside a void _mutex_release(mutex *m) function
extern void osal_arch_arm_armc99stdlib_mutex_release(void *m)
{
    // release a system mutex to unlock stdlib resources
    if(s_osal_info_status >= osal_info_status_running) 
    {
        // the 32 bit pointerd by m are a valid osal mutex only if osal is running
        oosiit_objptr_t *mutex = (oosiit_objptr_t*)m;
        // OOSIIT runnning, release a mutex. 
        oosiit_mut_release(*mutex);
    }
}

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




