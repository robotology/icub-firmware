/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEmemorymap.h"
#include "embot_hw_memory_mapping.h"


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
                    .major = 3, 
                    .minor = 5
                },  
                .builddate  = 
                {
                    .year  = 2025,
                    .month = 10,
                    .day   = 02,
                    .hour  = 11,
                    .min   = 11
                }
            },
            .rom        = 
            {   
                .addr   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eapplication00).address, 
                .size   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eapplication00).size
            },
            .ram        = 
            {   
                .addr   = 0,
                .size   = 0
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_eth,  
            .name           = "amcfoc eMaintn"
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



