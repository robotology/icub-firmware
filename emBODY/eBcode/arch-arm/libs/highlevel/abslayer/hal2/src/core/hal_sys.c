/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hal_sys.c
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

#include "hal_mpu.h"

#include "hal_middleware_interface.h"

#include "hal_base_hid.h" 


#include "hal_brdcfg.h"
#include "hal_flash.h"

#include "hal_mpu_name.h"

#include "hl_sys.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sys_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_SYS_VERIFY_STACK_HEAP_SIZES

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_sys_cfg_t hal_sys_cfg_default = { .stacksize = 0, .heapsize = 0 };



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_sys_cfg_t       config;
    volatile uint8_t    cs_takes;   /**<    this variable is used to allows one task or isr to make multiple calls to 
                                            hal_sys_criticalsection_take() and keep the critical section until the
                                            same number of calls to hal_sys_criticalsection_release() is done. */ 
    hal_void_fp_void_t  fn_SYSTICK_handler;  
} hal_sys_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

// static void s_hal_sys_set_sys_clock(uint32_t maxcpufreq);
// static void s_hal_sys_system_core_clock_update(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// this variable is used to allows one task or isr to make multiple calls to 
// hal_sys_criticalsection_take() and keep the critical section until the
// same number of calls to hal_sys_criticalsection_release() is done. 


static hal_sys_theinternals_t s_hal_sys_theinternals =
{
    .config                 = { 0 },
    .cs_takes               = 0,
    .fn_SYSTICK_handler     = NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_sys_init(const hal_sys_cfg_t* cfg)
{
    if(NULL == cfg)
    {
        cfg  = &hal_sys_cfg_default;
    } 

    // verify stack and heap
#if     defined(HAL_SYS_VERIFY_STACK_HEAP_SIZES)
    if(cfg->stacksize != hal_sys_stack_get_totalsize())
    {   
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect stack size");
        return(hal_res_NOK_generic);
    }

    if(cfg->heapsize != hal_sys_heap_get_totalsize())
    {   
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect heap size");
        return(hal_res_NOK_generic);
    }
#endif//defined(HAL_SYS_VERIFY_STACK_HEAP_SIZES)
     
    
    
    memcpy(&s_hal_sys_theinternals.config, cfg, sizeof(hal_sys_cfg_t));    
    s_hal_sys_theinternals.cs_takes            = 0;
    s_hal_sys_theinternals.fn_SYSTICK_handler  = NULL;
    
    
    // set priority levels
    
    // configure once and only once the nvic to hold 4 bits for interrupt priorities and 0 for subpriorities
    // in stm32 lib ... NVIC_PriorityGroup_4 is 0x300, thus cmsis priority group number 3, thus
    // bits[7:4] for pre-emption priority and bits[3:0} for subpriority. but stm32 only has the 4 msb bits.
    // see page 114 of joseph yiu's book.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    return(hal_res_OK);   
}

extern uint32_t hal_sys_stack_get_totalsize(void)
{
    return(hl_sys_stack_totalsize_get());   
}


extern uint32_t hal_sys_heap_get_totalsize(void)
{
    return(hl_sys_heap_totalsize_get());   
}


extern hal_result_t hal_sys_delay(hal_reltime_t reltime)
{
    return((hal_result_t)hl_sys_delay(reltime));
}


extern hal_result_t hal_sys_systemreset(void) 
{
    return((hal_result_t)hl_sys_systemreset());
}


extern hal_result_t hal_sys_canexecuteataddress(uint32_t addr)
{
#ifdef HAL_USE_FLASH    
    // test if user code is programmed starting from address addr
    // so far it is a simple condition which is pretty general for every CM3 ... where:
    // the reset vector is at beginning of flash

    uint32_t flash_BASEADDR = hal_flash_get_baseaddress();
    uint32_t flash_TOTALSIZE = hal_flash_get_totalsize();
    
    if((addr < flash_BASEADDR) || (addr >= (flash_BASEADDR+flash_TOTALSIZE)))
    {
        // out of valid flash ...
        return(hal_res_NOK_generic);
    }
#endif
    
    return((hal_result_t)hal_sys_canjump(addr));
}

extern hal_result_t hal_sys_executenowataddress(uint32_t addr) 
{
    return((hal_result_t)hl_sys_jumpto(addr));   
}

extern hal_result_t hal_sys_vectortable_relocate(uint32_t offset) 
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, offset);
    return(hal_res_OK);
}


extern hal_result_t hal_sys_systick_sethandler(void (*systickhandler)(void), hal_reltime_t period, hal_interrupt_priority_t priority)
{
    uint32_t tickinsec = 1000*1000 / period;
    uint8_t r = 0;

    if(hal_false == hal_base_hid_initted_is())
    {
         return(hal_res_NOK_generic);
    }

    if((0 == period) || (NULL == systickhandler) || (hal_int_priorityNONE == priority))
    {
        return(hal_res_NOK_generic);
    }

    s_hal_sys_theinternals.fn_SYSTICK_handler = systickhandler;

    // setup the clock for cortex-m3 as in the cmsis example 
    r = SysTick_Config(SystemCoreClock / tickinsec);


    hal_sys_irqn_priority_set(hal_mpu_arch_armcmx_SysTick_IRQn, priority);


    return((0==r) ? (hal_res_OK) : (hal_res_NOK_generic));
}


extern hal_void_fp_void_t hal_sys_systick_gethandler(void)
{
    return(s_hal_sys_theinternals.fn_SYSTICK_handler);
}



extern void hal_sys_irq_disable(void) 
{
    hl_sys_irq_disable();
}    


extern void hal_sys_irq_enable(void) 
{
     hl_sys_irq_enable();    
}


extern void hal_sys_irqn_disable(hal_irqn_t irqn) 
{
    hl_sys_irqn_disable((hl_irqn_t)irqn);
}    


extern void hal_sys_irqn_enable(hal_irqn_t irqn) 
{ 
    hl_sys_irqn_enable((hl_irqn_t)irqn);
}  

extern void hal_sys_irqn_priority_set(hal_irqn_t irqn, hal_interrupt_priority_t prio) 
{
    hl_sys_irqn_priority_set((hl_irqn_t)irqn, (hl_irqpriority_t) prio);
} 

extern hal_interrupt_priority_t hal_sys_irqn_priority_get(hal_irqn_t irqn) 
{
    return((hal_interrupt_priority_t)hl_sys_irqn_priority_get((hl_irqn_t)irqn));
}

extern hal_boolval_t hal_sys_irq_is_running(void) 
{
    if(0 != (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
    {
        return(hal_true);        
    }
    else
    {
        return(hal_false);
    }
} 


// - hal_sys_atomic_*:     functions which perform an operation operate without being interrupted by an ISR
// -                    dependencies: only from __disable_irq() and __enable_irq(), which is proper of armcc compiler.

// see http://www.keil.com/support/docs/2990.htm 
// and http://www.keil.com/support/man/docs/ca/ca_le_swi.htm
// and http://www.keil.com/support/man/docs/rlarm/rlarm_ar_svc_func.htm


extern hal_result_t hal_sys_criticalsection_take(void *p, hal_reltime_t tout) 
{

    if(0 == s_hal_sys_theinternals.cs_takes)
    {
        hal_sys_irq_disable();
    }

    if(s_hal_sys_theinternals.cs_takes < 255) 
    {
        s_hal_sys_theinternals.cs_takes ++;    
    }

    return(hal_res_OK);
}    


extern hal_result_t hal_sys_criticalsection_release(void *p) 
{
    if(s_hal_sys_theinternals.cs_takes > 0) 
    {
        s_hal_sys_theinternals.cs_takes --;
    }

    if(0 == s_hal_sys_theinternals.cs_takes) 
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



// extern hal_result_t hal_sys_hid_systeminit(void) 
// {
//     if(hal_false == hal_base_hid_initted_is())
//     {
//          return(hal_res_NOK_generic);
//     }
//     
//     // if we use hal, then at startup the system runs a SystemInit() which was re-defined by hal 
//     // the redefined function just configures the internal clock and does not use any prescaling for internal buses.
//     
//     // hence, ... we need to configure a proper clock and update the system core clock 
//   

// #warning --> dont use hal_cpu_clocks_set(): rework the code
// //    hal_cpu_clocks_set();
//     

//     // configure once and only once the nvic to hold 4 bits for interrupt priorities and 0 for subpriorities
//     // in stm32 lib ... NVIC_PriorityGroup_4 is 0x300, thus cmsis priority group number 3, thus
//     // bits[7:4] for pre-emption priority and bits[3:0} for subpriority. but stm32 only has the 4 msb bits.
//     // see page 114 of joseph yiu's book.
// //    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);


//     return(hal_res_OK);
// }





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


#endif//HAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





