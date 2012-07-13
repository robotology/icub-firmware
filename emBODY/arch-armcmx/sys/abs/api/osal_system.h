/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _OSAL_SYSTEM_H_
#define _OSAL_SYSTEM_H_


/** @file       osal_system.h
	@brief      This header file implements public interface to the os abstraction layer: system
	@author     marco.accame@iit.it
	@date       12/05/2011
**/

/* @defgroup osal_system OS abstraction layer: system
    cxecevcevce
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "osal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern void osal_system_start(void (*launcher_fn)(void))
    @brief      Start the scheduler which executes the function launcher_fn() inside the launcher task with priority 
                osal_prio_systsk_launcher (254). 
                The launcher_fn() function is responsible to start any other task and either returns or adjust its priority
                to a value between osal_prio_usrtsk_min and osal_prio_usrtsk_max and runs as a normal task. 
                At the end of the launcher_fn(), the system shall have the following tasks:
                (a) the scheduler with priority osal_prio_systsk_scheduler (255), (b) the idle task with priority
                osal_prio_systsk_idle (0), (c) the user-defined tasks with user priorities, and possibly (d) the launcher 
                task with user priority.
    @param      launcher_fn     Pointer to the launcher function. The function is responsible to init all required osal
                                data structures and to start tasks.  Finally it shall return or enter a forever loop.  
                                This function runs at highest priority in the system (osal_prio_systsk_launcher = 254) 
                                and allows other tasks to execute only when it returns or blocks itself in a forever loop 
                                or if its priority is lowered. 
    @param      launcherstacksize   Dimension in bytes if the stack used by the launcher task which runs launcher_fn.
    @return     The function never returns. If launcher_fn is NULL it calls the error manager, otherwise it starts the 
                scheduler.
 **/
extern void osal_system_start(void (*launcher_fn)(void));


/** @fn         extern void * osal_system_launcherbuffer_get(uint16_t *size)
    @brief      Retrieves the memory area which was used by the launcher task as its stack.  This function
                can be called only by an ordinary running task. It has a protection which returns NULL if the
                launcher tasks has not been launched yet or if it is still running. 
                after .
    @param      size        Points to the size of the returned memory.
    @return     The required memory or NULL.
 **/
extern void * osal_system_launcherbuffer_get(uint16_t *size);


/** @fn         extern void osal_system_ticks_abstime_set(osal_abstime_t tot)
    @brief      Sets the total time of life in micro seconds but truncated to 
                the lowest multiple of the configured tick time.
    @param      tot     The target time of life in usec.
 **/
extern void osal_system_ticks_abstime_set(osal_abstime_t tot);


/** @fn         extern osal_abstime_t osal_system_ticks_abstime_get(void)
    @brief      Returns total time of life in micro seconds but multiples of 
                the configured tick time
    @return     The time of life in usec.
 **/
extern osal_abstime_t osal_system_ticks_abstime_get(void);


/** @fn         extern osal_abstime_t osal_system_abstime_get(void)
    @brief      Returns total time of life in micro seconds with the best possible resolution.
    @return     The time of life in usec.
 **/
extern osal_abstime_t osal_system_abstime_get(void);


/** @fn         extern osal_nanotime_t osal_system_nanotime_get(void)
    @brief      Retrieves total time of life in nano seconds with the best possible resolution.
    @return     The time of life expressed in nano-seconds
 **/
extern osal_nanotime_t osal_system_nanotime_get(void);


/** @fn         extern void osal_system_scheduling_suspend(void)
    @brief      Suspends the scheduler. The calling task shall have exclusive execution privileges until it calls
                the function osal_system_scheduling_restart(). During scheduling suspension the system time does not
                advance.
                In case the OSAL has not started yet, this function can be safely called and simply does nothing. 
                This function can safely be called inside an ISR
    @warning    During the time the task has disabled scheduling any osal_* call which forces a wait or a block of the
                task is prohibited and if done shall cause a fatal error with error code osal_error_invalidcall.
 **/
extern void osal_system_scheduling_suspend(void);


/** @fn         extern void osal_system_scheduling_restart(void)
    @brief      Restarts the scheduling.
                This function can safely be called inside an ISR
                In case the OSAL has not started yet, this function can be safely called and simply does nothing. 
 **/
extern void osal_system_scheduling_restart(void);



/* @}            
    end of group osal_system  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

