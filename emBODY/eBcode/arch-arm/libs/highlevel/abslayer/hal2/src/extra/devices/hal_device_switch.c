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


/*  @file       hal_device_switch.c
	@brief      This file implements internals of a eth switch device.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
//#include "hal_stm32xx_include.h"  no need as it is a device
#include "hal_base_hid.h" 
#include "hal_i2c.h" 

#include "hal_sys.h"
#include "hal_periph_eth_hid.h"

#include "hal_periph_i2c_hid.h"

#include "hal_brdcfg.h"

#include "hl_bits.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_switch.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_switch_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_switch_id2index(t)              ((uint8_t)((t)))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_switch_cfg_t hal_switch_cfg_default = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_switch_cfg_t        config;
} hal_switch_internal_item_t;


typedef struct
{
    uint8_t                         initted;
    uint8_t                         started;
    hal_switch_internal_item_t*     items[hal_switches_number];   
} hal_switch_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_switch_supported_is(void);
static void s_hal_device_switch_initted_set(void);
static hal_boolval_t s_hal_device_switch_initted_is(void);
static void s_hal_device_switch_started_set(void);
static hal_boolval_t s_hal_device_switch_started_is(void);


static hal_result_t s_hal_device_switch_lowlevel_init(const hal_switch_cfg_t *cfg);
//static hal_result_t s_hal_device_switch_reg_config(void);
//static hal_result_t s_hal_device_switch_lowlevel_start(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_switch_theinternals_t s_hal_device_switch_theinternals =
{
    .initted            = 0,
    .started            = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_switch_init(const hal_switch_cfg_t *cfg)
{
    const hal_switch_t id = hal_switch1;
    hal_result_t res = hal_res_NOK_generic;
    hal_switch_internal_item_t* intitem = s_hal_device_switch_theinternals.items[HAL_device_switch_id2index(id)];


    if(hal_true != s_hal_device_switch_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == intitem)
    {
        intitem = s_hal_device_switch_theinternals.items[HAL_device_switch_id2index(id)] = hal_heap_new(sizeof(hal_switch_internal_item_t));
        // no initialisation is needed
    }


    if(hal_true == s_hal_device_switch_initted_is())
    {
        return(hal_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hal_switch_cfg_default;
    }

    memcpy(&intitem->config, cfg, sizeof(hal_switch_cfg_t));

    res = s_hal_device_switch_lowlevel_init(cfg);

    s_hal_device_switch_initted_set();

    return(res);
}


extern hal_result_t hal_switch_configure(hal_eth_phymode_t *usedmiiphymode)
{
    //hal_result_t res = hal_res_NOK_generic;


    if(hal_true != s_hal_device_switch_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_false == s_hal_device_switch_initted_is())
    {
        return(hal_res_NOK_generic);
    }
    

    if(hal_true == s_hal_device_switch_started_is())
    {
        return(hal_res_OK);
    }    

    
    hal_brdcfg_device_switch__theconfig.devcfg.chipif.config(usedmiiphymode);

    s_hal_device_switch_started_set();

    return(hal_res_OK);
 
}


extern hal_bool_t hal_switch_initted_is(void)
{
    return(s_hal_device_switch_initted_is());
}


extern hal_bool_t hal_switch_started_is(void)
{
    return(s_hal_device_switch_started_is());
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

static hal_bool_t s_hal_device_switch_supported_is(void)
{
    //const hal_switch_t id = hal_switch1;
    return(hal_brdcfg_device_switch__theconfig.supported); 
}

static hal_boolval_t s_hal_device_switch_initted_is(void)
{
    const hal_switch_t id = hal_switch1;
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_device_switch_theinternals.initted, HAL_device_switch_id2index(id)));
}

static void s_hal_device_switch_initted_set(void)
{
    const hal_switch_t id = hal_switch1;
    hl_bits_byte_bitset(&s_hal_device_switch_theinternals.initted, HAL_device_switch_id2index(id));
}


static hal_boolval_t s_hal_device_switch_started_is(void)
{
    const hal_switch_t id = hal_switch1;
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_device_switch_theinternals.started, HAL_device_switch_id2index(id)));
}


static void s_hal_device_switch_started_set(void)
{
    const hal_switch_t id = hal_switch1;
    hl_bits_byte_bitset(&s_hal_device_switch_theinternals.started, HAL_device_switch_id2index(id));
}



static hal_result_t s_hal_device_switch_lowlevel_init(const hal_switch_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_switch__theconfig.devcfg.chipif.init) && (NULL != hal_brdcfg_device_switch__theconfig.devcfg.chipif.config))
    {
        return(hal_brdcfg_device_switch__theconfig.devcfg.chipif.init(hal_brdcfg_device_switch__theconfig.devcfg.chipif.initpar));
    }

    return(hal_res_NOK_generic);    
}


#endif//HAL_USE_DEVICE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
