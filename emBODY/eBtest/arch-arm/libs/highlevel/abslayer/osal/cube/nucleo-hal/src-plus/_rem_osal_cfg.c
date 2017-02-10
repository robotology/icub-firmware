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

#include "stdlib.h"

#define DESINIT(f)      f =
//#define DESINIT(f)

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


#include "osal_cfg.h"




extern void* osal_ext_calloc(uint32_t s, uint32_t n)
{
    void* ret = calloc(s, n);
    return(ret);
}

extern void* osal_ext_realloc(void* m, uint32_t s)
{
    void* ret = realloc(m, s);
    return(ret);
}

extern void osal_ext_free(void* m)
{
    free(m);
}


static void s_osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg);
static void s_osal_cfg_on_idle(void);


//const osal_cfg_t osal_cfg = []()
//{ 
//    osal_cfg_t t{}; 
//    t.rtostype          = (osal_rtostype_t)OSAL_RTOSTYPE, 
//    t.memorymodel       = (osal_memorymodel_t)OSAL_MEMMODEL
//    ; 

//    return t; 
//}();





const osal_cfg_t osal_cfg = 
{   
    DESINIT(.rtostype)              (osal_rtostype_t)OSAL_RTOSTYPE,          // uint8_t         rtostype;
    DESINIT(.memorymodel)           (osal_memorymodel_t)OSAL_MEMMODEL,
    DESINIT(.prio)                  OSAL_PRIO, 
    DESINIT(.cpufam)                (osal_cpufamily_t)OSAL_CPUFAM,            // uint8_t         cpufam;                                 
    DESINIT(.cpufreq)               OSAL_CPUFREQ,           // uint32_t        cpufreq;                                
    DESINIT(.tick)                  (osal_reltime_t)OSAL_TICK,              // uint32_t        tick;                                   
    DESINIT(.launcherstacksize)     OSAL_LAUNSTKSIZE,       // uint16_t        launcherstacksize;                     
    DESINIT(.idlestacksize)         OSAL_IDLESTKSIZE,       // uint16_t        idlestacksize;
    DESINIT(.roundrobin)            (osal_bool_t)OSAL_RROBIN,            // uint8_t         roundrobin;
    DESINIT(.roundrobintick)        OSAL_RROBINTICK,        // uint32_t        roundrobintick;
    
    DESINIT(.tasknum)               OSAL_TASKNUM,           // uint8_t         tasknum;
    DESINIT(.globalstacksize)       OSAL_GLOBSTKSIZE,       // uint16_t        globalstacksize;
    DESINIT(.timernum)              OSAL_TIMERNUM,          // uint8_t         timernum;
    DESINIT(.mutexnum)              OSAL_MUTEXNUM,          // uint8_t         mutexnum;
    DESINIT(.semaphorenum)          OSAL_SEMAPHORENUM,      // uint8_t         semaphorenum;
    DESINIT(.mqueuenum)             OSAL_MQUEUENUM,         
    DESINIT(.mqueueelemnum)         OSAL_MQUEUEELEMNUM, 
    DESINIT(.arch)
    {
        DESINIT(.nothingsofar)      0
    },      
    DESINIT(.extfn)
    {
        DESINIT(.hal_sys_irqn_priority_set)     NULL,
        DESINIT(.usr_on_fatal_error)            s_osal_cfg_on_fatal_error, 
        DESINIT(.usr_on_idle)                   s_osal_cfg_on_idle
    }
};


const osal_cfg_t *osal_cfgMINEX = &osal_cfg;


static void s_osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg)
{
    uint8_t tskid = 0;
    char str[64];    
    osal_task_id_get((osal_task_t*)task, &tskid);
    snprintf(str, sizeof(str), "osalerror %d taskid %d: %s", errorcode, tskid, errormsg);
    
    uint32_t cnt = 0;


    for(;;)
    {
        cnt++;        
    }
 
   
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



