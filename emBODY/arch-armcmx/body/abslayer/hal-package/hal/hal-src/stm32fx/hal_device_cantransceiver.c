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

#include "utils/hal_utility_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_cantransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_cantransceiver_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_cantransceiver_canport2index(p)           ((uint8_t)((p)))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_device_cantransceiver_cfg_t hal_device_cantransceiver_cfg_default = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_cantransceiver_supported_is(hal_can_port_t port);
static void s_hal_device_cantransceiver_initted_set(hal_can_port_t port);
static hal_bool_t s_hal_device_cantransceiver_initted_is(hal_can_port_t port);


static hal_result_t s_hal_device_cantransceiver_lowlevel_init(hal_can_port_t port, const hal_device_cantransceiver_cfg_t *cfg);






// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_device_cantransceiver_cfg_t s_hal_device_cantransceiver_cfg          = {.dummy = 0};
static hal_bool_t s_hal_device_cantransceiver_initted[hal_can_ports_num]        =  {hal_false, hal_false};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_device_cantransceiver_init(hal_can_port_t port, const hal_device_cantransceiver_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;

    if(hal_true != s_hal_device_cantransceiver_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_true == s_hal_device_cantransceiver_initted_is(port))
    {
        return(hal_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hal_device_cantransceiver_cfg_default;
    }

    memcpy(&s_hal_device_cantransceiver_cfg, cfg, sizeof(hal_device_cantransceiver_cfg_t));

    res = s_hal_device_cantransceiver_lowlevel_init(port, cfg);

    s_hal_device_cantransceiver_initted_set(port);

    return(res);
}


extern hal_result_t hal_device_cantransceiver_enable(hal_can_port_t port)
{
    if(hal_false == s_hal_device_cantransceiver_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.enable(port);

    return(hal_res_OK); 
}

extern hal_result_t hal_device_cantransceiver_disable(hal_can_port_t port)
{
    if(hal_false == s_hal_device_cantransceiver_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.disable(port);

    return(hal_res_OK); 
}

extern hal_bool_t hal_device_cantransceiver_initted_is(hal_can_port_t port)
{
    return(s_hal_device_cantransceiver_initted_is(port));
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_device_cantransceiver_hid_getsize(const hal_cfg_t *cfg)
{
    return(0);
} 


extern hal_result_t hal_device_cantransceiver_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{

    // removed dependency from nzi ram
    memset(&s_hal_device_cantransceiver_cfg, 0, sizeof(s_hal_device_cantransceiver_cfg));
    memset(&s_hal_device_cantransceiver_initted, hal_false, sizeof(s_hal_device_cantransceiver_initted));

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_cantransceiver_supported_is(hal_can_port_t port)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_cantransceiver__theconfig.supported_mask, HAL_device_cantransceiver_canport2index(port)));
}


static hal_bool_t s_hal_device_cantransceiver_initted_is(hal_can_port_t port)
{
    return(s_hal_device_cantransceiver_initted[HAL_device_cantransceiver_canport2index(port)]);
}

static void s_hal_device_cantransceiver_initted_set(hal_can_port_t port)
{
    s_hal_device_cantransceiver_initted[HAL_device_cantransceiver_canport2index(port)] = hal_true;
}



static hal_result_t s_hal_device_cantransceiver_lowlevel_init(hal_can_port_t port, const hal_device_cantransceiver_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.init)   && 
       (NULL != hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.enable) &&
       (NULL != hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.disable) )
    {
        return(hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.init(port, hal_brdcfg_device_cantransceiver__theconfig.devcfg.chipif.initpar));
    }

    return(hal_res_NOK_generic);    
}


#endif//HAL_USE_DEVICE_CANTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
