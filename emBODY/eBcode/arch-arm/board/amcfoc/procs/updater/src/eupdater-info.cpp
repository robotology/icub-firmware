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
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "module-info.h"


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


constexpr eEmoduleExtendedInfo_t eupdater_modinfo_extended  __attribute__((section(EENV_MODULEINFO_UPDATER_AT))) =
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procUpdater,
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
                .addr   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eupdater).address,
                .size   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eupdater).size
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
            .communication  = ee_commtype_eth,  // later on we may also add can1 and can2
            .name           = "amcfoc eUpdatr"
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
    return &eupdater_modinfo_extended;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



