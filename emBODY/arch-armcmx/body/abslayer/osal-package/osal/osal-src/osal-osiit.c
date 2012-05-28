/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       osal-osiit.c
	@brief      This file keeps internal implementation of the osal module using osiit, a derivation from rtx
	@author     marco.accame@iit.it
    @date       01/05/2010
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

// use osiit
#include "osiit.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "osal_hid.h" 

// used only by osiit or by procedures derived by osiit
// it is called by osiit w/ error codes 1, 2, 3
void os_error(uint32_t err_code);

extern void osal_hid_reset_static_ram(void);
extern void osal_hid_entrypoint(void);


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define OSIIT_ERROR_1_rtosstackoverflow         1
#define OSIIT_ERROR_2_rtosisrfifooverflow       2
#define OSIIT_ERROR_3_rtosmessageboxoverflow    3
#define OSIIT_ERROR_4_rtosinvalidcall           4


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


extern const osal_reltime_t osal_reltimeZERO        = OSAL_reltimeZERO;
extern const osal_reltime_t osal_reltimeMAX         = OSAL_reltimeMAX;
extern const osal_reltime_t osal_reltimeINFINITE    = OSAL_reltimeINFINITE;

extern const osal_abstime_t osal_abstimeNONE        = OSAL_abstimeNONE;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

struct osal_task_opaque_t 
{   // 1+1+2+4 = 8 bytes
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

// i keep it extern to allow its name in microvision
void osal_launcher(void);

static void s_osal_on_idle(void);

static void s_osal_error(osal_fatalerror_t err_code, const char *err_msg);

static void s_osal_on_fatalerror(osal_task_t *task, osal_fatalerror_t errorcode, const char * errormsg);

static void s_osal_fill_cfg(osiit_params_cfg_t *osiit_c, const osal_cfg_t *osal_c);

static uint32_t s_osal_timeout2tick(osal_reltime_t tout);
static uint32_t s_osal_delay2tick(osal_reltime_t delay);
static uint64_t s_osal_abstime2tick(osal_abstime_t time);
static uint32_t s_osal_period2tick(osal_reltime_t period);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// mutex used to guarantee atomic access at user space in the osal layer
static osal_mutex_t *s_osal_mutex_api_protection = NULL;

// configuration of osiit
static osiit_params_cfg_t s_osal_osiit_cfg = {0};

// configuration of osal  
static osal_cfg_t  s_osal_osal_cfg;

// stack for launcher task
static uint64_t *s_osal_stacklauncher_data = NULL;
static uint16_t  s_osal_stacklauncher_size = 0;

// memory use to store osal_task data.
static osal_task_t *s_osal_task_data = NULL; // its size is number of tasks+2 (idle, launcher, tasks)
//static uint8_t s_osal_task_num = 0;

// indicates how many usec in a tick. used for time conversion
static uint32_t s_osal_usec_in_tick = 0;

// pointer to teh launcher function
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
    uint16_t size08;
    uint16_t size04;
    uint32_t retval = 0;

    if(NULL == cfg)
    {
        if(NULL != size08aligned)
        {
            *size08aligned = 0;
        }
        return(0);
    }

    if(osal_memmode_static != cfg->memorymodel)
    {
        if(NULL != size08aligned)
        {
            *size08aligned = 0;
        }
        return(0);
    }

    s_osal_fill_cfg(&s_osal_osiit_cfg, cfg);
    
    
    osiit_memory_getsize(&s_osal_osiit_cfg, &size04, &size08);


    // add space for the stack of tasks.
    // add space for an array of tasknum uint8_t so that the osal_task_t type can be a pointer 
    // to the taskid returned by osiit function
  
    

//    *size08aligned = (size04+7)/8 + (size08+7)/8 + (cfg->launcherstacksize+7)/8 + ((cfg->tasknum+2)*sizeof(osal_task_t)+7)/8;
//    *size08aligned *= 8;
//
//    return(*size08aligned);

    retval = (size04+7)/8 + (size08+7)/8 + (cfg->launcherstacksize+7)/8 + ((cfg->tasknum+2)*sizeof(osal_task_t)+7)/8;
    retval *= 8;

    if(NULL != size08aligned)
    {
        *size08aligned = retval;
    }

    return(retval);
}

extern osal_result_t osal_base_initialise(const osal_cfg_t *cfg, uint64_t *data08aligned)
{
    uint16_t size08;
    uint16_t size04;
    uint32_t *osiitdata04al;
    uint64_t *osiitdata08al;

    if(NULL == cfg)
    {
        s_osal_error(osal_error_missingconfiguration, "osal: NULL cfg in osal_initialise()");
        //return(osal_res_NOK_nullpointer);    
    }    
    
    if(osal_memmode_static != cfg->memorymodel)
    {
        s_osal_error(osal_error_unsupportedbehaviour, "osal: unsupported memory model");
        //return(osal_res_NOK_nullpointer);
    }

    if(NULL == data08aligned)
    {
        s_osal_error(osal_error_incorrectparameter, "osal: NULL memory in osal_initialise()");
        //return(osal_res_NOK_nullpointer);
    }

    if(osal_rtostype_iitmod_rtxarm != cfg->rtostype)
    {
        s_osal_error(osal_error_unsupportedbehaviour, "osal: unsupported rtos");
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

    s_osal_fill_cfg(&s_osal_osiit_cfg, cfg);
    
    osiit_memory_getsize(&s_osal_osiit_cfg, &size04, &size08);

    // memory for osiit
    osiitdata04al = (uint32_t*)(data08aligned);
    osiitdata08al = (uint64_t*)(data08aligned + (size04+7)/8);
    osiit_memory_load(&s_osal_osiit_cfg, osiitdata04al, osiitdata08al);
    
    // memory for launcher stack.
    s_osal_stacklauncher_data = (uint64_t*)(data08aligned + (size04+7)/8 + (size08+7)/8);
    s_osal_stacklauncher_size = cfg->launcherstacksize;
    memset(s_osal_stacklauncher_data, 0xEE, s_osal_stacklauncher_size); 

    // memory of task ids
    s_osal_task_data = (osal_task_t*)(data08aligned + (size04+7)/8 + (size08+7)/8 + (cfg->launcherstacksize+7)/8);
    //s_osal_task_num = cfg->tasknum + 2;

    // config entry for idle task (pos 0) and launcher task (pos 1)

    s_osal_task_data[0].rtosid     = 0;
    s_osal_task_data[0].prio       = osal_prio_systsk_idle;
    s_osal_task_data[0].stksize    = 0;
    s_osal_task_data[0].stkdata    = NULL;
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
    // RTOScall-rtx:    void os_sys_init_user(void(*task)(void), U8 priority, void *stack, U16 size) 
    os_sys_init_user(osal_launcher, osal_prio_systsk_launcher, &s_osal_stacklauncher_data[1], s_osal_stacklauncher_size-8);
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
    return(s_osal_usec_in_tick * osiit_idletime_get());
}

extern void * osal_system_launcherbuffer_get(uint16_t *size)
{
    if(NULL == size)
    {
        return(NULL);
    }

    if(NULL == s_osal_stacklauncher_data)
    {   // osal_initialise() not yet called
        *size = 0;
        return(NULL);
    }

    if(0 != s_osal_stacklauncher_data[0])
    {   // the signature is not-zero: the launcher task is either not running yet or not terminated
        *size = 0;
        return(NULL);
    }
    else 
    {
        *size = s_osal_stacklauncher_size-8;
        return(&s_osal_stacklauncher_data[1]);
    }
}

extern osal_abstime_t osal_system_abstime_get(void)
{
    return(s_osal_usec_in_tick * osiit_time_get());
}

extern void osal_system_abstime_set(osal_abstime_t tot)
{
    osiit_time_set(s_osal_abstime2tick(tot));
}


extern osal_nanotime_t osal_system_nanotime_get(void)
{
    uint32_t secs, nano;
    uint64_t res;
    osiit_nanotime_get(&secs, &nano);
    res = 1000000000*secs + nano; 
    return(res);
}

extern void osal_system_scheduling_suspend(void)
{
    if(osal_info_status_running == s_osal_info_status)
    {   // order is important in here
        tsk_lock();
		//osiit_isr_tsk_lock();
        s_osal_info_status = osal_info_status_suspended;
    }
}

extern void osal_system_scheduling_restart(void)
{
    if(osal_info_status_suspended == s_osal_info_status)
    {   // order is important in here
        s_osal_info_status = osal_info_status_running;
        tsk_unlock();
        //osiit_isr_tsk_unlock();
    }
}



extern osal_task_t * osal_task_new(void (*run_fn)(void*), void *run_arg, uint8_t prio, uint16_t stksize)
{
    uint64_t *stack = NULL;
    
    uint8_t taskid = 0;


    if((0 == stksize) || (prio<osal_prio_usrtsk_min) || (prio>osal_prio_usrtsk_max) || (NULL == run_fn))
    {
        //return(NULL);
        s_osal_error(osal_error_incorrectparameter, "osal: incorr param in osal_task_new()");
    }

    // protect vs concurrent call
    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    
    stack = osiit_stack_getmem(stksize);

    if(NULL == stack)
    {
        // cannot get a stack
        s_osal_error(osal_error_missingmemory, "osal: missing mem for stack in osal_task_new()");
        osal_mutex_release(s_osal_mutex_api_protection);
        //return(NULL);
    }

    // decrement stacksize
    s_resources_used[osal_info_entity_globalstack] += stksize;

    // use priority osal_prio_systsk_usrwhencreated = 1 to avoid that the run_fn starts straigth away 
    taskid = os_tsk_create_user_ex(run_fn, osal_prio_systsk_usrwhencreated, stack, stksize, run_arg);

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
    os_tsk_prio(taskid, prio);

    return(&s_osal_task_data[taskid]);
}



extern osal_result_t osal_task_wait(osal_reltime_t time)
{
    if(0 == time)
    {
        os_tsk_pass();
    }
    else
    {
        osiit_dly_wait(s_osal_delay2tick(time));
    }

    return(osal_res_OK);
}

extern osal_result_t osal_task_pass(void)
{
    os_tsk_pass();
    return(osal_res_OK);
}

extern osal_result_t osal_task_period_set(osal_reltime_t period)
{
    osiit_itv_set(s_osal_period2tick(period));
    return(osal_res_OK);
}


extern osal_result_t osal_task_period_wait(void)
{
    osiit_itv_wait();
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

    os_tsk_prio(tsk->rtosid, prio);

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
    
    if(osal_callerTSK == caller)
    {
        taskid = os_tsk_self();    
    }
    else
    {
        taskid = isr_tsk_get();
    }
    

    if(255 == taskid)
    {
        taskid = 0;
    }

    return(&s_osal_task_data[taskid]);
}

extern osal_messagequeue_t * osal_messagequeue_new(uint16_t maxmsg)
{
    osal_messagequeue_t *retptr = NULL;

    if(maxmsg == 0)
    {
        s_osal_error(osal_error_incorrectparameter, "osal: incorr param in osal_messagequeue_new()");
        //return(NULL);
    }
    
    // osiit_mbx_getmem returns a OS_ID (a void*), and if osal_messagequeue_t contains just a pointer the following is correct.
    retptr = osiit_mbx_getmem(maxmsg);

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
 
    // RTOSrequ-rtx:    required to initialise the memory of the mbox
    os_mbx_init(retptr, 4*(4+maxmsg));

    return(retptr);
}


extern osal_result_t osal_messagequeue_get(osal_messagequeue_t *mq, osal_message_t *pmsg, osal_reltime_t tout, osal_caller_t caller)
{
    osal_result_t res = osal_res_NOK_timeout; 
    uint32_t r = OS_R_TMO;
    void *p = NULL;

    if((NULL == mq) || (NULL == pmsg))
    {
        return(osal_res_NOK_nullpointer);
    }

    *pmsg = (osal_message_t)0;
    
    if(osal_callerTSK == caller)
    {
        // RTOScall-rtxiit: OS_RESULT osiit_mbx_wait(OS_ID mailbox, void **message, U32 timeout)
        //                  where:  OS_ID is void* (see rtl.h)
        //                  where:  timeout is in rtos-ticks (waits up to 4G-1)
        //                  where:  OS_RESULT is U32 w/ values 
        //                          OS_R_OK (msg was in mbox), 
        //                          OS_R_MBX (msg was not in mbox but arrived), 
        //                          OS_R_TMO (msg did not arrive within timeout).  
        
        r = osiit_mbx_wait(mq, &p, s_osal_timeout2tick(tout));
        
        if(OS_R_TMO != r)
        {
            //res = (OS_R_OK == r) ? (osal_res_OK_immediate) : (osal_res_OK_butwaited);
            res = osal_res_OK;
            *pmsg = (osal_message_t)p;
        }        
    }
    else if(osal_callerISR == caller)
    {
        // RTOScall-rtx:    OS_RESULT isr_mbx_receive(OS_ID mailbox, void**msg),
        //                  where:  OS_RESULT is U32 w/ values 
        //                          OS_R_OK (no msg in mbx, which is empty), 
        //                          OS_R_MBX (ok, a msg was available)
        r = isr_mbx_receive(mq, &p);
        
        if(OS_R_MBX == r)
        {
            //res = osal_res_OK_immediate;
            res = osal_res_OK;
            *pmsg = (osal_message_t)p;
        }
        else
        {
            res = osal_res_NOK_isrnowait;
        }
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

    if(osal_callerTSK == caller)
    {
        // RTOScall-rtxiit: OS_RESULT osiit_mbx_wait(OS_ID mailbox, void **message, U32 timeout)
        //                  where:  OS_ID is void* (see rtl.h)
        //                  where:  timeout is in rtos-ticks (waits up to 4G-1)
        //                  where:  OS_RESULT is U32 w/ values 
        //                          OS_R_OK (msg was in mbox), 
        //                          OS_R_MBX (msg was not in mbox but arrived), 
        //                          OS_R_TMO (msg did not arrive within timeout).  
        
        osiit_mbx_wait(mq, &p, s_osal_timeout2tick(tout));

    }
    else if(osal_callerISR == caller)
    {
        // RTOScall-rtx:    OS_RESULT isr_mbx_receive(OS_ID mailbox, void**msg),
        //                  where:  OS_RESULT is U32 w/ values 
        //                          OS_R_OK (no msg in mbx, which is empty), 
        //                          OS_R_MBX (ok, a msg was available)
        
        isr_mbx_receive(mq, &p);

    }
 
    return((osal_message_t)p);
}

extern uint16_t osal_messagequeue_available(osal_messagequeue_t *mq, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(0);
    }

    return( (osal_callerTSK == caller) ? (os_mbx_check(mq)) : (isr_mbx_check(mq)) );    
}


extern uint16_t osal_messagequeue_size(osal_messagequeue_t *mq, osal_caller_t caller)
{
    if(NULL == mq)
    {
        return(0);
    }

    return( (osal_callerTSK == caller) ? (osiit_mbx_count(mq)) : (osiit_isr_mbx_count(mq)) );    
}


extern osal_result_t osal_messagequeue_put(osal_messagequeue_t *mq, osal_message_t msg, osal_reltime_t tout, osal_caller_t caller)
{
    osal_result_t res = osal_res_NOK_timeout; 

    if(NULL == mq)
    {
        return(osal_res_NOK_nullpointer);
    }

    if(osal_callerTSK == caller)
    {    
        // RTOScall-rtxiit: OS_RESULT osiit_mbx_send(OS_ID mailbox, void *p_msg, U32 timeout), 
        //                  where:  OS_ID is void* (see rtl.h)
        //                  where:  timeout is in rtos-ticks (waits up to 4G-1)
        //                  where:  OS_RESULT is U32 w/ values 
        //                          OS_R_OK (msg could be put into mbox), 
        //                          OS_R_TMO (msg could not be put into a full mbox within timeout).      
        if(OS_R_TMO != osiit_mbx_send(mq, (void*)msg, s_osal_timeout2tick(tout)))
        {
            res = osal_res_OK;
        }
    }
    else if(osal_callerISR == caller)
    {
        // RTOScall-rtx:    void isr_mbx_send(OS_ID mailbox, void*msg)
        // RTOScall-rtx:    OS_RESULT isr_mbx_check(OS_ID mailbox, void*msg) returns number of msg that can be added
        if(0 != isr_mbx_check(mq))
        {
            isr_mbx_send(mq, (void*)msg);
            res = osal_res_OK;
        }
        else
        {
            res = osal_res_NOK_isrnowait;
        }
    }

    return(res);

}

extern osal_result_t osal_eventflag_set(osal_eventflag_t flag, osal_task_t * totask, osal_caller_t caller)
{
    if(NULL == totask)
    {
        return(osal_res_NOK_nullpointer);
    }

    if(osal_callerTSK == caller)
    {
        // RTOScall-rtx:    extern void osiit_evt_set(U32 wait_flags, OS_TID taskid), where: OS_TID is uint32_t
        osiit_evt_set(flag, totask->rtosid);       
    }
    else if(osal_callerISR == caller)
    {
        // RTOScall-rtx:    extern void osiit_isr_evt_set(U32 event_flags, OS_TID task_id), where: OS_TID is uint32_t
        osiit_isr_evt_set(flag, totask->rtosid);
    }
    
    return(osal_res_OK);
}


extern osal_result_t osal_eventflag_get(osal_eventflag_t waitmsk, osal_eventflag_waitmode_t waitmode, osal_eventflag_t *rxmsk, osal_reltime_t tout)
{
    osal_result_t res = osal_res_NOK_timeout; 
    uint32_t r = OS_R_TMO;

    if(NULL == rxmsk)
    {
        return(osal_res_NOK_nullpointer);
    }

    *rxmsk = 0;

    if(osal_waitALLflags == waitmode) 
    {
        // RTOScall-rtxiit:extern OS_RESULT osiit_evt_wait_and(U32 wait_flags, U32 timeout), 
        //                  where: OS_RESULT is uint32_t with values:
        //                  OS_R_EVT (every flag in evt_flags is set),
        //                  OS_R_TMO (timeout expired)
        r = osiit_evt_wait_and(waitmsk, s_osal_timeout2tick(tout));
        
        if(OS_R_EVT == r)
        {
            *rxmsk = waitmsk;
            res = osal_res_OK;
        }
    }
    else if(osal_waitANYflag == waitmode)
    {
        // RTOScall-rtxiit: extern OS_RESULT osiit_evt_wait_or(U32 wait_flags,  U32 timeout), 
        //                  where: OS_RESULT is uint32_t with values:
        //                  OS_R_EVT (at least one flag in evt_flags is set),
        //                  OS_R_TMO (timeout expired)
        r = osiit_evt_wait_or(waitmsk, s_osal_timeout2tick(tout));

        if(OS_R_EVT == r)
        {
            // RTOScall-rtx:    extern U32 osiit_evt_get(void), which returns the flags
            *rxmsk = osiit_evt_get();
            res = osal_res_OK;            
        }
    }

    return(res);
}


extern osal_eventflag_t osal_eventflag_getany(osal_reltime_t tout)
{
    // RTOScall-rtxiit: OS_RESULT osiit_evt_wait_or(U32 evt_flags, U32 tout), 
    //                  where: OS_RESULT is uint32_t with values:
    //                  OS_R_EVT (at least one flag in evt_flags is set),
    //                  OS_R_TMO (timeout expired)

    if(OS_R_EVT == osiit_evt_wait_or(0xFFFFFFFF, s_osal_timeout2tick(tout)))
    {
        // RTOScall-rtx:    extern U32 osiit_evt_get(void), which returns the flags
        return(osiit_evt_get());            
    }
    else
    {
        return(0);
    }

}


extern osal_mutex_t * osal_mutex_new(void) 
{
    osal_mutex_t *retptr = NULL;
    
    // RTOSrequ-rtx: mutex is 32-bit aligned and uses 3 uint32_t
    // keil rtx wants it to be 4-bytes aligned. it used 3 words
    // see its definition in RTL.h of __RL_ARM_VER    380
    // /* Definition of Mutex type */
    // typedef U32 OS_MUT[3];
    // RTOSreq-rtxiit: RTXiit uses the same data structure of 3 words. see OS_MUCB in rt_TypeDef.h
    retptr = osiit_mut_getmem();

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
    
    // RTOScall-rtx:    void os_mut_init(OS_ID mutex), where:  OS_ID is void* (see rtl.h)
    os_mut_init(retptr);

    
    // ok, all done
    return(retptr);
} 


extern osal_result_t osal_mutex_take(osal_mutex_t *mutex, osal_reltime_t tout) 
{
    osal_result_t res = osal_res_NOK_timeout; 
    uint32_t r = OS_R_TMO;
    
    if(NULL == mutex)
    {
        return(osal_res_NOK_nullpointer);
    }

    // RTOScall-rtxiit: OS_RESULT osiit_mut_wait(OS_ID mutex, U32 timeout)
    //                  where return is: OS_R_MUT (acquired immediately), OS_R_MUT (acquired with wait), 
    //                  OS_R_TMO (not acquired for timeout)
    r = osiit_mut_wait(mutex, s_osal_timeout2tick(tout));
    
    if(OS_R_TMO != r)
    {
        //res = (OS_R_OK == r) ? (osal_res_OK_immediate) : (osal_res_OK_butwaited);
        res = osal_res_OK;
    } 
    
    return(res);
}


extern osal_result_t osal_mutex_release(osal_mutex_t *mutex) 
{
    uint32_t r = OS_R_NOK; 
    
    if(NULL == mutex)
    {
        return(osal_res_NOK_nullpointer);
    }


    // the keil mutex returns OS_R_NOK if the caller is not the owner of the
    // mutex or if the function was called after the mutex was already released
    // the same number of times as it was taken
    // RTOScall-rtx:    OS_RESULT os_mut_release(OS_ID mutex) returns: OS_R_OK if released, or 
    //                  OS_R_NOK if not released (caller does not own the mutex or the internal
    //                  counter of mutex is not zero yet)
    r = os_mut_release(mutex);

  
    return((OS_R_NOK == r) ? (osal_res_NOK_generic) : (osal_res_OK));
}


extern osal_semaphore_t * osal_semaphore_new(uint8_t maxtokens, uint8_t tokens)
{
    osal_semaphore_t *retptr = NULL;

    if((0 == maxtokens) || (tokens > maxtokens))
    {
        s_osal_error(osal_error_incorrectparameter, "osal: value of maxtokens or tokens are incorrect in osal_semaphore_new()");
        //return(NULL);
    }
    
    retptr = osiit_sem_getmem();


    if(NULL == retptr)
    {
        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos sem in osal_semaphore_new()");
        //return(NULL);
    }


    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    s_resources_used[osal_info_entity_semaphore] ++;
    osal_mutex_release(s_osal_mutex_api_protection);
    
    // RTOScall-rtx: void osiit_sem_init ( )
    osiit_sem_init(retptr, maxtokens, tokens);

    // ok, all done
    return(retptr);
}

extern osal_result_t osal_semaphore_set(osal_semaphore_t *sem, uint8_t tokens)
{
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }
    
    osiit_sem_set(sem, tokens);

    // ok, all done
    return(osal_res_OK);
}


extern osal_result_t osal_semaphore_decrement(osal_semaphore_t *sem, osal_reltime_t tout)
{
    osal_result_t res = osal_res_NOK_timeout; 
    uint32_t r = OS_R_TMO;
    
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }

    // RTOScall-rtxiit: OS_RESULT osiit_sem_wait(OS_ID semaphore, U32 timeout)
    //                  where return is: OS_R_OK (acquired immediately), OS_R_SEM (acquired with wait), 
    //                  OS_R_TMO (not acquired for timeout)
    r = osiit_sem_wait(sem, s_osal_timeout2tick(tout));
    
    if(OS_R_TMO != r)
    {
        //res = (OS_R_OK == r) ? (osal_res_OK_immediate) : (osal_res_OK_butwaited);
        res = osal_res_OK;
    } 
    
    return(res);
}

extern osal_result_t osal_semaphore_increment(osal_semaphore_t *sem)
{
    U32 r;
    
    if(NULL == sem)
    {
        return(osal_res_NOK_nullpointer); 
    }

    // RTOScall-rtx:    OS_RESULT os_sem_send(OS_ID semaphore) returns: OS_R_OK if it can increment
    r = osiit_sem_send(sem);
  
    return((OS_R_OK == r) ? (osal_res_OK) : (osal_res_NOK_generic));
}


extern osal_timer_t * osal_timer_new(void)
{
    // osiit_advtmr_start returns a OS_ID, which is a void*.
    // as the osal_timer_t type contains only a ptr to the rtos timer then we can use the following:

    void *ret = osiit_advtmr_new();
    
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
    osiit_advtmr_timing_t tim;
    osiit_advtmr_action_t act;
    U32 r;
 
    if((NULL == timer) || (NULL == timing) || (NULL == onexpiry))
    { 
        return(osal_res_NOK_nullpointer);
    }

    tim.startat     = s_osal_abstime2tick(timing->startat);
    tim.firstshot   = s_osal_delay2tick(timing->count);
    tim.othershots  = (osal_tmrmodeONESHOT == timing->mode) ? (0) : (tim.firstshot);
    
    act.cbk         = (void(*)(void*, void*))onexpiry->cbk;
    act.par         = onexpiry->par;

    if(osal_callerTSK == caller)
    {
        r = osiit_advtmr_start(timer, &tim, &act);
    }
    else
    {   // calls the osiit_isr_ also if we have osal_callerTMRMAN (but only if the timer is exec by the scheduler)
        r = osiit_isr_advtmr_start(timer, &tim, &act);
    }


    return((OS_R_NOK == r) ? (osal_res_NOK_generic) : (osal_res_OK));
}


extern osal_result_t osal_timer_stop(osal_timer_t *timer, osal_caller_t caller)
{   
    U32 r;

    if(NULL == timer)
    { 
        return(osal_res_NOK_nullpointer);
    }


    if(osal_callerTSK == caller)
    {
        r = osiit_advtmr_stop(timer);
    }
    else
    {   // calls the osiit_isr_ also if we have osal_callerTMRMAN (but only if the timer is exec by the scheduler)
        r = osiit_isr_advtmr_stop(timer);
    }

    
    return((OS_R_NOK == r) ? (osal_res_NOK_generic) : (osal_res_OK));
}


extern osal_result_t osal_timer_isactive(osal_timer_t *timer, osal_caller_t caller)
{
    U32 r;

    if(NULL == timer)
    { 
        return(osal_res_NOK_nullpointer);
    }

    if(osal_callerTSK == caller)
    {
        r = osiit_advtmr_isactive(timer);
    }
    else
    {   // calls the osiit_isr_ also if we have osal_callerTMRMAN (but only if the timer is exec by the scheduler)
        r = osiit_isr_advtmr_isactive(timer);
    }

    return((OS_R_NOK == r) ? (osal_res_NOK_generic) : (osal_res_OK));
}

extern osal_result_t osal_timer_delete(osal_timer_t *timer)
{
    U32 r;

    if(NULL == timer)
    { 
        return(osal_res_NOK_nullpointer);
    }

    r = osiit_advtmr_delete(timer);


    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
    s_resources_used[osal_info_entity_timer] --;
    osal_mutex_release(s_osal_mutex_api_protection);

    return((OS_R_NOK == r) ? (osal_res_NOK_generic) : (osal_res_OK));
}



//extern osal_timer_t * osal_timer_new(osal_abstime_t startat, osal_reltime_t count, osal_timermode_t tmrmode, void (*fncbk)(void* tmr, void* arg), void* fnarg)
//{
//    // osiit_advtmr_create returns a OS_ID, which is a void*.
//    // if the osal_timer_t type contains only a ptr to the rtos timer then we can use the following:
//
//    void *ret = osiit_advtmr_create(s_osal_abstime2tick(startat), s_osal_delay2tick(count), (osal_tmrmodeONESHOT==tmrmode) ? (0) : (s_osal_delay2tick(count)), fncbk, fnarg);
//    
//    if(NULL == ret)
//    { 
//        s_osal_error(osal_error_missingmemory, "osal: missing mem for rtos timer in osal_timer_new()");
//        //return(NULL);
//    }
//
//    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
//    s_resources_used[osal_info_entity_timer] ++;
//    osal_mutex_release(s_osal_mutex_api_protection);
//
//    return((osal_timer_t*)ret);
//}


//extern osal_result_t osal_timer_delete(osal_timer_t *timer)
//{
//    if(NULL == timer)
//    {
//        return(osal_res_NOK_nullpointer);
//    }
//
//    // RTOScall-rtxiit: OS_ID osiit_advtmr_kill(OS_ID advtimer), where:  OS_ID is void* (see rtl.h),
//    //                  kills the timer and gives back NULL is the timer is succesfully killed.
//    if(NULL != osiit_advtmr_kill(timer))
//    {
//        return(osal_res_NOK_generic);
//    }
//
//    osal_mutex_take(s_osal_mutex_api_protection, OSAL_reltimeINFINITE);
//    s_resources_used[osal_info_entity_timer] --;
//    osal_mutex_release(s_osal_mutex_api_protection);
//
//    return(osal_res_OK);
//}



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
    U32 idx;

    if(1 == firstcall)
    {
        osal_hid_reset_static_ram();
    }


    if(s_osal_info_status < osal_info_status_running)
    {
        // osal not running yet
        return(&__libspace_start);
    }

    // we have osal running, thus also osiit. we get the osiit task id and we give it its memory
    idx = os_tsk_self();

    if(NULL == std_libspace)
    {
        s_osal_error(osal_error_internal2, "armc99stdlib: not enough libspace");
    }

    return ((void *)&std_libspace[idx-1]);
}


// as in http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0475c/Chdfjddj.html ...
// one should use it inside a void _mutex_acquire(void *m) function
extern int osal_arch_arm_armc99stdlib_mutex_initialize(void *m) 
{
    // this function accepts a pointer to a 32-bit word and initializes it as a valid mutex

    #define OSAL_SYSMUTEXCNT_HIDDEN  8
    
    static OSIIT_MUT s_osal_sysmutexes_hidden[OSAL_SYSMUTEXCNT_HIDDEN];
    void *mutex = NULL;

   
    if(s_osal_arch_arm_armc99_sysmutex_number >= OSAL_SYSMUTEXCNT_HIDDEN) 
    {
        // if in here, you need to increase OSIIT_SYSMUTEXCNT_HIDDEN
         s_osal_error(osal_error_internal3, "armc99stdlib: not enough sys mutex");
    }

    // i get a pointer to a mutex
    mutex = (void*) &s_osal_sysmutexes_hidden[s_osal_arch_arm_armc99_sysmutex_number++];
    // i initialise the ram pointed by the mutex. i use rt_mut_init becaus it does not use the svc handler
    rt_iit_no_svc_mut_init(mutex);

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
        OS_ID *mutex = (OS_ID*)m;
        // RTX running, acquire a mutex
        osiit_mut_wait(*mutex, 0xffffffff);
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
        OS_ID *mutex = (OS_ID*)m;
        // RTX runnning, release a mutex. 
        os_mut_release(*mutex);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// used by osiit
__task void os_idle_demon(void) 
{
    s_osal_on_idle();
}


// used only by osiit
void os_error(uint32_t err_code) 
{
    switch(err_code)
    {
        case OSIIT_ERROR_1_rtosstackoverflow:
        {
            s_osal_error(osal_error_stackoverflow, "osal: stack overflow");
        } break;

        case OSIIT_ERROR_2_rtosisrfifooverflow:
        {
             s_osal_error(osal_error_internal0, "osal: known error from rtos");
        } break;
        case OSIIT_ERROR_3_rtosmessageboxoverflow:
        {
            s_osal_error(osal_error_internal1, "osal: known error from rtos");
        } break;
        case OSIIT_ERROR_4_rtosinvalidcall:
        {
            s_osal_error(osal_error_invalidcall, "osal: invalid call");
        } break;

        default:
        {
            s_osal_error(osal_error_unknownfromrtos, "osal: unknown error from rtos");
        } break;
    }
}

// required by osiit for its 16-bit timers
void os_tmr_call(uint16_t info)
{
    info = info;
}


// used only by the shalib as an entrypoint
extern void osal_hid_entrypoint(void)
{
    osal_base_initialise(NULL, NULL);
}


extern void osal_hid_reset_static_ram(void)
{
    // init static ram. it is necessary to do that only if we use a shalib.

    memset(&s_osal_osiit_cfg, 0, sizeof(osiit_params_cfg_t));  
    memset(&s_osal_osal_cfg, 0, sizeof(osal_cfg_t));

    s_osal_mutex_api_protection     = NULL;

    s_osal_info_status                 = osal_info_status_zero;
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

    // values in osiit whcih cannot wait for a call of the osiit init function called by osiit_memory_load()
    // whcih is called by osal_initialise()
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

static void s_osal_on_idle(void)
{
    if(NULL != s_osal_osal_cfg.extfn.usr_on_idle)
    {
        s_osal_osal_cfg.extfn.usr_on_idle();
    }
    else
    {
        for(;;)
        {
            ;
        }
    }
}


static void s_osal_error(osal_fatalerror_t err_code, const char *err_msg) 
{
    uint32_t tid = 0;
    osal_task_t *task = NULL;

    osal_system_scheduling_suspend();
    
    if((osal_info_status_running == s_osal_info_status) || (osal_info_status_suspended == s_osal_info_status))
    { 
        tid = isr_tsk_get(); // the os-version crashes if osiit calls os_error().... os_tsk_self();
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


static void s_osal_fill_cfg(osiit_params_cfg_t *osiit_c, const osal_cfg_t *osal_c)
{
    osiit_c->numTask                        = osal_c->tasknum;
    osiit_c->numTaskWithUserProvidedStack   = osal_c->tasknum;
    osiit_c->sizeStack                      = osal_c->idlestacksize / 8; // or tell to use .... 128B / 8
    osiit_c->checkStack                     = 1;   // always check stack overflow w/ osiit
    osiit_c->priviledgeMode                 = 1; //osal_c->privmode;
    osiit_c->osClock                        = osal_c->cpufreq;
    osiit_c->osTick                         = osal_c->tick;
    osiit_c->roundRobin                     = osal_c->roundrobin;
    osiit_c->roundRobinTimeout              = osal_c->roundrobintick;
    osiit_c->numTimer                       = 0;  // the ugly rtx timer
    osiit_c->sizeISRFIFO                    = 16; // ???? the enqueable requests done to rtos within an ISR
    osiit_c->numAdvTimer                    = osal_c->timernum;
    osiit_c->numMutex                       = osal_c->mutexnum + 1;  // +1 for internal sys mutex s_osal_mutex_api_protection
    osiit_c->numSemaphore                   = osal_c->semaphorenum;
    osiit_c->numMessageBox                  = osal_c->mqueuenum;
    osiit_c->numMessageBoxElements          = osal_c->mqueueelemnum;
    osiit_c->numElements64Stack             = osal_c->globalstacksize / 8; 
}

void osal_launcher(void)
{
    const int32_t s_SVCall_IRQn  = -5;
    //const int32_t s_PendSV_IRQn  = -2;
    //const int32_t s_SysTick_IRQn = -1;

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
    os_tsk_delete_self();
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
        return(ASAPTIME);
    }

    return(time / s_osal_usec_in_tick);
}

static uint32_t s_osal_timeout2tick(osal_reltime_t tout)
{
    if(osal_reltimeINFINITE == tout)
    {     // first, because it is likely to be the most probable
        return(NOTIMEOUT); 
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




