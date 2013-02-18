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

/* @file       hal_mpu_sys.c
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

#include "hal_mpu_stm32xx_include.h"

#include "hal_base_hid.h" 
#include "hal_arch_arm.h"

#include "hal_brdcfg.h"
#include "hal_flash.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_sys_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#if     defined(USE_STM32F1)
extern uint32_t SystemCoreClock =  72000000; //HSI_VALUE;
#elif   defined(USE_STM32F4)
extern uint32_t SystemCoreClock = 168000000; //HSI_VALUE;
#endif


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_sys_set_sys_clock(uint32_t maxcpufreq);
static void s_hal_sys_system_core_clock_update(void);

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
    
    // if we use hal, then at startup the system runs a SystemInit() re-defined by hal which just configures
    // the internal clock and does not use any prescaling for internal buses.
    
    // hence, ... we need to configure a proper clock and update the system core clock 
    
    if(hal_true == hal_brdcfg_sys__theconfig.clockcfg.keepinternalclockatstartup)
    {
        s_hal_sys_set_sys_clock(hal_base_hid_params.cpu_freq);
        s_hal_sys_system_core_clock_update();
    }
    else
    {
        s_hal_sys_system_core_clock_update();
    }


//     hal_brdcfg_sys__clock_config();
//     hal_brdcfg_sys__gpio_default_init();

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

uint8_t hal_sys_howmanyARMv7ops(void)
{
#if     defined(USE_STM32F1)
    return(3+3);
#elif   defined(USE_STM32F4)
    return(3+1);
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

#if     defined(USE_STM32F1)

// redefinition of function weakly defined in system_stm32f10x.c so that the mpus uses hsi and runs at 8MHz (16MHz on CM4)
void SystemInit(void)
{

    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#ifndef STM32F10X_CL
    RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
    RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif /* STM32F10X_CL */   
  
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

#ifdef STM32F10X_CL
    /* Reset PLL2ON and PLL3ON bits */
    RCC->CR &= (uint32_t)0xEBFFFFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x00FF0000;

    /* Reset CFGR2 register */
    RCC->CFGR2 = 0x00000000;
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x009F0000;

    /* Reset CFGR2 register */
    RCC->CFGR2 = 0x00000000;      
#else
    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x009F0000;
#endif /* STM32F10X_CL */
    
// #if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
//     #ifdef DATA_IN_ExtSRAM
//         SystemInit_ExtMemCtl(); 
//     #endif /* DATA_IN_ExtSRAM */
// #endif 

//     /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
//     /* Configure the Flash Latency cycles and enable prefetch buffer */
//     SetSysClock();

// #ifdef VECT_TAB_SRAM
//     SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
// #else
//     SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
// #endif 

    SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH. */

    // now the HSI is selected as system clock, and no prescaler is used     

    if(hal_false == hal_brdcfg_sys__theconfig.clockcfg.keepinternalclockatstartup)
    {   // apply the clock straigth away
        s_hal_sys_set_sys_clock(72000000);
        s_hal_sys_system_core_clock_update();        
    }
    else
    {   // keep the hsi
        // update value ...
        SystemCoreClockUpdate();
    }
}

extern const volatile uint8_t AHBPrescTable[16];

// redefinition of the function weakly defined inside system_stm32f4xx.c
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0;

#ifdef  STM32F10X_CL
    uint32_t prediv1source = 0, prediv1factor = 0, prediv2factor = 0, pll2mull = 0;
#endif /* STM32F10X_CL */

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
    uint32_t prediv1factor = 0;
#endif /* STM32F10X_LD_VL or STM32F10X_MD_VL or STM32F10X_HD_VL */
    
    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;
  
    switch (tmp)
    {
        case 0x00:  /* HSI used as system clock */
        {    
            SystemCoreClock = HSI_VALUE;
        } break;
        
        case 0x04:  /* HSE used as system clock */
        {
            if(0 == hal_brdcfg_sys__theconfig.clockcfg.extclockspeed)
            {
                hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "SystemCoreClockUpdate() called with HSE mode but HSE clock is not defined");
            }
            SystemCoreClock = hal_brdcfg_sys__theconfig.clockcfg.extclockspeed;
        } break;
            
        case 0x08:  /* PLL used as system clock */
        {
            /* Get PLL clock source and multiplication factor ----------------------*/
            pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
            pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
          
#ifndef STM32F10X_CL      
            pllmull = ( pllmull >> 18) + 2;
          
            if(pllsource == 0x00)
            {
                /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
            }
            else
            {
    #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
                prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
                /* HSE oscillator clock selected as PREDIV1 clock entry */
                SystemCoreClock = (hal_brdcfg_sys__theconfig.clockcfg.extclockspeed / prediv1factor) * pllmull; 
    #else //defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
                /* HSE selected as PLL clock entry */
                if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != (uint32_t)RESET)
                {/* HSE oscillator clock divided by 2 */
                    SystemCoreClock = (hal_brdcfg_sys__theconfig.clockcfg.extclockspeed >> 1) * pllmull;
                }
                else
                {
                    SystemCoreClock = hal_brdcfg_sys__theconfig.clockcfg.extclockspeed * pllmull;
                }
    #endif //defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
            }
#else // it is defined STM32F10X_CL
            pllmull = pllmull >> 18;
          
            if (pllmull != 0x0D)
            {
                pllmull += 2;
            }
            else
            { /* PLL multiplication factor = PLL input clock * 6.5 */
                pllmull = 13 / 2; 
            }
                
            if (pllsource == 0x00)
            {
                /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
            }
            else
            {/* PREDIV1 selected as PLL clock entry */
            
                /* Get PREDIV1 clock source and division factor */
                prediv1source = RCC->CFGR2 & RCC_CFGR2_PREDIV1SRC;
                prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
            
                if (prediv1source == 0)
                { 
                    /* HSE oscillator clock selected as PREDIV1 clock entry */
                    SystemCoreClock = (hal_brdcfg_sys__theconfig.clockcfg.extclockspeed / prediv1factor) * pllmull;          
                }
                else
                {/* PLL2 clock selected as PREDIV1 clock entry */
              
                    /* Get PREDIV2 division factor and PLL2 multiplication factor */
                    prediv2factor = ((RCC->CFGR2 & RCC_CFGR2_PREDIV2) >> 4) + 1;
                    pll2mull = ((RCC->CFGR2 & RCC_CFGR2_PLL2MUL) >> 8 ) + 2; 
                    SystemCoreClock = (((hal_brdcfg_sys__theconfig.clockcfg.extclockspeed / prediv2factor) * pll2mull) / prediv1factor) * pllmull;                         
                }
            }
#endif // STM32F10X_CL
        } break;

        default:
        {
            SystemCoreClock = HSI_VALUE;
        } break;
    }     
  
    /* Compute HCLK clock frequency ----------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK clock frequency */
    SystemCoreClock >>= tmp;      
}

#elif   defined(USE_STM32F4)
    
// redefinition of function weakly defined in system_stm32f4xx.c so that the mpus uses hsi and runs at 16MHz
void SystemInit(void)
{

    /* FPU settings ------------------------------------------------------------*/
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
        SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    #endif

    
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;

//#ifdef DATA_IN_ExtSRAM
//  SystemInit_ExtMemCtl(); 
//#endif /* DATA_IN_ExtSRAM */
         
//  /* Configure the System clock source, PLL Multiplier and Divider factors, 
//     AHB/APBx prescalers and Flash settings ----------------------------------*/
//  SetSysClock();

  /* Configure the Vector Table location add offset address ------------------*/
//#ifdef VECT_TAB_SRAM
//  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
//#else
//  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
//#endif

    SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */
    
    // now the HSI is selected as system clock, and no prescaler is used     

    if(hal_false == hal_brdcfg_sys__theconfig.keepinternalclockatstartup)
    {   // apply the clock straigth away
        s_hal_sys_set_sys_clock(168000000);
        s_hal_sys_system_core_clock_update();        
    }
    else
    {   // keep the hsi
        // update value ...
        SystemCoreClockUpdate();
    }
}

extern const volatile uint8_t AHBPrescTable[16];

// redefinition of the function weakly defined inside system_stm32f4xx.c
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  
    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch(tmp)
    {
        case 0x00:  /* HSI used as system clock source */
        {
            SystemCoreClock = HSI_VALUE;
        } break;
        
        case 0x04:  /* HSE used as system clock source */
        {
            if(0 == hal_brdcfg_sys__theconfig.clockcfg.extclockspeed)
            {
                hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "SystemCoreClockUpdate() called with HSE mode but HSE clock is not defined");
            }
            SystemCoreClock = hal_brdcfg_sys__theconfig.clockcfg.extclockspeed;
        } break;
        
        case 0x08:  /* PLL used as system clock source */
        {
            /* PLL_VCO = (HSE_VALUE or HSI_VALUE /  HAL_SYS_PLL_M) *  HAL_SYS_PLL_N
                SYSCLK = PLL_VCO /  HAL_SYS_PLL_P
             */    
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
            pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      
            if (pllsource != 0)
            {
                /* HSE used as PLL clock source */
                if(0 == hal_brdcfg_sys__theconfig.clockcfg.extclockspeed)
                {
                    hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "SystemCoreClockUpdate() called with PLL w/ HSE ref mode but HSE clock is not defined");
                }
                pllvco = (hal_brdcfg_sys__theconfig.clockcfg.extclockspeed / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
            }
            else
            {
                /* HSI used as PLL clock source */
                pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
            }

            pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
            SystemCoreClock = pllvco/pllp;
            
        } break;
        
        default:
        {
            SystemCoreClock = HSI_VALUE;
        } break;
    }
    
    /* Compute HCLK frequency --------------------------------------------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK frequency */
    SystemCoreClock >>= tmp;
}

#endif


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

#if     defined(USE_STM32F1)

static void s_hal_sys_set_sys_clock(uint32_t maxcpufreq)
{   // only valid for 72 mhz
    
    if((72000000                                != hal_brdcfg_sys__theconfig.speeds.cpu)       ||
       (72000000                                != hal_brdcfg_sys__theconfig.speeds.fastbus)   ||
       (36000000                                != hal_brdcfg_sys__theconfig.speeds.slowbus)   ||
       (hal_sys_refclock_pll_on_external_xtl    != hal_brdcfg_sys__theconfig.clockcfg.refclock)            ||
       (25000000                                != hal_brdcfg_sys__theconfig.clockcfg.extclockspeed) 
      )
    {
            hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "s_hal_sys_set_sys_clock() supports only 72MHz and external xtl of 25MHz");
        
    }

    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
    /* Enable HSE */    
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;  
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }  

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    

     
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
          
        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
        
        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

#ifdef STM32F10X_CL
        /* Configure PLLs ------------------------------------------------------*/
        /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */
        
        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                                  RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                                 RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);
  
        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        
        /* Wait till PLL2 is ready */
        while((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }
    
   
        /* PLL configuration: PLLCLK = PREDIV1 * 9 = 72 MHz */ 
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | 
                            RCC_CFGR_PLLMULL9); 
#else    
        /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                            RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
#endif /* STM32F10X_CL */

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }
    
        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {    /* If HSE fails to start-up, the application will have wrong clock 
            configuration. User can add here some code to deal with this error */
    }
    
}


#elif   defined(USE_STM32F4)

static void s_hal_sys_set_sys_clock(uint32_t maxcpufreq)
{
    
    // in here we can make many verifications ...  but we just dont care and use 
    // data defined in the brdcfg file   
    
    
    // --- if specified, then init the external clock: xtal or oscillator
    
    if(0 != hal_brdcfg_sys__theconfig.clockcfg.extclockspeed)
    {
        __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
            
        // we enable the bypass only if we want using an external oscillator, either directly or through pll
        if( (hal_sys_refclock_external_osc        == hal_brdcfg_sys__theconfig.clockcfg.refclock) ||
            (hal_sys_refclock_pll_on_external_osc == hal_brdcfg_sys__theconfig.clockcfg.refclock) )
        {
            // enable hse bypass     
            RCC->CR |= ((uint32_t)RCC_CR_HSEBYP);
        }        
    
        // enable HSE 
        RCC->CR |= ((uint32_t)RCC_CR_HSEON);

        // wait till HSE is ready and if timeout is reached exit 
        do
        {
            HSEStatus = RCC->CR & RCC_CR_HSERDY;
            StartUpCounter++;
        } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

        if(RESET != (RCC->CR & RCC_CR_HSERDY))
        {
            HSEStatus = (uint32_t)0x01;
        }
        else
        {
            HSEStatus = (uint32_t)0x00;
        }


        // if HSE fails to start-up, the application will have wrong clock
        // configuration. user can add here some code to deal with this error

        if((uint32_t)0x00 == HSEStatus)
        {
            // we just do nothing. it means that we use the default clock which is already initted: the internal one.
            // we also do not configure any prescaler for the fast and slow bus.
            return;
        }
        
    }

    // --- configure regulator voltage
    // select regulator voltage output Scale 1 mode, system frequency up to 168 MHz
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
    
    
    // --- configure prescalers for internal buses
    
    
    uint32_t hpre_div    = RCC_CFGR_HPRE_DIV1;
    uint32_t ppre2_div   = 0; //RCC_CFGR_PPRE2_DIV2;
    uint32_t ppre1_div   = 0; //RCC_CFGR_PPRE1_DIV4;
    
    uint8_t factorfast = hal_brdcfg_sys__theconfig.speeds.cpu / hal_brdcfg_sys__theconfig.speeds.fastbus;   
    switch(factorfast)
    {   // APB2 is fast bus
        default:
        case 1: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV1;     
        } break;
        
        case 2: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV2;     
        } break; 
        
        case 4: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV4;       
        } break;  
        
        case 8: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV8;        
        } break;         
    }

    uint8_t factorslow = hal_brdcfg_sys__theconfig.speeds.cpu / hal_brdcfg_sys__theconfig.speeds.slowbus;    
    switch(factorslow)
    {   // APB2 is slow bus
        default:
        case 1: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV1;     
        } break;
        
        case 2: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV2;     
        } break; 
        
        case 4: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV4;       
        } break;  
        
        case 8: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV8;        
        } break;         
    }    
    
    // apply AHB (system) prescaler 
    RCC->CFGR |= hpre_div;      // RCC_CFGR_HPRE_DIV1;
    
    // apply APB2 (fast bus) prescaler  
    RCC->CFGR |= ppre2_div;     // RCC_CFGR_PPRE2_DIV2;
    
    // apply APB1 (slow bus) prescaler
    RCC->CFGR |= ppre1_div;     // RCC_CFGR_PPRE1_DIV4;
    
    
    // --- if required, configure the pll  
        
    if( (hal_sys_refclock_pll_on_internal     == hal_brdcfg_sys__theconfig.clockcfg.refclock) ||
        (hal_sys_refclock_pll_on_external_xtl == hal_brdcfg_sys__theconfig.clockcfg.refclock) ||
        (hal_sys_refclock_pll_on_external_osc == hal_brdcfg_sys__theconfig.clockcfg.refclock) )
    {
        uint32_t pll_m = hal_brdcfg_sys__theconfig.clockcfg.pllcfg.m;
        uint32_t pll_n = hal_brdcfg_sys__theconfig.clockcfg.pllcfg.n;
        uint32_t pll_p = hal_brdcfg_sys__theconfig.clockcfg.pllcfg.p;
        uint32_t pll_q = hal_brdcfg_sys__theconfig.clockcfg.pllcfg.q;

        // configure the main PLL 
        RCC->PLLCFGR =  pll_m | ( pll_n << 6) | ((( pll_p >> 1) -1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | ( pll_q << 24);

        // enable the main PLL 
        RCC->CR |= RCC_CR_PLLON;

        // wait till the main PLL is ready 
        while(0 == (RCC->CR & RCC_CR_PLLRDY))
        {
        }
    }
   
    // --- configure flash prefetch, instruction cache, data cache and wait state 
    
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
    
    
    // --- configure the system source clock
    
    uint32_t clockcfg = 0;
    uint32_t clocksta = 0;
    switch(hal_brdcfg_sys__theconfig.clockcfg.refclock)
    {  
        default:
        case hal_sys_refclock_internal:
        {
            clockcfg = RCC_CFGR_SW_HSI;  
            clocksta = RCC_CFGR_SWS_HSI;            
        } break;
               
        case hal_sys_refclock_external_xtl:
        case hal_sys_refclock_external_osc:
        {
            clockcfg = RCC_CFGR_SW_HSE;          
            clocksta = RCC_CFGR_SWS_HSE; 
        } break;                
                       
        case hal_sys_refclock_pll_on_internal:
        case hal_sys_refclock_pll_on_external_xtl:
        case hal_sys_refclock_pll_on_external_osc:
        {
            clockcfg = RCC_CFGR_SW_PLL;             
            clocksta = RCC_CFGR_SWS_PLL;
        } break;
        
    }
    
    // select the system clock source 
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= clockcfg;

    // wait till the chosen clock source is effective in clock status register
    while(clocksta != (RCC->CFGR & (uint32_t)RCC_CFGR_SWS ));
    {       
    }       
//     /* Select the main PLL as system clock source */
//     RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
//     RCC->CFGR |= RCC_CFGR_SW_PLL;

//     /* Wait till the main PLL is used as system clock source */
//     while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
//     {
//     }
  
}

#endif

static void s_hal_sys_system_core_clock_update(void)
{
    SystemCoreClockUpdate();
}


#endif//HAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



