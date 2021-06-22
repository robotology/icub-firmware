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

#undef DONTUSE_EOtheFatalError

#if !defined(DONTUSE_EOtheFatalError)
    #include "EOtheFatalError.h"
#else
    #include "EOtheErrorManager.h"
    #include "EoError.h"
    #include "hal_trace.h"
    #include "hal_sys.h"
    #include "hal_led.h"
#endif


#include "osal.h"
#include "osal_arch_arm.h"

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


const osal_cfg_t osal_cfg = 
{   
    EO_INIT(.rtostype)              (osal_rtostype_t)OSAL_RTOSTYPE,          // uint8_t         rtostype;
    EO_INIT(.memorymodel)           (osal_memorymodel_t)OSAL_MEMMODEL,
    EO_INIT(.prio)                  OSAL_PRIO,                                          
    EO_INIT(.cpufam)                (osal_cpufamily_t)OSAL_CPUFAM,            // uint8_t         cpufam;                                 
    EO_INIT(.cpufreq)               OSAL_CPUFREQ,           // uint32_t        cpufreq; 
    EO_INIT(.tick)                  (osal_reltime_t)OSAL_TICK,              // uint32_t        tick;                                   
    EO_INIT(.launcherstacksize)     OSAL_LAUNSTKSIZE,       // uint16_t        launcherstacksize;                     
    EO_INIT(.idlestacksize)         OSAL_IDLESTKSIZE,       // uint16_t        idlestacksize;
    EO_INIT(.roundrobin)            (osal_bool_t)OSAL_RROBIN,            // uint8_t         roundrobin;
    EO_INIT(.roundrobintick)        OSAL_RROBINTICK,        // uint32_t        roundrobintick;
    
    EO_INIT(.tasknum)               OSAL_TASKNUM,           // uint8_t         tasknum;
    EO_INIT(.globalstacksize)       OSAL_GLOBSTKSIZE,       // uint16_t        globalstacksize;
    EO_INIT(.timernum)              OSAL_TIMERNUM,          // uint8_t         timernum;
    EO_INIT(.mutexnum)              OSAL_MUTEXNUM,          // uint8_t         mutexnum;
    EO_INIT(.semaphorenum)          OSAL_SEMAPHORENUM,      // uint8_t         semaphorenum;
    EO_INIT(.mqueuenum)             OSAL_MQUEUENUM,         
    EO_INIT(.mqueueelemnum)         OSAL_MQUEUEELEMNUM, 
    EO_INIT(.arch)
    {
        EO_INIT(.nothingsofar)      0
    },      
    EO_INIT(.extfn)
    {
        EO_INIT(.hal_sys_irqn_priority_set)     NULL,
        EO_INIT(.usr_on_fatal_error)            s_osal_cfg_on_fatal_error, 
        EO_INIT(.usr_on_idle)                   s_osal_cfg_on_idle
    }
};


const osal_cfg_t *osal_cfgMINEX = &osal_cfg;


static void s_osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg)
{
#if !defined(DONTUSE_EOtheFatalError)        
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = fatalerror_handler_osal;
    des->handlererrorcode = errorcode;
    des->param = task;
    eo_fatalerror_Restart(eo_fatalerror_GetHandle(), des);
#else 
    #warning DONTUSE_EOtheFatalError is defined, are you sure?    
    uint8_t tskid = 0;
    char str[64];    
    osal_task_id_get((osal_task_t*)task, &tskid);
    snprintf(str, sizeof(str), "osalerror %d taskid %d: %s", errorcode, tskid, errormsg);
    
    if(eobool_true == eo_errman_IsErrorHandlerConfigured(eo_errman_GetHandle()))
    {
        // ok ... use the error manager, either in its simple form or in its networked form
        eOerrmanDescriptor_t errdes = {0};
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_osalerror);
        errdes.par16            = errorcode;
        errdes.par64            = 0;
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;    
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, str, "OSAL", &errdes);                
    }
    else
    {
        if(NULL != errormsg)
        {
            hal_trace_puts(errormsg);
        }
        

        // in case of fatal error we blink all leds but led1
        hal_led_init(hal_led0, NULL);
        hal_led_init(hal_led1, NULL);
        hal_led_init(hal_led2, NULL);
        hal_led_init(hal_led3, NULL);
        hal_led_init(hal_led4, NULL);
        hal_led_init(hal_led5, NULL);
    
        hal_led_off(hal_led0);
        hal_led_off(hal_led1);
        hal_led_off(hal_led2);
        hal_led_off(hal_led3);
        hal_led_off(hal_led4);
        hal_led_off(hal_led5);   

        for(;;)
        {
            hal_sys_delay(100);
            
            hal_led_toggle(hal_led0);
            //hal_led_toggle(hal_led1);
            hal_led_toggle(hal_led2);
            hal_led_toggle(hal_led3);
            hal_led_toggle(hal_led4);
            hal_led_toggle(hal_led5);  
        }
    }    
#endif   
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



