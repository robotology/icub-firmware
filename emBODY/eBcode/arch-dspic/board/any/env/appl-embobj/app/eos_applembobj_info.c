
/* @file       eos_applembobj_info.c
	@brief      This file keeps the module info of the updatre
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "eEcommon.h"
#include "eEmemorymap.h"

#include "hal.h"

#include "EOStheSystem.h"

#include "eos_applembobj_spec.h"


extern const hal_cfg_t     s_hal_cfg;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eos_applembobj_info.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

eEmoduleInfo_t eos_applembobj_info_modinfo = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2012,
                .month = 1,
                .day   = 11,
                .hour  = 18,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_ROM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .ram        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_RAM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_RAM_SIZE
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_can1,  // later on we may also add can1 and can2
        .name           = "EOSeAppl"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 0},
        .can1protversion = { .major = 1, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}
};



#warning --> it seems that const generates problems
eOssystem_cfg_t eos_applembobj_info_syscfg =
{
    .hal_fns                                =
    {
        .hal_base_init                      = eos_applembobj_spec_hal_init,
        .hal_sys_systeminit                 = (eOvoid_fp_void_t)hal_sys_systeminit,
        .hal_sys_systick_sethandler         = (eOvoid_fp_voidfpvoiduint32uint8_t)hal_sys_systick_sethandler,
        .hal_sys_atomic_bitwiseAND          = hal_sys_atomic_bitwiseAND,
        .hal_sys_atomic_bitwiseOR           = hal_sys_atomic_bitwiseOR,
        .hal_sys_criticalsection_take       = (eOres_fp_voidp_uint32_t)hal_sys_criticalsection_take,
        .hal_sys_criticalsection_release    = (eOres_fp_voidp_t)hal_sys_criticalsection_release,
        .hal_sys_irq_disable                = hal_sys_irq_disable,
        .hal_sys_irq_enable                 = hal_sys_irq_enable    
    }, 
    .fsal_fns                               =
    {
        .fsal_init                          = NULL
    }, 
    .userdef                                =
    {
        .systickperiod                      = 500,
        .on_systick                         = eos_applembobj_spec_sys_on_tick
    }
};

eOerrman_cfg_t eos_applembobj_info_errman_cfg =
{
    .extfn              =
    {
        .usr_on_error   = eos_applembobj_spec_errman_on_error
    }
};

eOstimerman_cfg_t eos_applembobj_info_thetimerman_cfg = 
{
    .timernum = 3
};

eOscallbackman_cfg_t eos_applembobj_info_thecallbackman_cfg = 
{
    .queuesize = 2
};

eOsfoop_cfg_t eos_applembobj_info_thefoopcfg = 
{ 
    .messagefifosize                    = 0,
    .callbackfifosize                   = 2, // overhidden by cbkman cfg  
    .usrfn                              =
    {
        .on_startup                     = NULL,
        .on_event                       = eos_applembobj_spec_foop_on_event,
        .on_message                     = NULL
    } 
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



