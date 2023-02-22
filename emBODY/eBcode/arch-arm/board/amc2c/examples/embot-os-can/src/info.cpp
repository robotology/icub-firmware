
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "eEsharedServices.h" 

#if defined(OFFSET128K)

#define ROMADDR 0x08120000
#define ROMSIZE 0x20000

#define EENV_MODULEINFO_LOADER_AT ".ARM.__at_0x08120400"

#else

#define ROMADDR 0x08100000
#define ROMSIZE 0x20000

#define EENV_MODULEINFO_LOADER_AT ".ARM.__at_0x08100400"

#endif

#define RAMADDR 0x10000000
#define RAMSIZE 0x00048000

constexpr eEmoduleExtendedInfo_t s_cm4app_info_extended __attribute__((section(EENV_MODULEINFO_LOADER_AT))) =
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procOther01,
                .version    = 
                { 
                    .major = 100, 
                    .minor = 100
                },  
                .builddate  = 
                {
                    .year  = 2022,
                    .month = 12,
                    .day   = 5,
                    .hour  = 11,
                    .min   = 12
                }
            },
            .rom        = 
            {   
                .addr   = ROMADDR,
                .size   = ROMSIZE
            },
            .ram        = 
            {   
                .addr   = RAMADDR,
                .size   = RAMSIZE
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_none,
            .name           = "eOther01"
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

#include <cstring>

eEmoduleExtendedInfo_t ss {};
void info_use()
{
    static volatile uint8_t used {0};
    if(s_cm4app_info_extended.moduleinfo.info.entity.type == ee_entity_process)
    {
        used = 1;
    }        
    else
    {
        used = 2;
    }
    
    std::memmove(&ss, &s_cm4app_info_extended, sizeof(ss));
    
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



