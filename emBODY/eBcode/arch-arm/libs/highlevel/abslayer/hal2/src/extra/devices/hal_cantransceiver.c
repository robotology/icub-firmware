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


/*  @file       hal_cantransceiver.c
	@brief      This file implements internals of a eth cantransceiver device.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_CANTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_heap.h"
#include "hl_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_cantransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_cantransceiver_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_cantransceiver_id2index(p)           ((uint8_t)((p)))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_cantransceiver_cfg_t hal_cantransceiver_cfg_default = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_cantransceiver_cfg_t                        config;
} hal_cantransceiver_internal_item_t;

typedef struct
{
    uint32_t                                        inittedmask;
    hal_cantransceiver_internal_item_t*             items[hal_cantransceivers_number];   
} hal_cantransceiver_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_cantransceiver_supported_is(hal_cantransceiver_t id);
static void s_hal_cantransceiver_initted_set(hal_cantransceiver_t id);
static hal_bool_t s_hal_cantransceiver_initted_is(hal_cantransceiver_t id);


static hal_result_t s_hal_cantransceiver_lowlevel_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hal_cantransceiver_theinternals_t s_hal_cantransceiver_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_cantransceiver_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;
    hal_cantransceiver_internal_item_t *intitem = s_hal_cantransceiver_theinternals.items[HAL_cantransceiver_id2index(id)];

    if(hal_true != s_hal_cantransceiver_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_cantransceiver_cfg_default;
    }    

    if(hal_true == s_hal_cantransceiver_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_cantransceiver_cfg_t)))
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
        intitem = s_hal_cantransceiver_theinternals.items[HAL_cantransceiver_id2index(id)] = hal_heap_new(sizeof(hal_cantransceiver_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }    
    memcpy(&intitem->config, cfg, sizeof(hal_cantransceiver_cfg_t));
    
    
    res = s_hal_cantransceiver_lowlevel_init(id, cfg);
    
    if(hal_res_OK == res)
    {
        s_hal_cantransceiver_initted_set(id);
    }

    return(res);
}


extern hal_result_t hal_cantransceiver_enable(hal_cantransceiver_t id)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == s_hal_cantransceiver_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].fun.enable(id);

    return(hal_res_OK); 
}

extern hal_result_t hal_cantransceiver_disable(hal_cantransceiver_t id)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == s_hal_cantransceiver_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].fun.disable(id);

    return(hal_res_OK); 
}

extern hal_bool_t hal_cantransceiver_initted_is(hal_cantransceiver_t id)
{
    return(s_hal_cantransceiver_initted_is(id));
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

static hal_bool_t s_hal_cantransceiver_supported_is(hal_cantransceiver_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_cantransceiver__theboardconfig.supportedmask, HAL_cantransceiver_id2index(id)));
}


static hal_bool_t s_hal_cantransceiver_initted_is(hal_cantransceiver_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_cantransceiver_theinternals.inittedmask, HAL_cantransceiver_id2index(id)));
}

static void s_hal_cantransceiver_initted_set(hal_cantransceiver_t id)
{
    hl_bits_word_bitset(&s_hal_cantransceiver_theinternals.inittedmask, HAL_cantransceiver_id2index(id));
}



static hal_result_t s_hal_cantransceiver_lowlevel_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg)
{
    if((NULL != hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].fun.init)   && 
       (NULL != hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].fun.enable) &&
       (NULL != hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].fun.disable) )
    {
        return(hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].fun.init(id, hal_cantransceiver__theboardconfig.driver[HAL_cantransceiver_id2index(id)].cfg.initpar));
    }

    return(hal_res_NOK_generic);    
}


#endif//HAL_USE_CANTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
