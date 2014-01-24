/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hal_cpu.c
	@brief      This file keeps implementation of the base hal module for stm32.
	@author     marco.accame@iit.it
    @date       02/27/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CPU

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"

#include "hal_middleware_interface.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_cpu.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_cpu_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_cpu_cfg_t hal_cpu_cfg_default = { .dummy = 0 };



#if     defined(HAL_USE_CPU_NAM_STM32F107)
extern uint32_t SystemCoreClock     =  72000000; //HSI_VALUE;
#elif   defined(HAL_USE_CPU_NAM_STM32F407)
extern uint32_t SystemCoreClock     = 168000000; //HSI_VALUE;
#else
    #error ERR --> define systemcoreclock for the mpu
#endif


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
  
typedef struct
{
    uint8_t     nothing;
} hal_cpu_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_cpu_clocks_set(void);

static void s_hal_cpu_set_sys_clock(void);
static void s_hal_cpu_system_core_clock_update(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// static hal_cpu_theinternals_t s_hal_cpu_theinternals =
// {
//     .nothing     = 0
// };

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_cpu_init(const hal_cpu_cfg_t* cfg)
{
    if(NULL == cfg)
    {
        cfg  = &hal_cpu_cfg_default;
    } 
   
    return(hal_res_OK);   
}

extern hal_result_t hal_cpu_clocks_set(void)
{

    s_hal_cpu_clocks_set();
    
    return(hal_res_OK);   
}


extern hal_cpu_architecture_t hal_cpu_architecture_get(void)
{
    return(hal_brdcfg_cpu__theconfig.architecture);
}


extern hal_cpu_family_t hal_cpu_family_get(void)
{
    return(hal_brdcfg_cpu__theconfig.family);
}


extern hal_cpu_name_t hal_cpu_name_get(void)
{
    return(hal_brdcfg_cpu__theconfig.name);
}


extern uint32_t hal_cpu_speed_get(hal_cpu_speedtype_t speedtype)
{
    uint32_t res  = 0;
    
    switch(speedtype)
    {
        case hal_cpu_speedtype_max:         res = hal_brdcfg_cpu__theconfig.speeds.max;         break;
        case hal_cpu_speedtype_cpu:         res = hal_brdcfg_cpu__theconfig.speeds.cpu;         break;
        case hal_cpu_speedtype_fastbus:     res = hal_brdcfg_cpu__theconfig.speeds.fastbus;     break;
        case hal_cpu_speedtype_slowbus:     res = hal_brdcfg_cpu__theconfig.speeds.slowbus;     break;
        default:                            res = 0;                                            break;        
    }
    
    return(res);   
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



extern hal_result_t hal_cpu_hid_static_memory_init(void)
{
     return(hal_res_OK); 
}



// -- redefinition of some functions weakly defined in cmsis

#if     defined(HAL_USE_CPU_FAM_STM32F1)

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

    if(hal_false == hal_brdcfg_cpu__theconfig.clockcfg.keepinternalclockatstartup)
    {   // apply the clock straigth away
        s_hal_cpu_set_sys_clock();
        s_hal_cpu_system_core_clock_update();        
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
            if(0 == hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed)
            {
                hal_base_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "SystemCoreClockUpdate() called with HSE mode but HSE clock is not defined");
            }
            SystemCoreClock = hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed;
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
                SystemCoreClock = (hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed / prediv1factor) * pllmull; 
    #else //defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
                /* HSE selected as PLL clock entry */
                if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != (uint32_t)RESET)
                {/* HSE oscillator clock divided by 2 */
                    SystemCoreClock = (hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed >> 1) * pllmull;
                }
                else
                {
                    SystemCoreClock = hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed * pllmull;
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
                    SystemCoreClock = (hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed / prediv1factor) * pllmull;          
                }
                else
                {/* PLL2 clock selected as PREDIV1 clock entry */
              
                    /* Get PREDIV2 division factor and PLL2 multiplication factor */
                    prediv2factor = ((RCC->CFGR2 & RCC_CFGR2_PREDIV2) >> 4) + 1;
                    pll2mull = ((RCC->CFGR2 & RCC_CFGR2_PLL2MUL) >> 8 ) + 2; 
                    SystemCoreClock = (((hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed / prediv2factor) * pll2mull) / prediv1factor) * pllmull;                         
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

#elif   defined(HAL_USE_CPU_FAM_STM32F4)
  
#warning ---> we dont redefine SystemInit()
#if 0
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

    if(hal_false == hal_brdcfg_cpu__theconfig.clockcfg.keepinternalclockatstartup)
    {   // apply the clock straigth away
        s_hal_cpu_set_sys_clock();
        s_hal_cpu_system_core_clock_update();        
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
            if(0 == hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed)
            {
                hal_base_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "SystemCoreClockUpdate() called with HSE mode but HSE clock is not defined");
            }
            SystemCoreClock = hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed;
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
                if(0 == hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed)
                {
                    hal_base_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "SystemCoreClockUpdate() called with PLL w/ HSE ref mode but HSE clock is not defined");
                }
                pllvco = (hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
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

#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_cpu_clocks_set(void)
{
    // if we use hal, then at startup the system runs a SystemInit() which is not the original one, but one re-defined
    // in hal_cpu.c 
    // the redefined one may either (a) just configures the internal clock without any prescaling for internal buses,
    // or (b) perform a full initialisation. the choice of (a) happens if hal_brdcfg_cpu__theconfig.clockcfg.keepinternalclockatstartup
    // is true.
    
    // hence, in here we need either to (a) configure a proper clock and update the system core clock, or (b) just update the clocks.
    
    
    if(hal_true == hal_brdcfg_cpu__theconfig.clockcfg.keepinternalclockatstartup)
    {
        s_hal_cpu_set_sys_clock();
        s_hal_cpu_system_core_clock_update();
    }
    else
    {
        s_hal_cpu_system_core_clock_update();
    }
}


#if     defined(HAL_USE_CPU_FAM_STM32F1)

static void s_hal_cpu_set_sys_clock(void)
{   // only valid for cpu speed of 72 mhz, and 72/36 fast/slow bus
    
    if((72000000                                != hal_brdcfg_cpu__theconfig.speeds.cpu)       ||
       (72000000                                != hal_brdcfg_cpu__theconfig.speeds.fastbus)   ||
       (36000000                                != hal_brdcfg_cpu__theconfig.speeds.slowbus)   ||
       (hal_cpu_refclock_pll_on_external_xtl    != hal_brdcfg_cpu__theconfig.clockcfg.refclock)            ||
       (25000000                                != hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed) 
      )
    {
            hal_base_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "s_hal_cpu_set_sys_clock() supports only 72MHz and external xtl of 25MHz");
        
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


#elif   defined(HAL_USE_CPU_FAM_STM32F4)

static void s_hal_cpu_set_sys_clock(void)
{
    
    // in here we can make many verifications ...  but we just dont care and use 
    // data defined in the brdcfg file   
    
    
    // --- if specified, then init the external clock: xtal or oscillator
    
    if(0 != hal_brdcfg_cpu__theconfig.clockcfg.extclockspeed)
    {
        __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
            
        // we enable the bypass only if we want using an external oscillator, either directly or through pll
        if( (hal_cpu_refclock_external_osc        == hal_brdcfg_cpu__theconfig.clockcfg.refclock) ||
            (hal_cpu_refclock_pll_on_external_osc == hal_brdcfg_cpu__theconfig.clockcfg.refclock) )
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
    
    uint8_t factorfast = hal_brdcfg_cpu__theconfig.speeds.cpu / hal_brdcfg_cpu__theconfig.speeds.fastbus;   
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

    uint8_t factorslow = hal_brdcfg_cpu__theconfig.speeds.cpu / hal_brdcfg_cpu__theconfig.speeds.slowbus;    
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
        
    if( (hal_cpu_refclock_pll_on_internal     == hal_brdcfg_cpu__theconfig.clockcfg.refclock) ||
        (hal_cpu_refclock_pll_on_external_xtl == hal_brdcfg_cpu__theconfig.clockcfg.refclock) ||
        (hal_cpu_refclock_pll_on_external_osc == hal_brdcfg_cpu__theconfig.clockcfg.refclock) )
    {
        uint32_t pll_m = hal_brdcfg_cpu__theconfig.clockcfg.pllcfg.m;
        uint32_t pll_n = hal_brdcfg_cpu__theconfig.clockcfg.pllcfg.n;
        uint32_t pll_p = hal_brdcfg_cpu__theconfig.clockcfg.pllcfg.p;
        uint32_t pll_q = hal_brdcfg_cpu__theconfig.clockcfg.pllcfg.q;

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
   

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
    
    
    // --- configure the system source clock
    
    uint32_t clockcfg = 0;
    uint32_t clocksta = 0;
    switch(hal_brdcfg_cpu__theconfig.clockcfg.refclock)
    {  
        default:
        case hal_cpu_refclock_internal:
        {
            clockcfg = RCC_CFGR_SW_HSI;  
            clocksta = RCC_CFGR_SWS_HSI;            
        } break;
               
        case hal_cpu_refclock_external_xtl:
        case hal_cpu_refclock_external_osc:
        {
            clockcfg = RCC_CFGR_SW_HSE;          
            clocksta = RCC_CFGR_SWS_HSE; 
        } break;                
                       
        case hal_cpu_refclock_pll_on_internal:
        case hal_cpu_refclock_pll_on_external_xtl:
        case hal_cpu_refclock_pll_on_external_osc:
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

#else
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif

static void s_hal_cpu_system_core_clock_update(void)
{
    SystemCoreClockUpdate();
}



#endif//HAL_USE_CPU

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



