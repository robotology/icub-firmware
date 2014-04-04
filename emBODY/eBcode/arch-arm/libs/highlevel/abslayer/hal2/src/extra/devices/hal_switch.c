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


/*  @file       hal_switch.c
	@brief      This file implements internals of a eth switch device.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_i2c.h" 
#include "hal_sys.h"
#include "hl_bits.h"
#include "hal_heap.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_switch.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_switch_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_switch_id2index(t)              ((uint8_t)((t)))

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
    hal_switch_cfg_t                config;
} hal_switch_internal_item_t;


typedef struct
{
    uint8_t                         inittedmask08;
    uint8_t                         startedmask08;
    hal_switch_internal_item_t*     items[hal_switches_number];   
} hal_switch_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_switch_supported_is(hal_switch_t id);
static void s_hal_switch_initted_set(hal_switch_t id);
static hal_boolval_t s_hal_switch_initted_is(hal_switch_t id);
static void s_hal_switch_started_set(hal_switch_t id);
static hal_boolval_t s_hal_switch_started_is(hal_switch_t id);


static hal_result_t s_hal_switch_lowlevel_init(const hal_switch_cfg_t *cfg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_switch_theinternals_t s_hal_switch_theinternals =
{
    .inittedmask08      = 0,
    .startedmask08      = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_switch_init(const hal_switch_cfg_t *cfg)
{
    const hal_switch_t id = hal_switch1;
    hal_result_t res = hal_res_NOK_generic;
    hal_switch_internal_item_t* intitem = s_hal_switch_theinternals.items[HAL_switch_id2index(id)];


    if(hal_true != s_hal_switch_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_switch_cfg_default;
    }    
    
    if(hal_true == s_hal_switch_initted_is(id))
    {
        return(hal_res_OK);
    }
        
    if(NULL == intitem)
    {
        intitem = s_hal_switch_theinternals.items[HAL_switch_id2index(id)] = hal_heap_new(sizeof(hal_switch_internal_item_t));
        // no initialisation is needed
    }
    memcpy(&intitem->config, cfg, sizeof(hal_switch_cfg_t));

    res = s_hal_switch_lowlevel_init(cfg);

    if(hal_res_OK == res)
    {
        s_hal_switch_initted_set(id);
    }

    return(res);
}


extern hal_result_t hal_switch_start(hal_ethtransceiver_phymode_t *usedmiiphymode)
{
    const hal_switch_t id = hal_switch1;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == s_hal_switch_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif    

    if(hal_true == s_hal_switch_started_is(id))
    {
        return(hal_res_OK);
    }    

    
    hal_switch__theboardconfig.driver[HAL_switch_id2index(id)].fun.start(id, usedmiiphymode);

    s_hal_switch_started_set(id);

    return(hal_res_OK);
 
}


extern hal_bool_t hal_switch_initted_is(void)
{
    const hal_switch_t id = hal_switch1;
    return(s_hal_switch_initted_is(id));
}


extern hal_bool_t hal_switch_started_is(void)
{
    const hal_switch_t id = hal_switch1;
    return(s_hal_switch_started_is(id));
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

static hal_bool_t s_hal_switch_supported_is(hal_switch_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_switch__theboardconfig.supportedmask, HAL_switch_id2index(id))); 
}

static hal_boolval_t s_hal_switch_initted_is(hal_switch_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_switch_theinternals.inittedmask08, HAL_switch_id2index(id)));
}

static void s_hal_switch_initted_set(hal_switch_t id)
{
    hl_bits_byte_bitset(&s_hal_switch_theinternals.inittedmask08, HAL_switch_id2index(id));
}


static hal_boolval_t s_hal_switch_started_is(hal_switch_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_switch_theinternals.startedmask08, HAL_switch_id2index(id)));
}


static void s_hal_switch_started_set(hal_switch_t id)
{
    hl_bits_byte_bitset(&s_hal_switch_theinternals.startedmask08, HAL_switch_id2index(id));
}


static hal_result_t s_hal_switch_lowlevel_init(const hal_switch_cfg_t *cfg)
{
    if((NULL != hal_switch__theboardconfig.driver[0].fun.init) && (NULL != hal_switch__theboardconfig.driver[0].fun.start))
    {
        return(hal_switch__theboardconfig.driver[0].fun.init(hal_switch1, hal_switch__theboardconfig.driver[0].cfg.initpar));
    }

    return(hal_res_NOK_generic);    
}


#endif//HAL_USE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
