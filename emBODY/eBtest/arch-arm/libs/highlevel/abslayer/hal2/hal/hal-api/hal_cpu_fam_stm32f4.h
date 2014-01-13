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
{   // values from stm32f40x_gpio.h
    hal_cpu_fam_stm32f4_gpio_speed_2mhz     = 0x00,
    hal_cpu_fam_stm32f4_gpio_speed_25mhz    = 0x01,    
    hal_cpu_fam_stm32f4_gpio_speed_50mhz    = 0x02,  
    hal_cpu_fam_stm32f4_gpio_speed_100mhz   = 0x03       
} hal_cpu_fam_stm32f4_gpio_speed_t;
typedef enum
{   // values from stm32f40x_gpio.h
    hal_cpu_fam_stm32f4_gpio_mode_IN        = 0x00,
    hal_cpu_fam_stm32f4_gpio_mode_OUT       = 0x01,
    hal_cpu_fam_stm32f4_gpio_mode_AF        = 0x02,
    hal_cpu_fam_stm32f4_gpio_mode_AN        = 0x03
} hal_cpu_fam_stm32f4_gpio_mode_t;
typedef enum
{
    hal_cpu_fam_stm32f4_gpio_otype_PP       = 0x00,
    hal_cpu_fam_stm32f4_gpio_otype_OD       = 0x01
} hal_cpu_fam_stm32f4_gpio_otype_t;
typedef enum
{
    hal_cpu_fam_stm32f4_gpio_pupd_nopull    = 0x00,
    hal_cpu_fam_stm32f4_gpio_pupd_up        = 0x01,
    hal_cpu_fam_stm32f4_gpio_pupd_down      = 0x02
} hal_cpu_fam_stm32f4_gpio_pupd_t;
typedef struct
{   // from stm32f40x_gpio.h
    uint32_t                                gpiopin;
    hal_cpu_fam_stm32f4_gpio_mode_t         gpiomode;
    hal_cpu_fam_stm32f4_gpio_speed_t        gpiospeed;
    hal_cpu_fam_stm32f4_gpio_otype_t        gpiootype;
    hal_cpu_fam_stm32f4_gpio_pupd_t         gpiopupd;
} hal_cpu_fam_stm32f4_gpio_altcfg_t;
#endif
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_cpu_fam_stm32f4  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



