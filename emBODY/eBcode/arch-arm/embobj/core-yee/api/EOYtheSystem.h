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
#ifndef _EOYTHESYSTEM_H_
#define _EOYTHESYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOYtheSystem.h
    @brief      This header file implements public interface to the system singleton.
    @author     marco.accame@iit.it
    @date       08/03/2011
**/

/** @defgroup eoy_thesystem Singleton EOYtheSystem
    The EOYtheSystem is derived from abstract object EOVtheSystem to give to embOBJ a YARP execution environment
    (YEE) based on ... YARP.  The EOYtheSystem must be first initialised and then started. 
    Its start initialise some objects of embOBJ such as the EOtheMemoryPool, the EOtheErrorManager, the EOMtheTimerManager
    and the EOMtheCallbackManager. It is responsibility of EOYtheSystem to initialise whatver is needed. 
 
    When the EOYtheSystem is later started, it will launch the OSAL and run a user-defined function which will launch
    other services of the embOBJ, and also launch proper user tasks using the EOMtask object.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct eOysystem_cfg_t
    @brief      eOysystem_cfg_t contains ...
 **/  
typedef struct
{
    uint32_t                    nothing;
} eOysystem_cfg_t;


/** @typedef    typedef struct EOYtheSystem_hid EOYtheSystem
    @brief      EOYtheSystem is an opaque struct. It is used to implement data abstraction for the timer manager 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOYtheSystem_hid EOYtheSystem;


/** @typedef    typedef void EOYtheSystemDerived
    @brief      EOYtheSystemDerived is used to implement polymorphism in the objects derived from EOYtheSystem
 **/
typedef void EOYtheSystemDerived;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern EOYtheSystem * eoy_sys_Initialise(const eOysystem_cfg_t *syscfg, 
                                         const eOmempool_cfg_t *mpoolcfg, 
                                         const eOerrman_cfg_t *errmancfg,
                                         const eOmtimerman_cfg_t *tmrmancfg,
                                         const eOmcallbackman_cfg_t *cbkmancfg)
    @brief      Initialise the singleton EOYtheSystem and all the other entities that are required by the system. In order they are:
                the error manager, the the memory pool, the HAL, the OSAL, and optionally the FSAL. 
                The EOMtheTimerManager and EOMtheCallbackManager are initialised later by eoy_sys_Start().
    @param      syscfg          The configuration of the system (HAL, OSAL, FSAL).  Only the config of FSAL can be NULL. In such a case
                                the FSAL is not initialised.
    @param      mpoolcfg        The configuration of the EOtheMemoryPool.  If NULL, it is used the default configuration
                                (@e eoy_mempool_DefaultCfg), which uses the heap.  The function eo_mempool_Initialise() is called internally.
    @param      errmancfg       The configuration of the EOtheErrorManager  If NULL, it is used the default configuration @e eoy_errman_DefaultCfg.
                                The function eo_errman_Initialise() is called internally.
    @param      tmrmancfg       The configuration of the EOMtheTimerManager  If NULL, it is used the default configuration @e eoy_timerman_DefaultCfg
                                The function eoy_timerman_Initialise() is called internally by the OSAL init task.
    @param      cbkmancfg       The configuration of the EOMtheCallbackManager  If NULL, it is used the default configuration @e eoy_callbackman_DefaultCfg
                                The function eoy_callbackman_Initialise() is called internally by the OSAL init task.
    @return     A not NULL handle to the singleton.  In case of errors it is called the EOtheErrorManager
    @wa
 
 **/
extern EOYtheSystem * eoy_sys_Initialise(const eOysystem_cfg_t *syscfg, 
                                         const eOmempool_cfg_t *mpoolcfg, 
                                         const eOerrman_cfg_t *errmancfg);


 
/** @fn         extern EOYtheSystem* eoy_sys_GetHandle(void)
    @brief      Returns an handle to the singleton EOYtheSystem. The singleton must have been initialised otherwise 
                this function call will return NULL.
    @return     The pointer to the required EOYtheSystem (or NULL upon in-initialised singleton).
 **/
extern EOYtheSystem* eoy_sys_GetHandle(void);


/** @fn         extern void eoy_sys_Start(EOYtheSystem *p, eOvoid_fp_void_t userinit_fn)
    @brief      It starts EOYtheSystem.  The singleton must have been initialised otherwise 
                this function call will issue a fatal error to the EOtheErrorManager.
                The function shall start the OSAL with its init task at maximum priority. The init task 
                initialises the EOMtheTimerManager and EOMtheCallbackManager, and calls the function @e userinit_fn(),
                where the user can start other services (e.g., the GPIO manager or the IP net) and also other
                service tasks. When the function @e userinit_fn() is finished, the OSAL removes the init task and
                allows other tasks to run according to their priorities.
    @param      p               The handler to the singleton.
    @param      userinit_fn     The starting function executed by the OSAL.  It is responsibility of the
                                programmer to place inside userinit_fn() the proper calls to create MEE objects
                                such as EOtask, etc.
    @return     The function does not return and execution stays in there forever.
 **/
extern void eoy_sys_Start(EOYtheSystem *p, eOvoid_fp_void_t userinit_fn);



extern eOabstime_t eoy_sys_abstime_get(EOYtheSystem *p);


/** @}            
    end of group eoy_thesystem  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



