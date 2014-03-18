/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HAL_SYS_H_
#define _HAL_SYS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_sys.h
    @brief      This header file implements public interface to the hal system module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal_sys HAL system

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct hal_sys_cfg_t 
    @brief      contains the configuration for the sys module. 
 **/ 
typedef struct
{
    uint32_t        stacksize;                  /**< The size of stack available to the system in bytes     */
    uint32_t        heapsize;                   /**< The size of heap available to the system in bytes      */
} hal_sys_cfg_t;
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_sys_cfg_t hal_sys_cfg_default;  // = { .stacksize = 0, .heapsize = 0 };



// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_result_t hal_sys_init(const hal_sys_cfg_t* cfg)
    @brief      This function initialises the sys module. It is to be called ONLY by HAL internals
    @return     hal_res_OK on success, otherwise an error code.
  */
extern hal_result_t hal_sys_init(const hal_sys_cfg_t* cfg);


/** @fn         extern uint32_t hal_sys_stack_get_totalsize(void);
    @brief      It returns size of configured stack.
    @return     The size of configured stack in bytes.
 **/
extern uint32_t hal_sys_stack_get_totalsize(void);


/** @fn         extern uint32_t hal_sys_heap_get_totalsize(void);
    @brief      It returns size of configured heap.
    @return     The size of configured heap in bytes.
 **/
extern uint32_t hal_sys_heap_get_totalsize(void);


/** @fn         extern hal_result_t hal_sys_delay(hal_reltime_t reltime)
    @brief      When called it returns after the CPU has executed this function for @e reltime microseconds.
                In this time it is not counted the time the system has spent inside ISRs or in other tasks.
    @param      reltime     the minumum time spent inside this funtion in microseconds                
    @return     hal_res_OK, or hal_res_NOK_generic if hal_sys_systeminit() was not already succesfully called.
 **/
extern hal_result_t hal_sys_delay(hal_reltime_t reltime);


/** @fn         extern hal_result_t hal_sys_systemreset(void)
    @brief      Force a reset of the system.
    @return     It should not return because a reset of the system is invoked, but hal_res_NOK_generic if it does not reset
 **/
extern hal_result_t hal_sys_systemreset(void);


/** @fn         extern hal_result_t hal_sys_canexecuteataddress(uint32_t addr)
    @brief      Tells if it is possible to start the system at a given address. It is used by the application loader
                in a multi-application system.
    @param      addr        The address of start of the application
    @return     hal_res_OK if in @e addr there is a valid application, or hal_res_NOK_generic otherwise.
 **/
extern hal_result_t hal_sys_canexecuteataddress(uint32_t addr);


/** @fn         extern hal_result_t hal_sys_executenowataddress(uint32_t addr)
    @brief      Tells if it is possible to start the system at a given address. It is used by the application loader
                in a multi-application system.
    @param      addr        The address of start of the application
    @return     It does not return if in @e addr there is a valid application. It returns hal_res_NOK_generic otherwise.
 **/
extern hal_result_t hal_sys_executenowataddress(uint32_t addr);


/** @fn         extern hal_result_t hal_sys_vectortable_relocate(uint32_t offset)
    @brief      Relocates the vector table at a given a given offset from base startup address of FLASH.
                It is used by the application loaded by a loader if it executes at address @e addr, and is called
                with @e offset = @e addr - @e hal_flash_get_baseaddress().
                in a multi-application system.
    @param      offset          From base address in FLASH
    @warning    MUST be called after hal_sys_systeminit() otherwise the vector table will be put again at @e offset zero. 
    @return     hal_res_OK or hal_res_NOK_unsupported if the feature is not supported
 **/
extern hal_result_t hal_sys_vectortable_relocate(uint32_t offset);


/** @fn         extern hal_result_t hal_sys_systick_sethandler(void (*systickhandler)(void), hal_reltime_t period, hal_interrupt_priority_t priority)
    @brief      Starts a periodic call every @e period usec to the passed @e systickhandler function. On ARM Cortex
                MPUs the caller is the SysTick ISR. The SysTick_Handler() function is internally defined as __weak, 
                so that it can be overriden by any other definition (e.g., by the one inside osal).
    @param      systickhandler  The called function. 
    @param      period          The calling period in usec (micro-seconds).
    @param      priority        The priority of the handler (if the architecture allows).
    @return     hal_res_OK, or hal_res_NOK_generic if hal_initialise() was not already succesfully called or id parameters
                are invalid.
 **/
extern hal_result_t hal_sys_systick_sethandler(void (*systickhandler)(void), hal_reltime_t period, hal_interrupt_priority_t priority);


/** @fn         extern hal_void_fp_void_t hal_sys_systick_gethandler(void)
    @brief      Retrieves the pointer to the syst tick handler. It can used to take outside this function in MPUs
                such as the Cortex Mx where the systick handlere must have a well defined name as SysTick_Handler().
    @return     The pointer to the sys tick handler of NULL. 
 **/
extern hal_void_fp_void_t hal_sys_systick_gethandler(void);


/** @fn         extern void hal_sys_irq_disable(void)
    @brief      Disable every IRQ in the system. 
 **/
extern void hal_sys_irq_disable(void);


/** @fn         extern void hal_sys_irq_enable(void)
    @brief      Enable every IRQ in the system. 
 **/
extern void hal_sys_irq_enable(void);


/** @fn         extern void hal_sys_irqn_disable(hal_irqn_t irqn)
    @brief      Disable a selected IRQ in the system.
    @param      irqn        The irq number to operate on.  Use an enum value in hal_cpu_fam_xxx_irqn_t
                            or hal_cpu_arc_xxx_irqn_t
 **/
extern void hal_sys_irqn_disable(hal_irqn_t irqn);


/** @fn         extern void hal_sys_irqn_enable(hal_irqn_t irqn)
    @brief      Enable a selected IRQ in the system.
    @param      irqn        The irq number to operate on.  Use an enum value in hal_cpu_fam_xxx_irqn_t
                            or hal_cpu_arc_xxx_irqn_t
 **/
extern void hal_sys_irqn_enable(hal_irqn_t irqn);


/** @fn         extern void hal_sys_irqn_priority_set(hal_irqn_t irqn, hal_interrupt_priority_t prio)
    @brief      Sets a priority to the selected IRQ in the system.
    @param      irqn        The irq number to operate on.  Use an enum value in hal_cpu_fam_xxx_irqn_t
                            or hal_cpu_arc_xxx_irqn_t
    @param      prio        The target priority
 **/
extern void hal_sys_irqn_priority_set(hal_irqn_t irqn, hal_interrupt_priority_t prio); 


/** @fn         extern hal_interrupt_priority_t hal_sys_irqn_priority_get(hal_irqn_t irqn)
    @brief      Sets a priority to the selected IRQ in the system.
    @param      irqn        The irq number to operate on.  Use an enum value in hal_cpu_fam_xxx_irqn_t
                            or hal_cpu_arc_xxx_irqn_t
    @return     The current priority of the IRQ
 **/
extern hal_interrupt_priority_t hal_sys_irqn_priority_get(hal_irqn_t irqn);
 
 
/** @fn         extern hal_boolval_t hal_sys_irq_is_running(void)
    @brief      Tells if the caller is an ISR.
    @return     hal_true or hal_false
 **/ 
extern hal_boolval_t hal_sys_irq_is_running(void); 

 
/** @fn         extern hal_result_t hal_sys_criticalsection_take(void *p, hal_reltime_t tout)
    @brief      Takes exclusive control of the system so that no ISR will interrupt the thread of execution contained
                between a _take() and a _release(). This function can be called multiple times with no harm, so that the
                protection vs. ISR can be nested in several layers of function calls.
    @param      p               A dummy pointer required by some EmbObj object. 
    @param      tout            A dummy timeout required by some EmbObj object.
    @return     Always hal_res_OK.
 **/
extern hal_result_t hal_sys_criticalsection_take(void *p, hal_reltime_t tout);


/** @fn         extern hal_result_t hal_sys_criticalsection_release(void *p)
    @brief      Releases the exclusive control of the system which was taken by hal_IRQsafe_criticalsection_take().
                In case of multiple calls to _take(), the systems is released only after the same number of calls to
                _release().
    @param      p               A dummy pointer required by some EO objects. 
    @return     Always hal_res_OK.
 **/
extern hal_result_t hal_sys_criticalsection_release(void *p);


/** @fn         extern void hal_sys_atomic_bitwiseOR(volatile uint32_t *value, uint32_t mask)
    @brief      Performs the following atomic bitwise OR: *value |= mask 
    @param      value           Pointer to the variable which will contain the @e mask. 
    @param      mask            Contains the bits to be inserted in @e value. 
 **/
extern void hal_sys_atomic_bitwiseOR(volatile uint32_t *value, uint32_t mask);


/** @fn         extern void hal_sys_atomic_bitwiseAND(volatile uint32_t *value, uint32_t mask)
    @brief      Performs the following atomic bitwise AND: *value &= mask 
    @param      value           Pointer to the variable which will be masked. 
    @param      mask            Contains the bits which will mask @e value. 
 **/
extern void hal_sys_atomic_bitwiseAND(volatile uint32_t *value, uint32_t mask);

 

/** @}            
    end of group doxy_group_hal_sys  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



