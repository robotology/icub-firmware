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

/* @file       hal_mpu.c
	@brief      This file keeps implementation of the base hal module for stm32.
	@author     marco.accame@iit.it
    @date       02/27/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"

#ifdef HAL_USE_MPU

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_mpu_cfg_t hal_mpu_cfg_default = { .dummy = 0 };



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
  
typedef struct
{
    uint8_t     nothing;
} hal_mpu_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// static hal_mpu_theinternals_t s_hal_mpu_theinternals =
// {
//     .nothing     = 0
// };

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_mpu_init(const hal_mpu_cfg_t* cfg)
{
    if(NULL == cfg)
    {
        cfg  = &hal_mpu_cfg_default;
    } 
    
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);      
    
    if((hal_mpu__theboardconfig.speeds.cpu     != clocks.SYSCLK_Frequency) ||
       (hal_mpu__theboardconfig.speeds.fastbus != clocks.PCLK2_Frequency)  ||
       (hal_mpu__theboardconfig.speeds.slowbus != clocks.PCLK1_Frequency) )
    {
        char str[64];
        snprintf(str, sizeof(str), "hal_mpu wrong speeds: cpu = %d hz, fbus = %d hz, sbus = %d hz", clocks.SYSCLK_Frequency, clocks.PCLK2_Frequency, clocks.PCLK1_Frequency);
        hal_base_on_fatalerror(hal_fatalerror_generic, str);       
    }    
   
    return(hal_res_OK);   
}


extern hal_mpu_arch_t hal_mpu_arch_get(void)
{
    return(hal_mpu__theboardconfig.architecture);
}


extern hal_mpu_type_t hal_mpu_type_get(void)
{
    return(hal_mpu__theboardconfig.type);
}


extern hal_mpu_name_t hal_mpu_name_get(void)
{
    return(hal_mpu__theboardconfig.name);
}


extern uint32_t hal_mpu_speed_get(hal_mpu_speedtype_t speedtype)
{
    uint32_t res  = 0;   
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);  
    
    switch(speedtype)
    {
        case hal_mpu_speedtype_max:         res = hal_mpu__theboardconfig.speeds.max;         break;
        case hal_mpu_speedtype_cpu:         res = clocks.SYSCLK_Frequency;                      break;
        case hal_mpu_speedtype_fastbus:     res = clocks.PCLK2_Frequency;                       break;
        case hal_mpu_speedtype_slowbus:     res = clocks.PCLK1_Frequency;                       break;
        default:                            res = 0;                                            break;        
    }
    
    return(res);   
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




#endif//HAL_USE_CPU

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



