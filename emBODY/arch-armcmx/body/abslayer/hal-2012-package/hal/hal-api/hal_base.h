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

#ifndef _HAL_BASE_H_
#define _HAL_BASE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_base.h
    @brief      This header file implements public interface to the common data structures and functions of hal
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal_base HAL base

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------

#define hal_NA08    ((uint8_t)(0xFF))
#define hal_NA16    ((uint16_t)(0xFFFF))
#define hal_NA32    ((uint32_t)(0xFFFFFFFF)) 
#define hal_NA64    ((uint64_t)(0xFFFFFFFFFFFFFFFF))

#define hal_BASE_U8toU32(a)                       ((uint32_t)(a)&0xff)
#define hal_BASE_U16toU32(a)                      ((uint32_t)(a)&0xffff)
#define hal_BASE_U8toU64(a)                       ((uint64_t)(a)&0xff)
#define hal_BASE_4BtoU32(a, b, c, d)              ((hal_BASE_U8toU32(a))|(hal_BASE_U8toU32(b)<<8)|(hal_BASE_U8toU32(c)<<16)|(hal_BASE_U8toU32(d)<<24)) 
#define hal_BASE_8BtoU64(a, b, c, d, e, f, g, h)  (((uint64_t)hal_BASE_4BtoU32(a, b, c, d))|((uint64_t)hal_BASE_4BtoU32(e, f, g, h)<<32)) 


// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef enum hal_result_t 
    @brief      hal_result_t contains all possible return values in functions.
 **/  
typedef enum 
{
    hal_res_OK                 = 0,
    hal_res_NOK_generic        = -1,
    hal_res_NOK_nullpointer   = -2,       /**< error: a null pointer was used */
//    hal_res_NOK_timeout       = -3,       /**< error: the caller (task) did not get the osal resource after it waited (even for zero time) */
//    hal_res_NOK_nowait        = -4,       /**< error: the calling isr did not get the osal resource when called the not waiting version of the function */ 
    hal_res_NOK_nodata        = -5,       /**< error: there was not data to be retrieved */
    hal_res_NOK_busy          = -6,       /**< error: the service was busy and request has not been served */
    hal_res_NOK_unsupported   = -15,      /**< error: unsupported feature */
    hal_res_NOK_wrongparam    = -16,      /**< error: one or more params are incorrect */
    hal_res_NOK_hwerror       = -17       /**< error: hardware error */
} hal_result_t;


/** @typedef    typedef enum hal_fatalerror_t 
    @brief      hal_fatalerror_t contains all possible fatal error codes.
 **/ 
typedef enum
{
    hal_fatalerror_generic                   = 0xE0,             
    hal_fatalerror_missingconfiguration      = 0xE1,       
    hal_fatalerror_missingmemory             = 0xE2,       
    hal_fatalerror_incorrectparameter        = 0xE3,       
    hal_fatalerror_unsupportedbehaviour      = 0xE4,
    hal_fatalerror_runtimefault              = 0xE5
} hal_fatalerror_t;


/** @typedef    typedef enum hal_boolval_t 
    @brief      hal_boolval_t contains values for bool type (hal_bool_t).
    @warning    C99 contains bool, true, and false. To use C99 include "stdbool.h" and -c99 compiler option.
                At this point please redefine eo_false and eo_true to be equal to false and true.
                Also, eObool_t must be typedef-ed as bool.
 **/ 
typedef enum
{
    hal_false = 0,      /**< false */
    hal_true  = 1       /**< true */
} hal_boolval_t;


/** @typedef    typedef enum hal_bool_t 
    @brief      hal_bool_t is the bool type.If C99 is available, use bool instead of uint8_t.
 **/ 
typedef uint8_t     hal_bool_t;


/** @typedef    typedef uint32_t hal_reltime_t 
    @brief      hal_reltime_t express time in hal in form of usec (micro-seconds). It is used for relative timing
                operations because its maximum value is about 4294 seconds, which is more than enough for most needs.
 **/  
typedef uint32_t    hal_reltime_t;


/** @typedef    typedef hal_reltime_t  hal_time_t
    @brief      hal_time_t is an alias for hal_reltime_t
 **/ 
typedef hal_reltime_t  hal_time_t;


/** @typedef    typedef uint64_t hal_abstime_t 
    @brief      hal_abstime_t express absolute time in hal in form of usec (micro-seconds) on a very long range.
                A hal_absttime_t variable keeps time up to 584 thousand years.
 **/  
typedef uint64_t    hal_abstime_t;


/** @typedef    typedef uint64_t hal_nanotime_t 
    @brief      hal_nanotime_t express very precise relative time in hal in form of nsec (nano-seconds).
                A hal_nanoime_t variable keeps time up to 584 years.
 **/  
typedef uint64_t    hal_nanotime_t;


/** @typedef    typedef hal_result_t (*hal_res_fp_void_t) (void)
    @brief      hal_res_fp_void_t is a pointer to a function which returns a hal_result_t and has a void argument.
 **/
typedef hal_result_t (*hal_res_fp_void_t) (void);


/** @typedef    typedef hal_result_t (*hal_res_fp_voidp_t) (void)
    @brief      hal_res_fp_voidp_t is a pointer to a function which returns a hal_result_t and has a void pointer argument.
 **/
typedef hal_result_t (*hal_res_fp_voidp_t) (void* p);


/** @typedef    typedef void (*hal_void_fp_void_t) (void)
    @brief      hal_void_fp_void_t is a pointer to a function which returns void and has a void argument.
 **/
typedef void (*hal_void_fp_void_t) (void);


/** @typedef    typedef uint8_t (*hal_uint8_fp_void_t) (void)
    @brief      hal_uint8_fp_void_t is a pointer to a function which returns uint8_t and has a void argument.
 **/
typedef uint8_t (*hal_uint8_fp_void_t) (void);


/** @typedef    typedef void (*hal_callback_t) (void* p)
    @brief      hal_callback_t is a pointer to a callback function which returns void and has a void pointer argument.
 **/
typedef void (*hal_callback_t) (void* p);


/** @typedef    typedef enum hal_interrupt_priority_t;
    @brief      rappresents the possible values for interrupt priorities.
                On ARMCM:   0 is highest priority. 
                On DSPIC:   0 is lowest priority
 **/
typedef enum
{
    hal_int_priorityNONE    = hal_NA08,
    hal_int_priority00      = 0,
    hal_int_priority01      = 1,
    hal_int_priority02      = 2,
    hal_int_priority03      = 3,
    hal_int_priority04      = 4,
    hal_int_priority05      = 5,
    hal_int_priority06      = 6,
    hal_int_priority07      = 7,
    hal_int_priority08      = 8,
    hal_int_priority09      = 9,
    hal_int_priority10      = 10,
    hal_int_priority11      = 11,
    hal_int_priority12      = 12,
    hal_int_priority13      = 13,
    hal_int_priority14      = 14,
    hal_int_priority15      = 15
} hal_interrupt_priority_t;  


/** @typedef    typedef int32_t hal_irqn_t
    @brief      It expresses the IRQ number. Use however the type hal_cpu_arc_xxx_irqn_t or hal_cpu_fam_xxx_irqn_t.
 **/ 
typedef int32_t hal_irqn_t;  


/** @typedef    typedef struct hal_extfn_cfg_t
    @brief      hal_extfn_cfg_t keeps pointers to externally defined functions which HAL may need.
 **/
typedef struct
{
    /** The function which is executed when a fatal error is detected. If not redefined (thus this value is NULL), 
        it just enters in a forever loop. */ 
    void            (*usr_on_fatal_error)(hal_fatalerror_t errorcode, const char * errormsg);
    /** The function which is executed to suspend scheduling. Use the function osal_system_scheduling_suspend() of OSAL.
        If not redefined (thus this value is NULL) it does nothing.  It is necessary to redefine only if the HAL is used 
        in a multitasking execution environment (with an RTOS) */ 
    void            (*osal_system_scheduling_suspend)(void);
    /** The function which is executed to restart scheduling. Use the function osal_system_scheduling_restart() of OSAL.
        If not redefined (thus this value is NULL) it does nothing.  It is necessary to redefine only if the HAL is used 
        in a multitasking execution environment (with an RTOS) */ 
    void            (*osal_system_scheduling_restart)(void);
} hal_extfn_cfg_t;


/** @typedef    typedef struct hal_base_cfg_t 
    @brief      hal_base_cfg_t contains the base hal configuration. Other modules shall use proper
                configuration data structures.
 **/  
typedef struct
{   
    hal_extfn_cfg_t extfn;                      /**< External functionalities offered to the HAL            */
} hal_base_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_base_init(const hal_base_cfg_t *cfg)
    @brief      Initialises the base module of HAL: stack, heap, funtionalities provided by external
                functions such as OSAL scheduling management and error handling.
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_OK or hal_res_NOK_generic on failure              
 **/
extern hal_result_t hal_base_init(const hal_base_cfg_t *cfg);


/** @fn         extern void hal_base_osal_scheduling_suspend(void)
    @brief      Suspends the OSAL scheduling. It calls the external funtion passed throught
                hal_base_cfg_t::extfn.osal_system_scheduling_suspend (if not NULL).
    @details    Its use is mostly internal to HAL, but it can be called by a HAL user as well. It is
                useful in multitasking environments running OSAL when a context-switch may delay
                too much a critical operation, such as a temporary disable of an ISR.                
 **/
extern void hal_base_osal_scheduling_suspend(void);


/** @fn         extern void hal_base_osal_scheduling_restart(void)
    @brief      Restart the OSAL scheduling after it was suspended by hal_base_hid_osal_scheduling_suspend(). 
                It calls the external funtion passed throught
                hal_base_cfg_t::extfn.osal_system_scheduling_restart (if not NULL). 
 **/
extern void hal_base_osal_scheduling_restart(void);


/** @fn         extern void hal_base_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
    @brief      It is called by HAL when an internal fatal error occurs, but it can be called by a HAL 
                user as well. The function at first stops OSAL scheduling by calling 
                hal_base_osal_scheduling_suspend(), then calls an externally defined function passed through 
                hal_base_cfg_t::extfn.osal_system_scheduling_suspend (if not NULL), and finally enters in an
                infinite loop.
    @param      errorcode       The error code. 
    @param      errormsg        The error message.
 **/
extern void hal_base_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);





/** @}            
    end of group doxy_group_hal_base  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



