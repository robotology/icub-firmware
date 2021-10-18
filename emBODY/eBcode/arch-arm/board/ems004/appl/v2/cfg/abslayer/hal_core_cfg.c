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


static void s_hal_core_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
#if !defined(DONTUSE_EOtheFatalError)    
    // in here there is just a test of emission of ipc data w/ a restart
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = fatalerror_handler_hal;
    des->handlererrorcode = errorcode;
    des->param = NULL;
    des->mpucontext = NULL;
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

void hw_handler_hf(fatal_error_handler_t feh, uint32_t *arg)
{
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = feh;
    des->handlererrorcode = 100;
    des->forfutureuse0 = 0x12;
    des->forfutureuse1 = 0x23;
    des->param = NULL;
    des->mpucontext = arg;
    eo_fatalerror_Restart(eo_fatalerror_GetHandle(), des);
}

void hw_handler(fatal_error_handler_t feh)
{
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = feh;
    des->handlererrorcode = 100;
    des->forfutureuse0 = 0x12;
    des->forfutureuse1 = 0x23;
    des->param = NULL;
    des->mpucontext = NULL;
    eo_fatalerror_Restart(eo_fatalerror_GetHandle(), des);
}

void NMI_Handler(void)
{
    hw_handler(fatalerror_handler_hw_NMI);
}

//void HardFault_Handler(void)
//{
//    hw_handler(fatalerror_handler_hw_HardFault);
//}

//void MemManage_Handler(void)
//{
//    hw_handler(fatalerror_handler_hw_MemManage);
//}

//void BusFault_Handler(void)
//{
//    hw_handler(fatalerror_handler_hw_BusFault);
//}

//void UsageFault_Handler(void)
//{
//    hw_handler(fatalerror_handler_hw_UsageFault);
//}

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

void McuHardFault_HandlerC(uint32_t *hardfault_args)
{
    hw_handler_hf(fatalerror_handler_hw_HardFault, hardfault_args);
}

void HardFault_Handler(void) __attribute__((naked));
void HardFault_Handler(void)
{
  __asm volatile (
    ".syntax unified              \n"  /* needed for the 'adds r1,#2' below */
    " movs r0,#4                  \n"  /* load bit mask into R0 */
    " mov r1, lr                  \n"  /* load link register into R1 */
    " tst r0, r1                  \n"  /* compare with bitmask */
    " beq _MSP                    \n"  /* if bitmask is set: stack pointer is in PSP. Otherwise in MSP */
    " mrs r0, psp                 \n"  /* otherwise: stack pointer is in PSP */
    " b _GetPC                    \n"  /* go to part which loads the PC */
  "_MSP:                          \n"  /* stack pointer is in MSP register */
    " mrs r0, msp                 \n"  /* load stack pointer into R0 */
  "_GetPC:                        \n"  /* find out where the hard fault happened */
    " ldr r1,[r0,#24]             \n"  /* load program counter into R1. R1 contains address of the next instruction where the hard fault happened */
    " b McuHardFault_HandlerC   \n"    /* decode more information. R0 contains pointer to stack frame */
  );
}

void McuBusFault_HandlerC(uint32_t *hardfault_args)
{
    hw_handler_hf(fatalerror_handler_hw_BusFault, hardfault_args);
}

void BusFault_Handler(void) __attribute__((naked));
void BusFault_Handler(void)
{
  __asm volatile (
    ".syntax unified              \n"  /* needed for the 'adds r1,#2' below */
    " movs r0,#4                  \n"  /* load bit mask into R0 */
    " mov r1, lr                  \n"  /* load link register into R1 */
    " tst r0, r1                  \n"  /* compare with bitmask */
    " beq _MSP1                   \n"  /* if bitmask is set: stack pointer is in PSP. Otherwise in MSP */
    " mrs r0, psp                 \n"  /* otherwise: stack pointer is in PSP */
    " b _GetPC1                   \n"  /* go to part which loads the PC */
  "_MSP1:                         \n"  /* stack pointer is in MSP register */
    " mrs r0, msp                 \n"  /* load stack pointer into R0 */
  "_GetPC1:                       \n"  /* find out where the hard fault happened */
    " ldr r1,[r0,#24]             \n"  /* load program counter into R1. R1 contains address of the next instruction where the hard fault happened */
    " b McuBusFault_HandlerC      \n"  /* decode more information. R0 contains pointer to stack frame */
  );
}

void McuMemManage_HandlerC(uint32_t *hardfault_args)
{
    hw_handler_hf(fatalerror_handler_hw_MemManage, hardfault_args);
}

void MemManage_Handler(void) __attribute__((naked));
void MemManage_Handler(void)
{
  __asm volatile (
    ".syntax unified              \n"  /* needed for the 'adds r1,#2' below */
    " movs r0,#4                  \n"  /* load bit mask into R0 */
    " mov r1, lr                  \n"  /* load link register into R1 */
    " tst r0, r1                  \n"  /* compare with bitmask */
    " beq _MSP2                   \n"  /* if bitmask is set: stack pointer is in PSP. Otherwise in MSP */
    " mrs r0, psp                 \n"  /* otherwise: stack pointer is in PSP */
    " b _GetPC2                   \n"  /* go to part which loads the PC */
  "_MSP2:                         \n"  /* stack pointer is in MSP register */
    " mrs r0, msp                 \n"  /* load stack pointer into R0 */
  "_GetPC2:                       \n"  /* find out where the hard fault happened */
    " ldr r1,[r0,#24]             \n"  /* load program counter into R1. R1 contains address of the next instruction where the hard fault happened */
    " b McuMemManage_HandlerC     \n"    /* decode more information. R0 contains pointer to stack frame */
  );
}

void McuUsageFault_HandlerC(uint32_t *hardfault_args)
{
    hw_handler_hf(fatalerror_handler_hw_UsageFault, hardfault_args);
}

void UsageFault_Handler(void) __attribute__((naked));
void UsageFault_Handler(void)
{
  __asm volatile (
    ".syntax unified              \n"  /* needed for the 'adds r1,#2' below */
    " movs r0,#4                  \n"  /* load bit mask into R0 */
    " mov r1, lr                  \n"  /* load link register into R1 */
    " tst r0, r1                  \n"  /* compare with bitmask */
    " beq _MSP3                   \n"  /* if bitmask is set: stack pointer is in PSP. Otherwise in MSP */
    " mrs r0, psp                 \n"  /* otherwise: stack pointer is in PSP */
    " b _GetPC3                   \n"  /* go to part which loads the PC */
  "_MSP3:                         \n"  /* stack pointer is in MSP register */
    " mrs r0, msp                 \n"  /* load stack pointer into R0 */
  "_GetPC3:                       \n"  /* find out where the hard fault happened */
    " ldr r1,[r0,#24]             \n"  /* load program counter into R1. R1 contains address of the next instruction where the hard fault happened */
    " b McuUsageFault_HandlerC    \n"    /* decode more information. R0 contains pointer to stack frame */
  );
}

#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



