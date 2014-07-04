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

/* @file       osal_cfg-open.c
	@brief      This file keeps internal configuration of the osal.
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
#include "userdef_onerror.h"
#include "stdlib.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



#include "osal_cfg.h"


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
        .usr_on_fatal_error             = osal_cfg_on_fatal_error, 
        .usr_on_idle                    = s_osal_cfg_on_idle
    }
};


extern const osal_cfg_t *osal_cfg_USERptr = &osal_cfg;



static void s_osal_cfg_on_idle(void)
{
    for(;;);
}

extern void* osal_ext_calloc(uint32_t s, uint32_t n)
{
    void* ret = calloc(s, n);
    return(ret);
}


/*README: if you need a static allocator instead of dynamic one (used by default), 
          you can comment previous definition of osal_ext_calloc function and use 
          following code under #ifdef USE_STATIC_ALLOCATOR
          with static allocator there is a significant savings of ram!!!
          Pay attention: if you use static allocator, you should set HAL_SYS_HEAPSIZE equal to 0x00000020
*/
#ifdef USE_STATIC_ALLOCATOR

#define SIZE_STATIC_HEAP        0xc170 //48kB
static uint8_t staticheap[SIZE_STATIC_HEAP];
uint32_t    tot_mem = 0;

extern void* osal_ext_calloc(uint32_t s, uint32_t n)
{
    char str[80];
    static uint32_t usedsize = 0;
    uint32_t reqsize; //required size
    uint8_t *retmem_ptr = NULL;
    
    reqsize = s*n;
    //uint32_t usersize = reqsize = s*n;
    //allineo a 8
    reqsize+=7; reqsize>>=3; reqsize<<=3;
    
    
    if((usedsize+reqsize) > SIZE_STATIC_HEAP)
    {
         snprintf(str, sizeof(str), "no more static memory!! usedmem=0x%x reqsize=0x%x", usedsize, reqsize);
         hal_trace_puts(str);
        return(NULL);
    }
    
    retmem_ptr = &staticheap[usedsize];
    usedsize += reqsize;
    tot_mem=usedsize;
//     otot += reqsize-usersize;
//     snprintf(str, sizeof(str), "userSize=0x%x totmem=0x%x osize=0x%x otot=0x%x", usersize, usedsize, (reqsize-usersize), otot);
//     hal_trace_puts(str);
    
    return((void*)retmem_ptr);
    
}



#endif

extern void* osal_ext_realloc(void* m, uint32_t s)
{
    return(realloc(m, s));
}

extern void osal_ext_free(void* m)
{
    free(m);
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



