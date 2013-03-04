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

/* @file       hal_device_termometer.c
	@brief      This file implements internals of the temperature sensing device
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_TERMOMETER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"


#include "hal_trace.h"
#include "stdio.h"

#include "hal_utility_bits.h"





 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_termometer.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_termometer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_termometer_port2index(t)              ((uint8_t)((t)))



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
    hal_termometer_cfg_t       cfg;
    uint32_t                    initialvalue;
} hal_device_termometer_internals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_termometer_supported_is(hal_termometer_port_t port);
static void s_hal_device_termometer_initted_set(hal_termometer_port_t port);
static hal_boolval_t s_hal_device_termometer_initted_is(hal_termometer_port_t port);

static hal_result_t s_hal_device_termometer_hw_init(hal_termometer_port_t port, const hal_termometer_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_hal_device_termometer_initted = 0;

static hal_device_termometer_internals_t* s_hal_device_termometer_internals[hal_termometer_ports_number] = { NULL };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_termometer_init(hal_termometer_port_t port, const hal_termometer_cfg_t *cfg)
{
//     hal_result_t res = hal_res_NOK_generic; // dont remove ...
//     hal_device_termometer_internals_t *info = NULL;

    if(hal_false == s_hal_device_termometer_supported_is(port))
    {
        return(hal_res_NOK_generic);
    }
     
    if(NULL == cfg)
    {
        cfg  = &hal_termometer_cfg_default;
    }
    
    if(hal_true == s_hal_device_termometer_initted_is(port))
    {
        return(hal_res_OK);
    } 
 
    if(hal_res_OK != s_hal_device_termometer_hw_init(port, cfg))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_device_termometer_initted_set(port);

    return(hal_res_OK);
}

extern hal_result_t hal_termometer_read(hal_termometer_port_t port, hal_termometer_degrees_t* degrees)
{
    hal_result_t res = hal_res_NOK_generic; 
//    hal_device_termometer_internals_t *info = NULL;
    int8_t data08 = 0;

    if(NULL == degrees)
    {
        return(hal_res_NOK_generic);
    }
    
    if(hal_false == s_hal_device_termometer_initted_is(port))
    {
        *degrees = -128;
        return(hal_res_NOK_generic);
    }


    res = hal_brdcfg_device_termometer__theconfig.devcfg[HAL_device_termometer_port2index(port)].chipif.read(&data08);
    *degrees = (int16_t) data08;  
    
    return(res);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_device_termometer_hid_static_memory_init(void)
{
    memset(s_hal_device_termometer_internals, 0, sizeof(s_hal_device_termometer_internals));
    s_hal_device_termometer_initted = 0;
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static hal_boolval_t s_hal_device_termometer_supported_is(hal_termometer_port_t port)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_termometer__theconfig.supported_mask, HAL_device_termometer_port2index(port)) );
}

static void s_hal_device_termometer_initted_set(hal_termometer_port_t port)
{
    hal_utility_bits_byte_bitset(&s_hal_device_termometer_initted, HAL_device_termometer_port2index(port));
    //s_hal_device_termometer_initted[HAL_device_termometer_port2index(port)] = hal_true;
}

static hal_boolval_t s_hal_device_termometer_initted_is(hal_termometer_port_t port)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_device_termometer_initted, HAL_device_termometer_port2index(port)));
    //return(s_hal_device_termometer_initted[HAL_device_termometer_port2index(port)]);
}


static hal_result_t s_hal_device_termometer_hw_init(hal_termometer_port_t port, const hal_termometer_cfg_t *cfg)
{   
    if((NULL != hal_brdcfg_device_termometer__theconfig.devcfg[HAL_device_termometer_port2index(port)].chipif.init) && (NULL != hal_brdcfg_device_termometer__theconfig.devcfg[HAL_device_termometer_port2index(port)].chipif.read))
    {
        return(hal_brdcfg_device_termometer__theconfig.devcfg[HAL_device_termometer_port2index(port)].chipif.init(hal_brdcfg_device_termometer__theconfig.devcfg[HAL_device_termometer_port2index(port)].chipif.initpar));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}


#endif//HAL_USE_DEVICE_TERMOMETER


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



