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

/* @file       oosiit.c
	@brief      This file keeps hidden implementation of the oosiit. 
	@author     marco.accame@iit.it
    @date       07/23/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#if defined(OOSIIT_DBG_ENABLE)
#include "api/eventviewer.h"
#endif

#include "oosiit_svc.h"


#include "rt_iit_changes.h"
#include "rt_iit_AdvTimer.h"
#include "rt_iit_memory.h"

#include "rtx_config.h"

#include "rt_TypeDef.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "oosiit.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "oosiit_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// used to avoid problems with the SVC declarations
#define FAKE_VOID_FP_VOIDP  void*
#define FAKE_VOID_FP_VOID   void*


#define IFN0(a, b)                  ((a)?(b):(0))
// used by: advanced timers in ram32size[0], mp_tcb in ram32size[5], normal timers m_tmr in ram32size[8]
#define SIZEINU32ofBOX4AL(sizeinbytes, cnt)           ((((sizeinbytes)+3)/4)*(cnt) + 3)
// used by: stack not externally provided in ram64size[1]
#define SIZEINU64ofBOX8AL(sizeinbytes, cnt)           ((((sizeinbytes)+7)/8)*(cnt) + 2)

//#define BX4(s, c)                   ((((s)+3)/4)*(c) + 3)
//#define BX8(s, c)                   ((((s)+7)/8)*(c) + 2)
//#define MAT(a, b)                   ((a)+(a)*(b))


//#define P4ALIGN(s, n)               ((((p)+3)/4)*(c) + 3)


#define SIZEOF_STDLIB32_ELEM            (24*4)
#define SIZEOF_ARRAY_HEAD               (sizeof(oosiit_arrayhead_t)-4)
#define SIZEOF_MUTEX_ELEM               (sizeof(struct OS_MUCB))
#define SIZEOF_SEMAPHORE_ELEM           (sizeof(struct OS_SCB))
#define SIZEOF_BASEMBOX_ELEM            (sizeof(struct OS_MCB)-4)
#define SIZEOF_POSTSERV_ELEM            (sizeof(struct OS_PSFE))
#define SIZEOF_POSTSERV_HEAD            (sizeof(struct OS_PSQ)-sizeof(struct OS_PSFE))
#define SIZEOF_TCBPOINTER               (4)
#define SIZEOF_ADVTIMER_ELEM            (sizeof(struct OSIIT_ADVTMR))
#define SIZEOF_TCB_ELEM                 (sizeof(struct OS_TCB))
#define SIZEOF_TMR_ELEM                 (sizeof(struct OS_TMR))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const uint32_t oosiit_notimeout = OOSIIT_NOTIMEOUT;
extern const uint64_t oosiit_asaptime = OOSIIT_ASAPTIME;

extern const oosiit_cfg_t *oosiit_cfg_in_use = NULL;

// it must point to an array of proper size, given by ...
extern       uint32_t *oosiit_params_ram32data = NULL;

// it must point to an array of proper size, given by ...
extern       uint64_t *oosiit_params_ram64data = NULL;

// it may be required to be initted at compile time if results to be called before main().
extern       uint32_t *oosiit_params_stdlib32data = NULL;


extern volatile uint64_t oosiit_time = 0;
//extern volatile uint64_t oosiit_idletime = 0;
extern uint32_t oosiit_ns_per_unit_of_systick = 0;
extern uint32_t oosiit_num_units_of_systick = 0;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_oosiit_params_get_ram32size(const oosiit_cfg_t *cfg);
static uint16_t s_oosiit_params_get_ram64size(const oosiit_cfg_t *cfg);


static __INLINE oosiit_taskid_t isr_oosiit_tsk_self(void);
static __INLINE oosiit_result_t isr_oosiit_mbx_retrieve(oosiit_objptr_t mailbox, void** message);
static __INLINE void isr_oosiit_mbx_send(oosiit_objptr_t mailbox, void* message);
static __INLINE uint16_t isr_oosiit_mbx_available(oosiit_objptr_t mailbox);
static __INLINE uint16_t isr_oosiit_mbx_used(oosiit_objptr_t mailbox);
static __INLINE oosiit_result_t isr_oosiit_evt_set(uint32_t flags, oosiit_taskid_t tskid);
static __INLINE oosiit_result_t isr_oosiit_sem_send(oosiit_objptr_t sem);
static __INLINE oosiit_result_t isr_oosiit_advtmr_start(oosiit_objptr_t timer, oosiit_advtmr_timing_t *timing, oosiit_advtmr_action_t *action);
static __INLINE oosiit_result_t isr_oosiit_advtmr_stop(oosiit_objptr_t timer);
static __INLINE oosiit_result_t isr_oosiit_advtmr_delete(oosiit_objptr_t timer);


// --------------------------------------------------------------------------------------------------------------------
// - declaration of svc functions
// --------------------------------------------------------------------------------------------------------------------
//      fname                           rettype             arg1type            arg2type                arg3type        arg4type        return register   

// - system
//SVC_4_1(svc_oosiit_sys_init_user,       int32_t,            void (*)(void),      uint8_t,                void*,          uint16_t,       RET_int32_t);
SVC_2_1(svc_oosiit_sys_start,           uint32_t,    oosiit_task_properties_t*, oosiit_task_properties_t*,                       RET_int32_t);
//SVC_2_0(svc_oosiit_sys_start,           void,    oosiit_task_properties_t*, oosiit_task_properties_t*,                       RET_int32_t);
SVC_4_1(svc_oosiit_sys_init_user,       oosiit_result_t,    FAKE_VOID_FP_VOID,  uint8_t,                void*,          uint16_t,       RET_int32_t);
SVC_0_1(svc_oosiit_sys_suspend,         oosiit_result_t,                                                                                RET_int32_t);
SVC_0_1(svc_oosiit_sys_resume,          oosiit_result_t,                                                                                RET_int32_t);

// - miscellanea
SVC_1_1(svc_oosiit_stack_getmem,        uint64_t*,          uint16_t,                                                                   RET_pointer);

// - task
SVC_4_1(svc_oosiit_tsk_create,          oosiit_taskid_t,    FAKE_VOID_FP_VOIDP, void*,                  void*,          uint32_t,       RET_uint32_t);
SVC_1_1(svc_oosiit_tsk_delete,          oosiit_result_t,    oosiit_taskid_t,                                                            RET_pointer);
SVC_0_1(svc_oosiit_tsk_self,            uint32_t,                                                                                       RET_uint32_t);
SVC_2_1(svc_oosiit_tsk_setprio,         oosiit_result_t,    oosiit_taskid_t,    uint8_t,                                                RET_int32_t);
SVC_0_1(svc_oosiit_tsk_pass,            oosiit_result_t,                                                                                RET_int32_t);


// - time
SVC_2_1(svc_oosiit_time_set,            oosiit_result_t,    uint32_t,           uint32_t,                                               RET_int32_t);

// delay
SVC_1_1(svc_oosiit_dly_wait,            oosiit_result_t,    uint32_t,                                                                   RET_int32_t);
SVC_1_1(svc_oosiit_itv_set,             oosiit_result_t,    uint32_t,                                                                   RET_int32_t);
SVC_0_1(svc_oosiit_itv_wait,            oosiit_result_t,                                                                                RET_int32_t);


// mailbox
SVC_1_1(svc_oosiit_mbx_create,          void*,              uint16_t,                                                                   RET_pointer);
SVC_3_1(svc_oosiit_mbx_retrieve,        oosiit_result_t,    void*,              void**,                 uint32_t,                       RET_int32_t);
SVC_3_1(svc_oosiit_mbx_send,            oosiit_result_t,    void*,              void*,                  uint32_t,                       RET_int32_t);
SVC_1_1(svc_oosiit_mbx_available,       uint16_t,           void*,                                                                      RET_uint32_t);
SVC_1_1(svc_oosiit_mbx_used,            uint16_t,           void*,                                                                      RET_uint32_t);


// event flags 
SVC_2_1(svc_oosiit_evt_set,             oosiit_result_t,    uint32_t,           oosiit_taskid_t,                                        RET_int32_t);
SVC_3_1(svc_oosiit_evt_wait,            oosiit_result_t,    uint32_t,           uint32_t,               oosiit_evt_wait_mode_t,         RET_int32_t);
SVC_0_1(svc_oosiit_evt_get,             uint32_t,                                                                                       RET_uint32_t);
SVC_1_1(svc_oosiit_evt_clr,             oosiit_result_t,    uint32_t,                                                                   RET_int32_t);

// semaphore
SVC_2_1(svc_oosiit_sem_create,          void*,              uint8_t,            uint8_t,                                                RET_pointer);
SVC_2_1(svc_oosiit_sem_set,             oosiit_result_t,    void*,              uint8_t,                                                RET_int32_t);
SVC_1_1(svc_oosiit_sem_send,            oosiit_result_t,    void*,                                                                      RET_int32_t);
SVC_2_1(svc_oosiit_sem_wait,            oosiit_result_t,    void*,              uint32_t,                                               RET_int32_t);

// mutex
SVC_0_1(svc_oosiit_mut_create,          void*,                                                                                          RET_pointer);
SVC_2_1(svc_oosiit_mut_wait,            oosiit_result_t,    void*,              uint32_t,                                               RET_int32_t);
SVC_1_1(svc_oosiit_mut_release,         oosiit_result_t,    void*,                                                                      RET_int32_t);

// advanced timer
SVC_0_1(svc_oosiit_advtmr_new,          void*,                                                                                          RET_pointer);
SVC_3_1(svc_oosiit_advtmr_start,        oosiit_result_t,    void*,              oosiit_advtmr_timing_t*,oosiit_advtmr_action_t*,        RET_int32_t);
SVC_1_1(svc_oosiit_advtmr_stop,         oosiit_result_t,    void*,                                                                      RET_int32_t);
SVC_1_1(svc_oosiit_advtmr_delete,       oosiit_result_t,    void*,                                                                      RET_int32_t);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint8_t s_oosiit_started = 0;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions whuch dont have a .h file
// --------------------------------------------------------------------------------------------------------------------

extern void os_set_env(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern oosiit_result_t oosiit_memory_getsize(const oosiit_cfg_t *cfg, uint16_t *size04aligned, uint16_t *size08aligned)
{
    if(NULL == cfg)
    {
        *size04aligned = *size08aligned = 0;
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    }     
    
    *size04aligned = 4*s_oosiit_params_get_ram32size(cfg);
    *size08aligned = 8*s_oosiit_params_get_ram64size(cfg);
    
    return(oosiit_res_OK);
}

extern oosiit_result_t oosiit_memory_load(const oosiit_cfg_t *cfg, uint32_t *data04aligned, uint64_t *data08aligned)
{
    if((NULL == cfg) || (NULL == data04aligned) || (NULL == data08aligned) )
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    
    if(1 == s_oosiit_started)
    {   // already started
        return(oosiit_res_NOK);
    }
    
    oosiit_cfg_in_use            = cfg;
    oosiit_params_stdlib32data   = data04aligned;
    oosiit_params_ram32data      = data04aligned + ((SIZEOF_STDLIB32_ELEM/4)*cfg->maxnumofusertasks);   
    oosiit_params_ram64data      = data08aligned;

    // need to initialise os_flags now because os_sys_init* calls os_set_env() which use it before rt_sys_init() is called
    os_flags      = 1; //oosiit_cfg_in_use->priviledgeMode;
    
    // set the environment to privileged / unprivileged
    os_set_env();
    
    // initialise the parameters and the memory straigth away.
    rt_iit_params_init();             
    rt_iit_memory_init();             

    return(oosiit_res_OK);
}


// - system functions -------------------------------------------------------------------------------------------------


extern oosiit_result_t oosiit_sys_start(oosiit_task_properties_t* tskinit, oosiit_task_properties_t* tskidle)
{
    if(1 == s_oosiit_started)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else if((NULL == tskinit) || (NULL == tskidle) )
    {
        return(oosiit_res_NOK);
    }
    else if((NULL == tskinit->function) || (0 == tskinit->stacksize) || (NULL == tskinit->stackdata) ||
            (NULL == tskidle->function) || (0 == tskidle->stacksize) || (NULL == tskidle->stackdata) )
    { 
        return(oosiit_res_NOK);
    }
    else
    {
        // set the environment to privileged / unprivileged
        os_set_env();
        // set the os as started
        s_oosiit_started = 1;
        // call svc
        __svc_oosiit_sys_start(tskinit, tskidle);
    }
    
    // but in here it nevor goes because the __svc_oosiit_sys_start() never returns
    return(oosiit_res_OK);    
}

extern oosiit_result_t oosiit_sys_init(void (*inittskfn)(void), uint8_t inittskpriority, void* inittskstackdata, uint16_t inittskstacksize)
{
    if(1 == s_oosiit_started)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else if((NULL == inittskfn) || (NULL == inittskstackdata) || (0 == inittskstacksize) )
    {
        return(oosiit_res_NOK);
    }
    else
    { 
        // set the environment to privileged / unprivileged
        os_set_env();
        // set the os as started
        s_oosiit_started = 1;        
        // call svc
        __svc_oosiit_sys_init_user(inittskfn, inittskpriority, inittskstackdata, inittskstacksize);
    }
    
    // but in here it nevor goes because the __svc_oosiit_sys_init_user() never returns
    return(oosiit_res_OK);
}


extern oosiit_result_t oosiit_sys_suspend(void)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
        __svc_oosiit_sys_suspend();
    }
    
    return(oosiit_res_OK);
}


extern oosiit_result_t oosiit_sys_resume(void)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
        __svc_oosiit_sys_resume();
    }
    
    return(oosiit_res_OK);
}

// - miscellanea ------------------------------------------------------------------------------------------------------

extern uint64_t* oosiit_memory_getstack(uint16_t bytes)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(NULL);
    } 
    else if(1 == s_oosiit_started)
    {   // call svc
        return(__svc_oosiit_stack_getmem(bytes));
    }
    else
    {   // dont call svc
        return(rt_iit_memory_getstack(bytes));
    }
    
}


// - task functions ---------------------------------------------------------------------------------------------------

extern oosiit_taskid_t oosiit_tsk_create_oldversion(void (*tskfn)(void *), void *tskfnarg, uint8_t tskpriority, void *tskstackdata, uint16_t tskstacksize)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return((oosiit_taskid_t)0);
    } 
    else if((NULL == tskfn) || (NULL == tskstackdata) || (0 == tskstacksize) )
    {
        return((oosiit_taskid_t)0);
    }
    else
    {   // call svc
        uint32_t tskstacksize24tskpriority08 = ((((uint32_t)(tskstacksize)) << 8)&0xffffff00) | (tskpriority&0xff);
        return(__svc_oosiit_tsk_create(tskfn, tskfnarg, tskstackdata, tskstacksize24tskpriority08));
    }   
}

extern oosiit_taskid_t oosiit_tsk_create(oosiit_task_properties_t* tskprop)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return((oosiit_taskid_t)0);
    } 
    else if((NULL == tskprop) || (NULL == tskprop->function) || (NULL == tskprop->stackdata) || (0 == tskprop->stacksize) )
    {
        return((oosiit_taskid_t)0);
    }
    else
    {   // call svc
        uint32_t tskstacksize24tskpriority08 = ((((uint32_t)(tskprop->stacksize)) << 8)&0xffffff00) | (tskprop->priority&0xff);
        return(__svc_oosiit_tsk_create(tskprop->function, tskprop->param, tskprop->stackdata, tskstacksize24tskpriority08));
    }   
}


extern oosiit_result_t oosiit_tsk_delete(oosiit_taskid_t tskid)
{
    if(0 == tskid)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
        return(__svc_oosiit_tsk_delete(tskid));
    } 
}

extern oosiit_taskid_t oosiit_tsk_self(void)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_tsk_self());
    } 
    else
    {   // call svc
        return(__svc_oosiit_tsk_self());
    } 
}

extern oosiit_result_t oosiit_tsk_setprio(oosiit_taskid_t tskid, uint8_t tskpriority)
{
    if(0 == tskid)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        //return(isr_oosiit_tsk_prio(tskid, tskpriority));
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
        return(__svc_oosiit_tsk_setprio(tskid, tskpriority));
    }   
}

extern oosiit_result_t oosiit_tsk_pass(void)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        //return(isr_oosiit_tsk_pass());
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
        return(__svc_oosiit_tsk_pass());
    }   
}

// - time management functions ----------------------------------------------------------------------------------------

extern uint64_t oosiit_time_get(void)
{
    return(oosiit_time);
}


extern oosiit_result_t oosiit_time_set(uint64_t target)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        //return(isr_oosiit_tsk_pass());
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
        return(__svc_oosiit_time_set(target&0xffffffff, (target>>32)&0xffffffff));
    }   
}


extern uint64_t oosiit_microtime_get(void)
{
    uint64_t microsecs;
    uint64_t nanosecs;
    volatile uint32_t reg0xE000E018 = *((volatile uint32_t *)0xE000E018);
    
    // add to microsecs the content of register systick_current_value_reg properly scaled.
    // if it is equal to systick_reload_value_reg, then extranano = 0;
    // if it is equal to 1, then extranano = ... (osTick*1000)/(systick_reload_value_reg+1)

    nanosecs = oosiit_ns_per_unit_of_systick * (oosiit_num_units_of_systick - reg0xE000E018);
    microsecs = (oosiit_time * oosiit_cfg_in_use->osTick) + nanosecs/1000LL;

    return(microsecs);    
}

extern uint64_t oosiit_nanotime_get(void)
{
    uint64_t nanosecs;// = osiit_time * osiit_params_cfg->osTick * 1000;
    volatile uint32_t reg0xE000E018 = *((volatile uint32_t *)0xE000E018);

    // add to nanosecs the content of register systick_current_value_reg properly scaled.
    // if it is equal to systick_reload_value_reg, then extranano = 0;
    // if it is equal to 1, then extranano = ... (osTick*1000)/(systick_reload_value_reg+1)

    nanosecs = oosiit_ns_per_unit_of_systick * (oosiit_num_units_of_systick - reg0xE000E018);
    nanosecs += (oosiit_time * oosiit_cfg_in_use->osTick * 1000LL);

    return(nanosecs);
}

#if 0
extern oosiit_result_t oosiit_secsnano_get(uint32_t *secs, uint32_t *nano)
{
    U64 nanosecs;// = osiit_time * osiit_params_cfg->osTick * 1000;

    // add to nanosecs the content of register systick_current_value_reg properly scaled.
    // if it is equal to systick_reload_value_reg, then extranano = 0;
    // if it is equal to 1, then extranano = ... (osTick*1000)/(systick_reload_value_reg+1)

    nanosecs = (os_iit_ns_per_unit_of_systick*(os_iit_num_units_of_systick-(*((volatile U32 *)0xE000E018))));
    nanosecs += (osiit_time * osiit_params_cfg->osTick * 1000);

    if(NULL != secs)
    {
        *secs = nanosecs / 1000000000;
    }
    if(NULL != nano)
    {
        *nano = nanosecs % 1000000000;
    }
}
#endif


// - delay functions --------------------------------------------------------------------------------------------------

extern void oosiit_dly_wait(uint32_t delay)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return;
    } 
    else
    {   // call svc
        __svc_oosiit_dly_wait(delay);
    }
}

extern void oosiit_itv_set(uint32_t period)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return;
    } 
    else
    {   // call svc
        __svc_oosiit_itv_set(period);
    }
}

extern void oosiit_itv_wait(void)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return;
    } 
    else
    {   // call svc
        __svc_oosiit_itv_wait();
    }
}


// - mailbox functions ------------------------------------------------------------------------------------------------

extern oosiit_objptr_t oosiit_mbx_create(uint16_t numofmessages)
{   // get memory and initialises it
    
    if(0 == numofmessages)
    {
        return(NULL);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(NULL);
    } 
    else
    {   // call svc
         return(__svc_oosiit_mbx_create(numofmessages));
    }
}

extern oosiit_result_t oosiit_mbx_retrieve(oosiit_objptr_t mailbox, void** message, uint32_t timeout)
{
    if((NULL == mailbox) || (NULL == message))
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_mbx_retrieve(mailbox, message));
    } 
    else
    {   // call svc
         return(__svc_oosiit_mbx_retrieve(mailbox, message, timeout));
    }
}

extern oosiit_result_t oosiit_mbx_send(oosiit_objptr_t mailbox, void* message, uint32_t timeout)
{
    if((NULL == mailbox) || (NULL == message))
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        isr_oosiit_mbx_send(mailbox, message);
        return(oosiit_res_OK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_mbx_send(mailbox, message, timeout));
    }
}

extern uint16_t oosiit_mbx_available(oosiit_objptr_t mailbox)
{
    if(NULL == mailbox)
    {
        return(0);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_mbx_available(mailbox));
    } 
    else
    {   // call svc
         return(__svc_oosiit_mbx_available(mailbox));
    }
}

extern uint16_t oosiit_mbx_used(oosiit_objptr_t mailbox)
{
    if(NULL == mailbox)
    {
        return(0);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_mbx_used(mailbox));
    } 
    else
    {   // call svc
         return(__svc_oosiit_mbx_used(mailbox));
    }
}


// - event flag functions ---------------------------------------------------------------------------------------------


extern oosiit_result_t oosiit_evt_set(uint32_t flags, oosiit_taskid_t tskid)
{
    if(0 == tskid)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_evt_set(flags, tskid));
    } 
    else
    {   // call svc
        return(__svc_oosiit_evt_set(flags, tskid));
    }
}

extern oosiit_result_t oosiit_evt_wait(uint32_t waitflags,  uint32_t timeout, oosiit_evt_wait_mode_t waitmode)
{   
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_evt_wait(waitflags, timeout, waitmode));
    }
}

extern uint32_t oosiit_evt_get(void)
{   
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(0);
    } 
    else
    {   // call svc
         return(__svc_oosiit_evt_get());
    }
}

extern oosiit_result_t oosiit_evt_clr(uint32_t flags)
{
    if(0 == flags)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_evt_clr(flags));
    }
}


// - semaphore functions ----------------------------------------------------------------------------------------------

extern oosiit_objptr_t oosiit_sem_create(uint8_t maxtokens, uint8_t ntokens)
{   // get memory and initialises it
    
    if(0 == maxtokens)
    {
        return(NULL);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(NULL);
    } 
    else
    {   // call svc
         return(__svc_oosiit_sem_create(maxtokens, ntokens));
    }
}

extern oosiit_result_t oosiit_sem_set(oosiit_objptr_t sem, uint8_t ntokens)
{
    if(NULL == sem)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_sem_set(sem, ntokens));
    }
}

extern oosiit_result_t oosiit_sem_send(oosiit_objptr_t sem)
{
    if(NULL == sem)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_sem_send(sem));
    } 
    else
    {   // call svc
         return(__svc_oosiit_sem_send(sem));
    }
}

extern oosiit_result_t oosiit_sem_wait(oosiit_objptr_t sem, uint32_t timeout)
{
    if(NULL == sem)
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_sem_wait(sem, timeout));
    }
}


// - mutex functions --------------------------------------------------------------------------------------------------

extern oosiit_objptr_t oosiit_mut_create(void)
{   // get memory and initialises it
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(NULL);
    } 
    else
    {   // call svc
         return(__svc_oosiit_mut_create());
    }
}


extern oosiit_result_t oosiit_mut_wait(oosiit_objptr_t mutex, uint32_t timeout)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_mut_wait(mutex, timeout));
    }
}


extern oosiit_result_t oosiit_mut_release(oosiit_objptr_t mutex)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(oosiit_res_NOK);
    } 
    else
    {   // call svc
         return(__svc_oosiit_mut_release(mutex));
    }
}


// - advanced timer functions -----------------------------------------------------------------------------------------


extern oosiit_objptr_t oosiit_advtmr_new(void)
{
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(NULL);
    } 
    else
    {   // call svc
         return(__svc_oosiit_advtmr_new());
    }
}

extern oosiit_result_t oosiit_advtmr_start(oosiit_objptr_t timer, oosiit_advtmr_timing_t *timing, oosiit_advtmr_action_t *action)
{
    if((NULL == timer) || (NULL == timing) || (NULL == action))
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_advtmr_start(timer, timing, action));
    } 
    else
    {   // call svc
         return(__svc_oosiit_advtmr_start(timer, timing, action));
    }
}

extern oosiit_result_t oosiit_advtmr_stop(oosiit_objptr_t timer)
{
    if(NULL == timer) 
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_advtmr_stop(timer));
    } 
    else
    {   // call svc
         return(__svc_oosiit_advtmr_stop(timer));
    }
}

extern oosiit_result_t oosiit_advtmr_isactive(oosiit_objptr_t timer)
{
    if(NULL == timer) 
    {
        return(oosiit_res_NOK);
    }
    
    return((OS_R_OK == rt_iit_advtmr_isactive(timer)) ? (oosiit_res_OK) : (oosiit_res_NOK));
    
//    if(0 != __get_IPSR()) 
//    {   // inside isr
//        return(isr_oosiit_advtmr_isactive(timer));
//    } 
//    else
//    {   // call svc
//         return(__svc_oosiit_advtmr_isactive(timer));
//    }
}

extern oosiit_result_t oosiit_advtmr_delete(oosiit_objptr_t timer)
{
    if(NULL == timer) 
    {
        return(oosiit_res_NOK);
    }
    
    if(0 != __get_IPSR()) 
    {   // inside isr
        return(isr_oosiit_advtmr_delete(timer));
    } 
    else
    {   // call svc
         return(__svc_oosiit_advtmr_delete(timer));
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t oosiit_hid_params_get_stdlib32(const oosiit_cfg_t *cfg, uint16_t *stdlib32size)
{
    uint16_t stdlib32datasizetot = 0;

    if(NULL == cfg)
    {
        return(0);
    }

    stdlib32size[0] = (SIZEOF_STDLIB32_ELEM/4)*cfg->maxnumofusertasks;
    stdlib32datasizetot += stdlib32size[0];

    return(stdlib32datasizetot);

}

extern uint16_t oosiit_hid_params_get_ram32(const oosiit_cfg_t *cfg, uint16_t *ram32size)
{
    uint16_t ram32datasizetot = 0;

    if(NULL == cfg)
    {
        return(0);
    }
    // the following use membox 4-aligned: adv-timers [0], task-control-block [5], timers [8]
    ram32size[0] = (SIZEOF_ARRAY_HEAD/4 + cfg->numAdvTimer) + IFN0(cfg->numAdvTimer, SIZEINU32ofBOX4AL(SIZEOF_ADVTIMER_ELEM, cfg->numAdvTimer));  
    ram32datasizetot += ram32size[0];
    ram32size[1] = (SIZEOF_MUTEX_ELEM/4)*cfg->numMutex;   
    ram32datasizetot += ram32size[1];
    ram32size[2] = (SIZEOF_SEMAPHORE_ELEM/4)*cfg->numSemaphore;       
    ram32datasizetot += ram32size[2];
    ram32size[3] = IFN0(cfg->numMessageBox*cfg->numMessageBoxElements, (SIZEOF_BASEMBOX_ELEM/4)*cfg->numMessageBox+cfg->numMessageBoxElements); 
    ram32datasizetot += ram32size[3];
    ram32size[4] = IFN0(cfg->numMutex+cfg->numSemaphore+cfg->numMessageBox+cfg->sizeof64alignedStack, (SIZEOF_MUTEX_ELEM/4));    
    ram32datasizetot += ram32size[4];
    ram32size[5] = IFN0(cfg->maxnumofusertasks, SIZEINU32ofBOX4AL(SIZEOF_TCB_ELEM, cfg->maxnumofusertasks)); 
    ram32datasizetot += ram32size[5];
    ram32size[6] = cfg->sizeISRFIFO*(SIZEOF_POSTSERV_ELEM/4) + (SIZEOF_POSTSERV_HEAD/4);      
    ram32datasizetot += ram32size[6];
    ram32size[7] = (SIZEOF_TCBPOINTER/4)*cfg->maxnumofusertasks;
    ram32datasizetot += ram32size[7];
    //ram32size[8] = IFN0(cfg->numTimer, SIZEINU32ofBOX4AL(SIZEOF_TMR_ELEM, cfg->numTimer));
    ram32size[8] = IFN0(0, SIZEINU32ofBOX4AL(SIZEOF_TMR_ELEM, 0));
    ram32datasizetot += ram32size[8];

    return(ram32datasizetot);
}

extern uint16_t oosiit_hid_params_get_ram64(const oosiit_cfg_t *cfg, uint16_t *ram64size)
{
    uint16_t ram64datasizetot = 0;

    if(NULL == cfg)
    {
        return(0);
    }

    // the following use membox 8-aligned: stack not externally provided [1]
    ram64size[0] = (cfg->sizeof64alignedStack+7)/8;    
    ram64datasizetot += ram64size[0];
    //ram64size[1] = SIZEINU64ofBOX8AL(4*cfg->sizeStack, cfg->maxnumofusertasks-cfg->numTaskWithUserProvidedStack+1);
    ram64size[1] = 0;
    ram64datasizetot += ram64size[1];

    return(ram64datasizetot);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static uint16_t s_oosiit_params_get_ram32size(const oosiit_cfg_t *cfg)
{
    uint16_t ram32datasizetot = 0;

    if(NULL == cfg)
    {
        return(0);
    }

    // for oosiit_params_stdlib32data
    ram32datasizetot += ((SIZEOF_STDLIB32_ELEM/4)*cfg->maxnumofusertasks);    

    // the following use membox 4-aligned: adv-timers [0], task-control-block [5], timers [8]
    // for oosiit_params_ram32data
    // 0. advanced timers
    ram32datasizetot += ((SIZEOF_ARRAY_HEAD/4 + cfg->numAdvTimer) + IFN0(cfg->numAdvTimer, SIZEINU32ofBOX4AL(SIZEOF_ADVTIMER_ELEM, cfg->numAdvTimer)));
     // 1. mutex
    ram32datasizetot += (SIZEOF_MUTEX_ELEM/4)*cfg->numMutex; 
    // 2. semaphore           
    ram32datasizetot += (SIZEOF_SEMAPHORE_ELEM/4)*cfg->numSemaphore;
    // 3. mbox        
    ram32datasizetot += IFN0(cfg->numMessageBox*cfg->numMessageBoxElements, (SIZEOF_BASEMBOX_ELEM/4)*cfg->numMessageBox+cfg->numMessageBoxElements); 
    // 4. mutex for ram
    ram32datasizetot += IFN0(cfg->numMutex+cfg->numSemaphore+cfg->numMessageBox+cfg->sizeof64alignedStack, (SIZEOF_MUTEX_ELEM/4));  
    // 5. acemor-fix-it
    ram32datasizetot += IFN0(cfg->maxnumofusertasks, SIZEINU32ofBOX4AL(SIZEOF_TCB_ELEM, cfg->maxnumofusertasks));
    // 6. isr post service
    ram32datasizetot += cfg->sizeISRFIFO*(SIZEOF_POSTSERV_ELEM/4) + (SIZEOF_POSTSERV_HEAD/4);  
    // 7. pointers to task control blocks      
    ram32datasizetot += (SIZEOF_TCBPOINTER/4)*cfg->maxnumofusertasks; 
    // 8. timers  
    //ram32datasizetot += IFN0(cfg->numTimer, SIZEINU32ofBOX4AL(SIZEOF_TMR_ELEM, cfg->numTimer));
    ram32datasizetot += IFN0(0, SIZEINU32ofBOX4AL(SIZEOF_TMR_ELEM, 0));

    
    return(ram32datasizetot);
}

static uint16_t s_oosiit_params_get_ram64size(const oosiit_cfg_t *cfg)
{
    uint16_t ram64datasizetot = 0;

    if(NULL == cfg)
    {
        return(0);
    }

    // the following use membox 8-aligned: stack not externally provided [1]
    ram64datasizetot += (cfg->sizeof64alignedStack+7)/8; 
    //ram64datasizetot += SIZEINU64ofBOX8AL(4*cfg->sizeStack, cfg->maxnumofusertasks-cfg->numTaskWithUserProvidedStack+1);
    ram64datasizetot += 0;
    return(ram64datasizetot);
}


// ------------------------------------------- svc functions -----------------------------------------------------------

// - system 


extern uint32_t svc_oosiit_sys_start(oosiit_task_properties_t* inittsk, oosiit_task_properties_t* idletsk)
{ 
    rt_iit_dbg_svc_enter();
     
    rt_iit_sys_start(inittsk, idletsk);
    
    rt_iit_dbg_svc_exit();
    
    // trick to have in teh stack of idle_task the param in the position of r0.
    // follow with debugger hal_cm3.c after return of the function to discover why.
    return((uint32_t)idletsk->param);
}


extern oosiit_result_t svc_oosiit_sys_init_user(FAKE_VOID_FP_VOID inittskfn, uint8_t inittskpriority, void* inittskstackdata, uint16_t inittskstacksize)
{   // void (*inittskfn)(void)
    uint32_t stacksize24priority08 = ((((uint32_t)(inittskstacksize)) << 8)&0xffffff00) | (inittskpriority&0xff);
    
    rt_iit_dbg_svc_enter();
     
    rt_sys_init((FUNCP)inittskfn, stacksize24priority08, inittskstackdata);
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}

extern oosiit_result_t svc_oosiit_sys_suspend(void)
{
    rt_iit_dbg_svc_enter();
    
    rt_tsk_lock();
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}

extern oosiit_result_t svc_oosiit_sys_resume(void)
{
    rt_iit_dbg_svc_enter();
    
    rt_tsk_unlock();
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}

// - miscellanea

extern uint64_t* svc_oosiit_stack_getmem(uint16_t bytes)
{
    uint64_t *ret;
    rt_iit_dbg_svc_enter();
    
    ret = rt_iit_memory_getstack(bytes);
    
    rt_iit_dbg_svc_exit();
    return(ret);
}

// - task

extern oosiit_taskid_t svc_oosiit_tsk_create(FAKE_VOID_FP_VOIDP tskfn, void *tskfnarg, void *tskstackdata, uint32_t tskstacksize24tskpriority08)
{   // rt_tsk_create_user_ex has tskfnarg as last argument
    // void (*tskfn)(void *)
    oosiit_taskid_t tid;
    rt_iit_dbg_svc_enter();
    
    tid = rt_tsk_create((FUNCP)tskfn, tskstacksize24tskpriority08, tskstackdata, tskfnarg);
    
    rt_iit_dbg_svc_exit();
    return(tid);
}

extern oosiit_result_t svc_oosiit_tsk_delete(oosiit_taskid_t tskid)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
    
    res = rt_tsk_delete(tskid);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);
}

extern oosiit_taskid_t svc_oosiit_tsk_self(void)
{
    oosiit_taskid_t tid;
    rt_iit_dbg_svc_enter();
    
    tid = rt_tsk_self();
    
    rt_iit_dbg_svc_exit();
    return(tid);
}

extern oosiit_result_t svc_oosiit_tsk_setprio(oosiit_taskid_t tskid, uint8_t tskpriority)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
    
    res = rt_tsk_prio(tskid, tskpriority);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);
}

extern oosiit_result_t svc_oosiit_tsk_pass(void)
{
    rt_iit_dbg_svc_enter();
    
    rt_tsk_pass();
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}


// time

extern oosiit_result_t svc_oosiit_time_set(uint32_t low, uint32_t high)
{
    rt_iit_dbg_svc_enter();
    
    rt_iit_time_set(low, high);
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}


// delay

extern oosiit_result_t svc_oosiit_dly_wait(uint32_t delay)
{
    rt_iit_dbg_svc_enter();
    
    iitchanged_rt_dly_wait(delay);
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}

extern oosiit_result_t svc_oosiit_itv_set(uint32_t period)
{
    rt_iit_dbg_svc_enter();
    
    iitchanged_rt_itv_set(period);
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}

extern oosiit_result_t svc_oosiit_itv_wait(void)
{
    rt_iit_dbg_svc_enter();
    
    iitchanged_rt_itv_wait();
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}


// mailbox

extern void* svc_oosiit_mbx_create(uint16_t numofmessages)
{
    void *ret;
    rt_iit_dbg_svc_enter();
    
    ret = rt_iit_memory_getmbx(numofmessages);
    if(NULL != ret)
    {
        rt_mbx_init(ret, 16+(4*numofmessages));  
    }
    
    rt_iit_dbg_svc_exit();
    return(ret);   
}


extern oosiit_result_t svc_oosiit_mbx_retrieve(oosiit_objptr_t mailbox, void** message, uint32_t timeout)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
    
    res = iitchanged_rt_mbx_wait(mailbox, message, timeout);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);
}

extern oosiit_result_t svc_oosiit_mbx_send(oosiit_objptr_t mailbox, void* message, uint32_t timeout)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
    
    res = iitchanged_rt_mbx_send(mailbox, message, timeout);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);
}

extern uint16_t svc_oosiit_mbx_available(oosiit_objptr_t mailbox)
{
    uint16_t res;
    rt_iit_dbg_svc_enter();
    
    res = (uint16_t)rt_mbx_check(mailbox);
    
    rt_iit_dbg_svc_exit();
    return(res);
}

extern uint16_t svc_oosiit_mbx_used(oosiit_objptr_t mailbox)
{
    uint16_t res;
    rt_iit_dbg_svc_enter();
    
    res = (uint16_t)rt_iit_mbx_count(mailbox);
    
    rt_iit_dbg_svc_exit();
    return(res);
}

// events

extern oosiit_result_t svc_oosiit_evt_set(uint32_t flags, oosiit_taskid_t tskid)
{
    rt_iit_dbg_svc_enter();
    
    iitchanged_rt_evt_set(flags, tskid);
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);
}

extern oosiit_result_t svc_oosiit_evt_wait(uint32_t waitflags,  uint32_t timeout, oosiit_evt_wait_mode_t waitmode)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
    
    res = iitchanged_rt_evt_wait(waitflags, timeout, (oosiit_evt_wait_mode_all == waitmode) ? (1) : (0));
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);
}

extern uint32_t svc_oosiit_evt_get(void)
{
    uint32_t res;
    rt_iit_dbg_svc_enter();
    
    res = iitchanged_rt_evt_get();
    
    rt_iit_dbg_svc_exit();
    return(res);
}


extern oosiit_result_t svc_oosiit_evt_clr(uint32_t flags)
{
    rt_iit_dbg_svc_enter();
    
    iitchanged_rt_evt_clr_runningtask(flags);
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_NOK);
}


//void iitchanged_rt_evt_clr_runningtask(EVENT_t flags)
//{
//    os_tsk.run->events &= ~flags;
//}

// semaphore

extern void* svc_oosiit_sem_create(uint8_t maxtokens, uint8_t ntokens)
{
    void *ret;
    rt_iit_dbg_svc_enter();
    
    ret = rt_iit_memory_getsem();
    if(NULL != ret)
    {
        iitchanged_rt_sem_init(ret, maxtokens, ntokens);  
    }
    
    rt_iit_dbg_svc_exit();
    return(ret);   
}


extern oosiit_result_t svc_oosiit_sem_set(oosiit_objptr_t sem, uint8_t ntokens)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = rt_iit_sem_set(sem, ntokens);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}


extern oosiit_result_t svc_oosiit_sem_send(oosiit_objptr_t sem)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = iitchanged_rt_sem_send(sem);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}


extern oosiit_result_t svc_oosiit_sem_wait(oosiit_objptr_t sem, uint32_t timeout)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = iitchanged_rt_sem_wait(sem, timeout);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}

// mutex

extern void* svc_oosiit_mut_create(void)
{
    void *ret;
    rt_iit_dbg_svc_enter();
    
    ret = rt_iit_memory_getmut();
    if(NULL != ret)
    {
        rt_mut_init(ret);  
    }
    
    rt_iit_dbg_svc_exit();
    return(ret);   
}

extern oosiit_result_t svc_oosiit_mut_wait(oosiit_objptr_t mutex, uint32_t timeout)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = iitchanged_rt_mut_wait(mutex, timeout);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}

extern oosiit_result_t svc_oosiit_mut_release(oosiit_objptr_t mutex)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = rt_mut_release(mutex);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}


// advanced timer

extern void* svc_oosiit_advtmr_new(void)
{
    void *ret;
    rt_iit_dbg_svc_enter();
    
    ret = rt_iit_advtmr_new();
     
    rt_iit_dbg_svc_exit();
    return(ret);   
}

extern oosiit_result_t svc_oosiit_advtmr_start(oosiit_objptr_t timer, oosiit_advtmr_timing_t *timing, oosiit_advtmr_action_t *action)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = rt_iit_advtmr_start(timer, timing, action);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}


extern oosiit_result_t svc_oosiit_advtmr_stop(oosiit_objptr_t timer)
{
    OS_RESULT res;
    rt_iit_dbg_svc_enter();
 
    res = rt_iit_advtmr_stop(timer);
    
    rt_iit_dbg_svc_exit();
    return((oosiit_result_t)res);   
}


extern oosiit_result_t svc_oosiit_advtmr_delete(oosiit_objptr_t timer)
{
    void* ret;
    rt_iit_dbg_svc_enter();
 
    ret = rt_iit_advtmr_delete(timer);
    ret =  ret;
    
    rt_iit_dbg_svc_exit();
    return(oosiit_res_OK);   
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static inline functions 
// --------------------------------------------------------------------------------------------------------------------



static __INLINE oosiit_taskid_t isr_oosiit_tsk_self(void)
{
    return(rt_tsk_self());
}

static __INLINE oosiit_result_t isr_oosiit_mbx_retrieve(oosiit_objptr_t mailbox, void** message)
{
    int32_t r = isr_mbx_receive(mailbox, message);
    
    if(OS_R_OK == r)
    {
        r = oosiit_res_TMO;
    }
    else
    {
        r = oosiit_res_OK;
    }
    return((oosiit_result_t)r);
}


static __INLINE void isr_oosiit_mbx_send(oosiit_objptr_t mailbox, void* message)
{
    isr_mbx_send(mailbox, message);
}


static __INLINE uint16_t isr_oosiit_mbx_available(oosiit_objptr_t mailbox)
{
    return((uint16_t)rt_mbx_check(mailbox));
}

static __INLINE uint16_t isr_oosiit_mbx_used(oosiit_objptr_t mailbox)
{
    return((uint16_t)rt_iit_mbx_count(mailbox));    
}

static __INLINE oosiit_result_t isr_oosiit_evt_set(uint32_t flags, oosiit_taskid_t tskid)
{
    iitchanged_isr_evt_set(flags, tskid);
    return(oosiit_res_OK);
}

static __INLINE oosiit_result_t isr_oosiit_sem_send(oosiit_objptr_t sem)
{
    iitchanged_isr_sem_send(sem);
    return(oosiit_res_OK);
}

static __INLINE oosiit_result_t isr_oosiit_advtmr_start(oosiit_objptr_t timer, oosiit_advtmr_timing_t *timing, oosiit_advtmr_action_t *action)
{
    return((oosiit_result_t)isr_iit_advtmr_start(timer, timing, action));
}

static __INLINE oosiit_result_t isr_oosiit_advtmr_stop(oosiit_objptr_t timer)
{
    return((oosiit_result_t)isr_iit_advtmr_stop(timer));
}

static __INLINE oosiit_result_t isr_oosiit_advtmr_delete(oosiit_objptr_t timer)
{
    return((oosiit_result_t)isr_iit_advtmr_delete(timer));
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



