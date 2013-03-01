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
#include "hal_middleware_interface.h"


#include "hal_base_hid.h" 

#include "hal_utility_bits.h" 

#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_gpio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hal_mpu_gpio_hid.h"

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

static hal_result_t s_hal_gpio_altfun_configure(hal_gpio_cfg_t cfg, const hal_gpio_altcfg_t* altcfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HAL_USE_CPU_FAM_STM32F1)

const uint8_t hal_gpio_hid_maxports = 7;

GPIO_TypeDef *const hal_gpio_hid_ports[hal_gpio_ports_number] = 
{
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, NULL, NULL
};

const uint32_t hal_gpio_hid_clocks[hal_gpio_ports_number] = 
{
    RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, RCC_APB2Periph_GPIOD, 
    RCC_APB2Periph_GPIOE, RCC_APB2Periph_GPIOF, RCC_APB2Periph_GPIOG, 0,
    0
};

const uint8_t hal_gpio_hid_speeds[hal_gpio_speeds_number] = 
{ 
    GPIO_Speed_2MHz,        // hal_gpio_speed_default
    GPIO_Speed_2MHz,        // hal_gpio_speed_low
    GPIO_Speed_10MHz,       // hal_gpio_speed_medium
    GPIO_Speed_50MHz,       // hal_gpio_speed_high  
    GPIO_Speed_50MHz        // hal_gpio_speed_max     
};

const uint8_t  hal_gpio_hid_pinpositions[]  =         { 0,  1,  2,  3,
                                                        4,  5,  6,  7,
                                                        8,  9,  10, 11,
                                                        12, 13, 14, 15 }; 

const uint16_t  hal_gpio_hid_pins[]         =         { GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
                                                        GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
                                                        GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
                                                        GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15 }; 

#elif   defined(HAL_USE_CPU_FAM_STM32F4)

const uint8_t hal_gpio_hid_maxports = 9;

GPIO_TypeDef *const hal_gpio_hid_ports[hal_gpio_ports_number] = 
{
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI
};

const uint32_t hal_gpio_hid_clocks[hal_gpio_ports_number] = 
{
    RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC, RCC_AHB1Periph_GPIOD, 
    RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF, RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, 
    RCC_AHB1Periph_GPIOI
};

const uint8_t hal_gpio_hid_speeds[hal_gpio_speeds_number] = 
{ 
    GPIO_Speed_2MHz,        // hal_gpio_speed_default
    GPIO_Speed_2MHz,        // hal_gpio_speed_low
    GPIO_Speed_25MHz,       // hal_gpio_speed_medium
    GPIO_Speed_50MHz,       // hal_gpio_speed_high,  
    GPIO_Speed_100MHz       // hal_gpio_speed_max       
};

const uint8_t  hal_gpio_hid_pinpositions[]     =      { GPIO_PinSource0,  GPIO_PinSource1,  GPIO_PinSource2,  GPIO_PinSource3,
                                                        GPIO_PinSource4,  GPIO_PinSource5,  GPIO_PinSource6,  GPIO_PinSource7,
                                                        GPIO_PinSource8,  GPIO_PinSource9,  GPIO_PinSource10, GPIO_PinSource11,
                                                        GPIO_PinSource12, GPIO_PinSource13, GPIO_PinSource14, GPIO_PinSource15 }; 

const uint16_t  hal_gpio_hid_pins[]          =        { GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
                                                        GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
                                                        GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
                                                        GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15 }; 
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hal_gpio_initted_mask[hal_gpio_ports_number] = { 0x0000 };
static uint16_t s_hal_gpio_output_mask[hal_gpio_ports_number] = { 0x0000 };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// __attribute__ ((weak)) extern void pippo(void)
// //__weak extern void pippo(void)
// {
//     volatile uint8_t aaa = 10;
//     uint8_t i;
//     for(i=0; i<10; i++)
//     {
//         aaa --;
//     }
// }
// extern void pippo(void)
// {
//    hal_gpio_init(hal_gpio_portA, hal_gpio_pin1, hal_gpio_dirNONE, hal_gpio_speed_default);
// }


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

    GPIO_StructInit(&GPIO_InitStructure);
    
    if(hal_gpio_dirOUT == dir)
    {
#if     defined(HAL_USE_CPU_FAM_STM32F1)
        // enable GPIO clock 
        RCC_APB2PeriphClockCmd(hal_gpio_hid_clocks[port], ENABLE);    
        // Configure the GPIO pin
        GPIO_InitStructure.GPIO_Pin     = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Speed   = (GPIOSpeed_TypeDef)hal_gpio_hid_speeds[(uint8_t)speed];
        GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
        // enable GPIO clock 
        RCC_AHB1PeriphClockCmd(hal_gpio_hid_clocks[port], ENABLE);    
        // Configure the GPIO pin
        GPIO_InitStructure.GPIO_Pin     = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Speed   = (GPIOSpeed_TypeDef)hal_gpio_hid_speeds[(uint8_t)speed];
        GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP; 
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
       
        GPIO_Init(hal_gpio_hid_ports[port], &GPIO_InitStructure);

        s_hal_gpio_output_set(port, pin);
    } 
    else if(hal_gpio_dirINP == dir)
    {
#if     defined(HAL_USE_CPU_FAM_STM32F1)
        // enable GPIO clock
        RCC_APB2PeriphClockCmd(hal_gpio_hid_clocks[port], ENABLE);
        // configure pin as input floating
        GPIO_InitStructure.GPIO_Pin     = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
#elif   defined(HAL_USE_CPU_FAM_STM32F4)    
        // enable GPIO clock                  
        RCC_AHB1PeriphClockCmd(hal_gpio_hid_clocks[port], ENABLE);        
        /// configure pin as input floating
        GPIO_InitStructure.GPIO_Pin     = (uint16_t)(0x0001 << pin);
        GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
        
        GPIO_Init(hal_gpio_hid_ports[port], &GPIO_InitStructure);

        s_hal_gpio_output_clear(port, pin);
    } 
    else 
    {
        return(hal_res_NOK_generic);
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

        
    GPIO_WriteBit(hal_gpio_hid_ports[port], (uint16_t)(0x0001 << pin), (hal_gpio_valLOW == val) ? (Bit_RESET) : (Bit_SET)); 
    
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

    return((hal_gpio_val_t)GPIO_ReadInputDataBit(hal_gpio_hid_ports[port], (uint16_t)(0x0001 << (uint8_t)pin)));

} 


extern void hal_gpio_quickest_setval(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_val_t val)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1)
    volatile uint32_t* outvalreg = (hal_gpio_valLOW == val) ?  (&hal_gpio_hid_ports[port]->BRR) : (&hal_gpio_hid_ports[port]->BSRR);      
    *outvalreg |= (1<<pin);
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
   volatile uint16_t* outcmdreg = (hal_gpio_valLOW == val) ?  (&hal_gpio_hid_ports[port]->BSRRH) : (&hal_gpio_hid_ports[port]->BSRRL); 
    *outcmdreg  = (1<<pin);
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif     
}


extern hal_result_t hal_gpio_configure(hal_gpio_cfg_t cfg, const hal_gpio_altcfg_t* altcfg) 
{
    if(hal_gpio_dirALT != cfg.dir)
    {
        return(hal_gpio_init(cfg.port, cfg.pin, cfg.dir, cfg.speed));
    }
    
    return(s_hal_gpio_altfun_configure(cfg, altcfg));
 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_gpio_hid_static_memory_init(void)
{
    memset(s_hal_gpio_initted_mask, 0, sizeof(s_hal_gpio_initted_mask));
    memset(s_hal_gpio_output_mask, 0, sizeof(s_hal_gpio_output_mask));
    return(hal_res_OK);
}

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_gpio_supported_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t p = hal_brdcfg_gpio__theconfig.supported_mask_byport[HAL_gpio_port2index(port)];
    return(hal_utility_bits_halfword_bitcheck(p, HAL_gpio_pin2index(pin)) );
}


static void s_hal_gpio_initted_set(hal_gpio_port_t port, hal_gpio_pin_t pin)
{   // cannot be called with port and pin of value NONE
    uint16_t *pp = &s_hal_gpio_initted_mask[HAL_gpio_port2index(port)];
    hal_utility_bits_halfword_bitset(pp, HAL_gpio_pin2index(pin));
}

static hal_boolval_t s_hal_gpio_initted_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{   // cannot be called with port and pin of value NONE
    uint16_t p = s_hal_gpio_initted_mask[HAL_gpio_port2index(port)];
    return(hal_utility_bits_halfword_bitcheck(p, HAL_gpio_pin2index(pin)) );
}

static void s_hal_gpio_output_set(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t *pp = &s_hal_gpio_output_mask[HAL_gpio_port2index(port)];
    hal_utility_bits_halfword_bitset(pp, HAL_gpio_pin2index(pin));
}

static void s_hal_gpio_output_clear(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t *pp = &s_hal_gpio_output_mask[HAL_gpio_port2index(port)];
    hal_utility_bits_halfword_bitclear(pp, HAL_gpio_pin2index(pin));
}

static hal_boolval_t s_hal_gpio_output_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t p = s_hal_gpio_output_mask[HAL_gpio_port2index(port)];
    return(hal_utility_bits_halfword_bitcheck(p, HAL_gpio_pin2index(pin)) );
}

static hal_result_t s_hal_gpio_altfun_configure(hal_gpio_cfg_t cfg, const hal_gpio_altcfg_t* altcfg)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    // someone must have copied into altcfg all it is required ...
    if(NULL == altcfg)
    {
        return(hal_res_NOK_generic);
    }
    
    // we dont have this port in the mpu
    if(0 == hal_gpio_hid_clocks[cfg.port])
    {
        return(hal_res_NOK_unsupported);
    }
    
    // we use cfg.port and cfg.pin from the hal_gpio_cfg_t, we understand that we are in alt mode from cfg.dir
    // and for the rest we use altcfg
    GPIO_StructInit(&GPIO_InitStructure);
    
    // configure gpio
    memcpy(&GPIO_InitStructure, &altcfg->gpioext, sizeof(GPIO_InitTypeDef));
    // any changes to GPIO_InitStructure ?? only the pin !
    GPIO_InitStructure.GPIO_Pin = hal_gpio_hid_pins[cfg.pin]; 
    if(hal_gpio_speed_default != cfg.speed)
    {   // if the gpiocfg specifies something different from default, then we use it.
        GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)hal_gpio_hid_speeds[cfg.speed];
    }
    else
    {   // else we use the speed in gpioext
        GPIO_InitStructure.GPIO_Speed = GPIO_InitStructure.GPIO_Speed;
    }
    
#if     defined(HAL_USE_CPU_FAM_STM32F1)    
    // enable gpio clock and its its afio clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | hal_gpio_hid_clocks[cfg.port], ENABLE);    
    // init the gpio
    GPIO_Init(hal_gpio_hid_ports[cfg.port], &GPIO_InitStructure);    
    // remap gpio ... but only if it is not 0
    if(HAL_GPIO_AFMODE_NONE != altcfg->afmode)
    {
        GPIO_PinRemapConfig(altcfg->afname, (FunctionalState)altcfg->afmode);   
    }        
#elif   defined(HAL_USE_CPU_FAM_STM32F4)  
    // enable gpio clock
    RCC_AHB1PeriphClockCmd(hal_gpio_hid_clocks[cfg.port], ENABLE);
    // init the gpio
    GPIO_Init(hal_gpio_hid_ports[cfg.port], &GPIO_InitStructure);    
    // remap gpio 
    if(HAL_GPIO_AFMODE_NONE != altcfg->afmode)   
    {        
        GPIO_PinAFConfig(hal_gpio_hid_ports[cfg.port], hal_gpio_hid_pinpositions[cfg.pin], altcfg->afname);
    }
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif   

    return(hal_res_OK);   
}



#endif//HAL_USE_GPIO
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



