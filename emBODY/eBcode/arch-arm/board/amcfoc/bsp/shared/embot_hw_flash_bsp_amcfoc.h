
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_FLASH_BSP_AMCFOC_H_
#define __EMBOT_HW_FLASH_BSP_AMCFOC_H_


#include "embot_hw_flash_bsp.h"

namespace embot::hw::flash::bsp::amcfoc {
    
    // the flash dimensions is independent on the bootstrap mode
    
    constexpr uint8_t numbanks {2};
    constexpr uint32_t banksize {1024*1024};
    constexpr uint32_t pagesize {128*1024};
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr BankDescriptor bank02 { Bank::ID::two, 0x08100000, banksize, pagesize };
    
    // but not the mapping of the processes
    
#if defined(STM32HAL_dualcore_BOOT_cm7master) 
    
    constexpr BankDescriptor masterbank { bank01 };
    constexpr BankDescriptor slavebank  { bank02 };
    
#elif defined(STM32HAL_dualcore_BOOT_cm4master)  

    constexpr BankDescriptor masterbank { bank02 };
    constexpr BankDescriptor slavebank  { bank01 };
    
#endif    
    
    // on masterbank
    constexpr Partition ldr {Partition::ID::eloader,        &masterbank,    masterbank.address,     128*1024}; 
    constexpr Partition upd {Partition::ID::eupdater,       &masterbank,    ldr.address+ldr.size,   256*1024};
    constexpr Partition a00 {Partition::ID::eapplication00, &masterbank,    upd.address+upd.size,   512*1024};  
    constexpr Partition b00 {Partition::ID::buffer00,       &masterbank,    a00.address+a00.size,   128*1024};
    
    // on slavebank
    constexpr Partition a01 {Partition::ID::eapplication01, &slavebank,     slavebank.address,      512*1024};     
    constexpr Partition b01 {Partition::ID::buffer01,       &slavebank,     a01.address+a01.size,   512*1024};    
    
    // the dummy
    constexpr embot::hw::flash::Partition dummy {};
}

namespace embot::hw::flash::bsp {
    
            
    constexpr const embot::hw::flash::Partition & partition(const embot::hw::flash::Partition::ID id)
    {        
        switch(id)
        {
            case embot::hw::flash::Partition::ID::eloader:          { return amcfoc::ldr; } break; 
            case embot::hw::flash::Partition::ID::eupdater:         { return amcfoc::upd; } break;
            case embot::hw::flash::Partition::ID::eapplication00:   { return amcfoc::a00; } break;
            case embot::hw::flash::Partition::ID::buffer00:         { return amcfoc::b00; } break;
            case embot::hw::flash::Partition::ID::eapplication01:   { return amcfoc::a01; } break;
            case embot::hw::flash::Partition::ID::buffer01:         { return amcfoc::b01; } break;
            default:                                                { return amcfoc::dummy;} break;
        }
    
    }
    
    constexpr const embot::hw::flash::RegularBank & bank(const Bank::ID id)
    {        
        switch(id)
        {
            case Bank::ID::one:             { return amcfoc::bank01; } break; 
            case Bank::ID::two:             { return amcfoc::bank02; } break;
            default:                        { return amcfoc::bank01;} break;
        }
    
    }
    
} // namespace embot::hw::flash::bsp {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



