
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_board_amc2c_info.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth.h"
#include "embot_app_board_amc2c_theCANagentCORE.h"

// --------------------------------------------------------------------------------------------------------------------
// - the configurable constants
// --------------------------------------------------------------------------------------------------------------------
    
namespace embot::app::board::amc2c::info { 
    
    constexpr embot::prot::can::applicationInfo applInfo 
    {   
        embot::prot::can::versionOfAPPLICATION {2, 0, 4},    
        embot::prot::can::versionOfCANPROTOCOL {2, 0}    
    };

    constexpr embot::app::eth::Date date
    {
        2023, embot::app::eth::Month::Oct, embot::app::eth::Day::sixteen, 15, 51
    };

    constexpr embot::hw::FLASHpartitionID codePartition 
    {
        embot::hw::FLASHpartitionID::eapplication01
    };

    constexpr embot::hw::CAN canBus 
    {
        embot::hw::CAN::two
    };
    
    constexpr uint8_t canaddress {3};
    
    // marco.accame: i use the macro INFO32 just because ... i want to init eEmoduleExtendedInfo_t::userdefined with the same string 
    // and i dont know how to fit it inside otherwise
    
                 // 0123456789abcde0123456789abcde
    #define INFO32 "hi, i am an amc2c on CAN2:3"

    constexpr const char *info32  
    { // 0123456789abcde0123456789abcde
      //"hi, i am an amc2c CAN2:3"
        INFO32
    };

} // embot::app::board::amc2c::info {


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

void force_placement_of_moduleinfo();

namespace embot::app::board::amc2c::info {
    
    embot::hw::FLASHpartitionID getCodePartition()
    {
        return codePartition;
    }  
    
    embot::app::application::CANagentCORE* getCANgentCORE()
    {
        static bool initted {false};
        if(!initted)
        {
            force_placement_of_moduleinfo();
            embot::app::board::amc2c::theCANagentCORE::getInstance().initialise({applInfo, canBus, canaddress, info32});
            initted = true;
        }
        return &embot::app::board::amc2c::theCANagentCORE::getInstance();
    }
    
} // namespace embot::app::board::amc2c::info {


// ------------------------------------------------------------------------------------------------------------------
// - for the partition table we need to place specific infor at a given location
// ------------------------------------------------------------------------------------------------------------------

#include "eEsharedServices.h" 


#define ROMADDR 0x08100000
#define ROMSIZE 0x20000

#define EENV_MODULEINFO_LOADER_AT ".ARM.__at_0x08100400"


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
                    .major = embot::app::board::amc2c::info::applInfo.version.major, 
                    .minor = embot::app::board::amc2c::info::applInfo.version.minor
                },  
                .builddate  = 
                {
                    .year  = embot::app::board::amc2c::info::date.year,
                    .month = embot::app::board::amc2c::info::date.month,
                    .day   = embot::app::board::amc2c::info::date.day,
                    .hour  = embot::app::board::amc2c::info::date.hour,
                    .min   = embot::app::board::amc2c::info::date.minute
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
            .communication  = ee_commtype_can2,
            .name           = "eOther01"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 0, .minor = 0},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 2, .minor = 0},
            .gtwprotversion  = { .major = 0, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {INFO32}
};


#include <cstring>

eEmoduleExtendedInfo_t ss {};
void force_placement_of_moduleinfo()
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



