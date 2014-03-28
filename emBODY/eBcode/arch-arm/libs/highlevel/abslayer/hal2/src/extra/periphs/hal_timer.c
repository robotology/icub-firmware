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

/* @file       hal_timer.c
	@brief      This file implements internal implementation of the hal timer module.
	@author     marco.accame@iit.it
    @date       09/12/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_TIMER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdlib.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"
#include "hl_bits.h" 
#include "hal_heap.h"

#include "hl_timer.h"

#include "hal_middleware_interface.h" 


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_timer.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_timer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_timer_id2index(t)       ((uint8_t)((t)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// it must be defined in order to use hl_timer.
extern const hl_timer_mapping_t* hl_timer_map = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_timer_supported_is(hal_timer_t id);

static void s_hal_timer_prepare_hl_timer_map(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_timer_init(hal_timer_t id, const hal_timer_cfg_t *cfg, hal_reltime_t *error)
{
    if(hal_false == s_hal_timer_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        return(hal_res_NOK_nullpointer);
    }
       

    // init the hl_timer_map    
    s_hal_timer_prepare_hl_timer_map();
    
    // now we convert the cfg
    hl_timer_cfg_t hlcfg;
    hlcfg.countdown = cfg->countdown;
    hlcfg.mode      = (hal_timer_mode_oneshot == cfg->mode) ? (hl_timer_mode_oneshot) : (hl_timer_mode_periodic);
    hlcfg.priority  = (hl_irqpriority_t)cfg->priority;
    hlcfg.callback  = cfg->callback_on_exp;
    hlcfg.arg       = cfg->arg;
    hl_result_t r = hl_timer_init((hl_timer_t)id, &hlcfg, error);
    if(hl_res_OK != r)
    {
        return(hal_res_NOK_generic);
    }


    return((hal_result_t)r);
}


extern hal_result_t hal_timer_start(hal_timer_t id)
{
    return((hal_result_t)hl_timer_start((hl_timer_t)id));
}



extern hal_result_t hal_timer_stop(hal_timer_t id)
{
    return((hal_result_t)hl_timer_stop((hl_timer_t)id));
}


#if 1
    #warning --> hal_timer_countdown_set() not implemented
#else
extern hal_result_t hal_timer_countdown_set(hal_timer_t id, hal_reltime_t countdown, hal_reltime_t *error)
{                                                                           
    hal_timer_cfg_t *curcfg = NULL;
    hal_timer_cfg_t newcfg;
    uint8_t wasrunning = 0;

    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // if running stop.
    if(hal_timer_status_running == hal_timer_status_get(id))
    {
        wasrunning = 1;
    }

    // computes the values to be put in registers
    curcfg = &s_hal_timer_theinternals.items[HAL_timer_id2index(id)]->config;
    memcpy(&newcfg, curcfg, sizeof(hal_timer_cfg_t));
    newcfg.countdown = countdown;

    hal_timer_init(id, &newcfg, error);

    if(1 == wasrunning)
    {
        hal_timer_start(id);
    }

    return(hal_res_OK);
  
}
#endif  

#if 1
    #warning --> hal_timer_priority_set() not implemented
#else
extern hal_result_t hal_timer_priority_set(hal_timer_t id, hal_interrupt_priority_t prio)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    return(hal_res_NOK_unsupported);
}
#endif


#if 1
    #warning --> hal_timer_interrupt_enable() not implemented
#else
extern hal_result_t hal_timer_interrupt_enable(hal_timer_t id)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    return(hal_res_NOK_unsupported);
}
#endif


#if 1
    #warning --> hal_timer_interrupt_disable() not implemented
#else
extern hal_result_t hal_timer_interrupt_disable(hal_timer_t id)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    return(hal_res_NOK_unsupported);
}
#endif


extern hal_result_t hal_timer_remainingtime_get(hal_timer_t id, hal_reltime_t *remaining_time)
{
    return((hal_result_t)hl_timer_remainingtime_get((hl_timer_t)id, remaining_time));
}


#if 1
    #warning --> hal_timer_offset_write() not implemented
#else
extern hal_result_t hal_timer_offset_write(hal_timer_t id, hal_nanotime_t offset)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    // not supported
    return(hal_res_NOK_unsupported);
}
#endif

extern hal_timer_status_t hal_timer_status_get(hal_timer_t id)
{
    return((hal_timer_status_t)hl_timer_status_get((hl_timer_t)id));
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_timer_supported_is(hal_timer_t id)
{
    return((hal_boolval_t)hl_bits_hlfword_bitcheck(hal_brdcfg_timer__theconfig.supported_mask, HAL_timer_id2index(id)));
}

static void s_hal_timer_prepare_hl_timer_map(void)
{
    // we must initialise hl_timer_map w/ suited values. 
    // we have built hal_brdcfg_timer__theconfig to have the same layout, but we verify it anyway
    hl_VERIFYproposition(xxx, sizeof(hl_timer_mapping_t) == sizeof(hal_timer_hid_brdcfg_t));   
    hl_timer_map = (hl_timer_mapping_t*)&hal_brdcfg_timer__theconfig;  
}

#endif//HAL_USE_TIMER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



