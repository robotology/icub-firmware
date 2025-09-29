/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEmemorymap.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "module-info.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


constexpr eEmoduleExtendedInfo_t emaintainer_modinfo_extended  __attribute__((section(EENV_MODULEINFO_MAINTAINER_AT))) =
{
    .moduleinfo     =
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
                    .year  = 2025,
                    .month = 9,
                    .day   = 8,
                    .hour  = 12,
                    .min   = 10
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_EMAINTAINER_ROMADDR, //rom and ram are the same as the application
                .size   = EENV_MEMMAP_EMAINTAINER_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = EENV_MEMMAP_EMAINTAINER_RAMADDR,
                .size   = EENV_MEMMAP_EMAINTAINER_RAMSIZE
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_eth,  // later on we may also add can1 and can2
            .name           = "amc eMaintain"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 1, .minor = 1},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 1},
            .gtwprotversion  = { .major = 1, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};

const eEmoduleExtendedInfo_t * env::dualcore::module::info::get()
{
    return &emaintainer_modinfo_extended;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



