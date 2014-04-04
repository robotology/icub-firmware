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

#ifndef _HAL_COMMON_H_
#define _HAL_COMMON_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_common.h
    @brief      This header file implements public interface to the common data structures of hal
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal_common HAL common

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define hal_NA08                                    ((uint8_t)(0xFF))
#define hal_NA16                                    ((uint16_t)(0xFFFF))
#define hal_NA32                                    ((uint32_t)(0xFFFFFFFF)) 
#define hal_NA64                                    ((uint64_t)(0xFFFFFFFFFFFFFFFF))

#define hal_BASE_U8toU32(a)                         ((uint32_t)(a)&0xff)
#define hal_BASE_U16toU32(a)                        ((uint32_t)(a)&0xffff)
#define hal_BASE_U8toU64(a)                         ((uint64_t)(a)&0xff)
#define hal_BASE_4BtoU32(a, b, c, d)                ((hal_BASE_U8toU32(a))|(hal_BASE_U8toU32(b)<<8)|(hal_BASE_U8toU32(c)<<16)|(hal_BASE_U8toU32(d)<<24)) 
#define hal_BASE_8BtoU64(a, b, c, d, e, f, g, h)    (((uint64_t)hal_BASE_4BtoU32(a, b, c, d))|((uint64_t)hal_BASE_4BtoU32(e, f, g, h)<<32)) 


#define hal_RELTIME_1microsec                       (1L)
#define hal_RELTIME_1millisec                       (1000L)                           
#define hal_RELTIME_1second                         (1000000L)
#define hal_RELTIME_1minute                         (60000000L)
#define hal_RELTIME_1hour                           (3600000000L)

#define hal_ABSTIME_1microsec                       (1LL)
#define hal_ABSTIME_1millisec                       (1000LL)                           
#define hal_ABSTIME_1second                         (1000000LL)
#define hal_ABSTIME_1minute                         (60000000LL)
#define hal_ABSTIME_1hour                           (3600000000LL)
#define hal_ABSTIME_1day                            (24*hal_ABSTIME_1hour)

#define hal_NANOTIME_1nanosec                       (1LL)
#define hal_NANOTIME_1microsec                      (1000LL)
#define hal_NANOTIME_1millisec                      (1000000LL)                           
#define hal_NANOTIME_1second                        (1000000000LL)


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
    hal_fatalerror_runtimefault              = 0xE5,
    hal_fatalerror_warning                   = 0xF0
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
                operations because its maximum value is about 4294 seconds (1 hour, 11 min, and 34 sec), which is 
                more than enough for most needs. As such it is used in systick, HW timers, watchdog.
 **/  
typedef uint32_t    hal_reltime_t;


///** @typedef    typedef hal_reltime_t  hal_time_t
//    @brief      hal_time_t is an alias for hal_reltime_t.
// **/ 
//typedef hal_reltime_t  hal_time_t;


/** @typedef    typedef uint64_t hal_abstime_t 
    @brief      hal_abstime_t express absolute time in hal in form of usec (micro-seconds) on a very long range.
                A hal_absttime_t variable keeps time up to 584 thousand years. It can be used to count time of 
                a system synce its bootstrap.
 **/  
typedef uint64_t    hal_abstime_t;


/** @typedef    typedef uint64_t hal_nanotime_t 
    @brief      hal_nanotime_t express very precise relative time in hal in form of nsec (nano-seconds).
                A hal_nanotime_t variable keeps time up to 584 years. It can be used in such peripherals where
                a precision sub-microsecond is required.
 **/  
typedef uint64_t    hal_nanotime_t;


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


// -- definition of function pointers


/** @typedef    typedef void (*hal_void_fp_void_t) (void)
    @brief      is a pointer to a function which returns void and has a void argument.
 **/
typedef void (*hal_void_fp_void_t) (void);


/** @typedef    typedef void (*hal_void_fp_voidp_t) (void* p)
    @brief      is a pointer to a function which returns void and has a void pointer argument.
 **/
typedef void (*hal_void_fp_voidp_t) (void* p);



/** @typedef    typedef void (*hal_void_fp_void_t) (void)
    @brief      is a pointer to a function which returns void and has a void argument.
 **/
typedef void (*hal_void_fp_void_t) (void);


/** @typedef    ttypedef void (*hal_void_fp_err_ccharp_t) (hal_fatalerror_t errorcode, const char * errormsg)
    @brief      is a pointer to a function which returns void and has a void pointer argument.
 **/
typedef void (*hal_void_fp_err_ccharp_t) (hal_fatalerror_t errorcode, const char *errormsg);


/** @typedef    typedef hal_result_t (*hal_res_fp_void_t) (void)
    @brief      is a pointer to a function which returns a hal_result_t and has a void argument.
 **/
typedef hal_result_t (*hal_res_fp_void_t) (void);


/** @typedef    typedef hal_result_t (*hal_res_fp_voidp_t) (void* p)
    @brief      is a pointer to a function which returns a hal_result_t and has a void pointer argument.
 **/
typedef hal_result_t (*hal_res_fp_voidp_t) (void* p);


/** @typedef    typedef hal_result_t (*hal_res_fp_voidp_voidp_t) (void* p, void* q)
    @brief      is a pointer to a function which returns a hal_result_t and has two void pointer arguments.
 **/
typedef hal_result_t (*hal_res_fp_voidp_voidp_t) (void* p, void* q);


/** @typedef    typedef hal_result_t (*hal_res_fp_voidp_uint8_t) (void* p, uint8_t n)
    @brief      is a pointer to a function which returns a hal_result_t and an void pointer argument and a size.
 **/
typedef hal_result_t (*hal_res_fp_voidp_uint8_t) (void* p, uint8_t n);


/** @typedef    typedef hal_result_t (*hal_res_fp_uint8_uint32_voidp_t) (uint8_t n, uint32_t u, void *p)
    @brief      is a pointer to a function which returns a hal_result_t and an void pointer argument and a size.
 **/
typedef hal_result_t (*hal_res_fp_uint8_uint32_voidp_t) (uint8_t n, uint32_t u, void *p);

typedef hal_result_t (*hal_res_fp_voidp_voidp_voidp_t) (void *p, void *q, void *r);


/** @typedef    typedef hal_result_t (*hal_res_fp_int32_voidp_uint8_t) (int32_t, void* p, uint8_t n)
    @brief      is a pointer to a function which returns a hal_result_t and has a int32_t, a void pointer argument and an uint8_t.
 **/
typedef hal_result_t (*hal_res_fp_int32_voidp_uint8_t) (int32_t, void* p, uint8_t n);


/** @typedef    typedef hal_result_t (*hal_res_fp_int32_voidp_t) (int32_t, void*)
    @brief      is a pointer to a function which returns a hal_result_t and has a void pointer argument.
 **/
typedef hal_result_t (*hal_res_fp_int32_voidp_t) (int32_t, void* p);


typedef hal_result_t (*hal_res_fp_int32_uint32_uint32_voidp_voidp_t) (int32_t, uint32_t a, uint32_t b, void* p, void* q);


typedef hal_result_t (*hal_res_fp_int32_voidp_voidp_voidp_t) (int32_t, void* p, void* q, void* r);


typedef hal_result_t (*hal_res_fp_int32_uint8_uint32_voidp_t) (int32_t, uint8_t a, uint32_t b, void* p);


/** @typedef    typedef hal_result_t (*hal_res_fp_int32_t) (int32_t)
    @brief      is a pointer to a function which returns a hal_result_t and has a int32_t argument.
 **/
typedef hal_result_t (*hal_res_fp_int32_t) (int32_t);


/** @typedef    typedef void (*hal_void_fp_void_t) (void)
    @brief      is a pointer to a function which returns void and has a void argument.
 **/
typedef void (*hal_void_fp_void_t) (void);


typedef void* (*hal_voidp_fp_uint32_t) (uint32_t);

typedef void (*hal_void_fp_voidp_t) (void*);


// - particular function pointers



/** @typedef    typedef hal_void_fp_voidp_t hal_function_callback_t
    @brief      is a pointer to a function which returns void and has a void pointer argument.
 **/
typedef hal_void_fp_voidp_t hal_callback_t;


typedef hal_void_fp_err_ccharp_t hal_errorhandler_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @}            
    end of group doxy_group_hal_common  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



