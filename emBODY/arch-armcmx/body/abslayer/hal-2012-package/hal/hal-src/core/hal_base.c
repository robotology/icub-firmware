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

/* @file       hal_mpu_base.c
	@brief      This file keeps implementation of the base hal module for stm32.
	@author     marco.accame@iit.it
    @date       09/12/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_mpu_stm32xx_include.h"

#include "hal_brdcfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_base.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_base_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_BASE_VERIFY_STACK_HEAP_SIZES



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern hal_cfg_t hal_base_hid_params = 
{ 
    .extfn      = NULL              // at least one
};





// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum
{
    hal_base_status_zero            = 0,
    hal_base_status_initialised     = 1    
} hal_base_status_t;

typedef struct
{
    hal_base_status_t   status; 
} hal_base_internals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if defined(HAL_BASE_VERIFY_STACK_HEAP_SIZES)
    // removed by acemor on 09-mar-2011 to avoid problems of ... 
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Heap_Size (referred from hal_stm32.o).
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Stack_Size (referred from hal_stm32.o).
__asm int s_hal_getstacksize (void) {
        IMPORT  Stack_Size
        LDR     R0,=Stack_Size
        BX      LR
}


__asm int s_hal_getheapsize (void) {
        IMPORT  Heap_Size
        LDR     R0,=Heap_Size
        BX      LR
}
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static void (*s_hal_fn_osal_system_scheduling_suspend)(void) = NULL;
static void (*s_hal_fn_osal_system_scheduling_restart)(void) = NULL;
static void (*s_hal_fn_on_error)(hal_fatalerror_t, const char *) = NULL;


static hal_base_internals_t s_hal_base_internals =
{
    .status     = hal_base_status_zero
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern uint32_t hal_base_memory_getsize(const hal_cfg_t *cfg, uint32_t *size04aligned)
{
    uint32_t retval = 0;
 
    if(NULL == cfg)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingconfiguration, "hal_base_memory_getsize() needs a cfg");
        return(0);
    }
    
    
    // - base ---------------------------------------------------------------------------------------------------------
    
#ifdef  HAL_USE_BASE
    retval += hal_base_hid_getsize(cfg);
#endif//HAL_USE_BASE
    
    // - sys ----------------------------------------------------------------------------------------------------------
    
#ifdef  HAL_USE_SYS
    retval += hal_sys_hid_getsize(cfg);
#endif//HAL_USE_SYS     
    

    // - cpu ---------------------------------------------------------------------------------------------------------

#ifdef  HAL_USE_CPU
    retval += hal_cpu_hid_getsize(cfg);
#endif//HAL_USE_CPU 
    
    
    // - brdcfg -------------------------------------------------------------------------------------------------------
    
    retval += hal_brdcfg__getsize(cfg);
    
    if(NULL != size04aligned)
    {
        *size04aligned = retval;
    }
    return(retval);
}


extern hal_result_t hal_base_initialise(const hal_cfg_t *cfg, uint32_t *data04aligned) 
{
    if(NULL == cfg)
    {   
        hal_base_hid_on_fatalerror(hal_fatalerror_missingconfiguration, "hal_base_initialise() needs cfg");
        return(hal_res_NOK_generic);
    }
    
    if(hal_base_status_initialised == s_hal_base_internals.status)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_generic, "hal_base_initialise() already called");
        return(hal_res_NOK_generic);    
    }


    // override functions
    s_hal_fn_on_error                       = cfg->extfn.usr_on_fatal_error;
    s_hal_fn_osal_system_scheduling_suspend = cfg->extfn.osal_system_scheduling_suspend;
    s_hal_fn_osal_system_scheduling_restart = cfg->extfn.osal_system_scheduling_restart;



//     if(cfg->cpu_freq != SystemCoreClock)
//     {   
//         hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect frequency");
//         return(hal_res_NOK_generic);
//     }

#if defined(HAL_BASE_VERIFY_STACK_HEAP_SIZES)
    // removed by acemor on 09-mar-2011 to avoid problems of ... 
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Heap_Size (referred from hal_stm32.o).
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Stack_Size (referred from hal_stm32.o).
    if(cfg->stacksize != s_hal_getstacksize())
    {   
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect stack size");
        return(hal_res_NOK_generic);
    }

    if(cfg->heapsize != s_hal_getheapsize())
    {   
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect heap size");
        return(hal_res_NOK_generic);
    }
#endif


    // - base ---------------------------------------------------------------------------------------------------------
    
#ifdef  HAL_USE_BASE
    if(hal_res_OK != hal_base_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_base_hid_getsize(cfg)/4;
#endif//HAL_USE_BASE
    
    
    // - cpu ----------------------------------------------------------------------------------------------------------
    
#ifdef  HAL_USE_CPU
    if(hal_res_OK != hal_cpu_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_cpu_hid_getsize(cfg)/4;
#endif//HAL_USE_CPU    
    
    // - sys ----------------------------------------------------------------------------------------------------------
    
#ifdef HAL_USE_SYS
    if(hal_res_OK != hal_sys_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_sys_hid_getsize(cfg)/4;
#endif//HAL_USE_SYS  


    
    // - brdcfg -------------------------------------------------------------------------------------------------------    
    
    if(hal_res_OK != hal_brdcfg__setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_brdcfg__getsize(cfg)/4;
    
    
    
    // finally ... sets used config
    memcpy(&hal_base_hid_params, cfg, sizeof(hal_cfg_t));

    s_hal_base_internals.status = hal_base_status_initialised;
    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_base_hid_getsize(const hal_cfg_t *cfg)
{   
    // no memory needed
    return(0);
}

extern hal_result_t hal_base_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }
    return(hal_res_OK); 
}

extern hal_bool_t hal_base_hid_initted_is(void)
{
    return( (hal_base_status_initialised == s_hal_base_internals.status) ? hal_true : hal_false );    
}


extern void hal_base_hid_osal_scheduling_suspend(void)
{
	if(0 != (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
	{	// no need if it is called by an isr
	 	return;
	}

    if(NULL != s_hal_fn_osal_system_scheduling_suspend)
    {
		s_hal_fn_osal_system_scheduling_suspend();
    }
}


extern void hal_base_hid_osal_scheduling_restart(void)
{
	if(0 != (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
	{	// no need if it is called by an isr
	 	return;
	}

    if(NULL != s_hal_fn_osal_system_scheduling_restart)
    {
		s_hal_fn_osal_system_scheduling_restart();
    }
} 


extern void hal_base_hid_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
    if(NULL != s_hal_fn_on_error)
    {
        s_hal_fn_on_error(errorcode, errormsg);
    } 
    else
    {   
        for(;;)
        {
            errorcode = errorcode;
        }
    }
}



extern void hal_base_hid_ram_basic_init(void)
{
    // in here we set values of memory which is required in hal_base_memory_getsize() and in hal_base_initialise()
    // before any specific initialisation.

    // for nzi data
    memset(&hal_base_hid_params, 0, sizeof(hal_base_hid_params));

    s_hal_fn_on_error = NULL;
    s_hal_fn_osal_system_scheduling_suspend = NULL;
    s_hal_fn_osal_system_scheduling_restart = NULL;

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



#endif//HAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



