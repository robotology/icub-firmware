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


/*  @file       hal_device_cantransceiver.c
	@brief      This file implements internals of a eth cantransceiver device.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_CANTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_sys.h"

#include "hal_brdcfg.h"

#include "hal_utility_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_cantransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_cantransceiver_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_cantransceiver_id2index(p)           ((uint8_t)((p)))

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
} hal_device_cantransceiver_internal_item_t;

typedef struct
{
    uint8_t                                         initted;
    hal_device_cantransceiver_internal_item_t*      items[hal_cantransceivers_number];   
} hal_device_cantransceiver_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_cantransceiver_supported_is(hal_cantransceiver_t id);
static void s_hal_device_cantransceiver_initted_set(hal_cantransceiver_t id);
static hal_bool_t s_hal_device_cantransceiver_initted_is(hal_cantransceiver_t id);


static hal_result_t s_hal_device_cantransceiver_lowlevel_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hal_device_cantransceiver_theinternals_t s_hal_device_cantransceiver_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_cantransceiver_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;

    if(hal_true != s_hal_device_cantransceiver_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_true == s_hal_device_cantransceiver_initted_is(id))
    {
        return(hal_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hal_cantransceiver_cfg_default;
    }


    res = s_hal_device_cantransceiver_lowlevel_init(id, cfg);

    s_hal_device_cantransceiver_initted_set(id);

    return(res);
}


extern hal_result_t hal_cantransceiver_enable(hal_cantransceiver_t id)
{
    if(hal_false == s_hal_device_cantransceiver_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.enable(id);

    return(hal_res_OK); 
}

extern hal_result_t hal_cantransceiver_disable(hal_cantransceiver_t id)
{
    if(hal_false == s_hal_device_cantransceiver_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.disable(id);

    return(hal_res_OK); 
}

extern hal_bool_t hal_cantransceiver_initted_is(hal_cantransceiver_t id)
{
    return(s_hal_device_cantransceiver_initted_is(id));
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_device_cantransceiver_hid_static_memory_init(void)
{
    // removed dependency from nzi ram
    memset(&s_hal_device_cantransceiver_theinternals, 0, sizeof(s_hal_device_cantransceiver_theinternals));

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_cantransceiver_supported_is(hal_cantransceiver_t id)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_cantransceiver__theconfig.supported_mask, HAL_device_cantransceiver_id2index(id)));
}


static hal_bool_t s_hal_device_cantransceiver_initted_is(hal_cantransceiver_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_device_cantransceiver_theinternals.initted, HAL_device_cantransceiver_id2index(id)));
}

static void s_hal_device_cantransceiver_initted_set(hal_cantransceiver_t id)
{
    hal_utility_bits_byte_bitset(&s_hal_device_cantransceiver_theinternals.initted, HAL_device_cantransceiver_id2index(id));
}



static hal_result_t s_hal_device_cantransceiver_lowlevel_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.init)   && 
       (NULL != hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.enable) &&
       (NULL != hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.disable) )
    {
        return(hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.init(id, hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.initpar));
    }

    return(hal_res_NOK_generic);    
}


#endif//HAL_USE_DEVICE_CANTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
