/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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
    @brief      This file has internal implementation of the hal gpio module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_GPIO

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include <p33fxxxx.h>

#include "hal_dspic_base_hid.h" 

#include "hal_brdcfg.h" 
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_gpio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hal_dspic_gpio_hid.h"

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

static int16_t * s_hal_gpio_arch_register_dir_ptr[HAL_BRDCFG_GPIO__maxnumber]= {(int16_t*)&TRISA, (int16_t*)&TRISB};
static int16_t * s_hal_gpio_arch_register_val_ptr[HAL_BRDCFG_GPIO__maxnumber]= {(int16_t*)&LATA, (int16_t*)&LATB};

static uint16_t s_hal_gpio_initted_mask[hal_gpio_ports_number] = { 0x0000 };
static uint16_t s_hal_gpio_output_mask[hal_gpio_ports_number] = { 0x0000 };

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_gpio_init(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_dir_t dir, hal_gpio_speed_t speed) 
{
 
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
        (*s_hal_gpio_arch_register_dir_ptr[HAL_gpio_port2index(port)]) &= ~(1 << HAL_gpio_pin2index(pin));    
        
        s_hal_gpio_output_set(port, pin);
    } 
    else if(hal_gpio_dirINP == dir)
    {
        (*s_hal_gpio_arch_register_dir_ptr[HAL_gpio_port2index(port)]) |= (1 << HAL_gpio_pin2index(pin));
        
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

    if(hal_gpio_valLOW == val) 
    {    
        (*s_hal_gpio_arch_register_val_ptr[HAL_gpio_port2index(port)]) &= ~(1 << HAL_gpio_pin2index(pin)); 
    }
    else
    {
        (*s_hal_gpio_arch_register_val_ptr[HAL_gpio_port2index(port)]) |= (1 << HAL_gpio_pin2index(pin));
    }
    return(hal_res_OK);
}

extern hal_gpio_val_t hal_gpio_getval(hal_gpio_port_t port, hal_gpio_pin_t pin) 
{
    hal_gpio_val_t val = hal_gpio_valNONE;
    
    if((hal_gpio_portNONE == port) || (hal_gpio_pinNONE == pin))
    {
        return(hal_gpio_valNONE);
    }

    // can write only initted port-pin
    if(hal_true != s_hal_gpio_initted_is(port, pin))
    {
        return(hal_gpio_valNONE);
    }
    
    if( (*s_hal_gpio_arch_register_val_ptr[HAL_gpio_port2index(port)]) && (1 << HAL_gpio_pin2index(pin)) )
    {
        val = hal_gpio_valHIGH;
    }
    else
    {
        val = hal_gpio_valLOW;
    }
    return(val);

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



