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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HL_SYS_H_
#define _HL_SYS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_sys.h
    @brief      This header file implements system utilities for hl library
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

/** @defgroup doxy_group_hl_sys HL SYS utilities

    The HL SYS ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hl_irqpriority_t;
    @brief      keeps the possible values for interrupt priorities (0 is highest)

 **/
typedef enum
{
    hl_irqpriorityNONE      = hl_NA08,
    hl_irqpriority00        = 0,
    hl_irqpriority01        = 1,
    hl_irqpriority02        = 2,
    hl_irqpriority03        = 3,
    hl_irqpriority04        = 4,
    hl_irqpriority05        = 5,
    hl_irqpriority06        = 6,
    hl_irqpriority07        = 7,
    hl_irqpriority08        = 8,
    hl_irqpriority09        = 9,
    hl_irqpriority10        = 10,
    hl_irqpriority11        = 11,
    hl_irqpriority12        = 12,
    hl_irqpriority13        = 13,
    hl_irqpriority14        = 14,
    hl_irqpriority15        = 15
} hl_irqpriority_t;  


/** @typedef    typedef int32_t hl_irqn_t
    @brief      It expresses the IRQ number. Use however the specific type of the used MPU: IRQn_Type.
 **/ 
typedef int32_t hl_irqn_t;  



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern uint32_t hl_sys_stack_totalsize_get(void)
    @brief      It returns size of configured stack.
    @return     The size of configured stack in bytes.
 **/
extern uint32_t hl_sys_stack_totalsize_get(void);


/** @fn         extern uint32_t hl_sys_heap_totalsize_get(void)
    @brief      It returns size of configured heap.
    @return     The size of configured heap in bytes.
 **/
extern uint32_t hl_sys_heap_totalsize_get(void);


/** @fn         extern void* hl_sys_heap_new(uint32_t size)
    @brief      It returns size bytes from heap. zeroed, as in calloc().
                It is weakly defined so that one can override it.
    @param      size        The requested bytes
    @return     The memory or NULL in case of failure.
 **/
extern void* hl_sys_heap_new(uint32_t size);


/** @fn         extern void hl_sys_heap_delete(void* p)
    @brief      It gives back the memory pointer by p to the heap, as in free().
                It is weakly defined so that one can override it.
    @param      p           The pointer to the heap memory to be released
 **/
extern void hl_sys_heap_delete(void* p);


/** @fn         extern hl_result_t hl_sys_delay(hl_reltime_t reltime)
    @brief      When called it returns after the CPU has executed this function for @e reltime microseconds.
                In this time it is not counted the time the system has spent inside ISRs or in other tasks.
                Apart from any of above interruptions, the precision of this delay is about 1 micro-second,
                depending on compiler optimisation (because some parts of the function are still in C). 
                On the STM32F407 running at 168Mhz with -O4speed optimisation the function lasts 0.45 
                micro-sec more.               
    @param      reltime     the minimum time spent inside this function in microseconds. If 0, it lasts 0.45 micro.
                            The max allowed time is 60 sec.    
    @return     hl_res_OK upon success.
    @warning    IT MUST BE CALLED AFTER that SystemCoreClockUpdate() has been called. The reason is that the
                function hl_sys_delay() internally computes how many simple operations to execute on the basis of  
                the value of SystemCoreClock.
 **/
extern hl_result_t hl_sys_delay(hl_reltime_t reltime);

extern uint32_t hl_sys_sysclock_get(void);


/** @fn         extern hl_result_t hl_sys_systemreset(void)
    @brief      Force a reset of the system.
    @return     It should not return because a reset of the system is invoked, but hl_res_NOK_generic if it does not reset
 **/
extern hl_result_t hl_sys_systemreset(void);

extern hl_result_t hal_sys_canjump(uint32_t addr);

extern hl_result_t hl_sys_jumpto(uint32_t addr);


/** @fn         extern void hl_sys_irq_disable(void)
    @brief      Disable every IRQ in the system. 
 **/
extern void hl_sys_irq_disable(void);


/** @fn         extern void hl_sys_irq_enable(void)
    @brief      Enable every IRQ in the system. 
 **/
extern void hl_sys_irq_enable(void);


/** @fn         extern void hl_sys_irqn_disable(hl_irqn_t irqn)
    @brief      Disable a selected IRQ in the system.
    @param      irqn        The irq number to operate on.  Use an enum value in IRQn_Type
                            or hl_cpu_arc_xxx_irqn_t
 **/
extern void hl_sys_irqn_disable(hl_irqn_t irqn);


/** @fn         extern void hl_sys_irqn_enable(hl_irqn_t irqn)
    @brief      Enable a selected IRQ in the system.
    @param      irqn        The irq number to operate on.  
 **/
extern void hl_sys_irqn_enable(hl_irqn_t irqn);


/** @fn         extern void hl_sys_irqn_priority_set(hl_irqn_t irqn, hlirqpriority_t prio)
    @brief      Sets a priority to the selected IRQ in the system.
    @param      irqn        The irq number to operate on.  
    @param      prio        The target priority
 **/
extern void hl_sys_irqn_priority_set(hl_irqn_t irqn, hl_irqpriority_t prio); 


/** @fn         extern hl_irqpriority_t hl_sys_irqn_priority_get(hl_irqn_t irqn)
    @brief      Gets the priority of the selected IRQ in the system.
    @param      irqn        The irq number to operate on.  
    @return     the priority    
 **/
extern hl_irqpriority_t hl_sys_irqn_priority_get(hl_irqn_t irqn);


/** @fn         extern int hl_sys_itm_puts(const char* str)
    @brief      Puts a zero-terminated string plus a '\n' into ITM peripheral
    @param      char        The zero-terminated string to print
    @return     The number of printed characters.
 **/
extern int hl_sys_itm_puts(const char* str);


/** @fn         extern uint64_t hl_sys_uniqueid64bit_get(void)
    @brief      returns a 64 bit number that is unique amongst all MPUs.
    @return     The unique number or hl_NA64 if such a feature is not available .
 **/
extern uint64_t hl_sys_uniqueid64bit_get(void);



/** @fn         extern void hl_sys_on_warning(const char * warningmsg)
    @brief      It is called by HL when a runtime warning is issued, but it can be called by a HL user as well. 
                The function is weakly defined so that the default implementation can be overridden by the user. 
                The default implementation simply prints erromsg to ITM and returns.
    @param      warningmsg      The warning message.
 **/
extern void hl_sys_on_warning(const char * warningmsg);


/** @fn         extern void hl_sys_on_error(hl_errorcode_t errorcode, const char * errormsg)
    @brief      It is called by HL when an internal error occurs, but it can be called by a HL user as well. 
                The function is weakly defined so that the default implementation can be overridden by the user. 
                The default implementation simply prints erromsg to ITM and enters in a forever loop.
    @param      errorcode       The error code. 
    @param      errormsg        The error message.
 **/
extern void hl_sys_on_error(hl_errorcode_t errorcode, const char * errormsg);
 

/** @}            
    end of group doxy_group_hl_sys  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



