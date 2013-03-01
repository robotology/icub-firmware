/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hal_device_mux.c
	@brief      This file implements internal implementation of the hal muxport module.
	@author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       02/07/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_MUX

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_brdcfg.h"
#include "hal_utility_bits.h" 



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mux.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_mux_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_mux_muxport2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_mux_cfg_t hal_mux_cfg_default = { .nothing = 0 };

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_mux_cfg_t           config;
    // altre cose tipo gpio etc etc ...
    hal_gpio_port_t         enable_port;
    hal_gpio_pin_t          enable_pin;
    hal_gpio_port_t         sel0_port;
    hal_gpio_pin_t          sel0_pin;  
    hal_gpio_port_t         sel1_port;
    hal_gpio_pin_t          sel1_pin;    
} hal_mux_internals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_mux_supported_is(hal_mux_port_t muxport);
static void s_hal_device_mux_initted_set(hal_mux_port_t muxport);
static hal_boolval_t s_hal_device_mux_initted_is(hal_mux_port_t muxport);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_hal_device_mux_initted = 0;

static hal_mux_internals_t* s_hal_device_mux_internals[hal_mux_ports_num] = { NULL };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_mux_init(hal_mux_port_t muxport, const hal_mux_cfg_t *cfg)
{
    hal_mux_internals_t* intitem = s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)];

    if(hal_false == s_hal_device_mux_supported_is(muxport))
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_mux_cfg_default;
    }
     
        
    // configure the required mux port and put it in selNONE.
    
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)] = hal_heap_new(sizeof(hal_mux_internals_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }       
    
    memcpy(&intitem->config, cfg, sizeof(hal_mux_cfg_t));   
    
    intitem->enable_port   = hal_brdcfg_device_mux__theconfig.gpio_enable[HAL_device_mux_muxport2index(muxport)].port;
    intitem->enable_pin    = hal_brdcfg_device_mux__theconfig.gpio_enable[HAL_device_mux_muxport2index(muxport)].pin;
    intitem->sel0_port     = hal_brdcfg_device_mux__theconfig.gpio_sel0[HAL_device_mux_muxport2index(muxport)].port;
    intitem->sel0_pin      = hal_brdcfg_device_mux__theconfig.gpio_sel0[HAL_device_mux_muxport2index(muxport)].pin;    
    intitem->sel1_port     = hal_brdcfg_device_mux__theconfig.gpio_sel1[HAL_device_mux_muxport2index(muxport)].port;
    intitem->sel1_pin      = hal_brdcfg_device_mux__theconfig.gpio_sel1[HAL_device_mux_muxport2index(muxport)].pin;   
    
    
    hal_gpio_configure(hal_brdcfg_device_mux__theconfig.gpio_enable[HAL_device_mux_muxport2index(muxport)], NULL);
    hal_gpio_setval(intitem->enable_port, intitem->enable_pin, hal_gpio_valHIGH);
    
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    
    hal_gpio_configure(hal_brdcfg_device_mux__theconfig.gpio_sel0[HAL_device_mux_muxport2index(muxport)], NULL);
    hal_gpio_setval(intitem->sel0_port, intitem->sel0_pin, hal_gpio_valHIGH);    
    
    hal_gpio_configure(hal_brdcfg_device_mux__theconfig.gpio_sel1[HAL_device_mux_muxport2index(muxport)], NULL);
    hal_gpio_setval(intitem->sel1_port, intitem->sel1_pin, hal_gpio_valHIGH);    
    
        

    s_hal_device_mux_initted_set(muxport);
    return(hal_res_OK);
}


extern hal_result_t hal_mux_enable(hal_mux_port_t muxport, hal_mux_sel_t muxsel)
{    
    hal_mux_internals_t* intitem = s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)];
    
    static const hal_gpio_val_t s_values_sel0[hal_mux_sels_num] = {hal_gpio_valLOW,   hal_gpio_valHIGH,   hal_gpio_valLOW};
    static const hal_gpio_val_t s_values_sel1[hal_mux_sels_num] = {hal_gpio_valLOW,   hal_gpio_valLOW,    hal_gpio_valHIGH};
    
    if(hal_false == s_hal_device_mux_initted_is(muxport))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    
    
    if(hal_mux_selNONE == muxsel)
    {
        return(hal_mux_disable(muxport));
    }
    
    hal_gpio_setval(intitem->enable_port, intitem->enable_pin, hal_gpio_valHIGH);
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    hal_gpio_setval(intitem->sel0_port, intitem->sel0_pin, s_values_sel0[(uint8_t)muxsel]);    
    hal_gpio_setval(intitem->sel1_port, intitem->sel1_pin, s_values_sel1[(uint8_t)muxsel]);   
    
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    
    hal_gpio_setval(intitem->enable_port, intitem->enable_pin, hal_gpio_valLOW);
    
    return(hal_res_OK);
}



extern hal_result_t hal_mux_disable(hal_mux_port_t muxport)
{
    hal_mux_internals_t* intitem = s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)];
    
    if(hal_false == s_hal_device_mux_initted_is(muxport))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    

    hal_gpio_setval(intitem->enable_port, intitem->enable_pin, hal_gpio_valHIGH);
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    hal_gpio_setval(intitem->sel0_port, intitem->sel0_pin, hal_gpio_valHIGH);    
    hal_gpio_setval(intitem->sel1_port, intitem->sel1_pin, hal_gpio_valHIGH);   
    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern hal_result_t hal_device_mux_hid_static_memory_init(void)
{
    s_hal_device_mux_initted = 0;
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_mux_supported_is(hal_mux_port_t muxport)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_mux__theconfig.supported_mask, HAL_device_mux_muxport2index(muxport)));
}


static void s_hal_device_mux_initted_set(hal_mux_port_t muxport)
{
    hal_utility_bits_byte_bitset(&s_hal_device_mux_initted, HAL_device_mux_muxport2index(muxport));
}


static hal_boolval_t s_hal_device_mux_initted_is(hal_mux_port_t muxport)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_device_mux_initted, HAL_device_mux_muxport2index(muxport)));
}




#endif//HAL_USE_DEVICE_MUX

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



