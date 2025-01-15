
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


#include "embot_app_msg.h"
#include "embot_app_icc.h"
#include "embot_app_eth.h"
#include "embot_app_board_amc2c_theCANagentCORE.h"
#include "embot_hw_sys.h"

// --------------------------------------------------------------------------------------------------------------------
// - the configurable constants
// --------------------------------------------------------------------------------------------------------------------
    
// for now we keep the dual situation only ICC or only CAN, but maybe later on we can have both
// we used macro USE_ICC_COMM to config the former or the latter
namespace embot::app::board::amc2c::info { 
    
    constexpr uint8_t address {3}; 
    
#if defined(USE_ICC_COMM)
    static constexpr embot::app::icc::Signature signature __attribute__((section(".ARM.__at_0x08100800"))) =
    {
        embot::app::boards::Board::amc2c,
        {embot::app::msg::BUS::icc1, address},
        {3, 1, 0, 0},   // application version
        {2, 0},         // protocol version
        {2024, embot::app::eth::Month::Oct, embot::app::eth::Day::seven, 16, 38}
    };
    
    constexpr embot::app::msg::Location icclocation {signature.location};
    
#if defined(USE_ICC_CAN_COMM)
    constexpr embot::app::msg::Location canlocation {embot::app::msg::BUS::can2, address};
#else
    constexpr embot::app::msg::Location canlocation {embot::app::msg::BUS::none, 0};
#endif
    
#else
    static constexpr embot::app::icc::Signature signature __attribute__((section(".ARM.__at_0x08100800"))) =
    {
        embot::app::boards::Board::amcbldc,
        {embot::app::msg::BUS::can2, address},
        {2, 1, 0, 0},  // application version
        {2, 0},         // protocol version
        {2024, embot::app::eth::Month::Oct, embot::app::eth::Day::seven, 16, 38}
    };
    
    constexpr embot::app::msg::Location canlocation {signature.location};
    constexpr embot::app::msg::Location icclocation {embot::app::msg::BUS::none, 0};
#endif   
        
    constexpr embot::hw::FLASHpartitionID codePartition 
    {
        embot::hw::FLASHpartitionID::eapplication01
    };
      
    
    // marco.accame: i use the macro INFO32 just because ... i want to init eEmoduleExtendedInfo_t::userdefined with the same string 
    // and i dont know how to fit it inside otherwise
 
#if defined(USE_ICC_COMM)     
                 // 0123456789abcde0123456789abcde
    #define INFO32 "i am an amc2c @ ICC1:3/CAN2:3"
#else
                 // 0123456789abcde0123456789abcde
    #define INFO32 "i am an amc2c @ CAN2:3"
#endif    

    constexpr const char *info32  
    { // 0123456789abcde0123456789abcde
      //"hi, i am an amc2c CAN2:3"
        INFO32
    };
    
    
    embot::app::msg::Location getCANlocation()
    {
        return canlocation;        
    }
    
    embot::app::msg::Location getICClocation()
    {
        return icclocation;        
    }
    
    
    bool OnROPrx(const embot::app::eth::icc::ItemROP &rxrop, embot::app::eth::icc::ItemROP &reply)
    {
        bool r {false};
        // it is mandatory that the memory of the varibles stays alive after defOnROPrx() returns so in here we mark it as static 
        // to move it away from being a temporary variable allocated on the stack
        // a proper implementation however requires to have these variables somehow global.
        static embot::core::Time timeoflife {0};
        static uint64_t uid {0};
        static uint32_t dummyUINT32 {0};
        static embot::app::icc::Signature sign {signature};

        // we fill the default w/ nothing to transmit back 
        reply.fill(embot::app::eth::icc::ItemROP::CMD::none, rxrop.var);
        
        switch(rxrop.cmd)
        {               
            case embot::app::eth::icc::ItemROP::CMD::ping:
            {
                reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, rxrop.var);
            } break;
            
            case embot::app::eth::icc::ItemROP::CMD::ask:
            {
                switch(rxrop.var.descriptor.id)
                {
                    case embot::app::eth::icc::ItemROP::IDtimeoflife:
                    {
                        timeoflife = embot::core::now();
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDtimeoflife, 8}, &timeoflife});
                    } break;
                    
                    case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                    {
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDdummyUINT32, 4}, &dummyUINT32});                            
                    } break;

                    case embot::app::eth::icc::ItemROP::IDunique64:
                    {
                        uid = 0; // embot::hw::sys::uniqueid();
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDunique64, 8}, &uid});                            
                    } break;                    
                    // ... other managed cases

                    case embot::app::eth::icc::ItemROP::IDsignature:
                    {
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDsignature, 16}, &sign});                            
                    } break;                       
                    
                    
                    default: 
                    {   // if the ID is not managed we do nothing but we send a nak back   
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, rxrop.var);
                    } break;                     
                }                    
            } break;
        
            case embot::app::eth::icc::ItemROP::CMD::set:
            {
                switch(rxrop.var.descriptor.id)
                {                        
                    case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                    {
                        dummyUINT32 = *reinterpret_cast<uint32_t*>(rxrop.var.memory);
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, {{embot::app::eth::icc::ItemROP::IDdummyUINT32, 4}, &dummyUINT32});                                                      
                    } break;
                    
                    // ... other managed cases
                    
                    default: 
                    {   // if the ID is not managed we do nothing but we send a nak back   
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, rxrop.var);
                    } break;                          
                }
                
            } break;                

            case embot::app::eth::icc::ItemROP::CMD::sig:
            {
                switch(rxrop.var.descriptor.id)
                {                        
                    case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                    {
                        dummyUINT32 = *reinterpret_cast<uint32_t*>(rxrop.var.memory);
                        // i dont send any ack back                                                      
                    } break;
                    
                    // ... other managed cases
                    
                    default: 
                    {   // if the ID is not managed we do nothing and we send nothing back   
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::none, rxrop.var);
                    } break;                        
                }
                
            } break;  
            
            default: { } break; // we dont manage any other received commands
            
        }

        r = (embot::app::eth::icc::ItemROP::CMD::none != rxrop.cmd);
        
        return r; 
    }    
    
} // embot::app::board::amc2c::info {


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

void force_placement_of_flashmappedinfo();

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
            force_placement_of_flashmappedinfo();
            // maybe we add in here also type of board and location
            
            static constexpr embot::prot::can::Board theboard {static_cast<embot::prot::can::Board>(signature.board)};

            static constexpr embot::prot::can::applicationInfo applInfo 
            {             
                embot::prot::can::versionOfAPPLICATION {signature.application.major, signature.application.minor, signature.application.build},    
                embot::prot::can::versionOfCANPROTOCOL {signature.protocol.major, signature.protocol.minor}        
            }; 
            
            static constexpr embot::app::msg::Location location {canlocation};
            
            embot::app::board::amc2c::theCANagentCORE::getInstance().initialise({theboard, applInfo, location, info32});
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
                    .major = embot::app::board::amc2c::info::signature.application.major, 
                    .minor = embot::app::board::amc2c::info::signature.application.minor
                },  
                .builddate  = 
                {
                    .year  = embot::app::board::amc2c::info::signature.date.year,
                    .month = embot::app::board::amc2c::info::signature.date.month,
                    .day   = embot::app::board::amc2c::info::signature.date.day,
                    .hour  = embot::app::board::amc2c::info::signature.date.hour,
                    .min   = embot::app::board::amc2c::info::signature.date.minute
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
embot::app::icc::Signature sig {};    
void force_placement_of_flashmappedinfo()
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
    std::memmove(&sig, &embot::app::board::amc2c::info::signature, sizeof(sig));     
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



