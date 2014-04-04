/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hal_termometer.c
	@brief      This file implements internals of the temperature sensing device
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_TERMOMETER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hal_heap.h"
#include "hl_bits.h"





 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_termometer.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_termometer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_termometer_id2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_termometer_cfg_t hal_termometer_cfg_default  = 
{ 
    .dummy = 0 
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_termometer_cfg_t                config;
} hal_termometer_internal_item_t;

typedef struct
{
    uint32_t                            inittedmask;
    hal_termometer_internal_item_t*     items[hal_termometers_number];
} hal_termometer_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_termometer_supported_is(hal_termometer_t id);
static void s_hal_termometer_initted_set(hal_termometer_t id);
static hal_boolval_t s_hal_termometer_initted_is(hal_termometer_t id);

static hal_result_t s_hal_termometer_hw_init(hal_termometer_t id, const hal_termometer_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_termometer_theinternals_t s_hal_termometer_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_termometer_init(hal_termometer_t id, const hal_termometer_cfg_t *cfg)
{
    hal_termometer_internal_item_t *intitem = s_hal_termometer_theinternals.items[HAL_termometer_id2index(id)];

    if(hal_false == s_hal_termometer_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
     
    if(NULL == cfg)
    {
        cfg  = &hal_termometer_cfg_default;
    }
        
    if(hal_true == s_hal_termometer_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_termometer_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }  

    if(NULL == intitem)
    {
        intitem = s_hal_termometer_theinternals.items[HAL_termometer_id2index(id)] = hal_heap_new(sizeof(hal_termometer_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }    
    memcpy(&intitem->config, cfg, sizeof(hal_termometer_cfg_t));    
    
 
    if(hal_res_OK != s_hal_termometer_hw_init(id, cfg))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_termometer_initted_set(id);

    return(hal_res_OK);
}

extern hal_result_t hal_termometer_read(hal_termometer_t id, hal_termometer_degrees_t* degrees)
{
    hal_result_t res = hal_res_NOK_generic; 
    int8_t data08 = 0;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)  
    if(hal_false == s_hal_termometer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)      
    if(NULL == degrees)
    {
        return(hal_res_NOK_generic);
    }
#endif    

    res = hal_termometer__theboardconfig.driver[HAL_termometer_id2index(id)].fun.read(id, &data08);
    *degrees = (int16_t) data08;  
    
    return(res);    
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


static hal_boolval_t s_hal_termometer_supported_is(hal_termometer_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_termometer__theboardconfig.supportedmask, HAL_termometer_id2index(id)) );
}

static void s_hal_termometer_initted_set(hal_termometer_t id)
{
    hl_bits_word_bitset(&s_hal_termometer_theinternals.inittedmask, HAL_termometer_id2index(id));
}

static hal_boolval_t s_hal_termometer_initted_is(hal_termometer_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_termometer_theinternals.inittedmask, HAL_termometer_id2index(id)));
}


static hal_result_t s_hal_termometer_hw_init(hal_termometer_t id, const hal_termometer_cfg_t *cfg)
{   
    if((NULL != hal_termometer__theboardconfig.driver[HAL_termometer_id2index(id)].fun.init) && (NULL != hal_termometer__theboardconfig.driver[HAL_termometer_id2index(id)].fun.read))
    {
        return(hal_termometer__theboardconfig.driver[HAL_termometer_id2index(id)].fun.init(id, hal_termometer__theboardconfig.driver[HAL_termometer_id2index(id)].cfg.initpar));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}


#endif//HAL_USE_TERMOMETER


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



