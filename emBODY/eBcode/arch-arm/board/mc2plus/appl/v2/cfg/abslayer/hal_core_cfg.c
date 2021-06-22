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

/* @file       hal_core_cfg.c
	@brief      This file keeps internal implementation of the hal.
	@author     marco.accame@iit.it
    @date       09/09/2011
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
    #include "hal_led.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_core.h"
#include "hal_core_cfg.h"


static void s_hal_core_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);

static void* myheap_new(uint32_t size);
static void myheap_delete(void* mem);


const hal_core_cfg_t hal_cfg = 
{   
    EO_INIT(.basecfg)
    {
        EO_INIT(.extfn)
        {
            EO_INIT(.usr_on_fatal_error)                s_hal_core_cfg_on_fatalerror,
            EO_INIT(.osal_system_scheduling_suspend)    NULL,
            EO_INIT(.osal_system_scheduling_restart)    NULL,
            EO_INIT(.ext_heap_new)                      myheap_new,
            EO_INIT(.ext_heap_delete)                   myheap_delete
        } 
    },
    EO_INIT(.mpucfg)
    {
        EO_INIT(.dummy) 0
    },
    EO_INIT(.flashcfg)
    {
        EO_INIT(.dummy) 0
    },
    EO_INIT(.syscfg)
    {
        EO_INIT(.stacksize)             HAL_SYS_CFG_STACKSIZE,
        EO_INIT(.heapsize)              HAL_SYS_CFG_HEAPSIZE,
    }        
};


const hal_core_cfg_t *hal_coreCFGptr = &hal_cfg;


#include "hal_trace.h"
#include "hal_led.h"

static void s_hal_core_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
#if !defined(DONTUSE_EOtheFatalError)    
    // in here there is just a test of emission of ipc data w/ a restart
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = fatalerror_handler_hal;
    des->handlererrorcode = errorcode;
    des->param = NULL;
    eo_fatalerror_Restart(eo_fatalerror_GetHandle(), des);
#else    
    #warning DONTUSE_EOtheFatalError is defined, are you sure?    
    if(eobool_true == eo_errman_IsErrorHandlerConfigured(eo_errman_GetHandle()))
    {
        // ok ... use the error manager, either in its simple form or in its networked form
        eOerrmanDescriptor_t errdes = {0};
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_halerror);
        errdes.par16            = errorcode;
        errdes.par64            = 0;
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;    
        eo_errman_Error(eo_errman_GetHandle(), (hal_fatalerror_warning == errorcode) ? eo_errortype_warning : eo_errortype_fatal, errormsg, "HAL", &errdes);                
    }
    else
    {
        if(NULL != errormsg)
        {
            hal_trace_puts(errormsg);
        }
        
        if(hal_fatalerror_warning == errorcode)
        {
            return;
        }

        // in case of fatal error we blink all leds but led0
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
            
            //hal_led_toggle(hal_led0);
            hal_led_toggle(hal_led1);
            hal_led_toggle(hal_led2);
            hal_led_toggle(hal_led3);
            hal_led_toggle(hal_led4);
            hal_led_toggle(hal_led5);  
        }
    }
#endif       
}

#include "osal_base.h"

static void* myheap_new(uint32_t size)
{
    return(osal_base_memory_new(size));  
}
static void myheap_delete(void* mem)
{
    osal_base_memory_del(mem);
}

#if !defined(DONTUSE_EOtheFatalError)

void hw_handler(fatal_error_handler_t feh)
{
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = feh;
    des->handlererrorcode = 100;
    des->forfutureuse0 = 0x12;
    des->forfutureuse1 = 0x23;
    des->param = NULL;
    eo_fatalerror_Restart(eo_fatalerror_GetHandle(), des);
}

void NMI_Handler(void)
{
    hw_handler(fatalerror_handler_hw_NMI);
}

void HardFault_Handler(void)
{
    hw_handler(fatalerror_handler_hw_HardFault);
}

void MemManage_Handler(void)
{
    hw_handler(fatalerror_handler_hw_MemManage);
}

void BusFault_Handler(void)
{
    hw_handler(fatalerror_handler_hw_BusFault);
}

void UsageFault_Handler(void)
{
    hw_handler(fatalerror_handler_hw_UsageFault);
}

void DebugMon_Handler(void)
{
    hw_handler(fatalerror_handler_hw_DebugMon);
}

void Default_Handler(void)
{
    hw_handler(fatalerror_handler_hw_Default);
}

void RTC_Alarm_IRQHandler(void)
{
    hw_handler(fatalerror_handler_hw_Default);
}

#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



