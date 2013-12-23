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

#ifndef _HL_COMMON_H_
#define _HL_COMMON_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_common.h
    @brief      This header file implements public interface to the common data structures in hl
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

/** @defgroup doxy_group_hl_common HL common types

    The HL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define hl_NA08                                     ((uint8_t)(0xFF))
#define hl_NA16                                     ((uint16_t)(0xFFFF))
#define hl_NA32                                     ((uint32_t)(0xFFFFFFFF)) 
#define hl_NA64                                     ((uint64_t)(0xFFFFFFFFFFFFFFFF))

#define hl_CONV_U8toU32(a)                          ((uint32_t)(a)&0xff)
#define hl_CONV_U16toU32(a)                         ((uint32_t)(a)&0xffff)
#define hl_CONV_U8toU64(a)                          ((uint64_t)(a)&0xff)
#define hl_CONV_4BtoU32(a, b, c, d)                 ((hl_CONV_U8toU32(a))|(hl_CONV_U8toU32(b)<<8)|(hl_CONV_U8toU32(c)<<16)|(hl_CONV_U8toU32(d)<<24)) 
#define hl_CONV_8BtoU64(a, b, c, d, e, f, g, h)     (((uint64_t)hl_CONV_4BtoU32(a, b, c, d))|((uint64_t)hl_CONV_4BtoU32(e, f, g, h)<<32)) 


#define hl_RELTIME_1microsec                        (1L)
#define hl_RELTIME_1millisec                        (1000L)                           
#define hl_RELTIME_1second                          (1000000L)
#define hl_RELTIME_1minute                          (60000000L)
#define hl_RELTIME_1hour                            (3600000000L)

#define hl_ABSTIME_1microsec                        (1LL)
#define hl_ABSTIME_1millisec                        (1000LL)                           
#define hl_ABSTIME_1second                          (1000000LL)
#define hl_ABSTIME_1minute                          (60000000LL)
#define hl_ABSTIME_1hour                            (3600000000LL)
#define hl_ABSTIME_1day                             (24*hl_ABSTIME_1hour)

#define hl_NANOTIME_1nanosec                        (1LL)
#define hl_NANOTIME_1microsec                       (1000LL)
#define hl_NANOTIME_1millisec                       (1000000LL)                           
#define hl_NANOTIME_1second                         (1000000000LL)


// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef enum hl_result_t 
    @brief      hl_result_t contains all possible return values in functions.
 **/  
typedef enum 
{
    hl_res_OK                   = 0,
    hl_res_NOK_generic          = -1,
    hl_res_NOK_nullpointer      = -2,       /**< error: a null pointer was used */
    hl_res_NOK_nodata           = -5,       /**< error: there was not data to be retrieved */
    hl_res_NOK_unsupported      = -15       /**< error: unsupported feature */
} hl_result_t;


/** @typedef    typedef enum hl_error_t 
    @brief      hl_error_t contains specific error codes.
 **/ 
typedef enum
{
    hl_error_generic                    = 0xE0,             
    hl_error_missingconfiguration       = 0xE1,       
    hl_error_missingmemory              = 0xE2,       
    hl_error_incorrectparameter         = 0xE3,       
    hl_error_unsupportedbehaviour       = 0xE4,
    hl_error_runtimefault               = 0xE5
} hl_error_t;


/** @typedef    typedef int32_t hl_errorcode_t 
    @brief      contains a generic error code.
 **/ 
typedef int32_t hl_errorcode_t;


/** @typedef    typedef enum hl_boolval_t 
    @brief      hl_boolval_t contains values for bool type (hl_bool_t).
    @warning    C99 contains bool, true, and false. To use C99 include "stdbool.h" and -c99 compiler option.
                At this point please redefine eo_false and eo_true to be equal to false and true.
                Also, eObool_t must be typedef-ed as bool.
 **/ 
typedef enum
{
    hl_false = 0,       /**< false */
    hl_true  = 1        /**< true */
} hl_boolval_t;


/** @typedef    typedef enum hl_bool_t 
    @brief      hl_bool_t is the bool type.If C99 is available, use bool instead of uint8_t.
 **/ 
typedef uint8_t     hl_bool_t;


/** @typedef    typedef uint32_t hl_reltime_t 
    @brief      hl_reltime_t express time in hl in form of usec (micro-seconds). It is used for relative timing
                operations because its maximum value is about 4294 seconds (1 hour, 11 min, and 34 sec), which is 
                more than enough for most needs. As such it is used in systick, HW timers, watchdog.
 **/  
typedef uint32_t    hl_reltime_t;


/** @typedef    typedef uint64_t hl_abstime_t 
    @brief      hl_abstime_t express absolute time in hl in form of usec (micro-seconds) on a very long range.
                A hl_absttime_t variable keeps time up to 584 thousand years. It can be used to count time of 
                a system synce its bootstrap.
 **/  
typedef uint64_t    hl_abstime_t;


/** @typedef    typedef uint64_t hl_nanotime_t 
    @brief      hl_nanotime_t express very precise relative time in hl in form of nsec (nano-seconds).
                A hl_nanotime_t variable keeps time up to 584 years. It can be used in such peripherals where
                a precision sub-microsecond is required.
 **/  
typedef uint64_t    hl_nanotime_t;


/** @typedef    typedef hl_result_t (*hl_res_fp_void_t) (void)
    @brief      hl_res_fp_void_t is a pointer to a function which returns a hl_result_t and has a void argument.
 **/
typedef hl_result_t (*hl_res_fp_void_t) (void);


/** @typedef    typedef hl_result_t (*hl_res_fp_voidp_t) (void)
    @brief      hl_res_fp_voidp_t is a pointer to a function which returns a hl_result_t and has a void pointer argument.
 **/
typedef hl_result_t (*hl_res_fp_voidp_t) (void* p);


/** @typedef    typedef void (*hl_void_fp_void_t) (void)
    @brief      hl_void_fp_void_t is a pointer to a function which returns void and has a void argument.
 **/
typedef void (*hl_void_fp_void_t) (void);


/** @typedef    typedef uint8_t (*hl_uint8_fp_void_t) (void)
    @brief      hl_uint8_fp_void_t is a pointer to a function which returns uint8_t and has a void argument.
 **/
typedef uint8_t (*hl_uint8_fp_void_t) (void);


/** @typedef    typedef void (*hl_callback_t) (void* p)
    @brief      hl_callback_t is a pointer to a callback function which returns void and has a void pointer argument.
 **/
typedef void (*hl_callback_t) (void* p);


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group doxy_group_hl_common  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



