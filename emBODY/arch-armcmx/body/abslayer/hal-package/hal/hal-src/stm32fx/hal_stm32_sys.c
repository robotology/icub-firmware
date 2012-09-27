/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/* @file       hal_stm32_sys.c
	@brief      This file contains internal implementation of the hal sys module.
	@author     marco.accame@iit.it
    @date       09/09/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_stm32xx_include.h"

#include "hal_stm32_base_hid.h" 
#include "hal_arch_arm.h"

#include "hal_brdcfg.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_sys_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// this variable is used to allows one task or isr to make multiple calls to 
// hal_sys_criticalsection_take() and keep the critical section until the
// same number of calls to hal_sys_criticalsection_release() is done. 
static volatile uint8_t s_cs_takes = 0;
static void (*s_hal_sys_SYSTICK_handler_fn)(void) = NULL;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_sys_systeminit(void) 
{
    if(0 == hal_base_hid_params.cpu_freq)
    {
         return(hal_res_NOK_generic);
    }


    SystemInit();

    hal_brdcfg_sys__clock_config();
    hal_brdcfg_sys__gpio_default_init();

#warning --> verify differences between stm32 and stm32f4

    // configure once and only once the nvic to hold 4 bits for interrupt priorities and 0 for subpriorities
    // in stm32 lib ... NVIC_PriorityGroup_4 is 0x300, thus cmsis priority group number 3, thus
    // bits[7:4] for pre-emption priority and bits[3:0} for subpriority. but stm32 only has the 4 msb bits.
    // see page 114 of joseph yiu's book.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // acemor: added to remove dependencies from NZI data
    s_cs_takes = 0;
    s_hal_sys_SYSTICK_handler_fn = NULL;

    return(hal_res_OK);
}


extern hal_result_t hal_sys_systemreset(void) 
{
    NVIC_SystemReset();
    
    return(hal_res_NOK_generic);
}


extern hal_result_t hal_sys_canexecuteataddress(uint32_t addr)
{
    // test if user code is programmed starting from address addr
    // so far it is a simple condition which is pretty general for every CM3 ... where:
    // the reset vector is at beginning of flash

    extern const uint32_t hal_flash_BASEADDR;
    extern const uint32_t hal_flash_TOTALSIZE;
    
    if((addr < hal_flash_BASEADDR) || (addr >= (hal_flash_BASEADDR+hal_flash_TOTALSIZE)))
    {
        // out of valid flash ...
        return(hal_res_NOK_generic);
    }

    if(((*(volatile uint32_t*)addr) & 0x2FFE0000 ) == 0x20000000)
    {
        return(hal_res_OK);
    }
    else 
    {
        return(hal_res_NOK_generic);
    }
}

extern hal_result_t hal_sys_executenowataddress(uint32_t addr) 
{
    volatile uint32_t jumpaddr;
    void (*app_fn)(void) = NULL;

    if(hal_res_NOK_generic == hal_sys_canexecuteataddress(addr))
    {
        return(hal_res_NOK_generic);
    }

    // prepare jump address 
    jumpaddr = *(volatile uint32_t*) (addr + 4);
    // prepare jumping function
    app_fn = (void (*)(void)) jumpaddr;
    // initialize user application's stack pointer 
    __set_MSP(*(volatile uint32_t*) addr);
    // jump.
    app_fn(); 
    
    // never in here.
    return(hal_res_NOK_generic);   
}

extern hal_result_t hal_sys_vectortable_relocate(uint32_t offset) 
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, offset);

    return(hal_res_OK);
}


extern hal_result_t hal_sys_systick_sethandler(void (*systickhandler)(void), hal_time_t period, hal_interrupt_priority_t priority)
{
    uint32_t tickinsec = 1000*1000 / period;
    uint8_t r = 0;

    if(0 == hal_base_hid_params.cpu_freq)
    {
         return(hal_res_NOK_generic);
    }

    if((0 == period) || (NULL == systickhandler) || (hal_int_priorityNONE == priority))
    {
        return(hal_res_NOK_generic);
    }

    s_hal_sys_SYSTICK_handler_fn = systickhandler;

    // setup the clock for cortex-m3 as in the cmsis example 
    r = SysTick_Config(SystemCoreClock / tickinsec);


    hal_sys_irqn_priority_set(hal_arch_arm_SysTick_IRQn, priority);


    return((0==r) ? (hal_res_OK) : (hal_res_NOK_generic));
}


extern hal_void_fp_void_t hal_sys_systick_gethandler(void)
{
    return(s_hal_sys_SYSTICK_handler_fn);
}



extern void hal_sys_irq_disable(void) 
{
    __disable_irq();
}    


extern void hal_sys_irq_enable(void) 
{
     __enable_irq();    
}


extern void hal_sys_irqn_disable(hal_irqn_t irqn) 
{
    NVIC_DisableIRQ((IRQn_Type)irqn);
}    


extern void hal_sys_irqn_enable(hal_irqn_t irqn) 
{
     NVIC_EnableIRQ((IRQn_Type)irqn);    
}  

extern void hal_sys_irqn_priority_set(hal_irqn_t irqn, hal_interrupt_priority_t prio) 
{
    if(hal_int_priorityNONE == prio)
    {
        return;
    }

    NVIC_SetPriority((IRQn_Type)irqn, prio);    
} 

extern hal_interrupt_priority_t hal_sys_irqn_priority_get(hal_irqn_t irqn) 
{
    return((hal_interrupt_priority_t)NVIC_GetPriority((IRQn_Type)irqn));    
}


// - hal_sys_atomic_*:     functions which perform an operation operate without being interrupted by an ISR
// -                    dependencies: only from __disable_irq() and __enable_irq(), which is proper of armcc compiler.

// see http://www.keil.com/support/docs/2990.htm 
// and http://www.keil.com/support/man/docs/ca/ca_le_swi.htm
// and http://www.keil.com/support/man/docs/rlarm/rlarm_ar_svc_func.htm


extern hal_result_t hal_sys_criticalsection_take(void *p, hal_time_t tout) 
{

    if(0 == s_cs_takes)
    {
        hal_sys_irq_disable();
    }

    if(s_cs_takes < 255) 
    {
        s_cs_takes ++;    
    }

    return(hal_res_OK);
}    


extern hal_result_t hal_sys_criticalsection_release(void *p) 
{
    if(s_cs_takes > 0) 
    {
        s_cs_takes --;
    }

    if(0 == s_cs_takes) 
    {
        hal_sys_irq_enable();    
    }

    return(hal_res_OK);
}
 

extern void hal_sys_atomic_bitwiseOR(volatile uint32_t *value, uint32_t mask) 
{
    hal_sys_criticalsection_take(NULL, 0);
    *value |= mask;
    hal_sys_criticalsection_release(NULL);    
}



extern void hal_sys_atomic_bitwiseAND(volatile uint32_t *value, uint32_t mask) 
{
    hal_sys_criticalsection_take(NULL, 0);
    *value &= mask;
    hal_sys_criticalsection_release(NULL);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


//#warning In SysTick_Handler(): remove __weak for debug, but leave it for osiit or osal 
//
//#ifdef TEST_NO_OSIIT
//void SysTick_Handler(void)
//#else
//__weak void SysTick_Handler(void)
//#endif
//{
//    if(NULL != s_hal_sys_SYSTICK_handler_fn)
//    {
//        s_hal_sys_SYSTICK_handler_fn();
//    }
//}

extern uint32_t hal_sys_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_sys_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependency from nzi data
    s_cs_takes = 0;
    s_hal_sys_SYSTICK_handler_fn = NULL;

    return(hal_res_OK);  
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------







#endif//HAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



