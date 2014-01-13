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

#ifndef _HAL_CPU_FAM_STM32F1_H_
#define _HAL_CPU_FAM_STM32F1_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_cpu_fam_stm32f1.h
    @brief      This header file keeps public interface to the hal parts which are specific of cpu family: stm32f1
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_cpu_fam_stm32f1 HAL CPU STM32F1 FAMILY

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------



// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

#include "hal_base.h"
#include "hal_sys.h"
#include "hal_cpu.h"



// - declaration of public user-defined types ------------------------------------------------------------------------- 





#if 1
typedef enum
{   // values from stm32f10x_gpio.h
    hal_cpu_fam_stm32f1_gpio_speed_10mhz    = 1,
    hal_cpu_fam_stm32f1_gpio_speed_2mhz,
    hal_cpu_fam_stm32f1_gpio_speed_50mhz
} hal_cpu_fam_stm32f1_gpio_speed_t;
typedef enum
{   // values from stm32f10x_gpio.h
    hal_cpu_fam_stm32f1_gpio_mode_AIN           = 0x0,
    hal_cpu_fam_stm32f1_gpio_mode_IN_FLOATING   = 0x04,
    hal_cpu_fam_stm32f1_gpio_mode_IPD           = 0x28,
    hal_cpu_fam_stm32f1_gpio_mode_IPU           = 0x48,
    hal_cpu_fam_stm32f1_gpio_mode_Out_OD        = 0x14,
    hal_cpu_fam_stm32f1_gpio_mode_Out_PP        = 0x10,
    hal_cpu_fam_stm32f1_gpio_mode_AF_OD         = 0x1C,
    hal_cpu_fam_stm32f1_gpio_mode_AF_PP         = 0x18   
} hal_cpu_fam_stm32f1_gpio_mode_t;
typedef struct
{   // from stm32f10x_gpio.h
    uint16_t                                gpiopin;
    hal_cpu_fam_stm32f1_gpio_speed_t        gpiospeed;
    hal_cpu_fam_stm32f1_gpio_mode_t         gpiomode;
} hal_cpu_fam_stm32f1_gpio_altcfg_t;
#endif
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_cpu_fam_stm32f1  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



