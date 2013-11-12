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

/** @typedef    typedef enum hl_interrupt_priority_t;
    @brief      keeps the possible values for interrupt priorities (0 is highest)

 **/
typedef enum
{
    hl_int_priorityNONE     = hl_NA08,
    hl_int_priority00       = 0,
    hl_int_priority01       = 1,
    hl_int_priority02       = 2,
    hl_int_priority03       = 3,
    hl_int_priority04       = 4,
    hl_int_priority05       = 5,
    hl_int_priority06       = 6,
    hl_int_priority07       = 7,
    hl_int_priority08       = 8,
    hl_int_priority09       = 9,
    hl_int_priority10       = 10,
    hl_int_priority11       = 11,
    hl_int_priority12       = 12,
    hl_int_priority13       = 13,
    hl_int_priority14       = 14,
    hl_int_priority15       = 15
} hl_interrupt_priority_t;  


/** @typedef    typedef int32_t hl_irqn_t
    @brief      It expresses the IRQ number. Use however the specific type of the used MPU: IRQn_Type.
 **/ 
typedef int32_t hl_irqn_t;  


/** @typedef    typedef struct hl_sys_speeds_t 
    @brief      hl_sys_speeds_t contains the main speeds of the system 
 **/
typedef struct
{
    uint32_t            cpuspeed;   /**< the cpu speed in Hz */
    uint32_t            fastbus;    /**< the fast bus in Hz */
    uint32_t            slowbus;    /**< the slow bus in Hz */
} hl_sys_speeds_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// it must be defined externally to be coherent with the chosen cpu speed and slow and fast bus
extern const hl_sys_speeds_t hl_sys_speeds;


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
    @param      reltime     the minimum time spent inside this function in microseconds                
    @return     hl_res_OK upon success.
 **/
extern hl_result_t hl_sys_delay(hl_reltime_t reltime);


/** @fn         extern hl_result_t hl_sys_systemreset(void)
    @brief      Force a reset of the system.
    @return     It should not return because a reset of the system is invoked, but hl_res_NOK_generic if it does not reset
 **/
extern hl_result_t hl_sys_systemreset(void);


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
    @param      irqn        The irq number to operate on.  Use an enum value in IRQn_Type
                            or hl_cpu_arc_xxx_irqn_t
 **/
extern void hl_sys_irqn_enable(hl_irqn_t irqn);


/** @fn         extern void hl_sys_irqn_priority_set(hl_irqn_t irqn, hl_interrupt_priority_t prio)
    @brief      Sets a priority to the selected IRQ in the system.
    @param      irqn        The irq number to operate on.  Use an enum value in IRQn_Type
                            or hl_cpu_arc_xxx_irqn_t
    @param      prio        The target priority
 **/
extern void hl_sys_irqn_priority_set(hl_irqn_t irqn, hl_interrupt_priority_t prio); 


/** @fn         extern int hl_sys_itm_puts(const char* str)
    @brief      Puts a zero-terminated string plus a '\n' into ITM peripheral
    @param      char        The zero-terminated string to print
    @return     The number of printed characters.
 **/
extern int hl_sys_itm_puts(const char* str);


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



