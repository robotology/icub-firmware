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

#include "hal_cpu.h"

#include "hal_middleware_interface.h"

#include "hal_base_hid.h" 


#include "hal_brdcfg.h"
#include "hal_flash.h"

extern uint32_t SystemCoreClock;
 
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
// - definition of __asm functions
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HAL_SYS_VERIFY_STACK_HEAP_SIZES)
    // removed by acemor on 09-mar-2011 to avoid problems of ... 
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Heap_Size (referred from hal_stm32.o).
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Stack_Size (referred from hal_stm32.o).
__asm static int hal_sys_getstacksize (void) {
        IMPORT  Stack_Size
        LDR     R0,=Stack_Size
        BX      LR
}


__asm static int hal_sys_getheapsize (void) {
        IMPORT  Heap_Size
        LDR     R0,=Heap_Size
        BX      LR
}
#endif//defined(HAL_SYS_VERIFY_STACK_HEAP_SIZES)



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
    // removed by acemor on 09-mar-2011 to avoid problems of ... 
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Heap_Size (referred from hal_stm32.o).
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Stack_Size (referred from hal_stm32.o).
    // but ... as long as we dont use shalPART.axf (shared library) we dont care.
    if(cfg->stacksize != hal_sys_getstacksize())
    {   
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect stack size");
        return(hal_res_NOK_generic);
    }

    if(cfg->heapsize != hal_sys_getheapsize())
    {   
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect heap size");
        return(hal_res_NOK_generic);
    }
#endif//defined(HAL_SYS_VERIFY_STACK_HEAP_SIZES)
     
    
    
    memcpy(&s_hal_sys_theinternals.config, cfg, sizeof(hal_sys_cfg_t));
    
    // acemor: added to remove dependencies from NZI data
    s_hal_sys_theinternals.cs_takes            = 0;
    s_hal_sys_theinternals.fn_SYSTICK_handler  = NULL;
    
    return(hal_res_OK);   
}

extern uint32_t hal_sys_stack_get_totalsize(void)
{
    return(hal_sys_getstacksize());   
}


extern uint32_t hal_sys_heap_get_totalsize(void)
{
    return(hal_sys_getheapsize());   
}


uint8_t hal_sys_howmanyARMv7ops(void)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1)
    //return(3+3);
    return(3+3+3);      // number seems to be rather empirical. can anybody help me finding a rule?
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    return(3+1);
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif  
}
__asm void hal_sys_someARMv7ops(uint32_t numberof) 
{   // it takes 3+p cycles: 1+1+1+p, p = 1, 2, or 3. where p is what is needed to fill the pipeline
    // cm3: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0337i/index.html
    ALIGN
loop
    CBZ     r0, exit
    SUB     r0,#1
    B       loop
exit
    BX      LR     

    ALIGN
}


extern hal_result_t hal_sys_delay(hal_reltime_t reltime)
{
    static uint32_t s_hal_sys_numofops1usec = 0;
    if(0 == s_hal_sys_numofops1usec)
    {
        if(0 == SystemCoreClock)
        {
            return(hal_res_NOK_generic);
        }
        // to occupy a microsec i execute an operation for a number of times which depends on: SystemCoreCloc and 1.25 dmips/mhz, 
        //s_hal_sys_numofinstructions1usec = ((SystemCoreClock/1000000) * 125l) / 100l;
        s_hal_sys_numofops1usec = (SystemCoreClock/1000000) / hal_sys_howmanyARMv7ops();
    }
        
    hal_sys_someARMv7ops(s_hal_sys_numofops1usec * reltime);

    return(hal_res_OK);
}


extern hal_result_t hal_sys_systemreset(void) 
{
    NVIC_SystemReset();
    
    return(hal_res_NOK_generic);
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


    hal_sys_irqn_priority_set(hal_cpu_arc_armcmx_SysTick_IRQn, priority);


    return((0==r) ? (hal_res_OK) : (hal_res_NOK_generic));
}


extern hal_void_fp_void_t hal_sys_systick_gethandler(void)
{
    return(s_hal_sys_theinternals.fn_SYSTICK_handler);
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


extern hal_result_t hal_sys_hid_static_memory_init(void)
{
    // removed dependency from nzi data
    s_hal_sys_theinternals.cs_takes            = 0;
    s_hal_sys_theinternals.fn_SYSTICK_handler  = NULL;

    return(hal_res_OK);  
}



extern hal_result_t hal_sys_hid_systeminit(void) 
{
    if(hal_false == hal_base_hid_initted_is())
    {
         return(hal_res_NOK_generic);
    }
    
    // if we use hal, then at startup the system runs a SystemInit() which was re-defined by hal 
    // the redefined function just configures the internal clock and does not use any prescaling for internal buses.
    
    // hence, ... we need to configure a proper clock and update the system core clock 
    
    hal_cpu_clocks_set();
    
//     if(hal_true == hal_brdcfg_sys__theconfig.clockcfg.keepinternalclockatstartup)
//     {
//         s_hal_sys_set_sys_clock(hal_cpu_maxspeed_get());
//         s_hal_sys_system_core_clock_update();
//     }
//     else
//     {
//         s_hal_sys_system_core_clock_update();
//     }


//     hal_brdcfg_sys__clock_config();
//     hal_brdcfg_sys__gpio_default_init();

    // configure once and only once the nvic to hold 4 bits for interrupt priorities and 0 for subpriorities
    // in stm32 lib ... NVIC_PriorityGroup_4 is 0x300, thus cmsis priority group number 3, thus
    // bits[7:4] for pre-emption priority and bits[3:0} for subpriority. but stm32 only has the 4 msb bits.
    // see page 114 of joseph yiu's book.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);


    return(hal_res_OK);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


#endif//HAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





