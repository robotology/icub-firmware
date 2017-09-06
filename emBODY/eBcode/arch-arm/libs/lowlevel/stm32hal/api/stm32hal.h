/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _STM32HAL_H_
#define _STM32HAL_H_

#ifdef __cplusplus
extern "C" {
#endif
    
    
// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       stm32hal.h
    @brief      This header file implements public interface to the stm32 hal library, when initialised by cube-mx.
    @author     marco.accame@iit.it
    @date       03/21/2017
**/

/** @defgroup stm32hal_lib_api STM32HAL STM32HAL

    The STM32HAL is ..... 
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------  
    
#include "stdint.h"   
#include "stdbool.h" 

// generates macros needed to shape stm code from STM32HAL_BOARD_xxx
#include "stm32hal_define.h"
    
// includes the correct stm32lxxx_hal.h etc files depending on generated macros
#include "stm32hal_driver.h"

// contains specific prototypes for the bsp
#include "stm32hal_bsp.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types -------------------------------------------------------------------------    

/** @typedef    typedef struct stm32hal_config_t
    @brief      contains the configuration required by stm32hal.
 **/
typedef struct
{
    uint32_t (*tick1ms_get)(void);  // required by some stm32 functions to generate a proper 1 ms delay. use osal or nticks incremented by systick
    bool initbsp;
} stm32hal_config_t;


typedef enum 
{ 
    stm32hal_res_OK     = 0, 
    stm32hal_res_NOK    = -1 
} stm32hal_res_t;


// - declaration of extern public functions ---------------------------------------------------------------------------


// it inits the stm32hal with a proper configuration, it inits the bsp. if cfg is NULL, then a default value is used
// return value 0 is ok. 
extern stm32hal_res_t stm32hal_init(const stm32hal_config_t *cfg);



/** @}            
    end of group stm32hal_lib_api  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

