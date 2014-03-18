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


/* @file       hal_device_led.c
	@brief      This file implements internal implementation of the hal led module.
	@author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       09/16/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_LED

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_brdcfg.h"
#include "hl_bits.h" 



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_led.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_led_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_led_id2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// emty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_gpio_cfg_t s_hal_led_gpio_config = 
{
    .dir    = hal_gpio_dirOUT,
    .speed  = hal_gpio_speed_low,
    .altcfg = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_led_supported_is(hal_led_t id);
static void s_hal_device_led_initted_set(hal_led_t id);
static hal_boolval_t s_hal_device_led_initted_is(hal_led_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hal_device_led_initted = 0;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_led_init(hal_led_t id, const hal_led_cfg_t *cfg)
{
    const hal_gpio_maP_t *gm = NULL;
    hal_result_t res = hal_res_NOK_generic;

    if(hal_false == s_hal_device_led_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
     
    gm = &hal_brdcfg_device_led__theconfig.gpiomap[HAL_device_led_id2index(id)].led;
    
    res = hal_gpio_init(gm->gpio, &s_hal_led_gpio_config);
    
    if(hal_res_OK != res)
    {
        return(res);
    }
    
    hal_gpio_setval(gm->gpio, hal_brdcfg_device_led__theconfig.value_off);
 
    s_hal_device_led_initted_set(id);
    return(hal_res_OK);
}


extern hal_result_t hal_led_on(hal_led_t id)
{
    const hal_gpio_maP_t *gm = NULL;
    
    if(hal_false == s_hal_device_led_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    gm = &hal_brdcfg_device_led__theconfig.gpiomap[HAL_device_led_id2index(id)].led;
    
    return(hal_gpio_setval(gm->gpio, hal_brdcfg_device_led__theconfig.value_on));
}



extern hal_result_t hal_led_off(hal_led_t id)
{
    const hal_gpio_maP_t *gm = NULL;
    
    if(hal_false == s_hal_device_led_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    gm = &hal_brdcfg_device_led__theconfig.gpiomap[HAL_device_led_id2index(id)].led;
    
    return(hal_gpio_setval(gm->gpio, hal_brdcfg_device_led__theconfig.value_off));
}



extern hal_result_t hal_led_toggle(hal_led_t id)
{
    const hal_gpio_maP_t *gm = NULL;
    hal_gpio_val_t val = hal_gpio_valNONE;
    
    if(hal_false == s_hal_device_led_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    gm = &hal_brdcfg_device_led__theconfig.gpiomap[HAL_device_led_id2index(id)].led;
    
    val = hal_gpio_getval(gm->gpio);
    
    if(hal_brdcfg_device_led__theconfig.value_off == val)
    {
        val = hal_brdcfg_device_led__theconfig.value_on;
    }
    else
    {
        val = hal_brdcfg_device_led__theconfig.value_off;
    }

    return(hal_gpio_setval(gm->gpio, val));
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

static hal_boolval_t s_hal_device_led_supported_is(hal_led_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(hal_brdcfg_device_led__theconfig.supported_mask, HAL_device_led_id2index(id)) );
}

static void s_hal_device_led_initted_set(hal_led_t id)
{
    hl_bits_hlfword_bitset(&s_hal_device_led_initted, HAL_device_led_id2index(id));
}

static hal_boolval_t s_hal_device_led_initted_is(hal_led_t id)
{
    return((hal_boolval_t)hl_bits_hlfword_bitcheck(s_hal_device_led_initted, HAL_device_led_id2index(id)));
}




#endif//HAL_USE_DEVICE_LED

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



