/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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

/* @file       hal_dspic_sys.c
    @brief      This file contains internal implementation of the hal sys module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
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

#include "hal_base.h"

#include <p33fxxxx.h>
#include <libpic30.h>

#include "hal_arch_dspic.h" 

#include "hal_dspic_base_hid.h" 
#include "hal_timer.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dspic_sys_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_USE_SYSTICK

#define FREQ_OSC    80

#define FCY         (FREQ_OSC/2) //(express in MHz). It is the frequency in input to clocks.
                    // For dspic33f it is Freq oscillator(80MHz) divided by 2.


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

// used as systick handler
#ifdef HAL_USE_SYSTICK
static void s_hal_sys_SysTick_Handler(void *p);
#endif

// mimic the static __INLINE uint32_t SysTick_Config(uint32_t ticks) used in cmsis, only that handler is not external
static hal_result_t s_hal_sys_SysTick_Config(void (*handler)(void *p), hal_time_t period, hal_interrupt_priority_t priority);

// mimic the SystemInit(void) used in cmsis
static void s_hal_sys_SystemInit(void);

static void s_hal_sys_osc_config(void);
static void s_hal_sys_gpio_init(void);
static void s_hal_sys_dspengine_config(hal_boolval_t moltipl_unsigned, hal_boolval_t enable_fractional_mode);

static void s_hal_sys_irqn_write(hal_irqn_t irqn, uint8_t bit);
static void s_hal_sys_irqn_prioset(hal_irqn_t irqn, uint8_t prio);
static uint8_t s_hal_sys_irqn_prioget(hal_irqn_t irqn);


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

    s_hal_sys_SystemInit();
    
    // be sure to have a certain number of priorities being set.

    // acemor: added to remove dependencies from NZI data
    s_cs_takes = 0;
    s_hal_sys_SYSTICK_handler_fn = NULL;

    return(hal_res_OK);
}


extern hal_result_t hal_sys_systemreset(void) 
{
    __asm__ ("reset");

    return(hal_res_NOK_generic);
}


extern hal_result_t hal_sys_canexecuteataddress(uint32_t addr)
{
    return(hal_res_NOK_unsupported);
}

extern hal_result_t hal_sys_executenowataddress(uint32_t addr) 
{
    switch(addr)
    {
        case 0x200:
        {
            __asm__ volatile("goto 0x200");
        } break;
        
        case 0xD000:
        {
            __asm__ volatile("goto 0xD000");
        } break;

        case 0x400:
        {
            __asm__ volatile("goto 0x400");
        } break;

        default:
        {
            __asm__ volatile("goto 0x200");
        } break;
    }
    
    return(hal_res_OK);
}

extern hal_result_t hal_sys_vectortable_relocate(uint32_t offset) 
{
    return(hal_res_NOK_unsupported);
}


// defined just to allow compilation, but never used in code execution. if we have systick_useexternal_handler set to 1,
// we must provide an externally defined SysTick_Handler() function. 
__attribute__((__weak__)) void SysTick_Handler(void)
{
    hal_void_fp_void_t systickhandler = hal_sys_systick_gethandler();

    if(NULL != systickhandler)
    {
        systickhandler();
    }    
}

#ifdef HAL_USE_SYSTICK
extern hal_result_t hal_sys_systick_sethandler(void (*systickhandler)(void), hal_time_t period, hal_interrupt_priority_t priority)
{
    uint8_t r = 0;
    //hal_void_fp_void_t handler = NULL;

    if(0 == hal_base_hid_params.cpu_freq)
    {
         return(hal_res_NOK_generic);
    }

    if((0 == period) || (NULL == systickhandler))
    {
        return(hal_res_NOK_generic);
    }

    s_hal_sys_SYSTICK_handler_fn = systickhandler;

//    handler = (hal_true == hal_base_hid_params.arch.systick_useexternal_handler) ? (SysTick_Handler) : (s_hal_sys_SYSTICK_handler_fn);

    // setup a timer for execution of handler every period micro-seconds 
    r = s_hal_sys_SysTick_Config(s_hal_sys_SysTick_Handler, period, priority);
    return(r);
}
#endif

extern hal_void_fp_void_t hal_sys_systick_gethandler(void)
{
    return(s_hal_sys_SYSTICK_handler_fn);
    //return(NULL);
}


extern void hal_sys_irq_disable(void) 
{
    __asm__ volatile("disi #0x3FFF");   // disable all interrupts
}    


extern void hal_sys_irq_enable(void) 
{
    __asm__ volatile("disi #0x0000");  // enables all interrupts    
}


extern void hal_sys_irqn_disable(hal_irqn_t irqn) 
{
    s_hal_sys_irqn_write(irqn, 0);
}    


extern void hal_sys_irqn_enable(hal_irqn_t irqn) 
{
    s_hal_sys_irqn_write(irqn, 1);     
}  

extern void hal_sys_irqn_priority_set(hal_irqn_t irqn, hal_interrupt_priority_t prio) 
{
    uint8_t dspicprio = (uint8_t)prio;
    
    if(hal_int_priorityNONE == prio)
    {
        s_hal_sys_irqn_prioset(irqn, 0);    // on dspic 0 disable the isr
    }
    
    if(dspicprio > 7)
    {
        dspicprio = 7;
    }
    dspicprio = 7 - dspicprio;              // on dspic 1 is less priotity than 2, 3, 4, and 5 whcih is the highest interruptible priority 
    
    s_hal_sys_irqn_prioset(irqn, dspicprio);
      
} 

extern hal_interrupt_priority_t hal_sys_irqn_priority_get(hal_irqn_t irqn) 
{
    uint8_t prio = s_hal_sys_irqn_prioget(irqn);
    
    if((0 == prio) || (255 == prio))
    {
        return(hal_int_priorityNONE);
    }
    
    prio = 7 - prio;
    
    return((hal_interrupt_priority_t)prio);
}

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

#ifdef HAL_USE_SYSTICK
// handler for timer-1, which is used as sys-tick by the system
static void s_hal_sys_SysTick_Handler(void *p)
{
//    if(NULL != s_hal_sys_SYSTICK_handler_fn)
//    {
//        s_hal_sys_SYSTICK_handler_fn();
//    }
    SysTick_Handler();
}
#endif

#ifdef HAL_USE_SYSTICK
static hal_result_t s_hal_sys_SysTick_Config(void (*handler)(void *p), hal_time_t period, hal_interrupt_priority_t priority)
{
    // in here we encapsulate the code required to activate timer-0 every period micro-seconds so that
    // the callback is the passed handler()
    
    hal_time_t error;
    
    hal_timer_cfg_t timer_cfg =
    {
        .prescaler          = hal_timer_prescaler8,
        .countdown          = 0,
        .priority           = priority,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL    
    };
    
    timer_cfg.countdown        = period;
    timer_cfg.priority         = priority;
    timer_cfg.callback_on_exp  = handler;
    
 
    hal_timer_init(hal_timer1, &timer_cfg, &error);
    
    hal_timer_start(hal_timer1);

    
    return(hal_res_OK);
}
#endif


static void s_hal_sys_SystemInit(void)
{
    s_hal_sys_gpio_init();

    s_hal_sys_osc_config();
    
    //enable nested interrupt
    INTCON1bits.NSTDIS = 0;

    s_hal_sys_dspengine_config(hal_base_hid_params.arch.dspengine_moltipl_unsigned, hal_base_hid_params.arch.dspengine_enable_fractional_mode);

}


static void s_hal_sys_osc_config(void)
{
    uint32_t dpll;
    
    dpll = hal_base_hid_params.cpu_freq / hal_base_hid_params.arch.pll_freq;

    // acemor: removed. i trust the cpu_freq and the pll_freq
//    dpll = ( FREQ_OSC / pllin_mhz);

    if(0b011 == OSCCONbits.COSC)
    {
        // already running on PLL (set by bootloader)
        // TODO decide wheter to trust the bootloader or
        // to switch back to EC or FRC and reconfigure
        return;
    }

    PLLFBD = (dpll * 4 - 2);
    CLKDIVbits.PLLPOST = 0;            // N1=2
    CLKDIVbits.PLLPRE = 0;            // N2=2
    __builtin_write_OSCCONH(0x03);    // Initiate Clock Switch to Primary Oscillator (EC) with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(0x01);    // Start clock switching
    
    while(OSCCONbits.COSC != 0b011);  // Wait for PLL to lock
    while(OSCCONbits.LOCK != 1);
}

static void s_hal_sys_gpio_init(void)
{
    // set everything as input
    LATA  = 0x0000;
    TRISA = 0xFFFF;

    LATB  = 0x0000;
    TRISB = 0xFFFF;
    
    AD1PCFGL = 0xffff;
}


static void s_hal_sys_dspengine_config(hal_boolval_t moltipl_unsigned, hal_boolval_t enable_fractional_mode)
{

    if(hal_true == moltipl_unsigned )
    {
        CORCONbits.US  = 1; //  DSP engine multiplies are unsigned
    }
    else
    {
        CORCONbits.US  = 0;
    }

    if(hal_true == enable_fractional_mode)
    {
        CORCONbits.IF  = 0;  
    }
    else
    {
        CORCONbits.IF  = 1; 
    }


    // enable saturation on DSP data write
    CORCONbits.SATDW = 1;
    // DISABLE saturation on accumulator A.
    CORCONbits.SATA  = 0;
    
    // do not do super saturation (probably don't care because SATA = 0
    CORCONbits.ACCSAT  = 0;

    // conventional rounding mode
    CORCONbits.RND  = 1;
}


static void s_hal_sys_irqn_write(hal_irqn_t irqn, uint8_t bit)
{
    switch(irqn)
    {
        case hal_arch_dspic_TIM1_IRQn:
        {
            IEC0bits.T1IE       = bit;
        } break;  
        case hal_arch_dspic_DMA0_IRQn:
        {
            IEC0bits.DMA0IE     = bit;
        } break;        
        case hal_arch_dspic_TIM2_IRQn:
        {
            IEC0bits.T2IE       = bit;
        } break; 
        case hal_arch_dspic_TIM3_IRQn:
        {
            IEC0bits.T3IE       = bit;
        } break;
        case hal_arch_dspic_TIM4_IRQn:
        {
            IEC1bits.T4IE       = bit;
        } break; 
        case hal_arch_dspic_TIM5_IRQn:
        {
            IEC1bits.T5IE       = bit;
        } break; 
        case hal_arch_dspic_DMA1_IRQn:
        {
            IEC0bits.DMA1IE     = bit;
        } break;
        case hal_arch_dspic_C1RX_IRQn:
        {
            IEC2bits.C1RXIE     = bit;
        } break;        
        case hal_arch_dspic_CAN1_IRQn:
        {
            IEC2bits.C1IE       = bit;
        } break;
    }
}


static void s_hal_sys_irqn_prioset(hal_irqn_t irqn, uint8_t prio)
{
    switch(irqn)
    {
        case hal_arch_dspic_TIM1_IRQn:
        {
            IPC0bits.T1IP       = prio;
        } break;  
        case hal_arch_dspic_DMA0_IRQn:
        {
            IPC1bits.DMA0IP     = prio;
        } break;
        case hal_arch_dspic_TIM2_IRQn:
        {
            IPC1bits.T2IP       = prio;
        } break; 
        case hal_arch_dspic_TIM3_IRQn:
        {
            IPC2bits.T3IP       = prio;
        } break;
        case hal_arch_dspic_TIM4_IRQn:
        {
            IPC6bits.T4IP       = prio;
        } break; 
        case hal_arch_dspic_TIM5_IRQn:
        {
            IPC7bits.T5IP       = prio;
        } break; 
        case hal_arch_dspic_DMA1_IRQn: 
        {
            IPC3bits.DMA1IP     = prio;
        } break;
        case hal_arch_dspic_C1RX_IRQn:
        {
            IPC8bits.C1RXIP     = prio;
        } break;        
        case hal_arch_dspic_CAN1_IRQn:
        {
            IPC8bits.C1IP       = prio;
        } break;
    }
}

static uint8_t s_hal_sys_irqn_prioget(hal_irqn_t irqn)
{
    uint8_t prio = 255;
    switch(irqn)
    {
        case hal_arch_dspic_TIM1_IRQn:
        {
            prio    = IPC0bits.T1IP;
        } break;  
        case hal_arch_dspic_DMA0_IRQn:
        {
            prio    = IPC1bits.DMA0IP;
        } break;
        case hal_arch_dspic_TIM2_IRQn:
        {
            prio    = IPC1bits.T2IP;
        } break; 
        case hal_arch_dspic_TIM3_IRQn:
        {
            prio    = IPC2bits.T3IP;
        } break;
        case hal_arch_dspic_TIM4_IRQn:
        {
            prio    = IPC6bits.T4IP;
        } break; 
        case hal_arch_dspic_TIM5_IRQn:
        {
            prio    = IPC7bits.T5IP;
        } break; 
        case hal_arch_dspic_DMA1_IRQn: 
        {
            prio    = IPC3bits.DMA1IP;
        } break;
        case hal_arch_dspic_C1RX_IRQn:
        {
            prio    = IPC8bits.C1RXIP;
        } break;        
        case hal_arch_dspic_CAN1_IRQn:
        {
            prio    = IPC8bits.C1IP;
        } break;
    }
    
    return(prio);
}

#endif//HAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



