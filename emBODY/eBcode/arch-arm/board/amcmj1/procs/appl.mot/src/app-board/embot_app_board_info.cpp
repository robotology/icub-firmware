
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_board_info.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_msg.h"
#include "embot_app_icc.h"
#include "embot_app_eth.h"
#include "embot_app_board_theCANagentCORE.h"
#include "embot_hw_sys.h"
#include "embot_hw_memory_mapping.h"

// --------------------------------------------------------------------------------------------------------------------
// - the configurable constants
// --------------------------------------------------------------------------------------------------------------------
    
// we use macro USE_ICC_COMM + USE_ICC_CAN_COMM to config the both icc and can

#warning dobbimao usare embot::app::boards::Board::amcmj1_mot, opuure basta embot::app::boards::Board::amcmj1 ???
// 
namespace embot::app::board::info { 
    
    constexpr embot::app::msg::BUS bus {embot::app::msg::BUS::can1};
    constexpr uint8_t address {1}; 

#if defined(USE_ICC_COMM)
    static constexpr embot::app::icc::Signature signature __attribute__((section(EENV_SIGNATURE_APPL_MOT_AT))) =
    {
        embot::app::boards::Board::amcmj1_mot,
        {embot::app::msg::BUS::icc1, address},
        {1, 0, 0, 0},   // application version
        {2, 0},         // protocol version
        {2025, embot::app::eth::Month::Oct, embot::app::eth::Day::sixteen, 10, 47}
    };
    
    constexpr embot::app::msg::Location icclocation {signature.location};
    
#if defined(USE_ICC_CAN_COMM)
    constexpr embot::app::msg::Location canlocation {bus, address};
#else
    constexpr embot::app::msg::Location canlocation {embot::app::msg::BUS::none, 0};
#endif
    
#else
    static constexpr embot::app::icc::Signature signature __attribute__((section(EENV_SIGNATURE_APPL_MOT_AT))) =
    {
        embot::app::boards::Board::amcmj1_mot,
        {embot::app::msg::BUS::can1, address},
        {1, 0, 1, 0},  // application version
        {2, 0},         // protocol version
        {2024, embot::app::eth::Month::Dec, embot::app::eth::Day::twelve, 10, 33}
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
    #define INFO32 "amcmj1 @ ICC1:1/CAN1:1"
#else
                 // 0123456789abcde0123456789abcde
    #define INFO32 "amcmj1 @ CAN1:1-2"
#endif    

    constexpr const char *info32  
    { // 0123456789abcde0123456789abcde
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
                        uid = embot::hw::sys::uniqueid();
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
    
} // embot::app::board::info {


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

void force_placement_of_flashmappedinfo();

namespace embot::app::board::info {
    
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
            
            embot::app::board::theCANagentCORE::getInstance().initialise({theboard, applInfo, location, info32});
            initted = true;
        }
        return &embot::app::board::theCANagentCORE::getInstance();
    }
    
} // namespace embot::app::board::info {


// ------------------------------------------------------------------------------------------------------------------
// - for the partition table we need to place specific infor at a given location
// ------------------------------------------------------------------------------------------------------------------

#include "eEsharedServices.h" 

//#include "eEmemorymap.h"
#include "embot_hw_memory_mapping.h"


constexpr eEmoduleExtendedInfo_t s_cm7app_info_extended  __attribute__((section(EENV_MODULEINFO_APPL_MOT_AT))) =
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
                    .major = embot::app::board::info::signature.application.major, 
                    .minor = embot::app::board::info::signature.application.minor
                },  
                .builddate  = 
                {
                    .year  = embot::app::board::info::signature.date.year,
                    .month = embot::app::board::info::signature.date.month,
                    .day   = embot::app::board::info::signature.date.day,
                    .hour  = embot::app::board::info::signature.date.hour,
                    .min   = embot::app::board::info::signature.date.minute
                }
            },
            .rom        = 
            {   
                .addr   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eapplication01).address,
                .size   = embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eapplication01).size
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
        
    if(s_cm7app_info_extended.moduleinfo.info.entity.type == ee_entity_process)
    {
        used = 1;
    }        
    else
    {
        used = 2;
    }
    
    std::memmove(&ss, &s_cm7app_info_extended, sizeof(ss)); 
    std::memmove(&sig, &embot::app::board::info::signature, sizeof(sig));     
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



