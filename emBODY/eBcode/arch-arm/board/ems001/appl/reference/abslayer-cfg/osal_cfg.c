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

/* @file       osal_cfg.c
	@brief      This file keeps internal implementation of the osal.
	@author     marco.accame@iit.it
    @date       11/27/2009
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdio.h"
#include "string.h"
#include "osal.h"
#include "osal_arch_arm.h"
#include "hal_trace.h"
#include "stdlib.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "osal_cfg.h"
extern void* osal_ext_calloc(uint32_t s, uint32_t n)
{
//    char str[64];
  
    void* ret = calloc(s, n);
        
//    snprintf(str, sizeof(str), "mycalloc %d bytes: [%x, %x]", n*s, (uint32_t)ret, (uint32_t)ret+n*s);
//    hal_trace_puts(str);
    
    return(ret);
}

extern void osal_ext_free(void* m)
{
//    char str[64];
//    snprintf(str, sizeof(str), "myfree %x", (uint32_t)m);
//    hal_trace_puts(str);

    free(m);
}


static void s_osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg);
static void s_osal_cfg_on_idle(void);


extern const osal_cfg_t osal_cfg = 
{   
    .rtostype               = (osal_rtostype_t)OSAL_RTOSTYPE,          // uint8_t         rtostype;
    .memorymodel            = (osal_memorymodel_t)OSAL_MEMMODEL,
    .cpufam                 = (osal_cpufamily_t)OSAL_CPUFAM,            // uint8_t         cpufam;                                 
    .cpufreq                = OSAL_CPUFREQ,           // uint32_t        cpufreq; 
    .prio                   = OSAL_PRIO,                                          
    .tick                   = (osal_reltime_t)OSAL_TICK,              // uint32_t        tick;                                   
    .launcherstacksize      = OSAL_LAUNSTKSIZE,       // uint16_t        launcherstacksize;                     
    .idlestacksize          = OSAL_IDLESTKSIZE,       // uint16_t        idlestacksize;
    .roundrobin             = (osal_bool_t)OSAL_RROBIN,            // uint8_t         roundrobin;
    .roundrobintick         = OSAL_RROBINTICK,        // uint32_t        roundrobintick;
    
    .tasknum                = OSAL_TASKNUM,           // uint8_t         tasknum;
    .globalstacksize        = OSAL_GLOBSTKSIZE,       // uint16_t        globalstacksize;
    .timernum               = OSAL_TIMERNUM,          // uint8_t         timernum;
    .mutexnum               = OSAL_MUTEXNUM,          // uint8_t         mutexnum;
    .semaphorenum           = OSAL_SEMAPHORENUM,      // uint8_t         semaphorenum;
    .mqueuenum              = OSAL_MQUEUENUM,         
    .mqueueelemnum          = OSAL_MQUEUEELEMNUM, 
    .arch                   =
    {
        .nothingsofar                   = 0
    },      
    .extfn                  = 
    {
        .hal_sys_irqn_priority_set      = NULL,
        .usr_on_fatal_error             = s_osal_cfg_on_fatal_error, 
        .usr_on_idle                    = s_osal_cfg_on_idle
    }
};


extern const osal_cfg_t *osal_cfgMINE = &osal_cfg;


static void s_osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg)
{
    uint8_t tskid = 0;
    char str[128];
    
    osal_task_id_get(task, &tskid);
    sprintf(str, "error %d from taskid %d: %s\n\r", errorcode, tskid, errormsg);
    hal_trace_puts(str);
    for(;;);
}

static void s_osal_cfg_on_idle(void)
{
    for(;;);
}


// -- redefinition of ...

// required by the arm c stdlib: gives a different memory space for the stdlib to each thread in the arm compiler
void * __user_perthread_libspace(void) 
{ 
    static volatile uint8_t fc = 1; 
    void *ret = osal_arch_arm_armc99stdlib_getlibspace(fc);
    fc = 0; 
    return(ret);
}

// required by the arm c stdlib: initialises a mutex
int _mutex_initialize(void *m) 
{ 
    return(osal_arch_arm_armc99stdlib_mutex_initialize(m)); 
}

// required by the arm c stdlib: takes a mutex
void _mutex_acquire(void *m) 
{ 
    osal_arch_arm_armc99stdlib_mutex_acquire(m); 
} 

// required by the arm c stdlib: releases a mutex
void _mutex_release(void *m) 
{ 
    osal_arch_arm_armc99stdlib_mutex_release(m); 
}  





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



