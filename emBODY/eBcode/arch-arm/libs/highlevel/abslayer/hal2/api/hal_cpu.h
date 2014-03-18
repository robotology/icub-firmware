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

#ifndef _HAL_CPU_H_
#define _HAL_CPU_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_cpu.h
    @brief      This header file keeps public interface to the hal parts which are specific of a given cpu.
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_cpu HAL CPU GENERIC

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h" 


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_cpu_architecture_t 
    @brief      hal_cpu_architecture_t contains all possible architectures of CPUs supported by HAL
 **/ 
typedef enum
{
    hal_cpu_arc_armcm3              = 0,
    hal_cpu_arc_armcm4              = 1,
    hal_cpu_arc_none                = hal_NA08
} hal_cpu_architecture_t;

enum { hal_cpu_architectures_number = 2 };


/** @typedef    typedef enum hal_cpu_family_t 
    @brief      hal_cpu_family_t contains the family of the CPU within a given architecture
 **/ 
typedef enum
{
    hal_cpu_fam_stm32f1             = 0,
    hal_cpu_fam_stm32f4             = 1,
    hal_cpu_fam_none                = hal_NA08
} hal_cpu_family_t;

enum { hal_cpu_families_number = 2 };


/** @typedef    typedef enum hal_cpu_name_t 
    @brief      hal_cpu_name_t contains the name of the CPU within a given family
 **/ 
typedef enum
{
    hal_cpu_nam_stm32f103           = 0,
    hal_cpu_nam_stm32f107           = 1,
    hal_cpu_nam_stm32f407           = 2,
    hal_cpu_nam_none                = hal_NA08
} hal_cpu_name_t;

enum { hal_cpu_names_number = 3 };


/** @typedef    typedef enum hal_cpu_speed_t 
    @brief      contains all speeds in CPU
 **/ 
typedef enum
{
    hal_cpu_speedtype_max           = 0,
    hal_cpu_speedtype_cpu           = 1,
    hal_cpu_speedtype_fastbus       = 2,
    hal_cpu_speedtype_slowbus       = 3
} hal_cpu_speedtype_t;

enum { hal_cpu_speedtypes_number = 4 };


/** @typedef    typedef struct hal_cpu_cfg_t 
    @brief      contains the configuration for the cpu module. 
 **/  
typedef struct
{
    uint8_t     dummy;    
} hal_cpu_cfg_t;
 

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_cpu_cfg_t hal_cpu_cfg_default;  // = { .dummy = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_result_t hal_cpu_init(const hal_cpu_cfg_t* cfg)
    @brief      This function initialises the cpu module. It is to be called ONLY by HAL internals.
    @return     hal_res_OK on success, otherwise an error code.
  */
extern hal_result_t hal_cpu_init(const hal_cpu_cfg_t* cfg);


/** @fn         extern hal_cpu_architecture_t hal_cpu_architecture_get(void)
    @brief      This function retrieves the architecture of the used cpu.
    @return     a value of hal_cpu_architecture_t (hal_cpu_arc_none means ... unconfigured mode or something wrong)
  */
extern hal_cpu_architecture_t hal_cpu_architecture_get(void);


/** @fn         extern hal_cpu_family_t hal_cpu_family_get(void)
    @brief      This function retrieves the family of cpu .
    @return     a value of hal_cpu_family_t (hal_cpu_fam_none means ... unconfigured mode or something wrong)
  */
extern hal_cpu_family_t hal_cpu_family_get(void);


/** @fn         extern hal_cpu_name_t hal_cpu_name_get(void)
    @brief      This function retrieves the name of the cpu
    @return     a value of hal_cpu_name_t (hal_cpu_fam_none means ... unconfigured mode or something wrong)
  */
extern hal_cpu_name_t hal_cpu_name_get(void);


/** @fn         extern uint32_t hal_cpu_speed_get(hal_cpu_speedtype_t speedtype)
    @brief      This function retrieves the specified speed in the cpu
    @return     the speed in hz.
  */
extern uint32_t hal_cpu_speed_get(hal_cpu_speedtype_t speedtype);


/** @}            
    end of group doxy_group_hal_cpu  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

