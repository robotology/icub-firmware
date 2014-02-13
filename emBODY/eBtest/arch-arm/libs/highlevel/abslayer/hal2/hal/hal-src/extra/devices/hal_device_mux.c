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
	@brief      This file implements internal implementation of the hal id module.
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
#include "hal_heap.h" 


 
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

#define HAL_device_mux_id2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_mux_cfg_t hal_mux_cfg_default = { .dummy = 0 };

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_mux_cfg_t           config;
    // altre cose tipo gpio etc etc ...
    hal_gpio_t              enable;
    hal_gpio_t              sel0;
    hal_gpio_t              sel1;     
} hal_mux_internal_item_t;


typedef struct
{
    uint8_t                     initted;
    hal_mux_internal_item_t*    items[hal_muxes_number];   
} hal_mux_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_mux_supported_is(hal_mux_t id);
static void s_hal_mux_initted_set(hal_mux_t id);
static hal_boolval_t s_hal_mux_initted_is(hal_mux_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_gpio_cfg_t s_hal_mux_gpio_config = 
{
    .dir    = hal_gpio_dirOUT,
    .speed  = hal_gpio_speed_max,
    .altcfg = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_mux_theinternals_t s_hal_mux_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_mux_init(hal_mux_t id, const hal_mux_cfg_t *cfg)
{
    hal_mux_internal_item_t* intitem = s_hal_mux_theinternals.items[HAL_device_mux_id2index(id)];

    if(hal_false == s_hal_mux_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(hal_true == s_hal_mux_initted_is(id))
    {
        return(hal_res_OK);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_mux_cfg_default;
    }
     
        
    // configure the required mux port and put it in selNONE.
    
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_mux_theinternals.items[HAL_device_mux_id2index(id)] = hal_heap_new(sizeof(hal_mux_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }       
    
    memcpy(&intitem->config, cfg, sizeof(hal_mux_cfg_t));   
    
    memcpy(&intitem->enable, &hal_brdcfg_device_mux__theconfig.gpiomap[HAL_device_mux_id2index(id)].gpio_enable.gpio, sizeof(hal_gpio_t));
    memcpy(&intitem->sel0,   &hal_brdcfg_device_mux__theconfig.gpiomap[HAL_device_mux_id2index(id)].gpio_sel0.gpio, sizeof(hal_gpio_t));
    memcpy(&intitem->sel1,   &hal_brdcfg_device_mux__theconfig.gpiomap[HAL_device_mux_id2index(id)].gpio_sel1.gpio, sizeof(hal_gpio_t));

    
    
    hal_gpio_init(intitem->enable, &s_hal_mux_gpio_config);
    hal_gpio_setval(intitem->enable, hal_gpio_valHIGH);
    
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    
    hal_gpio_init(intitem->sel0, &s_hal_mux_gpio_config);
    hal_gpio_setval(intitem->sel0, hal_gpio_valHIGH);
    
    hal_gpio_init(intitem->sel1, &s_hal_mux_gpio_config);
    hal_gpio_setval(intitem->sel1, hal_gpio_valHIGH);   
    
        

    s_hal_mux_initted_set(id);
    return(hal_res_OK);
}


extern hal_result_t hal_mux_enable(hal_mux_t id, hal_mux_sel_t muxsel)
{    
    hal_mux_internal_item_t* intitem = s_hal_mux_theinternals.items[HAL_device_mux_id2index(id)];
    
    static const hal_gpio_val_t s_values_sel0[hal_mux_sels_number] = {hal_gpio_valLOW,   hal_gpio_valHIGH,   hal_gpio_valLOW};
    static const hal_gpio_val_t s_values_sel1[hal_mux_sels_number] = {hal_gpio_valLOW,   hal_gpio_valLOW,    hal_gpio_valHIGH};
    
    if(hal_false == s_hal_mux_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    
    
    if(hal_mux_selNONE == muxsel)
    {
        return(hal_mux_disable(id));
    }
    
    hal_gpio_setval(intitem->enable, hal_gpio_valHIGH);
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    hal_gpio_setval(intitem->sel0, s_values_sel0[(uint8_t)muxsel]);    
    hal_gpio_setval(intitem->sel1, s_values_sel1[(uint8_t)muxsel]);   
    
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    
    hal_gpio_setval(intitem->enable, hal_gpio_valLOW);
    
    return(hal_res_OK);
}



extern hal_result_t hal_mux_disable(hal_mux_t id)
{
    hal_mux_internal_item_t* intitem = s_hal_mux_theinternals.items[HAL_device_mux_id2index(id)];
    
    if(hal_false == s_hal_mux_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    

    hal_gpio_setval(intitem->enable, hal_gpio_valHIGH);
    hal_sys_delay(1);   // we use 1 microsec, but it is actually 50 ns
    hal_gpio_setval(intitem->sel0, hal_gpio_valHIGH);    
    hal_gpio_setval(intitem->sel1, hal_gpio_valHIGH);   
    
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
    memset(&s_hal_mux_theinternals, 0, sizeof(s_hal_mux_theinternals));
    
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_mux_supported_is(hal_mux_t id)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_mux__theconfig.supported_mask, HAL_device_mux_id2index(id)));
}


static void s_hal_mux_initted_set(hal_mux_t id)
{
    hal_utility_bits_byte_bitset(&s_hal_mux_theinternals.initted, HAL_device_mux_id2index(id));
}


static hal_boolval_t s_hal_mux_initted_is(hal_mux_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_mux_theinternals.initted, HAL_device_mux_id2index(id)));
}




#endif//HAL_USE_DEVICE_MUX

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



