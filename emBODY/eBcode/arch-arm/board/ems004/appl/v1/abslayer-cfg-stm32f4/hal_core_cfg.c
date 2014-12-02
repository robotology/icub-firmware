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

#include "stdlib.h"
#include "hal_core.h"

#include "EOtheErrorManager.h"
#include "EoError.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_core_cfg.h"


static void s_hal_core_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);

static void* myheap_new(uint32_t size);
static void myheap_delete(void* mem);


const hal_core_cfg_t hal_cfg = 
{   
    .basecfg    =
    {
        .extfn                  =
        {
            .usr_on_fatal_error                 = s_hal_core_cfg_on_fatalerror,
            .osal_system_scheduling_suspend     = NULL,
            .osal_system_scheduling_restart     = NULL,
            .ext_heap_new                       = myheap_new,
            .ext_heap_delete                    = myheap_delete
        } 
    },
    .mpucfg     =
    {
        .dummy                  = 0
    },
    .flashcfg   =
    {
        .dummy                  = 0
    },
    .syscfg     =
    {
        .stacksize              = HAL_SYS_CFG_STACKSIZE,
        .heapsize               = HAL_SYS_CFG_HEAPSIZE,
    }        
};


const hal_core_cfg_t *hal_coreCFGptr = &hal_cfg;


// void SysTick_Handler(void)
// {
//     hal_void_fp_void_t systickhandler = hal_sys_systick_gethandler();

//     if(NULL != systickhandler)
//     {
//         systickhandler();
//     }
// }

#include "hal_trace.h"
#include "hal_led.h"

static void s_hal_core_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
    if(eobool_true == eo_errman_IsErrorHandlerConfigured(eo_errman_GetHandle()))
    {
        // ok ... use the error manager, either in its simple form or in its networked form
        eOerrmanDescriptor_t errdes = {0};
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_halerror);
        errdes.param            = errorcode;
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




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



