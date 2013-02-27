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
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_mux_initted[hal_mux_ports_num] = { hal_false };

static hal_mux_internals_t s_hal_device_mux_internals[hal_mux_ports_num] = {0};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_mux_init(hal_mux_port_t muxport, const hal_mux_cfg_t *cfg)
{
    hal_mux_internals_t* mint = &s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)];

    if(hal_false == s_hal_device_mux_supported_is(muxport))
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_mux_cfg_default;
    }
     
        
    // configure the required mux port and put it in selNONE.
    
    memcpy(&mint->config, cfg, sizeof(hal_mux_cfg_t));   
    
    mint->enable_port   = hal_brdcfg_device_mux__theconfig.gpio_enable[HAL_device_mux_muxport2index(muxport)].port;
    mint->enable_pin    = hal_brdcfg_device_mux__theconfig.gpio_enable[HAL_device_mux_muxport2index(muxport)].pin;
    mint->sel0_port     = hal_brdcfg_device_mux__theconfig.gpio_sel0[HAL_device_mux_muxport2index(muxport)].port;
    mint->sel0_pin      = hal_brdcfg_device_mux__theconfig.gpio_sel0[HAL_device_mux_muxport2index(muxport)].pin;    
    mint->sel1_port     = hal_brdcfg_device_mux__theconfig.gpio_sel1[HAL_device_mux_muxport2index(muxport)].port;
    mint->sel1_pin      = hal_brdcfg_device_mux__theconfig.gpio_sel1[HAL_device_mux_muxport2index(muxport)].pin;   
    
    
    hal_gpio_configure(hal_brdcfg_device_mux__theconfig.gpio_enable[HAL_device_mux_muxport2index(muxport)], NULL);
    hal_gpio_setval(mint->enable_port, mint->enable_pin, hal_gpio_valHIGH);
    
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    
    hal_gpio_configure(hal_brdcfg_device_mux__theconfig.gpio_sel0[HAL_device_mux_muxport2index(muxport)], NULL);
    hal_gpio_setval(mint->sel0_port, mint->sel0_pin, hal_gpio_valHIGH);    
    
    hal_gpio_configure(hal_brdcfg_device_mux__theconfig.gpio_sel1[HAL_device_mux_muxport2index(muxport)], NULL);
    hal_gpio_setval(mint->sel1_port, mint->sel1_pin, hal_gpio_valHIGH);    
    
        

    s_hal_device_mux_initted_set(muxport);
    return(hal_res_OK);
}


extern hal_result_t hal_mux_enable(hal_mux_port_t muxport, hal_mux_sel_t muxsel)
{    
    static const hal_gpio_val_t s_values_sel0[hal_mux_sels_num] = {hal_gpio_valLOW,   hal_gpio_valHIGH,   hal_gpio_valLOW};
    static const hal_gpio_val_t s_values_sel1[hal_mux_sels_num] = {hal_gpio_valLOW,   hal_gpio_valLOW,    hal_gpio_valHIGH};
    
    if(hal_false == s_hal_device_mux_initted_is(muxport))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    
    hal_mux_internals_t* mint = &s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)];
    
    if(hal_mux_selNONE == muxsel)
    {
        return(hal_mux_disable(muxport));
    }
    
    hal_gpio_setval(mint->enable_port, mint->enable_pin, hal_gpio_valHIGH);
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    hal_gpio_setval(mint->sel0_port, mint->sel0_pin, s_values_sel0[(uint8_t)muxsel]);    
    hal_gpio_setval(mint->sel1_port, mint->sel1_pin, s_values_sel1[(uint8_t)muxsel]);   
    
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    
    hal_gpio_setval(mint->enable_port, mint->enable_pin, hal_gpio_valLOW);
    
    return(hal_res_OK);
}



extern hal_result_t hal_mux_disable(hal_mux_port_t muxport)
{
    if(hal_false == s_hal_device_mux_initted_is(muxport))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    
    hal_mux_internals_t* mint = &s_hal_device_mux_internals[HAL_device_mux_muxport2index(muxport)];

    hal_gpio_setval(mint->enable_port, mint->enable_pin, hal_gpio_valHIGH);
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    hal_gpio_setval(mint->sel0_port, mint->sel0_pin, hal_gpio_valHIGH);    
    hal_gpio_setval(mint->sel1_port, mint->sel1_pin, hal_gpio_valHIGH);   
    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_device_mux_hid_getsize(const hal_base_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_device_mux_hid_setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    memset(s_hal_device_mux_initted, hal_false, sizeof(s_hal_device_mux_initted));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_mux_supported_is(hal_mux_port_t muxport)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_mux__theconfig.supported_mask, HAL_device_mux_muxport2index(muxport)) );
}


static void s_hal_device_mux_initted_set(hal_mux_port_t muxport)
{
    s_hal_device_mux_initted[HAL_device_mux_muxport2index(muxport)] = hal_true;
}


static hal_boolval_t s_hal_device_mux_initted_is(hal_mux_port_t muxport)
{
    return(s_hal_device_mux_initted[HAL_device_mux_muxport2index(muxport)]);
}




#endif//HAL_USE_DEVICE_MUX

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



