
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// api 
#include "board-info.h"
#include "module-info.h"

// dependencies
#include "eEmemorymap.h"
#include "embot_hw_memory_mapping.h"
#include "EoBoards.h"

#if defined(dontuseMEMmapping)
constexpr eEmoduleExtendedInfo_t s_loader_info_extended =
#else
constexpr eEmoduleExtendedInfo_t s_loader_info_extended __attribute__((section(EENV_MODULEINFO_LOADER_AT))) =
#endif
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procLoader,
                .version    = 
                { 
                    .major = 3, 
                    .minor = 3
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
                .addr   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eloader).address,
                .size   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eloader).size
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
            .communication  = ee_commtype_none,
            .name           = "eLoader"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 0, .minor = 0},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 0},
            .gtwprotversion  = { .major = 0, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};


static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_amcmj1,
            .version    = 
            { 
                .major = 1, 
                .minor = 1
            },  
            .builddate  = 
            {
                .year  = 2025,
                .month = 10,
                .day   = 14,
                .hour  = 13,
                .min   = 17
            }
        },
        .rom        = 
        {   
            .addr   = 0, 
            .size   = 0  
        },
        .ram        = 
        {   
            .addr   = 0,
            .size   = 0
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE, // who cares to know how mauch eeprom we have in here?
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1,
        .name           = "amcmj1"
    },
    .uniqueid       = 0,
    .extra          = {0}
};




const eEmoduleExtendedInfo_t * env::dualcore::module::info::get()
{
    return &s_loader_info_extended;
}

void env::dualcore::board::info::set(uint64_t uniqueid)
{
    s_loader_boardinfo.uniqueid = uniqueid;
}

const eEboardInfo_t * env::dualcore::board::info::get()
{
    return &s_loader_boardinfo;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
