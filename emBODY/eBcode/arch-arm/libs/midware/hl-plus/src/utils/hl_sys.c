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


/* @file       hl_sys.c
	@brief      This file contains internal implementation of the hl sys utilities.
	@author     marco.accame@iit.it
    @date       10/21/2013
**/


// - modules to be built: contains the HL_USE_* macros ----------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_UTIL_SYS)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"


#include "hl_core.h"


extern uint32_t SystemCoreClock;
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_sys_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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

//static uint8_t s_hl_sys_howmanyARMv7ops(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of __asm functions
// --------------------------------------------------------------------------------------------------------------------

__asm static int s_hl_sys_asm_getstacksize (void) {
        IMPORT  Stack_Size
        LDR     R0,=Stack_Size
        BX      LR
}


__asm static int s_hl_sys_asm_getheapsize (void) {
        IMPORT  Heap_Size
        LDR     R0,=Heap_Size
        BX      LR
}

// it should use 4 cycles per iteration: subs 1, and bne 3. however, we have 1.25 gain of cortex, and access to flash whcih may slows it.
__asm static void s_hl_sys_asm_xnumARMv7ops(uint32_t numberof) 
{
   align
dowaitloop
   subs r0,r0,#1
   bne dowaitloop
   bx lr 
   align    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t hl_sys_stack_totalsize_get(void)
{
    return(s_hl_sys_asm_getstacksize());   
}


extern uint32_t hl_sys_heap_totalsize_get(void)
{
    return(s_hl_sys_asm_getheapsize());   
}

__weak extern void* hl_sys_heap_new(uint32_t size)
{
    return(calloc(size, 1));    
}


__weak extern void hl_sys_heap_delete(void* p)
{
    free(p);
}


extern hl_result_t hl_sys_delay(hl_reltime_t reltime)
{
    static uint64_t s_hl_sys_numofops1sec = HSI_VALUE;
    static uint32_t s_hl_sys_used_systemcoreclock = 0;
    if(s_hl_sys_used_systemcoreclock != SystemCoreClock)
    {

        // to occupy a millisec i execute an operation for a number of times which depends on: 
        // SystemCoreClock, cortex gain(1.25 dmips/mhz), flash access, etc.
        // to overcome all this i just consider SystemCoreClock w/out 1.25 gain and i measures
        // extra gain with on a simple assembly function which should take 4 cycles per iteration (?).      
        //s_hl_sys_numofops1sec = (5*(SystemCoreClock)) / 4; 
        s_hl_sys_used_systemcoreclock = SystemCoreClock;
        s_hl_sys_numofops1sec = SystemCoreClock;
        
       
#if     defined(HL_USE_MPU_NAME_STM32F103RB)
        s_hl_sys_numofops1sec /= 3;  
        // ok, but flah may be slower than cpu speed. lets see what latency is used. (prefetching seems not affect exec speed)       
        uint32_t flash_latency = FLASH->ACR & FLASH_ACR_LATENCY; 
        if(FLASH_ACR_LATENCY_0 == flash_latency)
        {
            // no loss of speed
        }
        else if(FLASH_ACR_LATENCY_1 == flash_latency)
        {
            s_hl_sys_numofops1sec *= 3;
            s_hl_sys_numofops1sec /= 4;
        }
        else if(FLASH_ACR_LATENCY_2 == flash_latency)
        {   // 2 wait states: cpu too fast for flash.
            s_hl_sys_numofops1sec /= 2;
        }
#elif   defined(HL_USE_MPU_NAME_STM32F107VC)
        s_hl_sys_numofops1sec /= 3;  
        // ok, but flah may be slower than cpu speed. lets see what latency is used. (prefetching seems not affect exec speed)       
        uint32_t flash_latency = FLASH->ACR & FLASH_ACR_LATENCY; 
        if(FLASH_ACR_LATENCY_0 == flash_latency)
        {
            // no loss of speed
        }
        else if(FLASH_ACR_LATENCY_1 == flash_latency)
        {
            s_hl_sys_numofops1sec *= 3;
            s_hl_sys_numofops1sec /= 4;
        }
        else if(FLASH_ACR_LATENCY_2 == flash_latency)
        {   // 2 wait states: cpu too fast for flash.
            s_hl_sys_numofops1sec /= 2;
        }
#elif   defined(HL_USE_MPU_NAME_STM32F407IG)
        s_hl_sys_numofops1sec /= 3;             
        // with art technology enabled the flash is seen as fast as the cpu. wow.     
#else //defined(HL_USE_MPU_NAME_*)
    #error ERROR --> choose a HL_USE_MPU_NAME_*
#endif 

        // at this point i normalise the variable to keep not the nymber of operations for 1 sec,
        // but for 1024*1024 microsec. by doing so, later on i shift by 20 instead of using a division. 
        s_hl_sys_numofops1sec <<= 20;
        s_hl_sys_numofops1sec /= 1000000;
    }
    
    
    volatile uint64_t num = s_hl_sys_numofops1sec * reltime;
    num >>= 20; 
    //num -= offset; //we may remove some cycles to compensates for previous instructions, but ... we dont do it. it depends on c compiler optimisation 
    if(0 == num)
    {
        return(hl_res_OK);
    }
    s_hl_sys_asm_xnumARMv7ops((uint32_t)num);
    
    return(hl_res_OK);
}


extern uint32_t hl_sys_sysclock_get(void) 
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);  
    return(clocks.SYSCLK_Frequency);    
} 

extern hl_result_t hl_sys_systemreset(void) 
{
    NVIC_SystemReset();   
    return(hl_res_NOK_generic);
}

extern hl_result_t hal_sys_canjump(uint32_t addr)
{    
    if(((*(volatile uint32_t*)addr) & 0x2FFE0000 ) == 0x20000000)
    {
        return(hl_res_OK);
    }
    else 
    {
        return(hl_res_NOK_generic);
    }
}


extern hl_result_t hl_sys_jumpto(uint32_t addr) 
{
    volatile uint32_t jumpaddr;
    void (*app_fn)(void) = NULL;

    if(hl_res_NOK_generic == hal_sys_canjump(addr))
    {
        return(hl_res_NOK_generic);
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
    return(hl_res_NOK_generic);   
}

extern void hl_sys_irq_disable(void) 
{
    __disable_irq();
}    


extern void hl_sys_irq_enable(void) 
{
     __enable_irq();    
}


extern void hl_sys_irqn_disable(hl_irqn_t irqn) 
{
    NVIC_DisableIRQ((IRQn_Type)irqn);
}    


extern void hl_sys_irqn_enable(hl_irqn_t irqn) 
{
     NVIC_EnableIRQ((IRQn_Type)irqn);    
}  

extern void hl_sys_irqn_priority_set(hl_irqn_t irqn, hl_irqpriority_t prio) 
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)     
    if(hl_irqpriorityNONE == prio)
    {
        return;
    }
#endif  
    
    NVIC_SetPriority((IRQn_Type)irqn, prio);    
} 

extern hl_irqpriority_t hl_sys_irqn_priority_get(hl_irqn_t irqn) 
{
    return((hl_irqpriority_t)NVIC_GetPriority((IRQn_Type)irqn));    
}


extern int hl_sys_itm_puts(const char* str) 
{    
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)     
    if(NULL == str)
    {
        return(0);
    }
#endif 
    
    uint32_t ch;
    int num = 0;
    while('\0' != (ch = *str))
    {
        ITM_SendChar(ch);
        str++;
        num++;
    }
     
    ITM_SendChar('\n');
    return(++num);    
}

extern uint64_t hl_sys_uniqueid64bit_get(void)
{
#if     defined(HL_USE_MPU_ARCH_STM32F1)
    #define UniqueDeviceID96_baseaddress 0x1FFFF7E8
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
    #define UniqueDeviceID96_baseaddress 0x1FFF7A10
#else 
    #error ERR --> choose a HL_USE_MPU_ARCH_STM32F*
#endif     
    
    uint64_t val = *((const uint64_t *) (UniqueDeviceID96_baseaddress));
    val += *((const uint32_t *) (UniqueDeviceID96_baseaddress+8));
    if((0xFFFFFFFFFFFFFFFF+0xFFFFFFFF) == val)
    {   // 0xFFFFFFFE (=0xFFFFFFFFFFFFFFFF+0xFFFFFFFF) is the result when all the 96 bits have value 1 (some old versions of stm32f107 have such a hw bug)
        return(hl_NA64);
    }
    else
    {
        return(val);
    }    
}


__weak extern void hl_sys_on_warning(const char * warningmsg)
{
    hl_sys_itm_puts(warningmsg);
    //for(;;);
}

__weak extern void hl_sys_on_error(hl_errorcode_t errorcode, const char * errormsg)
{
    errorcode = errorcode;
    hl_sys_itm_puts(errormsg);
    if(hl_error_warning == errorcode)
    {
        return;
    }
    for(;;);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// static uint8_t s_hl_sys_howmanyARMv7ops(void)
// {
// #if     defined(HL_USE_MPU_ARCH_STM32F1)
//     //return(3+3);
//     return(3+3+3);      // number seems to be rather empirical. can anybody help me finding a rule?
// #elif   defined(HL_USE_MPU_ARCH_STM32F4)
//     //return(3+1);
//     return 3;
// #else //defined(HL_USE_MPU_ARCH_*)
//     #error ERROR --> choose a HL_USE_MPU_ARCH_*
// #endif  
// }


#endif//defined(HL_USE_UTIL_SYS)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





