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

#include "hal_common.h"


// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct hal_extfn_cfg_t
    @brief      hal_extfn_cfg_t keeps pointers to externally defined functions which HAL may need.
 **/
typedef struct
{
    /** If NULL, the HAL manages fatal errors entering in a forever loop (it quits it only if errortype is hal_fatalerror_warning). 
        Otherwise, it calls hal_extfn_cfg_t::usr_on_fatal_error(errorcode, errormsg) */ 
    hal_errorhandler_t          usr_on_fatal_error;
    /** If NULL, the HAL does not handle scheduling suspend. Otherwise, it calls hal_extfn_cfg_t::osal_system_scheduling_suspend(). */ 
    hal_void_fp_void_t          osal_system_scheduling_suspend;
    /** If NULL, the HAL does not handle scheduling restart. Otherwise, it calls hal_extfn_cfg_t::osal_system_scheduling_restart(). */  
    hal_void_fp_void_t          osal_system_scheduling_restart;
    /** If NULL, the HAL allocates memory using calloc(). Otherwise, it uses: mem = hal_extfn_cfg_t::ext_heap_new(size)  */ 
    hal_voidp_fp_uint32_t       ext_heap_new;  
    /** If NULL the HAL deallocates using free(). Otherwise, it uses: hal_extfn_cfg_t::ext_heap_delete(mem)  */     
    hal_void_fp_voidp_t         ext_heap_delete;    
} hal_extfn_cfg_t;


/** @typedef    typedef struct hal_base_cfg_t 
    @brief      hal_base_cfg_t contains the base hal configuration. Other modules shall use proper
                configuration data structures.
 **/  
typedef struct
{   
    hal_extfn_cfg_t extfn;                      /**< External functionalities offered to the HAL    */
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


/** @fn         extern hal_bool_t hal_base_initted_is(void)
    @brief      tells if alread initted

    @return 	hal_bool or hal_false             
 **/
extern hal_bool_t hal_base_initted_is(void);


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
                hal_base_cfg_t::extfn.osal_system_scheduling_suspend (if not NULL), and if error is not hal_fatalerror_warning
                it finally enters in an infinite loop. If error is hal_fatalerror_warning it resumes scheduling and returns.
    @param      errorcode       The error code. 
    @param      errormsg        The error message.
 **/
extern void hal_base_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);


/** @fn         extern void hal_base_on_warning(const char * warningmsg)
    @brief      It is called by HAL when an internal warning occurs, but it can be called by a HAL 
                user as well. The function at first stops OSAL scheduling by calling 
                hal_base_osal_scheduling_suspend(), then calls an externally defined function passed through 
                hal_base_cfg_t::extfn.osal_system_scheduling_suspend (if not NULL), and then it resumes scheduling and returns.
    @param      errormsg        The warning message.
 **/
extern void hal_base_on_warning(const char * warningmsg);


/** @fn         extern void * hal_base_heap_new(uint32_t size)
    @brief      allocates memory from the heap. if not NULL it calls the allocator specified by hal_base_cfg_t::extfn.ext_heap_new(),
                otherwise a normal calloc()
    @param      size            the number of bytes to allocate.
    @return     a valid pointer or NULL
    @warning    use hal_heap_new() which internally calls hal_base_heap_new() and also performs verification of NULL pointer.
 **/
extern void * hal_base_heap_new(uint32_t size);


/** @fn         extern void hal_base_heap_delete(void* mem)
    @brief      deallocates memory previously obtained with hal_base_heap_new(). if not NULL it calls the deallocater
                specified by hal_base_cfg_t::extfn.ext_heap_delete(), otherwise a normal free()
    @param      mem             the memory to deallocate.
    @wraning    use hal_heap_delete() which internally calls hal_base_heap_delete() and also performs verification of 
                parameter.
 **/
extern void hal_base_heap_delete(void* mem);



/** @}            
    end of group doxy_group_hal_base  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



