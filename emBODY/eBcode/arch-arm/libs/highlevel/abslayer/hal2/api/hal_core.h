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

#ifndef _HAL_CORE_H_
#define _HAL_CORE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_core.h
    @brief      This header file implements public interface to the hal core.
    @author     marco.accame@iit.it
    @date       02/28/2013
**/

/** @defgroup doxy_group_hal_core HAL CORE

    The Hardware Abstraction Layer offers service to the system which are independent from the underlying
    HW platform. This file is an interface towards core services.
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_mpu.h"
#include "hal_flash.h"
#include "hal_sys.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct hal_core_cfg_t 
    @brief      contains the configuration for the core part of hal: base, cpu, and sys. 
 **/  
typedef struct
{
    hal_base_cfg_t      basecfg;    /**< the configuration of the base module */
    hal_mpu_cfg_t       mpucfg;     /**< the configuration of the cpu module */
    hal_flash_cfg_t     flashcfg;   /**< the confoguration of the flash module */
    hal_sys_cfg_t       syscfg;     /**< the configuration of the sys module */
} hal_core_cfg_t;

typedef hal_core_cfg_t hal_cfg_t;


/** @typedef    typedef enum hal_core_status_t 
    @brief      contains the status of the core part of HAL. 
 **/  
typedef enum
{
    hal_core_status_zero            = 0,    /**< nothing has been called yet */
    hal_core_status_initialised     = 1,    /**< hal_core_init() has been succesfully called */
    hal_core_status_started         = 2     /**< hal_core_start() has been succesfully called */    
} hal_core_status_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_core_init(const hal_core_cfg_t *cfg)
    @brief      Initialise the core part of hal to work for a given configuration. The required RAM is internally
                allocated by using hal_heap_new(). In case of no more memory on the heap, a 
                hal_fatalerror_missingmemory is triggered.
    @param      cfg             The target core configuration. 
    @return     hal_res_OK or hal_res_NOK_generic 
 **/
extern hal_result_t hal_core_init(const hal_core_cfg_t *cfg); 


/** @fn         extern hal_result_t hal_core_start(void)
    @brief      It starts the core part of HAL: base, cpu, and sys. The clock specified in brdcfg file will be applied, 
                and the system will be able to init all the other modules (gpio, can, spi, etc.).
    @return     hal_res_OK or hal_res_NOK_generic 
 **/
extern hal_result_t hal_core_start(void); 


/** @fn         extern hal_core_status_t hal_core_status_get(void)
    @brief      gets the status of the core module.
    @return     the status
 **/
extern hal_core_status_t hal_core_status_get(void);


/** @fn         onst hal_core_cfg_t* hal_core_cfg_get(void)
    @brief      it retrieves the core configuration pointer.
    @return     pointer to the core configuration
 **/
const hal_core_cfg_t* hal_core_cfg_get(void);


/** @}            
    end of group doxy_group_hal_core  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



