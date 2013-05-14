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

/* @file       osal-oosiit.c
	@brief      This file keeps internal implementation of the osal module using open oosiit
	@author     marco.accame@iit.it
    @date       08/30/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "oosiit.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


extern void osal_hid_reset_static_ram(void);
extern void osal_hid_entrypoint(void);

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

// we keep 12 bytes for a mutex
typedef uint32_t OOSIIT_MUT[3];

struct osal_task_opaque_t 
{   // 1+1+2+4+4 = 12 bytes
    osal_task_id_t  rtosid;
    uint8_t         prio;
    uint16_t        stksize;
    uint64_t        *stkdata;
    void            *ext;
};

struct osal_timer_opaque_t 
{
    uint32_t rtosdata;
};

struct osal_messagequeue_opaque_t
{
    uint32_t rtosdata;
};

struct osal_mutex_opaque_t
{   
    uint32_t rtosdata;
};

struct osal_semaphore_opaque_t
{
    uint32_t rtosdata;
};


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_osal_on_idle(void *p);

static void s_osal_error(osal_fatalerror_t err_code, const char *err_msg);

static void s_osal_on_fatalerror(osal_task_t *task, osal_fatalerror_t errorcode, const char * errormsg);

static void s_osal_fill_cfg(oosiit_cfg_t *oosiit_c, const osal_cfg_t *osal_c);

static uint32_t s_osal_timeout2tick(osal_reltime_t tout);
static uint32_t s_osal_delay2tick(osal_reltime_t delay);
static uint64_t s_osal_abstime2tick(osal_abstime_t time);
static uint32_t s_osal_period2tick(osal_reltime_t period);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static osal_cpufamily_t s_osal_cpufam = 
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

// configuration of osiit
static oosiit_cfg_t s_osal_oosiit_cfg = {0};

// configuration of osal  
static osal_cfg_t  s_osal_osal_cfg;

// stack for launcher task
static uint64_t *s_osal_stacklauncher_data = NULL;
static uint16_t  s_osal_stacklauncher_size = 0;

// stack for idle task
static uint64_t *s_osal_stackidle_data = NULL;
static uint16_t  s_osal_stackidle_size = 0;


// memory use to store osal_task data.
static osal_task_t *s_osal_task_data = NULL; // its size is number of tasks+2 (idle, launcher, tasks)
//static uint8_t s_osal_task_num = 0;

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
static uint8_t s_osal_arch_arm_armc99_sysmutex_number = 0;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t osal_base_memory_getsize(const osal_cfg_t *cfg, uint32_t *size08aligned)
{
    uint16_t size08 = 0;
    uint16_t size04 = 0;
    uint32_t retval = 0;

    if(NULL == cfg)
    {
        if(NULL != size08aligned)
        {
            *size08aligned = 0;
        }
        return(0);
    }

//     if(osal_memmode_static != cfg->memorymodel)
//     {
//         if(NULL != size08aligned)
//         {
//             *size08aligned = 0;
//         }
//         return(0);
//     }

    s_osal_fill_cfg(&s_osal_oosiit_cfg, cfg);   
    oosiit_memory_getsize(&s_osal_oosiit_cfg, &size04, &size08);
    
    retval = (size04+7)/8 + (size08+7)/8;
    
    // if memory mode is dynamic ... oossit will return 0 memory
    
    if(osal_memmode_static == cfg->memorymodel)
    {   // add the memory required for the osal
        retval += (cfg->launcherstacksize+7)/8 + (cfg->idlestacksize+7)/8 + ((cfg->tasknum+2)*sizeof(osal_task_t)+7)/8;
        retval *= 8;                
    }


    if(NULL != size08aligned)
    {
        *size08aligned = retval;
    }

    return(retval);
}

extern void* osal_base_memory_new(uint32_t size)
{
    return(oosiit_memory_new(size));
}


extern osal_result_t osal_base_memory_del(void* mem)
{
    oosiit_result_t rr = oosiit_memory_del(mem);
    return((oosiit_res_OK == rr) ? (osal_res_OK) : (osal_res_NOK_generic));
}

extern osal_result_t osal_base_initialise(const osal_cfg_t *cfg, uint64_t *data08aligned)
{
    uint16_t size08 = 0;
    uint16_t size04 = 0;
    uint32_t *oosiitdata04al;
    uint64_t *oosiitdata08al;

    if(NULL == cfg)
    {
        s_osal_error(osal_error_missingconfiguration, "osal: NULL cfg in osal_initialise()");
        //return(osal_res_NOK_nullpointer);    
    }    
    
//    if(osal_memmode_static != cfg->memorymodel)
//    {
//        s_osal_error(osal_error_unsupportedbehaviour, "osal: unsupported memory model");
//        //return(osal_res_NOK_nullpointer);
//    }

    if((osal_memmode_static == cfg->memorymodel) && (NULL == data08aligned))
    {
        s_osal_error(osal_error_incorrectparameter, "osal: NULL memory in osal_initialise() for static memory model");
        //return(osal_res_NOK_nullpointer);
    }

    if(osal_rtostype_oosiit != cfg->rtostype)
    {
        s_osal_error(osal_error_unsupportedbehaviour, "osal: unsupported rtos");
        //return(osal_res_NOK_generic);
    }
    
    if(s_osal_cpufam != cfg->cpufam)
    {
        s_osal_error(osal_error_unsupportedbehaviour, "osal: cfg has a wrong cpu family");
        //return(osal_res_NOK_generic);        
    }

    if((osal_info_status_running == s_osal_info_status) || (osal_info_status_suspended == s_osal_info_status))
    {
        s_osal_error(osal_error_generic, "osal: called osal_initialise() after osal_start()");
        //return(osal_res_NOK_generic);
    }

    if((NULL != cfg->extfn.hal_sys_irqn_priority_set) && (cfg->prio > 15))
    {
        s_osal_error(osal_error_generic, "osal: if used, prio must be between 0 and 15");
        //return(osal_res_NOK_generic);
    }


    memcpy(&s_osal_osal_cfg, cfg, sizeof(osal_cfg_t));

    s_osal_fill_cfg(&s_osal_oosiit_cfg, cfg);
    
    oosiit_memory_getsize(&s_osal_oosiit_cfg, &size04, &size08);

    // memory for oosiit
    oosiitdata04al = (uint32_t*)(data08aligned);
    oosiitdata08al = (uint64_t*)(data08aligned + (size04+7)/8);
    oosiit_memory_load(&s_osal_oosiit_cfg, oosiitdata04al, oosiitdata08al);
    
    // memory for launcher stack.
    if(osal_memmode_static == cfg->memorymodel)
    {
        s_osal_stacklauncher_data = (uint64_t*)(data08aligned + (size04+7)/8 + (size08+7)/8);
    }
    else
    {
        s_osal_stacklauncher_data = osal_base_memory_new(cfg->launcherstacksize);
    }
    s_osal_stacklauncher_size = cfg->launcherstacksize;
    memset(s_osal_stacklauncher_data, 0xEE, s_osal_stacklauncher_size); 
   
    // memory for idle stack.
    if(osal_memmode_static == cfg->memorymodel)
    {
        s_osal_stackidle_data = (uint64_t*)(data08aligned + (size04+7)/8 + (size08+7)/8 + (cfg->launcherstacksize+7)/8);
    }
    else
    {
        s_osal_stackidle_data = osal_base_memory_new(cfg->idlestacksize);
    }
    s_osal_stackidle_size = cfg->idlestacksize;
    memset(s_osal_stackidle_data, 0xEE, s_osal_stackidle_size);    
    
    // memory for task ids
    if(osal_memmode_static == cfg->memorymodel)
    {
        s_osal_task_data = (osal_task_t*)(data08aligned + (size04+7)/8 + (size08+7)/8 + (cfg->launcherstacksize+7)/8 + (cfg->idlestacksize+7)/8);
    } 
    else
    {
        s_osal_task_data = osal_base_memory_new(sizeof(osal_task_t)*(cfg->tasknum + 2));
    }
        //s_osal_task_num = cfg->tasknum + 2;

    // config entry for idle task (pos 0) and launcher task (pos 1)

    s_osal_task_data[0].rtosid     = 0;
    s_osal_task_data[0].prio       = osal_prio_systsk_idle;
    s_osal_task_data[0].stksize    = s_osal_stackidle_size;
    s_osal_task_data[0].stkdata    = s_osal_stackidle_data;
    s_osal_task_data[0].ext        = NULL;

    s_osal_task_data[1].rtosid     = 1;
    s_osal_task_data[1].prio       = osal_prio_systsk_launcher;
    s_osal_task_data[1].stksize    = s_osal_stacklauncher_size-8;
    s_osal_task_data[1].stkdata    = &s_osal_stacklauncher_data[1];
    s_osal_task_data[1].ext        = NULL;


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


extern void osal_system_start(void (*launcher_fn)(void)) 
{
    oosiit_task_properties_t tskpinit;
    oosiit_task_properties_t tskpidle;

    if(s_osal_info_status != osal_info_status_initialised)
    {
        if(s_osal_info_status == osal_info_status_zero)
        {
            s_osal_error(osal_error_generic, "osal: called osal_start() w/out prior osal_initialise()"); 
        }
        else
        {
            s_osal_error(osal_error_generic, "osal: called osal_start() again");  
        }
           
    }

    s_osal_info_status = osal_info_status_running;

    s_osal_launcher_fn = launcher_fn; 
     
    if(NULL == s_osal_launcher_fn)
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incorrect param in osal_start()");
    }
    
    tskpinit.function   = osal_launcher;
    tskpinit.param      = NULL;
    tskpinit.priority   = osal_prio_systsk_launcher;
    tskpinit.stacksize  = s_osal_stacklauncher_size-8;
    tskpinit.stackdata  = &s_osal_stacklauncher_data[1];
    
    tskpidle.function   = s_osal_on_idle;
    tskpidle.param      = NULL;
    tskpidle.priority   = osal_prio_systsk_idle;
    tskpidle.stacksize  = s_osal_stackidle_size;
    tskpidle.stackdata  = s_osal_stackidle_data;    
       
    
    // start the system
    oosiit_sys_start(&tskpinit, &tskpidle);
}



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


extern osal_abstime_t osal_info_idletime_get(void)
{
    return(s_osal_usec_in_tick * 0);
}


// extern void * osal_system_launcherbuffer_get(uint16_t *size)
// {
//     if(NULL == size)
//     {
//         return(NULL);
//     }

//     if(NULL == s_osal_stacklauncher_data)
//     {   // osal_initialise() not yet called
//         *size = 0;
//         return(NULL);
//     }

//     if(0 != s_osal_stacklauncher_data[0])
//     {   // the signature is not-zero: the launcher task is either not running yet or not terminated
//         *size = 0;
//         return(NULL);
//     }
//     else 
//     {
//         *size = s_osal_stacklauncher_size-8;
//         return(&s_osal_stacklauncher_data[1]);
//     }
// }



extern void osal_system_ticks_abstime_set(osal_abstime_t tot)
{
    oosiit_time_set(s_osal_abstime2tick(tot));
}


extern osal_abstime_t osal_system_ticks_abstime_get(void)
{
    return((uint64_t)s_osal_usec_in_tick * oosiit_time_get());
}


extern osal_abstime_t osal_system_abstime_get(void)
{
    return(oosiit_microtime_get());
}

extern osal_nanotime_t osal_system_nanotime_get(void)
{
    return(oosiit_nanotime_get());
}

extern void osal_system_scheduling_suspend(void)
{
    if(osal_info_status_running == s_osal_info_status)
    {   // order is important in here
        oosiit_sys_suspend();
        s_osal_info_status = osal_info_status_suspended;
    }
}

extern void osal_system_scheduling_restart(void)
{
    if(osal_info_status_suspended == s_osal_info_status)
    {   // order is important in here
        s_osal_info_status = osal_info_status_running;
        oosiit_sys_resume();
    }
}

extern osal_task_t * osal_task_new1(osal_task_properties_t *tskprop)
{
    if(NULL == tskprop)
    {
        return(NULL);
    }
    return(osal_task_new(tskprop->function, tskprop->param, tskprop->priority, tskprop->stacksize));
}


extern osal_task_t * osal_task_new(void (*run_fn)(void*), void *run_arg, uint8_t prio, uint16_t stksize)
{
    uint64_t *stack = NULL;
    
    uint8_t taskid = 0;
    oosiit_task_properties_t tskprop;


    if((0 == stksize) || (prio<osal_prio_usrtsk_min) || (prio>osal_prio_usrtsk_max) || (NULL == run_fn))
    {
        //return(NULL);
        s_osal_error(osal_error_incorrectparameter, "osal: incorr param in osal_task_new()");
    }

    // protect vs concurrent call
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    
    stack = oosiit_memory_getstack(stksize);

    if(NULL == stack)
    {
        // cannot get a stack
        s_osal_error(osal_error_missingmemory, "osal: missing mem for stack in osal_task_new()");
        osal_mutex_release(s_osal_mutex_api_protection);
        //return(NULL);
    }

    // increment used stacksize
    s_resources_used[osal_info_entity_globalstack] += stksize;

    // use priority osal_prio_systsk_usrwhencreated = 1 to avoid that the run_fn starts straigth away 

    tskprop.function        = run_fn;
    tskprop.param           = run_arg;
    tskprop.priority        = osal_prio_systsk_usrwhencreated;
    tskprop.stacksize       = stksize;
    tskprop.stackdata       = stack;
    
    //taskid = oosiit_tsk_create_oldversion(run_fn, run_arg, osal_prio_systsk_usrwhencreated, stack, stksize);
    taskid = oosiit_tsk_create(&tskprop);

    if(0 == taskid)
    {
        // cannot get a task
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos task in osal_task_new()");
        osal_mutex_release(s_osal_mutex_api_protection);
        //return(NULL);
    }

    s_resources_used[osal_info_entity_task] ++;


    s_osal_task_data[taskid].rtosid     = taskid;
    s_osal_task_data[taskid].prio       = prio;
    s_osal_task_data[taskid].stksize    = stksize;
    s_osal_task_data[taskid].stkdata    = stack;
    s_osal_task_data[taskid].ext        = NULL;

    // can release now
    osal_mutex_release(s_osal_mutex_api_protection);

    // the destiny of the task is now given to the scheduler. it can start now if prio is higher than the priority of the caller  
    oosiit_tsk_setprio(taskid, prio);

    return(&s_osal_task_data[taskid]);
}



extern osal_result_t osal_task_wait(osal_reltime_t time)
{
    if(0 == time)
    {
        oosiit_tsk_pass();
    }
    else
    {
        oosiit_dly_wait(s_osal_delay2tick(time));
    }

    return(osal_res_OK);
}

extern osal_result_t osal_task_pass(void)
{
    oosiit_tsk_pass();
    return(osal_res_OK);
}

extern osal_result_t osal_task_period_set(osal_reltime_t period)
{
    oosiit_itv_set(s_osal_period2tick(period));
    return(osal_res_OK);
}


extern osal_result_t osal_task_period_wait(void)
{
    oosiit_itv_wait();
    return(osal_res_OK);
}

extern osal_result_t osal_task_delete(osal_task_t *tsk)
{
    osal_task_id_t taskid;
    //oosiit_result_t res;
    
    if(NULL == tsk)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    // some tasks could call it at the same time ... better to protect
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);   
    
    taskid = tsk->rtosid;   
    oosiit_tsk_delete(taskid);
    
    s_resources_used[osal_info_entity_task] --;
    if(osal_memmode_dynamic == s_osal_osal_cfg.memorymodel)
    {
        s_resources_used[osal_info_entity_globalstack] -= s_osal_task_data[taskid].stksize;
    }

    s_osal_task_data[taskid].rtosid     = 0;
    s_osal_task_data[taskid].prio       = 0;
    s_osal_task_data[taskid].stksize    = 0;
    s_osal_task_data[taskid].stkdata    = NULL;
    s_osal_task_data[taskid].ext        = NULL;        
 
    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);    
}

extern osal_result_t osal_task_priority_get(osal_task_t *tsk, uint8_t *prio)
{

    if((NULL == tsk) || (NULL == prio))
    {
        return(osal_res_NOK_nullpointer);
    }
    
    *prio = tsk->prio;

    return(osal_res_OK);
}

extern osal_result_t osal_task_priority_set(osal_task_t *tsk, uint8_t prio)
{

    if(NULL == tsk)
    {
        return(osal_res_NOK_nullpointer);
    }

    if((prio<osal_prio_usrtsk_min) || (prio>osal_prio_usrtsk_max))
    {
        return(osal_res_NOK_generic);
    }
   
    // some tasks could call it at the same time ... better to protect
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);

    tsk->prio = prio;

    oosiit_tsk_setprio(tsk->rtosid, prio);

    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);
}

extern osal_result_t osal_task_id_get(osal_task_t *tsk, osal_task_id_t *id)
{
    if((NULL == tsk) || (NULL == id))
    {
        return(osal_res_NOK_nullpointer);
    }
    
    *id = tsk->rtosid;

    return(osal_res_OK);
}

extern osal_result_t osal_task_extdata_set(osal_task_t *tsk, void *ext)
{
    if(NULL == tsk)
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
    
    return(tsk->ext);
}


extern osal_task_t * osal_task_get(osal_caller_t caller)
{
    // in rtx it is the same function 
    uint8_t taskid = 0;

#if 0    
    if(osal_callerTSK == caller)
    {
        taskid = oosiit_tsk_self();    
    }
    else
    {
        taskid = oosiit_tsk_self();
    }
#endif
    
    taskid = oosiit_tsk_self();    

    if(255 == taskid)
    {
        taskid = 0;
    }

    return(&s_osal_task_data[taskid]);
}

extern void * osal_task_stack_get(uint16_t *size)
{
    uint8_t taskid = oosiit_tsk_self();    

    if(255 == taskid)
    {
        taskid = 0;
    }
    
    if(NULL != size)
    {
        *size = s_osal_task_data[taskid].stksize;
    }

    return(s_osal_task_data[taskid].stkdata);
}

extern osal_messagequeue_t * osal_messagequeue_new(uint16_t maxmsg)
{
    osal_messagequeue_t *retptr = NULL;

    if(maxmsg == 0)
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incorr param in osal_messagequeue_new()");
        //return(NULL);
    }
    
  
    retptr = oosiit_mbx_create(maxmsg);

    if(NULL == retptr)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos msg queue in osal_messagequeue_new()");
        //return(NULL);
    }

    // protect he because the osiit_* calls are already protected as they are executes by svc
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);

    s_resources_used[osal_info_entity_messagequeue] ++;
    s_resources_used[osal_info_entity_message] += maxmsg;

    osal_mutex_release(s_osal_mutex_api_protection);

    return(retptr);
}


extern osal_result_t osal_messagequeue_get(osal_messagequeue_t *mq, osal_message_t *pmsg, osal_reltime_t tout, osal_caller_t caller)
{
    osal_result_t res = osal_res_NOK_timeout; 
    oosiit_result_t rr;
    void *p = NULL;

    if((NULL == mq) || (NULL == pmsg))
    {
        return(osal_res_NOK_nullpointer);
    }

    *pmsg = (osal_message_t)0;
    
    // any caller ...
    rr = oosiit_mbx_retrieve(mq, &p, s_osal_timeout2tick(tout));
    
    *pmsg = (osal_message_t)p;
    
    switch(rr)
    {
        case oosiit_res_OK:         res = osal_res_OK;              break;
        case oosiit_res_MBX:        res = osal_res_OK;              break;
//        case oosiit_res_TMO:        res = (osal_callerTSK == caller) ? (osal_res_NOK_timeout) : (osal_res_NOK_isrnowait);             break;
        case oosiit_res_TMO:        res = osal_res_NOK_timeout;     break;
        case oosiit_res_NOK: 
        default:                    res = osal_res_NOK_nullpointer; break; 
    }
    
    return(res);
}


extern osal_message_t osal_messagequeue_getquick(osal_messagequeue_t *mq, osal_reltime_t tout, osal_caller_t caller)
{
    void *p = NULL;

    if(NULL == mq)
    {
        return((osal_message_t)0);
    }
    
    // any caller
    oosiit_mbx_retrieve(mq, &p, s_osal_timeout2tick(tout));

    
    return((osal_message_t)p);
}

extern uint16_t osal_messagequeue_available(osal_messagequeue_t *mq, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(0);
    }

    return( oosiit_mbx_available(mq) );    
}


extern uint16_t osal_messagequeue_size(osal_messagequeue_t *mq, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(0);
    }

    return( oosiit_mbx_used(mq) );    
}


extern osal_result_t osal_messagequeue_put(osal_messagequeue_t *mq, osal_message_t msg, osal_reltime_t tout, osal_caller_t caller)
{
    oosiit_result_t rr;

    if(NULL == mq)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    rr = oosiit_mbx_send(mq, (void*)msg, s_osal_timeout2tick(tout));

    if(rr == oosiit_res_OK)
    {
        return(osal_res_OK);
    }
    else
    {
        return(osal_res_NOK_timeout);
    }

}

extern osal_result_t osal_messagequeue_delete(osal_messagequeue_t *mq)
{
    s_osal_error(osal_error_unsupportedbehaviour, "osal_messagequeue_delete():");
    return(osal_res_NOK_generic);
}

extern osal_result_t osal_eventflag_set(osal_eventflag_t flag, osal_task_t * totask, osal_caller_t caller)
{
    if(NULL == totask)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    oosiit_evt_set(flag, totask->rtosid);

    return(osal_res_OK);
}


extern osal_result_t osal_eventflag_get(osal_eventflag_t waitmsk, osal_eventflag_waitmode_t waitmode, osal_eventflag_t *rxmsk, osal_reltime_t tout)
{
    osal_result_t res = osal_res_NOK_timeout; 
    oosiit_result_t rr;

    if(NULL == rxmsk)
    {
        return(osal_res_NOK_nullpointer);
    }

    *rxmsk = 0;
    
    rr = oosiit_evt_wait(waitmsk, s_osal_timeout2tick(tout), (osal_waitALLflags == waitmode) ? (oosiit_evt_wait_mode_all) : (oosiit_evt_wait_mode_any));
    
    if(oosiit_res_EVT == rr)
    {
         res = osal_res_OK;
         *rxmsk = (osal_waitALLflags == waitmode) ? (waitmsk) : (oosiit_evt_get());
    }

    return(res);

}


extern osal_eventflag_t osal_eventflag_getany(osal_reltime_t tout)
{

    if(oosiit_res_EVT == oosiit_evt_wait(0xFFFFFFFF, s_osal_timeout2tick(tout), oosiit_evt_wait_mode_any))
    {
        return(oosiit_evt_get());
    }
    else
    {
        return(0);
    }

}


extern osal_mutex_t * osal_mutex_new(void) 
{
    osal_mutex_t *retptr = NULL;
    
    retptr = oosiit_mut_create();
    
    if(NULL == retptr)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos mutex in osal_mutex_new()");
        //return(NULL);
    }

    if(NULL != s_osal_mutex_api_protection)
    {
        osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
        s_resources_used[osal_info_entity_mutex] ++;
        osal_mutex_release(s_osal_mutex_api_protection);
    }
 
    // ok, all done
    return(retptr);
} 


extern osal_result_t osal_mutex_take(osal_mutex_t *mutex, osal_reltime_t tout) 
{
    oosiit_result_t rr;
    
    if(NULL == mutex)
    {
        return(osal_res_NOK_nullpointer);
    }
    
    rr = oosiit_mut_wait(mutex, s_osal_timeout2tick(tout));
    
    if((oosiit_res_OK == rr) || (oosiit_res_MUT == rr))
    {
        return(osal_res_OK);
    }
    else
    {
        return(osal_res_NOK_timeout);
    }

}


extern osal_result_t osal_mutex_release(osal_mutex_t *mutex) 
{
    return((oosiit_res_NOK == oosiit_mut_release(mutex)) ? (osal_res_NOK_generic) : (osal_res_OK));
}

extern osal_result_t osal_mutex_delete(osal_mutex_t *mutex)
{
    s_osal_error(osal_error_unsupportedbehaviour, "osal_mutex_delete():");
    return(osal_res_NOK_generic);
}


extern osal_semaphore_t * osal_semaphore_new(uint8_t maxtokens, uint8_t tokens)
{
    osal_semaphore_t *retptr = NULL;

    if((0 == maxtokens) || (tokens > maxtokens))
    {
        s_osal_error(osal_error_incorrectparameter, "osal: value of maxtokens or tokens are incorrect in osal_semaphore_new()");
        //return(NULL);
    }
    
    retptr = oosiit_sem_create(maxtokens, tokens);


    if(NULL == retptr)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos sem in osal_semaphore_new()");
        //return(NULL);
    }


    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    s_resources_used[osal_info_entity_semaphore] ++;
    osal_mutex_release(s_osal_mutex_api_protection);
    
    // ok, all done
    return(retptr);
}

extern osal_result_t osal_semaphore_set(osal_semaphore_t *sem, uint8_t tokens)
{
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }
    
    oosiit_sem_set(sem, tokens);

    // ok, all done
    return(osal_res_OK);
}


extern osal_result_t osal_semaphore_decrement(osal_semaphore_t *sem, osal_reltime_t tout)
{
    oosiit_result_t rr;
    
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }
    
    rr = oosiit_sem_wait(sem, s_osal_timeout2tick(tout));
    
    if((oosiit_res_OK == rr) || (oosiit_res_SEM == rr))
    {
        return(osal_res_OK);
    }
    else
    {
        return(osal_res_NOK_timeout);
    }
    

}

extern osal_result_t osal_semaphore_increment(osal_semaphore_t *sem, osal_caller_t caller)
{
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }
  
    return((oosiit_res_OK == oosiit_sem_send(sem)) ? (osal_res_OK) : (osal_res_NOK_generic));
}


extern osal_result_t osal_semaphore_delete(osal_semaphore_t *sem)
{
    s_osal_error(osal_error_unsupportedbehaviour, "osal_semaphore_delete():");
    return(osal_res_NOK_generic); 
}


extern osal_timer_t * osal_timer_new(void)
{
    // oosiit_advtmr_start returns a oosiit_objptr_t, which is a void*.
    // as the osal_timer_t type contains only a ptr to the rtos timer then we can use the following:

    void *ret = oosiit_advtmr_new();
    
    if(NULL == ret)
    { 
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos timer in osal_timer_new()");
        //return(NULL);
    }

    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    s_resources_used[osal_info_entity_timer] ++;
    osal_mutex_release(s_osal_mutex_api_protection);

    return((osal_timer_t*)ret);
}



extern osal_result_t osal_timer_start(osal_timer_t *timer, osal_timer_timing_t *timing, osal_timer_onexpiry_t *onexpiry, osal_caller_t caller)
{
    oosiit_advtmr_timing_t tim;
    oosiit_advtmr_action_t act;
    oosiit_result_t rr;
 
    if((NULL == timer) || (NULL == timing) || (NULL == onexpiry))
    { 
        return(osal_res_NOK_nullpointer);
    }

    tim.startat     = s_osal_abstime2tick(timing->startat);
    tim.firstshot   = s_osal_delay2tick(timing->count);
    tim.othershots  = (osal_tmrmodeONESHOT == timing->mode) ? (0) : (tim.firstshot);
    
    act.cbk         = (void(*)(void*, void*))onexpiry->cbk;
    act.par         = onexpiry->par;
    
    
    rr = oosiit_advtmr_start(timer, &tim, &act);


    return((oosiit_res_NOK == rr) ? (osal_res_NOK_generic) : (osal_res_OK));
}


extern osal_result_t osal_timer_stop(osal_timer_t *timer, osal_caller_t caller)
{   
    if(NULL == timer)
    { 
        return(osal_res_NOK_nullpointer);
    }

    return((oosiit_res_NOK == oosiit_advtmr_stop(timer)) ? (osal_res_NOK_generic) : (osal_res_OK));
}


extern osal_result_t osal_timer_isactive(osal_timer_t *timer, osal_caller_t caller)
{
    if(NULL == timer)
    { 
        return(osal_res_NOK_nullpointer);
    }

    return((oosiit_res_NOK == oosiit_advtmr_isactive(timer)) ? (osal_res_NOK_generic) : (osal_res_OK));
}

extern osal_result_t osal_timer_delete(osal_timer_t *timer)
{
    if(NULL == timer)
    { 
        return(osal_res_NOK_nullpointer);
    }

    oosiit_advtmr_delete(timer);

    
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    s_resources_used[osal_info_entity_timer] --;
    osal_mutex_release(s_osal_mutex_api_protection);

    return(osal_res_OK);
}




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
    oosiit_taskid_t idx;

    if(1 == firstcall)
    {
        osal_hid_reset_static_ram();
    }


    if(s_osal_info_status < osal_info_status_running)
    {
        // osal not running yet
        return(&__libspace_start);
    }

    // we have osal running, thus also oosiit. we get the osiit task id and we give it its memory
    idx = oosiit_tsk_self();

#if 0  
    if(NULL == std_libspace)
    {
        s_osal_error(osal_error_internal2, "armc99stdlib: not enough libspace");
    }

    return ((void *)&std_libspace[idx-1]);
#else
    return(oosiit_tsk_get_perthread_libspace(idx));
#endif      
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



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// used only by osiit
void oosiit_sys_error(oosiit_error_code_t errorcode) 
{
    switch(errorcode)
    {
        case oosiit_error_stackoverflow:
        {
            s_osal_error(osal_error_stackoverflow, "osal: stack overflow");
        } break;

        case oosiit_error_isrfifooverflow:
        {
             s_osal_error(osal_error_internal0, "osal: known error from rtos");
        } break;
        case oosiit_error_mbxoverflow:
        {
            s_osal_error(osal_error_internal1, "osal: known error from rtos");
        } break;
 
        default:
        {
            s_osal_error(osal_error_unknownfromrtos, "osal: unknown error from rtos");
        } break;
    }
}



// used only by the shalib as an entrypoint
extern void osal_hid_entrypoint(void)
{
    osal_base_initialise(NULL, NULL);
}


extern void osal_hid_reset_static_ram(void)
{
    // init static ram. it is necessary to do that only if we use a shalib.

    memset(&s_osal_oosiit_cfg, 0, sizeof(oosiit_cfg_t));  
    memset(&s_osal_osal_cfg, 0, sizeof(osal_cfg_t));

    s_osal_mutex_api_protection     = NULL;

    s_osal_info_status              = osal_info_status_zero;
    s_osal_stacklauncher_data       = NULL;
    s_osal_stacklauncher_size       = 0;

    s_osal_task_data                = NULL; // its size is number of tasks+2 (idle, launcher, tasks)
    //s_osal_task_num               = 0;

    s_osal_usec_in_tick             = 0;
    s_osal_launcher_fn              = NULL;

    memset(s_resources_free, 0, osal_info_entity_numberof);
    memset(s_resources_used, 0, osal_info_entity_numberof);

    s_osal_arch_arm_armc99_sysmutex_number = 0;

    s_osal_launcher_fn = NULL;

    // values in oosiit whcih cannot wait for a call of the osiit init function called by osiit_memory_load()
    // which is called by osal_initialise()
    {
        extern struct OS_TSK os_tsk;
        memset(&os_tsk, 0, 2*sizeof(void*));        // trick .... it has two pointers
    }
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
    uint32_t tid = 0;
    osal_task_t *task = NULL;

    osal_system_scheduling_suspend();
    
    if((osal_info_status_running == s_osal_info_status) || (osal_info_status_suspended == s_osal_info_status))
    { 
        tid = oosiit_tsk_self(); //isr_tsk_get(); // the os-version crashes if osiit calls os_error().... os_tsk_self();
        if(255 == tid)  // idle-task... use zero-th position in s_osal_task_data[]
        {
            tid = 0;
        }
        task = &s_osal_task_data[tid];
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


static void s_osal_fill_cfg(oosiit_cfg_t *oosiit_c, const osal_cfg_t *osal_c)
{
#if 0    
    oosiit_c->maxnumofusertasks                 = osal_c->tasknum;
    oosiit_c->checkStack                        = 1;   // always check stack overflow w/ osiit
    oosiit_c->sizeISRFIFO                       = 16; // ???? the enqueable requests done to rtos within an ISR
    oosiit_c->roundRobin                        = osal_c->roundrobin;
    oosiit_c->osClock                           = osal_c->cpufreq;
    oosiit_c->osTick                            = osal_c->tick;
    oosiit_c->roundRobinTimeout                 = osal_c->roundrobintick;
    
    oosiit_c->numAdvTimer                       = osal_c->timernum;
    oosiit_c->numMutex                          = osal_c->mutexnum + 1;  // +1 for internal sys mutex s_osal_mutex_api_protection
    oosiit_c->numSemaphore                      = osal_c->semaphorenum;
    oosiit_c->numMessageBox                     = osal_c->mqueuenum;
    oosiit_c->numMessageBoxElements             = osal_c->mqueueelemnum;
    oosiit_c->sizeof64alignedStack              = osal_c->globalstacksize; 
#else
    oosiit_c->cpufreq                           = osal_c->cpufreq;
    oosiit_c->ticktime                          = osal_c->tick;
    oosiit_c->capacityofpostpendcommandfifo     = 16; // ???? the enqueable requests done to rtos within an ISR
    oosiit_c->checkstackoverflow                = 1;   // always check stack overflow w/ osiit
    oosiit_c->memorymode                        = (osal_memmode_static == osal_c->memorymodel) ? oosiit_memmode_static : oosiit_memmode_dynamic; //oosiit_memmode_dynamic;
    oosiit_c->roundrobinenabled                 = osal_c->roundrobin;    
    oosiit_c->roundrobinnumberofticks           = osal_c->roundrobintick;    
    oosiit_c->maxnumofusertasks                 = osal_c->tasknum;
       
    oosiit_c->numAdvTimer                       = osal_c->timernum;
    oosiit_c->numMutex                          = osal_c->mutexnum + 1;  // +1 for internal sys mutex s_osal_mutex_api_protection
    oosiit_c->numSemaphore                      = osal_c->semaphorenum;
    oosiit_c->numMessageBox                     = osal_c->mqueuenum;
    oosiit_c->numMessageBoxElements             = osal_c->mqueueelemnum;
    oosiit_c->sizeof64alignedStack              = osal_c->globalstacksize; 
#endif    
}

//     oosiit_c->cpufreq                           = osal_c->cpufreq;
//     oosiit_c->ticktime                          = osal_c->tick;
//     oosiit_c->capacityofpostpendcommandfifo     = 16; // ???? the enqueable requests done to rtos within an ISR
//     oosiit_c->checkstackoverflow                = 1;   // always check stack overflow w/ osiit
//     oosiit_c->memoryallocation                  = oosiit_memalloc_atstartup;
//     oosiit_c->roundrobinenabled                 = osal_c->roundrobin;    
//     oosiit_c->roundrobinticktime                = osal_c->roundrobintick;
//     
//     oosiit_c->maxnumofusertasks                 = osal_c->tasknum;
//        
//     oosiit_c->numAdvTimer                       = osal_c->timernum;
//     oosiit_c->numMutex                          = osal_c->mutexnum + 1;  // +1 for internal sys mutex s_osal_mutex_api_protection
//     oosiit_c->numSemaphore                      = osal_c->semaphorenum;
//     oosiit_c->numMessageBox                     = osal_c->mqueuenum;
//     oosiit_c->numMessageBoxElements             = osal_c->mqueueelemnum;
//     oosiit_c->sizeof64alignedStack              = osal_c->globalstacksize; 

void osal_launcher(void* p)
{
    const int32_t s_SVCall_IRQn  = -5;
    //const int32_t s_PendSV_IRQn  = -2;
    //const int32_t s_SysTick_IRQn = -1;
    
    p = p;

    s_osal_mutex_api_protection = osal_mutex_new();

    if(NULL == s_osal_mutex_api_protection)
    {
        s_osal_error(osal_error_missingmemory, "osal: cannot create the s_osal_mutex_api_protection");
    }

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
    s_osal_stacklauncher_data[0] = (uint64_t)0;

    // delete the launcher task
    oosiit_tsk_delete(oosiit_tsk_self());
}

static uint32_t s_osal_period2tick(osal_reltime_t period)
{
    period /= s_osal_usec_in_tick;

    if(osal_reltimeZERO == period)
    {   // cannot be zero
        period = 1; 
    }
    else if(period >= 0x80000000)
    {
        // there is a bug if called with period higher than 0x80000000
        // however, we have this value only if s_osal_usec_in_tick is either 1 or 2
        // and in case of s_osal_usec_in_tick = 1, the period is 2147 seconds.
        period = 0x80000000 - 1;
    }

    return(period);
}

static uint32_t s_osal_delay2tick(osal_reltime_t delay)
{
    return(delay / s_osal_usec_in_tick);
}

static uint64_t s_osal_abstime2tick(osal_abstime_t time)
{
    if(osal_abstimeNONE == time)
    {
        return(OOSIIT_ASAPTIME);
    }

    return(time / s_osal_usec_in_tick);
}

static uint32_t s_osal_timeout2tick(osal_reltime_t tout)
{
    if(osal_reltimeINFINITE == tout)
    {     // first, because it is likely to be the most probable
        return(OOSIIT_NOTIMEOUT); 
    }
    else if(osal_reltimeZERO == tout)
    {
        // second, because it is likely to be the next most probable
        return(0);
    }
    else
    {
        // else  ... must do division.
        return(tout / s_osal_usec_in_tick);
    }

}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




