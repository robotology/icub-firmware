/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Accame
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

/* @file       hal_5v.c
	@brief      This file implements internal implementation of the 5v module
	@author     marco.accame@iit.it
    @date       05/25/2015
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"

#ifdef HAL_USE_5V

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_gpio.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_5v.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_5v_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_5v_cfg_t hal_5v_cfg_default =
{
    .requiredvoltage            = 12*1000,          // 12 volts
    .timeout                    = 200*1000          // 200 microsec
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_5v_cfg_t                config;
    uint8_t                     initted;
    uint8_t                     status;
} hal_5v_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_5v_supported_is(void);
static void s_hal_5v_initted_set(void);
static hal_boolval_t s_hal_5v_initted_is(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_5v_theinternals_t s_hal_5v_theinternals =
{
    .config             = {0},
    .initted            = 0,
    .status             = 0 // 0 is off
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_5v_init(const hal_5v_cfg_t *cfg)
{
    hal_5v_theinternals_t* intitem = &s_hal_5v_theinternals;

    if(hal_false == s_hal_5v_supported_is())
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL == cfg)
    {
        cfg  = &hal_5v_cfg_default;
    }

    if(hal_true == s_hal_5v_initted_is())
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_5v_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }    
    
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hal_5v_cfg_t));
    
    // ok, now i init
    hal_gpio_init(hal_5v__theboardconfig.gpiomap.gpio, &hal_5v__theboardconfig.gpiocfg); 
    
    
    s_hal_5v_initted_set();

    return(hal_res_OK);
}


extern hal_result_t hal_5v_on(void)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)   
    if(hal_false == s_hal_5v_initted_is())
    {
        return(hal_res_NOK_generic);
    }
#endif

    if(hal_gpio_valHIGH == hal_gpio_getval(hal_5v__theboardconfig.gpiomap.gpio))
    {
        return(hal_res_OK);
    }
 
    hal_gpio_setval(hal_5v__theboardconfig.gpiomap.gpio, hal_gpio_valHIGH);   

	return(hal_res_OK);
}


extern hal_result_t hal_5v_off(void) 
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)    
    if(hal_false == s_hal_5v_initted_is())
    {
        return(hal_res_NOK_generic);
    }
#endif


    if(hal_gpio_valLOW == hal_gpio_getval(hal_5v__theboardconfig.gpiomap.gpio))
    {
        return(hal_res_OK);
    }
 
    hal_gpio_setval(hal_5v__theboardconfig.gpiomap.gpio, hal_gpio_valLOW);

    return(hal_res_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----


// ---- isr of the module: end ------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_5v_supported_is(void)
{
    return((hal_boolval_t)hal_5v__theboardconfig.supported);
}

static void s_hal_5v_initted_set(void)
{
   s_hal_5v_theinternals.initted = 1;
}

static hal_boolval_t s_hal_5v_initted_is(void)
{
    return((hal_boolval_t)s_hal_5v_theinternals.initted);
}


#endif//HAL_USE_5V

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------










