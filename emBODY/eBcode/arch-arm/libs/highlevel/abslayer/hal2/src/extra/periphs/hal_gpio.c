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
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"

#ifdef HAL_USE_GPIO

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hl_bits.h" 
#include "hl_gpio.h"
#include "hl_arch.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_gpio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hal_gpio_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_gpio_port2index(po)             ((uint8_t)(po))

#define HAL_gpio_speed2index(sp)            ((uint8_t)(sp))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_gpio_cfg_t hal_gpio_cfg_default =
{
    .dir                    = hal_gpio_dirOUT,
    .speed                  = hal_gpio_speed_low,
    .altcfg                 = NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    uint16_t    inittedmask_byport[hal_gpio_ports_number];
    uint16_t    outputmask_byport[hal_gpio_ports_number];
} hal_gpio_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static hal_boolval_t s_hal_gpio_supported_is(hal_gpio_port_t port, hal_gpio_pin_t pin);

static void s_hal_gpio_initted_set(hal_gpio_port_t port, hal_gpio_pin_t pin);

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK) 
static hal_boolval_t s_hal_gpio_initted_is(hal_gpio_port_t port, hal_gpio_pin_t pin);
#endif

static void s_hal_gpio_output_set(hal_gpio_port_t port, hal_gpio_pin_t pin);

static void s_hal_gpio_output_clear(hal_gpio_port_t port, hal_gpio_pin_t pin);

static hal_boolval_t s_hal_gpio_output_is(hal_gpio_port_t port, hal_gpio_pin_t pin);

static hal_result_t s_hal_gpio_altfun_configure(hal_gpio_t gpio, const hal_gpio_cfg_t* cfg);

static GPIOSpeed_TypeDef s_hal_gpio_get_stm32_speed( hal_gpio_speed_t speed);

static hal_result_t s_hal_gpio_init(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_dir_t dir, hal_gpio_speed_t speed);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_gpio_theinternals_t s_hal_gpio_theinternals =
{
    .inittedmask_byport     = { 0 },
    .outputmask_byport      = { 0 }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_gpio_init(hal_gpio_t gpio, const hal_gpio_cfg_t* cfg)
{
    if(NULL == cfg)
    {
        cfg = &hal_gpio_cfg_default;
    }
    
    if(hal_gpio_dirALT != cfg->dir)
    {
        return(s_hal_gpio_init(gpio.port, gpio.pin, cfg->dir, cfg->speed));
    }   
    
    return(s_hal_gpio_altfun_configure(gpio, cfg));    
}


extern hal_result_t hal_gpio_setval(hal_gpio_t gpio, hal_gpio_val_t val)
{
    hl_gpio_t hlgpio = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE };

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK) 
    if((hal_gpio_portNONE == gpio.port) || (hal_gpio_pinNONE == gpio.pin) || (hal_gpio_valNONE == val))
    {
        return(hal_res_NOK_generic);
    }
#endif

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    // can write only initted port-pin
    if(hal_true != s_hal_gpio_initted_is(gpio.port, gpio.pin))
    {
        return(hal_res_NOK_generic);
    }

    // can write only output port-pin
    if(hal_true != s_hal_gpio_output_is(gpio.port, gpio.pin))
    {
        return(hal_res_NOK_generic);
    }
#endif
   
    hlgpio.port = (hl_gpio_port_t)gpio.port;
    hlgpio.pin = (hl_gpio_pin_t)gpio.pin;
    hl_gpio_pin_write(hlgpio, (hl_gpio_val_t)val);
    
    return(hal_res_OK);
}

extern hal_gpio_val_t hal_gpio_getval(hal_gpio_t gpio) 
{
    hl_gpio_t hlgpio = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE };

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK) 
    if((hal_gpio_portNONE == gpio.port) || (hal_gpio_pinNONE == gpio.pin))
    {
        return(hal_gpio_valNONE);
    }
#endif
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    // can write only initted port-pin
    if(hal_true != s_hal_gpio_initted_is(gpio.port, gpio.pin))
    {
        return(hal_gpio_valNONE);
    }
#endif
    
    hlgpio.port = (hl_gpio_port_t)gpio.port;
    hlgpio.pin  = (hl_gpio_pin_t)gpio.pin;
    hl_gpio_val_t v;
    
    
    if(hal_true == s_hal_gpio_output_is(gpio.port, gpio.pin))
    {
        v = hl_gpio_pin_output_read(hlgpio);
    }
    else
    {
        v = hl_gpio_pin_input_read(hlgpio);    
    }
    
    return((hal_gpio_val_t)v);
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

static hal_boolval_t s_hal_gpio_supported_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t p = hal_gpio__theboardconfig.supportedmask_byport[HAL_gpio_port2index(port)];
    return((hal_boolval_t)hl_bits_hlfword_maskcheck(p, pin));
}


static void s_hal_gpio_initted_set(hal_gpio_port_t port, hal_gpio_pin_t pin)
{   // cannot be called with port and pin of value NONE
    uint16_t *pp = &s_hal_gpio_theinternals.inittedmask_byport[HAL_gpio_port2index(port)];
    hl_bits_hlfword_maskset(pp, pin);
}

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK) 
static hal_boolval_t s_hal_gpio_initted_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{   // cannot be called with port and pin of value NONE
    uint16_t p = s_hal_gpio_theinternals.inittedmask_byport[HAL_gpio_port2index(port)];
    return((hal_boolval_t)hl_bits_hlfword_maskcheck(p, pin));
}
#endif

static void s_hal_gpio_output_set(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t *pp = &s_hal_gpio_theinternals.outputmask_byport[HAL_gpio_port2index(port)];
    hl_bits_hlfword_maskset(pp, pin);
}

static void s_hal_gpio_output_clear(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t *pp = &s_hal_gpio_theinternals.outputmask_byport[HAL_gpio_port2index(port)];
    hl_bits_hlfword_maskclear(pp, pin);
}

static hal_boolval_t s_hal_gpio_output_is(hal_gpio_port_t port, hal_gpio_pin_t pin)
{
    uint16_t p = s_hal_gpio_theinternals.outputmask_byport[HAL_gpio_port2index(port)];
    return((hal_boolval_t)hl_bits_hlfword_maskcheck(p, pin));
}

static hal_result_t s_hal_gpio_init(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_dir_t dir, hal_gpio_speed_t speed) 
{
    
    if((hal_gpio_portNONE == port) || (hal_gpio_pinNONE == pin) || (hal_gpio_dirNONE == dir) || (hal_gpio_speed_NONE == speed))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_true != s_hal_gpio_supported_is(port, pin))
    {
        return(hal_res_NOK_unsupported);
    }


#if     defined(HAL_USE_MPU_TYPE_STM32F1)
    const hl_gpio_initmode_f1_t gpiomodeOUT =
    {
        .gpio_pins  = 0,
        .gpio_speed = 0,
        .gpio_mode  = GPIO_Mode_Out_PP        
    };
    const hl_gpio_initmode_f1_t gpiomodeINP =
    {
        .gpio_pins  = 0,
        .gpio_speed = 0,
        .gpio_mode  = GPIO_Mode_IN_FLOATING                  
    };    
#elif   defined(HAL_USE_MPU_TYPE_STM32F4) 
    const hl_gpio_initmode_fx_t gpiomodeOUT =
    {
        .gpio_pins  = 0,
        .gpio_mode  = GPIO_Mode_OUT,
        .gpio_speed = GPIO_Speed_2MHz,
        .gpio_otype = GPIO_OType_PP,
        .gpio_pupd  = GPIO_PuPd_UP        
    };
    const hl_gpio_initmode_fx_t gpiomodeINP =
    {
        .gpio_pins  = 0,
        .gpio_mode  = GPIO_Mode_IN,
        .gpio_speed = GPIO_Speed_2MHz,  
        .gpio_otype = GPIO_OType_PP, 
        .gpio_pupd  = GPIO_PuPd_NOPULL    
    };
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif

    hl_gpio_init_t gpioinit;
    gpioinit.port   = (hl_gpio_port_t)port;
    if(hal_gpio_dirOUT == dir)
    {
        memcpy(&gpioinit.mode, &gpiomodeOUT, sizeof(gpiomodeOUT));
        gpioinit.mode.gpio_pins     = pin;
        gpioinit.mode.gpio_speed    = s_hal_gpio_get_stm32_speed(speed);
        
        if(hl_res_OK != hl_gpio_init(&gpioinit))
        {
            return(hal_res_NOK_generic);
        }
        s_hal_gpio_output_set(port, pin);
    }
    else if(hal_gpio_dirINP == dir)
    {
        memcpy(&gpioinit.mode, &gpiomodeINP, sizeof(gpiomodeINP));
        gpioinit.mode.gpio_pins     = pin;
        
        if(hl_res_OK != hl_gpio_init(&gpioinit))
        {
            return(hal_res_NOK_generic);
        }        
        s_hal_gpio_output_clear(port, pin);
    }

 
    s_hal_gpio_initted_set(port, pin);
    
    return(hal_res_OK);
}

static hal_result_t s_hal_gpio_altfun_configure(hal_gpio_t gpio, const hal_gpio_cfg_t* cfg)
{   // not supported so far
    return(hal_res_NOK_generic);    
}

// typedef struct 
// {
//     hal_gpio_port_t     port;
//     hal_gpio_pin_t      pin;
//     hal_gpio_dir_t      dir;
//     hal_gpio_speed_t    speed;
// } hal_gpio_xxx_t;
//     hal_gpio_xxx_t xxx;
//     xxx.port    = gpio.port;
//     xxx.pin     = gpio.pin;
//     xxx.dir     = cfg->dir;
//     xxx.speed   = cfg->speed;
//
// it should contain a meaninful altcfg
// static hal_result_t s_hal_gpio_altfun_configure(hal_gpio_xxx_t xxx, void* altcfg)
// {
//     
//     hl_gpio_init_t gpioinit;
//     hl_gpio_altf_t gpioaltf;
//     
//     // someone must have copied into altcfg all it is required ...
//     if(NULL == altcfg)
//     {
//         return(hal_res_NOK_generic);
//     }
//     
//     gpioinit.port = (hl_gpio_port_t)xxx.port;
//     memcpy(&gpioinit.mode, &altcfg->gpioext, sizeof(gpioinit.mode));
//     gpioinit.mode.gpio_pins = xxx.pin; 
//     if(hal_gpio_speed_default != xxx.speed)
//     {   // if the gpiocfg specifies something different from default, then we use it.
//         gpioinit.mode.gpio_speed = s_hal_gpio_get_stm32_speed(xxx.speed);
//     }
//     hl_gpio_init(&gpioinit);
//     
//     
//     hl_gpio_map_t gpiomap = {.gpio.port = gpioinit.port, .gpio.pin = (hl_gpio_pin_t)gpioinit.mode.gpio_pins, .af32 = altcfg->afname};
//     
//     if(HAL_GPIO_AFMODE_NONE != altcfg->afmode)
//     {
//         hl_gpio_fill_altf(&gpioaltf, &gpiomap);
//     }

//     return(hal_res_OK);       
// }

static GPIOSpeed_TypeDef s_hal_gpio_get_stm32_speed( hal_gpio_speed_t speed)
{
#if     defined(HAL_USE_MPU_TYPE_STM32F1)
    static const uint8_t s_hal_gpio_hid_speeds[hal_gpio_speeds_number] = 
    { 
        GPIO_Speed_2MHz,        // hal_gpio_speed_default
        GPIO_Speed_2MHz,        // hal_gpio_speed_low
        GPIO_Speed_10MHz,       // hal_gpio_speed_medium
        GPIO_Speed_50MHz,       // hal_gpio_speed_high  
        GPIO_Speed_50MHz        // hal_gpio_speed_max     
    }; 
#elif   defined(HAL_USE_MPU_TYPE_STM32F4)
    static const uint8_t s_hal_gpio_hid_speeds[hal_gpio_speeds_number] = 
    { 
        GPIO_Speed_2MHz,        // hal_gpio_speed_default
        GPIO_Speed_2MHz,        // hal_gpio_speed_low
        GPIO_Speed_25MHz,       // hal_gpio_speed_medium
        GPIO_Speed_50MHz,       // hal_gpio_speed_high,  
        GPIO_Speed_100MHz       // hal_gpio_speed_max       
    };
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif 

    return((GPIOSpeed_TypeDef)s_hal_gpio_hid_speeds[HAL_gpio_speed2index(speed)]);
}

#endif//HAL_USE_GPIO

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



