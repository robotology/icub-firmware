
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
                    .minor = 2
                },  
                .builddate  = 
                {
                    .year  = 2022,
                    .month = 9,
                    .day   = 16,
                    .hour  = 9,
                    .min   = 15
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_ROMADDR,
                .size   = EENV_MEMMAP_ELOADER_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_RAMADDR,
                .size   = EENV_MEMMAP_ELOADER_RAMSIZE
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


eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_amc,
            .version    = 
            { 
                .major = 1, 
                .minor = 1
            },  
            .builddate  = 
            {
                .year  = 2022,
                .month = 4,
                .day   = 1,
                .hour  = 19,
                .min   = 19
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "amc"
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
