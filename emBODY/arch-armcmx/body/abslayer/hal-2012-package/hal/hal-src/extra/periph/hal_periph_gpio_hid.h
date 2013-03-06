/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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
#ifndef _HAL_PERIPH_GPIO_HID_H_
#define _HAL_PERIPH_GPIO_HID_H_


/* @file       hal_periph_gpio_hid.h
    @brief      This header file implements hidden interface to hal gpio.
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"

#include "hal_middleware_interface.h"

// to see HAL_USE_CPU_FAM_xxxx 
#include "hal_brdcfg_modules.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_gpio.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

#define HAL_GPIO_AFNAME_NONE     0x00000000
#define HAL_GPIO_AFMODE_NONE     0xFFFFFFFF


// - definition of the hidden struct implementing the object ----------------------------------------------------------

#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
struct hal_gpio_hid_altcfg_t 
{
    GPIO_InitTypeDef        gpioext;
    uint32_t                afname;     // HAL_GPIO_AFNAME_NONE, and other specific such as GPIO_Remap_ETH (stm32f1) or others
    uint32_t                afmode;     // HAL_GPIO_AFMODE_NONE, ENABLE, DISABLE (latter two are enum FunctionalState type)
};
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif  


typedef struct
{
    uint16_t supported_mask_byport[hal_gpio_ports_number];    /**< the bit in position j-th of supported_mask_port[i] tells if port i and pin j is supported */
} hal_gpio_hid_brdcfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

// used by other peripherals which need to initialise their pins

extern const uint8_t hal_gpio_hid_maxports;
extern GPIO_TypeDef *const hal_gpio_hid_ports[];
extern const uint32_t hal_gpio_hid_clocks[];
extern const uint8_t hal_gpio_hid_speeds[];
extern const uint8_t  hal_gpio_hid_pinpositions[];
extern const uint16_t  hal_gpio_hid_pins[]; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------


extern hal_result_t hal_gpio_hid_static_memory_init(void);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




