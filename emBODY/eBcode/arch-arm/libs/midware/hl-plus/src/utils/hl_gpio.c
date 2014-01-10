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


/* @file       hl_gpio.c
	@brief      This file contains internal implementation of the hl gpio utilities.
	@author     marco.accame@iit.it
    @date       10/21/2013
**/


// - modules to be built: contains the HL_USE_* macros ----------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_GPIO)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_arch.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_gpio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hl_gpio_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


#if     defined(HL_USE_MPU_ARCH_STM32F1)
    // function which activates the GPIO clock
    #define HL_RCC_AxBx_PeriphClockCmd      RCC_APB2PeriphClockCmd
    // peripheral clock for AFIO
    #define HL_RCC_AxBxPeriph_AFIO          RCC_APB2Periph_AFIO
#elif   defined(HL_USE_MPU_ARCH_STM32F4)    
    // function which activates the GPIO clock                  
    #define HL_RCC_AxBx_PeriphClockCmd      RCC_AHB1PeriphClockCmd  
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HL_USE_MPU_ARCH_STM32F4)
static uint8_t s_hl_gpio_pin2pos(hl_gpio_pin_t pin); 
#endif//defined(HL_USE_MPU_ARCH_STM32F4)




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HL_USE_MPU_ARCH_STM32F1)

const uint8_t hl_gpio_hid_maxports = 7;

static GPIO_TypeDef *const hl_gpio_hid_ports[hl_gpio_ports_number] = 
{
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, NULL, NULL, NULL, NULL, NULL
};

const uint32_t hl_gpio_hid_clocks[hl_gpio_ports_number] = 
{
    RCC_APB2Periph_GPIOA,   RCC_APB2Periph_GPIOB,   RCC_APB2Periph_GPIOC,   RCC_APB2Periph_GPIOD, 
    RCC_APB2Periph_GPIOE,   RCC_APB2Periph_GPIOF,   RCC_APB2Periph_GPIOG,   0,
    0,                      0,                      0,                      0                       
};

#elif   defined(HL_USE_MPU_ARCH_STM32F4)

const uint8_t hl_gpio_hid_maxports = 9;

GPIO_TypeDef *const hl_gpio_hid_ports[hl_gpio_ports_number] = 
{
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, NULL, NULL, NULL
};

const uint32_t hl_gpio_hid_clocks[hl_gpio_ports_number] = 
{
    RCC_AHB1Periph_GPIOA,   RCC_AHB1Periph_GPIOB,   RCC_AHB1Periph_GPIOC,   RCC_AHB1Periph_GPIOD, 
    RCC_AHB1Periph_GPIOE,   RCC_AHB1Periph_GPIOF,   RCC_AHB1Periph_GPIOG,   RCC_AHB1Periph_GPIOH, 
    RCC_AHB1Periph_GPIOI,   0,                      0,                      0
};

#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hl_result_t hl_gpio_init(hl_gpio_init_t* init)
{
    if(NULL == init)
    {
        return(hl_res_NOK_generic);
    }

    hl_gpio_port_t      port = hl_gpio_portNONE;
    GPIO_InitTypeDef*   mode = NULL;    
    
#if     defined(HL_USE_MPU_ARCH_STM32F1)
    port = init->port;
    mode = (GPIO_InitTypeDef*)&init->mode;    
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
    port = init->port;
    mode = (GPIO_InitTypeDef*)&init->mode;
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif 

    
    if(port > hl_gpio_hid_maxports)
    {
        return(hl_res_NOK_generic);
    }
                
    // 1. enable GPIO clock 
    HL_RCC_AxBx_PeriphClockCmd(hl_gpio_hid_clocks[port], ENABLE);
    
    // 2. init
    GPIO_Init(hl_gpio_hid_ports[port], mode);
      
    return(hl_res_OK);
}


extern hl_result_t hl_gpio_altf(hl_gpio_altf_t* altf)
{
    if(NULL == altf)
    {
        return(hl_res_NOK_generic);
    }

#if     defined(HL_USE_MPU_ARCH_STM32F1)
    // 1. enable AFIO clock
    HL_RCC_AxBx_PeriphClockCmd(HL_RCC_AxBxPeriph_AFIO, ENABLE);    
    // 2. remap
    GPIO_PinRemapConfig(altf->gpio_remap, ENABLE); 
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
    uint8_t pinpos = s_hl_gpio_pin2pos(altf->gpio.pin);     
    if((0xff == pinpos) || (altf->gpio.port > hl_gpio_hid_maxports))
    {
        return(hl_res_NOK_generic);
    }
    GPIO_PinAFConfig(hl_gpio_hid_ports[altf->gpio.port], pinpos, altf->gpio_af);
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif 

    return(hl_res_OK);
}


extern hl_result_t hl_gpio_pin_write(hl_gpio_t gpio, hl_gpio_val_t val)
{  
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    if((hl_gpio_portNONE == gpio.port) || (hl_gpio_pinNONE == gpio.pin) || (hl_gpio_valUNDEF == val))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    GPIO_WriteBit(hl_gpio_hid_ports[gpio.port], gpio.pin, (hl_gpio_valRESET == val) ? (Bit_RESET) : (Bit_SET));    
    return(hl_res_OK);
}

extern hl_result_t hl_gpio_pin_toggle(hl_gpio_t gpio)
{       
    
#if     defined(HL_USE_MPU_ARCH_STM32F1)
    
    uint8_t v = GPIO_ReadOutputDataBit(hl_gpio_hid_ports[gpio.port], gpio.pin);
    GPIO_WriteBit(hl_gpio_hid_ports[gpio.port], gpio.pin, (1 == v) ? (Bit_RESET) : (Bit_SET));
    
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
    
    GPIO_ToggleBits(hl_gpio_hid_ports[gpio.port], gpio.pin);
    
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif     
    return(hl_res_OK);
}

extern hl_gpio_val_t hl_gpio_pin_input_read(hl_gpio_t gpio) 
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    if((hl_gpio_portNONE == gpio.port) || (hl_gpio_pinNONE == gpio.pin))
    {
        return(hl_gpio_valUNDEF);
    }
#endif
    
    return((hl_gpio_val_t)GPIO_ReadInputDataBit(hl_gpio_hid_ports[gpio.port], gpio.pin));
} 

extern hl_gpio_val_t hl_gpio_pin_output_read(hl_gpio_t gpio) 
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    if((hl_gpio_portNONE == gpio.port) || (hl_gpio_pinNONE == gpio.pin))
    {
        return(hl_gpio_valUNDEF);
    }
#endif
    
    return((hl_gpio_val_t)GPIO_ReadOutputDataBit(hl_gpio_hid_ports[gpio.port], gpio.pin));
} 


extern hl_result_t hl_gpio_fill_init(hl_gpio_init_t* init, const hl_gpio_map_t* gpiomap)
{
    
    if((NULL == gpiomap) || (NULL == init))
    {
        return(hl_res_NOK_generic);
    }    

#if     defined(HL_USE_MPU_ARCH_STM32F1)

    init->port                  = gpiomap->gpio.port;
    init->mode.gpio_pins        = gpiomap->gpio.pin;
 
#elif   defined(HL_USE_MPU_ARCH_STM32F4)

    init->port                  = gpiomap->gpio.port;
    init->mode.gpio_pins        = gpiomap->gpio.pin;
    
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif 
    
    return(hl_res_OK);    
}


extern hl_result_t hl_gpio_fill_altf(hl_gpio_altf_t* altf, const hl_gpio_map_t* gpiomap)
{
    if((NULL == gpiomap) || (NULL == altf))
    {
        return(hl_res_NOK_generic);
    }    

#if     defined(HL_USE_MPU_ARCH_STM32F1)

    altf->gpio_remap    = gpiomap->af32;
 
#elif   defined(HL_USE_MPU_ARCH_STM32F4)

    altf->gpio.port     = gpiomap->gpio.port;
    altf->gpio.pin      = gpiomap->gpio.pin;
    altf->gpio_af       = (uint8_t)gpiomap->af32;
    
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif 
    
    return(hl_res_OK);     
    
}

extern hl_result_t hl_gpio_pin_output_init(hl_gpio_t gpio)
{
    hl_result_t res = hl_res_OK;
    
    static const hl_gpio_init_t outgpioinit = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins  = 0,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_mode  = GPIO_Mode_Out_PP            
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins  = 0,
            .gpio_mode  = GPIO_Mode_OUT,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_otype = GPIO_OType_PP,
            .gpio_pupd  = GPIO_PuPd_UP
        }
#else
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif        
    };
    

    hl_gpio_init_t gpioinit;
    hl_gpio_map_t gpiomap;
    memcpy(&gpioinit, &outgpioinit, sizeof(hl_gpio_init_t));
    gpiomap.gpio.port = gpio.port;
    gpiomap.gpio.pin  = gpio.pin;
    gpiomap.af32      = hl_NA32;
    hl_gpio_fill_init(&gpioinit, &gpiomap);
    res = hl_gpio_init(&gpioinit);  
   
    return(res);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HL_USE_MPU_ARCH_STM32F4)
static uint8_t s_hl_gpio_pin2pos(hl_gpio_pin_t pin)
{
    if((hl_gpio_pinNONE == pin) || (hl_gpio_pinALL == pin))
    {
        return(0xff);
    }
    
    uint8_t pos = 0;
    uint32_t msk = 0x00000000;
    const uint32_t ref = (uint32_t)pin;
    for(pos=0; pos<hl_gpio_pins_number; pos++)
    {
        msk = 0x00000001 << pos;
        if((ref & msk) == msk)
        {
            return(pos);
        }
    }
    
    return(0xff);
}
#endif//defined(HL_USE_MPU_ARCH_STM32F4)

#endif//defined(HL_USE_UTIL_GPIO)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



