/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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


/* @file       hal_gpio.c
	@brief      This file implements internal implementation of the hal gpio module.
	@author     marco.accame@iit.it
    @date       05/27/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_GPIO

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stm32f1.h"


#include "hal_stm32_base_hid.h" 

#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_gpio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hal_stm32_gpio_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_gpio_port2index(po)             ((uint8_t)(po))
#define HAL_gpio_pin2index(pi)              ((uint8_t)(pi))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static hal_boolval_t s_hal_gpio_supported_is(hal_gpio_port_t port, hal_gpio_pin_t pin);

static void s_hal_gpio_initted_set(hal_gpio_port_t port, hal_gpio_pin_t pin);

static hal_boolval_t s_hal_gpio_initted_is(hal_gpio_port_t port, hal_gpio_pin_t pin);

static void s_hal_gpio_output_set(hal_gpio_port_t port, hal_gpio_pin_t pin);

static void s_hal_gpio_output_clear(hal_gpio_port_t port, hal_gpio_pin_t pin);

static hal_boolval_t s_hal_gpio_output_is(hal_gpio_port_t port, hal_gpio_pin_t pin);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static GPIO_TypeDef *const s_GPIO_PORT[7] = 
{
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
};

static const uint32_t s_GPIO_CLK[7] = 
{
    RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, 
    RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOE, RCC_APB2Periph_GPIOF, RCC_APB2Periph_GPIOG
};

static uint16_t s_hal_gpio_initted_mask[hal_gpio_ports_number] = { 0x0000 };
static uint16_t s_hal_gpio_output_mask[hal_gpio_ports_number] = { 0x0000 };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_gpio_init(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_dir_t dir, hal_gpio_speed_t speed) 
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    
    if((hal_gpio_portNONE == port) || (hal_gpio_pinNONE == pin) || (hal_gpio_dirNONE == dir))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_true != s_hal_gpio_supported_is(port, pin))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_gpio_dirOUT == dir)
    {
        GPIOSpeed_TypeDef stm32speed = ((hal_gpio_speed_high == speed) ? (GPIO_Speed_50MHz) : ( (hal_gpio_speed_medium==speed)?(GPIO_Speed_10MHz):( GPIO_Speed_2MHz) ) );
        /* Enable the GPIO_LED Clock */
        RCC_APB2PeriphClockCmd(s_GPIO_CLK[port], ENABLE);    
        /* Configure the GPIO_LED pin */
        GPIO_InitStructure.GPIO_Pin = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_InitStructure.GPIO_Speed = stm32speed;
        GPIO_Init(s_GPIO_PORT[port], &GPIO_InitStructure);

        s_hal_gpio_output_set(port, pin);
    } 
    else if(hal_gpio_dirINP == dir)
    {
        /* Enable Button GPIO clock */
        RCC_APB2PeriphClockCmd(s_GPIO_CLK[port] | RCC_APB2Periph_AFIO, ENABLE);        
        /* Configure Button pin as input floating */
        GPIO_InitStructure.GPIO_Pin = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(s_GPIO_PORT[port], &GPIO_InitStructure);

        s_hal_gpio_output_clear(port, pin);
    }

    s_hal_gpio_initted_set(port, pin);
    
    return(hal_res_OK);
}

extern hal_result_t hal_gpio_setval(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_val_t val)
{
    if((hal_gpio_portNONE == port) || (hal_gpio_pinNONE == pin) || (hal_gpio_valNONE == val))
    {
        return(hal_res_NOK_generic);
    }

    // can write only initted port-pin
    if(hal_true != s_hal_gpio_initted_is(port, pin))
    {
        return(hal_res_NOK_generic);
    }

    // can write only output port-pin
    if(hal_true != s_hal_gpio_output_is(port, pin))
    {
        return(hal_res_NOK_generic);
    }


        
    GPIO_WriteBit(s_GPIO_PORT[port], (uint16_t)(0x0001 << pin), (0 == val) ? (Bit_RESET) : (Bit_SET)); 
    
    return(hal_res_OK);
}

extern hal_gpio_val_t hal_gpio_getval(hal_gpio_port_t port, hal_gpio_pin_t pin) 
{
    if((hal_gpio_portNONE == port) || (hal_gpio_pinNONE == pin))
    {
        return(hal_gpio_valNONE);
    }

    // can write only initted port-pin
    if(hal_true != s_hal_gpio_initted_is(port, pin))
    {
        return(hal_gpio_valNONE);
    }

    return((hal_gpio_val_t)GPIO_ReadInputDataBit(s_GPIO_PORT[port], (uint16_t)(0x0001 << (uint8_t)pin)));

} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_gpio_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_gpio_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    memset(s_hal_gpio_initted_mask, 0, sizeof(s_hal_gpio_initted_mask));
    memset(s_hal_gpio_output_mask, 0, sizeof(s_hal_gpio_output_mask));
    return(hal_res_OK);
}

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_gpio_supported_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t p = hal_brdcfg_gpio__supported_mask[HAL_gpio_port2index(port)];
    return(hal_base_hid_halfword_bitcheck(p, HAL_gpio_pin2index(pin)) );
}


static void s_hal_gpio_initted_set(hal_gpio_port_t port, hal_gpio_pin_t pin)
{   // cannot be called with port and pin of value NONE
    uint16_t *pp = &s_hal_gpio_initted_mask[HAL_gpio_port2index(port)];
    hal_base_hid_halfword_bitset(pp, HAL_gpio_pin2index(pin));
}

static hal_boolval_t s_hal_gpio_initted_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{   // cannot be called with port and pin of value NONE
    uint16_t p = s_hal_gpio_initted_mask[HAL_gpio_port2index(port)];
    return(hal_base_hid_halfword_bitcheck(p, HAL_gpio_pin2index(pin)) );
}

static void s_hal_gpio_output_set(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t *pp = &s_hal_gpio_output_mask[HAL_gpio_port2index(port)];
    hal_base_hid_halfword_bitset(pp, HAL_gpio_pin2index(pin));
}

static void s_hal_gpio_output_clear(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t *pp = &s_hal_gpio_output_mask[HAL_gpio_port2index(port)];
    hal_base_hid_halfword_bitclear(pp, HAL_gpio_pin2index(pin));
}

static hal_boolval_t s_hal_gpio_output_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t p = s_hal_gpio_output_mask[HAL_gpio_port2index(port)];
    return(hal_base_hid_halfword_bitcheck(p, HAL_gpio_pin2index(pin)) );
}



#endif//HAL_USE_GPIO
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



