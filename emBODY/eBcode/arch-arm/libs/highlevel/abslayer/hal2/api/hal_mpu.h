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

#ifndef _HAL_MPU_H_
#define _HAL_MPU_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_mpu.h
    @brief      This header file keeps public interface to the hal parts which are specific of a given mpu.
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_mpu HAL MPU GENERIC

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h" 


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_mpu_arch_t 
    @brief      hal_mpu_arch_t contains all possible architectures of MPUs supported by HAL
 **/ 
typedef enum
{
    hal_mpu_arch_armcm3             = 0,    /**< it is associated to HAL_USE_MPU_ARCH_ARMCM3 */
    hal_mpu_arch_armcm4             = 1,    /**< it is associated to HAL_USE_MPU_ARCH_ARMCM4 */
    hal_mpu_arch_none               = hal_NA08
} hal_mpu_arch_t;

enum { hal_mpu_archs_number = 2 };


/** @typedef    typedef enum hal_mpu_type_t 
    @brief      hal_mpu_type_t contains the type of the MPU within a given architecture
 **/ 
typedef enum
{
    hal_mpu_type_stm32f1            = 0,        /**< it is associated to HAL_USE_MPU_TYPE_STM32F1 */
    hal_mpu_type_stm32f4            = 1,        /**< it is associated to HAL_USE_MPU_TYPE_STM32F4 */
    hal_mpu_type_none               = hal_NA08
} hal_mpu_type_t;

enum { hal_mpu_types_number = 2 };


/** @typedef    typedef enum hal_mpu_name_t 
    @brief      hal_mpu_name_t contains the name of the MPU within a given type
 **/ 
typedef enum
{
    hal_mpu_name_stm32f103rb        = 0,        /**< it is associated to HAL_USE_MPU_NAME_STM32F103RB */
    hal_mpu_name_stm32f107vc        = 1,        /**< it is associated to HAL_USE_MPU_NAME_STM32F107VC */
    hal_mpu_name_stm32f407ig        = 2,        /**< it is associated to HAL_USE_MPU_NAME_STM32F407IG */
    hal_mpu_name_none               = hal_NA08
} hal_mpu_name_t;

enum { hal_mpu_names_number = 3 };


/** @typedef    typedef enum hal_mpu_speed_t 
    @brief      contains all speeds in MPU
 **/ 
typedef enum
{
    hal_mpu_speedtype_max           = 0,
    hal_mpu_speedtype_cpu           = 1,
    hal_mpu_speedtype_fastbus       = 2,
    hal_mpu_speedtype_slowbus       = 3
} hal_mpu_speedtype_t;

enum { hal_mpu_speedtypes_number = 4 };


/** @typedef    typedef struct hal_mpu_cfg_t 
    @brief      contains the configuration for the mpu module. 
 **/  
typedef struct
{
    uint8_t     dummy;    
} hal_mpu_cfg_t;
 

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_mpu_cfg_t hal_mpu_cfg_default;  // = { .dummy = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_result_t hal_mpu_init(const hal_mpu_cfg_t* cfg)
    @brief      This function initialises the mpu module. It is to be called ONLY by HAL internals.
    @return     hal_res_OK on success, otherwise an error code.
  */
extern hal_result_t hal_mpu_init(const hal_mpu_cfg_t* cfg);


/** @fn         extern hal_mpu_arch_t hal_mpu_arch_get(void)
    @brief      This function retrieves the architecture of the used mpu.
    @return     a value of hal_mpu_arch_t (hal_mpu_arc_none means ... unconfigured mode or something wrong)
  */
extern hal_mpu_arch_t hal_mpu_arch_get(void);


/** @fn         extern hal_mpu_type_t hal_mpu_type_get(void)
    @brief      This function retrieves the type of mpu .
    @return     a value of hal_mpu_type_t (hal_mpu_fam_none means ... unconfigured mode or something wrong)
  */
extern hal_mpu_type_t hal_mpu_type_get(void);


/** @fn         extern hal_mpu_name_t hal_mpu_name_get(void)
    @brief      This function retrieves the name of the mpu
    @return     a value of hal_mpu_name_t (hal_mpu_fam_none means ... unconfigured mode or something wrong)
  */
extern hal_mpu_name_t hal_mpu_name_get(void);


/** @fn         extern uint32_t hal_mpu_speed_get(hal_mpu_speedtype_t speedtype)
    @brief      This function retrieves the specified speed in the mpu
    @return     the speed in hz.
  */
extern uint32_t hal_mpu_speed_get(hal_mpu_speedtype_t speedtype);


/** @}            
    end of group doxy_group_hal_mpu  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

