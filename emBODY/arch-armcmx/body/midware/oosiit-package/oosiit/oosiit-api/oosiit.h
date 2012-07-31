/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _OOSIIT_H_
#define _OOSIIT_H_


#ifdef __cplusplus
extern "C" {
#endif


/** @file       oosiit.h
	@brief      This header file implements public interface to the IIT extension to open source CMSIS-RTOS RTX kernel
	@author     marco.accame@iit.it
	@date       07/20/2012
**/

/* @defgroup oosiit IIT extension to open source CMSIS-RTOS RTX kernel
    The IIT extension to the open source CMSIS-RTOS RTX kernel adds support to time with 32 bits range, event flags 
    extended to 32 bits, plus advanced timers with individual callback execution and reload capabilities. 
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define OOSIIT_NOTIMEOUT    0xffffffff
#define OOSIIT_ASAPTIME     0xffffffffffffffff


// - declaration of public user-defined types ------------------------------------------------------------------------- 


//typedef uint32_t OOSIIT_MUT[3];

typedef enum
{
    oosiit_res_OK       = 0x00,
    oosiit_res_NOK      = 0xff,
    oosiit_res_TMO      = 0x01,
    oosiit_res_EVT      = 0x02,
    oosiit_res_SEM      = 0x03,
    oosiit_res_MBX      = 0x04,
    oosiit_res_MUT      = 0x05
} oosiit_result_t;

typedef uint32_t oosiit_taskid_t; 

typedef void* oosiit_objptr_t;

typedef struct
{  
    // from cmsis-rtx
    uint8_t         numTask;                                
    uint8_t         numTaskWithUserProvidedStack;           
    uint16_t        sizeStack;                              
    uint8_t         checkStack;             // bool                
    uint8_t         priviledgeMode;         // bool               
    uint32_t        osClock;                                
    uint32_t        osTick;                                 
    uint8_t         roundRobin;             // bool                    
    uint32_t        roundRobinTimeout;                      
    uint8_t         numTimer;                               
    uint8_t         sizeISRFIFO;                            
    // iit extension
    uint8_t         numAdvTimer;                            
    uint8_t         numMutex;                               
    uint8_t         numSemaphore;                           
    uint8_t         numMessageBox;                          
    uint16_t        numMessageBoxElements;                  
    uint16_t        numElements64Stack;                     
} oosiit_params_cfg_t;


typedef enum
{
    oosiit_evt_wait_mode_any        = 0,
    oosiit_evt_wait_mode_all        = 1
} oosiit_evt_wait_mode_t;


typedef struct
{
    uint64_t        startat;
    uint32_t        firstshot;
    uint32_t        othershots;
} oosiit_advtmr_timing_t;


typedef struct
{
    void(*cbk)(void* tmr, void* par);
    void *par;
} oosiit_advtmr_action_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// memory functions

/** @fn         extern void oosiit_memory_getsize(const oosiit_params_cfg_t *cfg, uint16_t *num04aligned, uint16_t *num08aligned)
    @brief      Gives back the size in bytes of the memory required by the oosiit. 
    @param      cfg             A pointer to a configuration. If NULL, return values are non-valid (both zero).
    @param      size04aligned   The number of bytes of four-aligned required memory.
    @param      size08aligned   The number of bytes of eigth-aligned required memory.
 **/ 
extern void oosiit_memory_getsize(const oosiit_params_cfg_t *cfg, uint16_t *size04aligned, uint16_t *size08aligned);


/** @fn         extern uint32_t oosiit_memory_set(const oosiit_params_cfg_t *cfg, uint32_t *data04aligned, uint64_t *data08aligned)
    @brief      Loads to the internals of oosiit the references to the configuration and of required memory. 
                If any is NULL, any successive attempt to run the oosiit will fail due to lack of memory.
    @param      cfg             A pointer to the wanted configuration.
    @param      data04aligned   The four-aligned required memory.
    @param      data08aligned   The eigth-aligned required memory.
    @return     0 upon success, 1 if any of the argument is NULL.
 **/ 
extern uint32_t oosiit_memory_load(const oosiit_params_cfg_t *cfg, uint32_t *data04aligned, uint64_t *data08aligned);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// system functions

extern oosiit_result_t oosiit_sys_init(void (*inittskfn)(void), uint8_t inittskpriority, void* inittskstackdata, uint16_t inittskstacksize);

extern oosiit_result_t oosiit_sys_suspend(void); // former tsk_lock

extern oosiit_result_t oosiit_sys_resume(void); // former tsk_unlock


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// miscellanea


/** @fn         extern uint64_t* osiit_stack_getmem(uint16_t bytes)
    @brief      Gets @e bytes of memory to be used as stack for a task. If @e bytes is not a multiple
                of four the memory returned will be the smallest multiple of four bigger than @e bytes.   
    @param      bytes        The size in bytes. It must be a multiple of four.
    @return     Not NULL pointer to memory, NULL if no memory is available.
 **/ 
extern uint64_t* oosiit_stack_getmem(uint16_t bytes);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// task functions

extern oosiit_taskid_t oosiit_tsk_create(void (*tskfn)(void *), void *tskfnarg, uint8_t tskpriority, void *tskstackdata, uint16_t tskstacksize);
extern oosiit_result_t oosiit_tsk_delete(oosiit_taskid_t tskid);
extern oosiit_taskid_t oosiit_tsk_self(void);
extern oosiit_result_t oosiit_tsk_prio(oosiit_taskid_t tskid, uint8_t tskpriority);
extern oosiit_result_t oosiit_tsk_pass(void);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// time functions

/** @fn         extern uint64_t oosiit_time_get(void)
    @brief      Returns the time since last bootstrap in system ticks.
    @return     System ticks since bootstrap.
 **/
extern uint64_t oosiit_time_get(void);


/** @fn         extern oosiit_result_t oosiit_time_set(uint64_t target)
    @brief      Changes by brute-force the time since last bootstrap in system ticks.
    @param      target          The target time in sys ticks.
 **/
extern oosiit_result_t oosiit_time_set(uint64_t target);


/** @fn         extern uint64_t oosiit_microtime_get(void)
    @brief      Returns the number of microseconds elapsed since last bootstrap.
    @return     microseconds since bootstrap.
 **/
extern uint64_t oosiit_microtime_get(void);


/** @fn         extern uint64_t oosiit_nanotime_get(void)
    @brief      Gets the number of nanoseconds elapsed since last bootstrap
    @return     nanoseconds since bootstrap.
 **/
extern uint64_t oosiit_nanotime_get(void);


///** @fn         extern void oosiit_secsnano_get(uint32_t* secs, uint32_t* nano)
//    @brief      Gets the number of seconds and nanoseconds elapsed since last bootstrap. 
//                Any of the arguments can safely be NULL.
//    @param      secs            Number of seconds. It can be NULL.
//    @param      nano            Number of nano-seconds. It can be NULL.
// **/
//extern void oosiit_secsnano_get(uint32_t* secs, uint32_t* nano);


///** @fn         extern uint64_t oosiit_idletime_get(void)
//    @brief      Returns the time spent inside idle task since last bootstrap in system ticks.
//    @return     System ticks of idle time since bootstrap.
// **/
//extern uint64_t oosiit_idletime_get(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// delay functions


/** @fn         extern void oosiit_dly_wait(uint32_t delay)
    @brief      Pauses the calling task for a specified interval
    @param      delay      the delay.
 **/
extern void oosiit_dly_wait(uint32_t delay);


/** @fn         extern void oosiit_itv_set(uint32_t period)
    @brief      Sets up the calling task for periodic wake-up 
    @param      period      the period.
 **/
extern void oosiit_itv_set(uint32_t period);


/** @fn         extern void oosiit_itv_wait(void)
    @brief      Waits for the period set with oosiit_itv_set() to expire.
 **/
extern void oosiit_itv_wait(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mailbox functions

//extern oosiit_objptr_t oosiit_mbx_getmem(uint16_t numofmessages); // osiit_

//extern void oosiit_mbx_init(oosiit_objptr_t mailbox, uint16_t numofmessages); // attenzione ... rtx richiede il numero di bytes che e': 16+(4*numofmessages) !!!

extern oosiit_objptr_t oosiit_mbx_create(uint16_t numofmessages); // osiit_


/** @fn         extern oosiit_result_t oosiit_mbx_retrieve(oosiit_objptr_t mailbox, void **message, uint32_t timeout)
    @brief      Gets a pointer to a message from the mailbox. The function can be called from inside an ISR. In such a case it is used as if timeout were zero.
    @param      mailbox         the mailbox.
    @param      message         location where to store the message pointer
    @param      timeout         the timeout.
    @return     oosiit_res_OK upon immediate success, oosiit_res_MBX upon success after some wait time, oosiit_res_TMO if timeout failure
                (or if there is no msg when called by an ISR), oosiit_res_NOK if mailbox is invalid
 **/
extern oosiit_result_t oosiit_mbx_retrieve(oosiit_objptr_t mailbox, void** message, uint32_t timeout); 


/** @fn         extern oosiit_result_t oosiit_mbx_send (oosiit_objptr_t mailbox, void* message, uint32_t timeout)
    @brief      puts the pointer to a message inside the mailbox.
    @param      mailbox         the mailbox.                                                 
    @param      message         the message
    @param      timeout         the timeout
    @return     oosiit_res_OK upon success, oosiit_res_TMO if timeout failure 
 **/ 
extern oosiit_result_t oosiit_mbx_send(oosiit_objptr_t mailbox, void* message, uint32_t timeout); // osiit_



/** @fn         extern uint16_t oosiit_mbx_available(oosiit_objptr_t mailbox)
    @brief      Gives the number of messages which can still be put inside the mailbox
                It can be called also by an ISR.
    @param      mailbox         The mailbox
    @return     The number of messages which can be put inside the mailbox. Zero if mailbox is full (or invalid).
 **/
extern uint16_t oosiit_mbx_available(oosiit_objptr_t mailbox);


/** @fn         extern uint16_t oosiit_mbx_used(oosiit_objptr_t mailbox)
    @brief      Gives the number of available messages stored inside the mailbox
                It can be called also by an ISR.
    @param      mailbox         The mailbox
    @return     The number of messages stored inside the mailbox. Zero if mailbox is empty (or invalid).
 **/
extern uint16_t oosiit_mbx_used(oosiit_objptr_t mailbox); 





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// event flag functions


/** @fn         extern oosiit_result_t oosiit_evt_set(uint32_t flags, oosiit_taskid_t tskid)
    @brief      It sends the event flags specified by @e flags to the task @e taskid. The function can be called also by an ISR.
    @param      flags           the mask of the event flags to be sent to the task.
    @param      tskid           the task.
    @return     oosiit_res_NOK if the specified task is invalid, oosiit_res_OK in all other cases.
 **/ 
extern oosiit_result_t oosiit_evt_set(uint32_t flags, oosiit_taskid_t tskid);


/** @fn         extern oosiit_result_t oosiit_evt_wait(uint32_t waitflags,  uint32_t timeout, oosiit_evt_wait_mode_t waitmode)
    @brief      It forces the caller task to wait for the event flags specified by @e waitflags in the mode specified by @e waitmode
                with timeout specified by @e timeout.
    @param      waitflags       the mask of the event flags to be waited.
    @param      timeout         the timeout.
    @param      waitmode        if oosiit_evt_wait_mode_any, the function waits until any of the flags becomes available; 
                                else if oosiit_evt_wait_mode_all, it waits until all the flags become available
    @return     oosiit_res_TMO is the function exits for timeout. oosiit_res_EVT the condition specified by @e waitflag and @e waitmode
                is satisfied within the specifeid timeout. oosiit_res_NOK if the function is called from inside an ISR.
    @warning    the internal flags are not reset. the caller must retrieve them with oosiit_evt_get() and reset them with oosiit_evt_clr().
 **/ 
extern oosiit_result_t oosiit_evt_wait(uint32_t waitflags,  uint32_t timeout, oosiit_evt_wait_mode_t waitmode); 


/** @fn         extern uint32_t oosiit_evt_get(void)
    @brief      It retrieves the mask of event flags received by the calling task.
    @return     the mask of received event flags. 0 if called by an ISR.
 **/ 
extern uint32_t oosiit_evt_get(void);


/** @fn         extern oosiit_result_t oosiit_evt_clr(uint32_t flags)
    @brief      clears in the calling task all the events specified by @e flags . 
    @param      flags           the flag mask
    @return     oosiit_res_OK or oosiit_res_NOK if called from inside an ISR.
 **/ 
extern oosiit_result_t oosiit_evt_clr(uint32_t flags); 



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// semaphore functions


/** @fn         extern oosiit_objptr_t oosiit_sem_create(uint8_t maxtokens, uint8_t ntokens)
    @brief      It creates and initialises a semaphore to contain @e maxtokens and have its initial value set to @e ntokens. 
    @param      maxtokens       Maximum number of tokens.
    @param      ntokens         Number of tokens (must be lower or equal than @e maxtokens, otherwise it is clipped to its value)
    @return     the semaphore or NULL if no memory is available.

 **/ 
extern oosiit_objptr_t oosiit_sem_create(uint8_t maxtokens, uint8_t ntokens);


/** @fn         extern oosiit_objptr_t oosiit_sem_getmem(void)
    @brief      Gets memory for a semaphore
    @return     Not NULL pointer to memory, NULL if no memory is available.
 **/ 
//extern oosiit_objptr_t oosiit_sem_getmem(void);


/** @fn         extern void oosiit_sem_init(oosiit_objptr_t sem, uint8_t maxtokens, uint8_t ntokens)
    @brief      It initialise the memory of a semaphore to contain @e maxtokens and sets its initial value to @e ntokens. 
    @param      sem             the semaphore
    @param      maxtokens       Maximum number of tokens.
    @param      ntokens         Number of tokens (must be lower or equal than @e maxtokens, otherwise it is clipped to its value)
 **/ 
//extern void oosiit_sem_init(oosiit_objptr_t sem, uint8_t maxtokens, uint8_t ntokens);


/** @fn         extern oosiit_result_t oosiit_sem_set(oosiit_objptr_t sem, uint8_t ntokens)
    @brief      It sets the number of tokens to @e ntokens. The function wakes up a task that may wait if the
                number of tokens @e notokens is one or higher.
    @param      sem             As with the os_ version..
    @param      ntokens         Number of tokens.
    @warning    the semaphore must have been previously initialised with oosiit_sem_init().
    @return     oosiit_res_NOK if @ntokens is higher than the number of tokens that the semaphore can contain, oosiit_res_NOK
                if the semaphore is invalid or the function is called from inside an ISR. oosiit_res_OK otherwise
 **/ 
extern oosiit_result_t oosiit_sem_set(oosiit_objptr_t sem, uint8_t ntokens);



/** @fn         extern oosiit_result_t oosiit_sem_send(oosiit_objptr_t sem)
    @brief      It adds a token to the semaphore, but it fails if the number of tokens inside
                has reached its maximum number
    @param      sem             The handler to the semaphore
    @return     oosiit_res_NOK if the number of tokens are already the maximum number, or oosiit_res_OK otherwise
 **/ 
extern oosiit_result_t oosiit_sem_send(oosiit_objptr_t sem);


/** @fn         extern oosiit_result_t oosiit_sem_wait(oosiit_objptr_t sem, uint32_t timeout)
    @brief      It removes a token from the semaphore with a given timeout.
    @param      sem             The handler to the semaphore
    @param      timeout         maximum wait time.
    @return     oosiit_res_OK upon immediate success, oosiit_res_SEM upon success after some wait time, oosiit_res_TMO if timeout failure,
                finally oosiit_res_OK if called from an ISR or if the semaphore is invalid
 **/ 
extern oosiit_result_t oosiit_sem_wait(oosiit_objptr_t sem, uint32_t timeout);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mutex functions



extern oosiit_objptr_t oosiit_mut_create(void);

/** @fn         extern oosiit_objptr_t oosiit_mut_getmem(void)
    @brief      Gets memory for a mutex
    @return     Not NULL pointer to memory, NULL if no memory is available.
 **/ 
//extern oosiit_objptr_t oosiit_mut_getmem(void);


/** @fn         extern void oosiit_mbx_init(oosiit_objptr_t mutex)
    @brief      Initialises memory for a mutex
 **/
//extern void oosiit_mbx_init(oosiit_objptr_t mutex); 


/** @fn         extern oosiit_result_t oosiit_mut_wait(oosiit_objptr_t mutex, uint32_t timeout)
    @brief      It takes a mutex with priority inheritance.
    @param      mutex           the handle to the mutex
    @param      timeout         maximum wait time.
    @return     oosiit_res_OK upon immediate success, oosiit_res_MUT upon success after some wait time, oosiit_res_TMO if timeout failure. 
                oosiit_res_NOK on invalid mutex or if the function is called from inside an ISR.
 **/
extern oosiit_result_t oosiit_mut_wait(oosiit_objptr_t mutex, uint32_t timeout);


/** @fn         extern oosiit_result_t oosiit_mut_release(oosiit_objptr_t mutex)
    @brief      It releases a mutex.
    @param      mutex           the handle to the mutex
    @return     oosiit_res_OK upon success, oosiit_res_NOK upon error (i.e., the caller is not the same task which called oosiit_mut_wait() or
                there is unbalanced release).
 **/
extern oosiit_result_t oosiit_mut_release(oosiit_objptr_t mutex);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// advanced timer functions


    
/** @fn         extern oosiit_objptr_t oosiit_advtmr_new(void)
    @brief      Creates but NOT starts an advanced timer.
    @return     The handle to the advanced timer or NULL upon failure. The function cannot be called from within an ISR.
 **/
extern oosiit_objptr_t oosiit_advtmr_new(void);


/** @fn         extern oosiit_result_t oosiit_advtmr_start(oosiit_objptr_t timer, oosiit_advtmr_timing_t *timing, oosiit_advtmr_action_t *action)
    @brief      Starts an advanced timer which expires for the first time after timing->firstshot ticks 
                and the other times after timing->othershots ticks. At each expiry it calls the
                user-defined callback function action->cbk(timer, action->arg). If timing->othershots is zero then the timer 
                is one-shot. If timing->firstshot is zero the first shot is not executed.
                Moreover, if timing->startat is equal to ASAPTIME then the start of the timer is relative to the moment it is
                called the oosiit_advtmr_start() function. Otherwise everything is shifter to begin at the absolute time given 
                by timing->startat
                The functions can be called also from within an ISR with a limitation: there can not be two or more calls to the same 
                timer inside any ISR before the post-ISR is processed. In such a case the timer is simply not started.
    @param      timing          keeps the timing information. The values in timing->firstshot and timing->othershots indicates
                                when the expiry shall happen relative to teh value timing->startat.
                                If timing->startat is equal to ASAPTIME, then the timer is relative to the calling time of
                                oosiit_advtmr_start(), otehrwise to teh value in timing->startat.
                                In this latter case, any expiry of a single-shot due in the past relative to current absolute 
                                time is not going to cause any trigger. In case of periodic timer, instead the rsult is that
                                the expiries can happen at given times equispaced by timing->othershots.
    @param      action          Specified the callback function executed at each expiry of the advanced timer and its second
                                argument. As this function is executed directly by the scheduler, some limitations arise: keep 
                                it short and calls only oosiit_isr* or isr* functions.
    @return     oosiit_res_OK if the timer was succesfully started or oosiit_res_NOK otherwise.
    @warning    A call of oosiit_time_set() that changes the current time shall also shifts the timers started with timing->startat
                different from ASAPTIME.
 **/
extern oosiit_result_t oosiit_advtmr_start(oosiit_objptr_t timer, oosiit_advtmr_timing_t *timing, oosiit_advtmr_action_t *action);


/** @fn         extern oosiit_result_t oosiit_advtmr_stop(oosiit_objptr_t timer)
    @brief      Stops an advanced timer. 
                The functions can be called also from within an ISR with a limitation: there can not be two or more calls to the same 
                timer inside any ISR before the post-ISR is processed. In such a case the timer is simply not stopped.
    @param      timer           the advanced timer.
    @return     oosiit_res_OK if the timer was succesfully stopped or oosiit_res_NOK otherwise.
 **/
extern oosiit_result_t oosiit_advtmr_stop(oosiit_objptr_t timer);


/** @fn         extern oosiit_result_t oosiit_advtmr_isactive(oosiit_objptr_t timer)
    @brief      tells if an advanced timer is active or not (not yet started or started as single shot and expired
                or also stopped. The functions can be called also from within an ISR.
    @param      timer           the advanced timer.
    @return     oosiit_res_OK if the timer is active or oosiit_res_NOK otherwise.
 **/
extern oosiit_result_t oosiit_advtmr_isactive(oosiit_objptr_t timer);


/** @fn         extern oosiit_result_t oosiit_advtmr_delete(oosiit_objptr_t timer)
    @brief      Deletes an advanced timer.
                The functions can be called also from within an ISR with a limitation: there can not be two or more calls to the same 
                timer inside any ISR before the post-ISR is processed. In such a case the timer is simply not deleted.
    @param      timer           the advanced timer.
    @return     Always oosiit_res_OK 
 **/
extern oosiit_result_t oosiit_advtmr_delete(oosiit_objptr_t timer);



/* @}            
    end of group oosiit  
 **/
 
 
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

