/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hal_core.c
	@brief      This file keeps implementation of the base hal module for stm32.
	@author     marco.accame@iit.it
    @date       02/28/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_CORE

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_base.h"
#include "hal_sys.h"
#include "hl_sys.h"
#include "stdio.h"
#include "hal_mpu.h"
#include "hal_flash.h"
#include "hal_heap.h"
#include "hal_brdcfg.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_core.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_core_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_core_status_t   status; 
    hal_core_cfg_t      config;
} hal_core_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hal_core_theinternals_t s_hal_core_theinternals =
{
    .status     = hal_core_status_zero,
    .config     = { 0 }
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_core_init(const hal_core_cfg_t *cfg) 
{
   
    if(NULL == cfg)
    {   
        hal_base_on_fatalerror(hal_fatalerror_missingconfiguration, "hal_core_init() needs cfg");
        return(hal_res_NOK_generic);
    }
    
    if(hal_core_status_zero != s_hal_core_theinternals.status)
    {
        hal_base_on_fatalerror(hal_fatalerror_generic, "hal_core_init() already called");
        return(hal_res_NOK_generic);    
    }
    
    
    // sets used config. it may be used to retrieve params for hal_cpu_init() and hal_sys_init()
    
    memcpy(&s_hal_core_theinternals.config, cfg, sizeof(hal_core_cfg_t));    
               
    // -- call initialisation of base    
    hal_base_init(&cfg->basecfg);
    
    // -- call initialisation of cpu
    hal_mpu_init(&cfg->mpucfg);
    
    // -- call initialisation of flash   
    hal_flash_init(&cfg->flashcfg);    
    
    // -- call initialisation of sys   
    hal_sys_init(&cfg->syscfg);
    
   
    // sets initialisation done
    s_hal_core_theinternals.status = hal_core_status_initialised;
    
    return(hal_res_OK);
}


extern hal_result_t hal_core_start(void)
{

    if(hal_core_status_initialised != s_hal_core_theinternals.status)
    {
        hal_base_on_fatalerror(hal_fatalerror_generic, "hal_core_init() not called yet or hal_core_start() already called");
        return(hal_res_NOK_generic);    
    }
    
            
    s_hal_core_theinternals.status = hal_core_status_started;
    
    hal_brdcfg__start();
    
    return(hal_res_OK);
}


extern hal_core_status_t hal_core_status_get(void)
{
    return(s_hal_core_theinternals.status); 
}


const hal_core_cfg_t* hal_core_cfg_get(void)
{
    return(&s_hal_core_theinternals.config);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



//
// extern hal_bool_t hal_core_hid_initted_is(void)
// {
//     return( (hal_core_status_initialised == s_hal_core_theinternals.status) ? hal_true : hal_false );    
// }




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



#endif//HAL_USE_CORE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



