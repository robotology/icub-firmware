
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::flash

#include "embot_hw_flash_bsp.h"

#if !defined(EMBOT_ENABLE_hw_flash)

namespace embot::hw::flash::bsp {
    
    constexpr BSP thebsp { };
    void BSP::init() const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else
    
namespace embot::hw::flash::bsp {
     
    
    constexpr uint8_t numbanks {2};
    constexpr uint32_t banksize {1024*1024};
    constexpr uint32_t pagesize {128*1024};
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr BankDescriptor bank02 { Bank::ID::two, 0x08100000, banksize, pagesize };
    constexpr theBanks thebanks 
    {
        numbanks, 
        { &bank01, &bank02 }
    }; 
    
    // on Bank::two
    constexpr Partition ldr {Partition::ID::eloader,        &bank02,    bank02.address,         128*1024}; 
    constexpr Partition upd {Partition::ID::eupdater,       &bank02,    ldr.address+ldr.size,   256*1024};
    constexpr Partition a00 {Partition::ID::eapplication00, &bank02,    upd.address+upd.size,   512*1024};  
    constexpr Partition b00 {Partition::ID::buffer00,       &bank02,    a00.address+a00.size,   128*1024};
    
    // on Bank::one
    constexpr Partition a01 {Partition::ID::eapplication01, &bank01,    bank01.address,         512*1024};     
    constexpr Partition b01 {Partition::ID::buffer01,       &bank01,    a01.address+a01.size,   512*1024};
    
    constexpr thePartitions thepartitions
    {
        { &ldr, &upd, &a00, &b00, &a01, &b01 }
    };

    constexpr BSP thebsp {        
        thebanks,
        thepartitions
    };   
                  
    
    void BSP::init() const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::flash::bsp {


// some checks now
#include "eEmemorymap.h"


namespace embot::hw::flash::bsp {
    
static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eloader)->address == EENV_MEMMAP_ELOADER_ROMADDR, 
    "see EENV_MEMMAP_ELOADER_ROMADDR"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eloader)->size == EENV_MEMMAP_ELOADER_ROMSIZE, 
    "see EENV_MEMMAP_ELOADER_ROMSIZE"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eupdater)->address == EENV_MEMMAP_EUPDATER_ROMADDR, 
    "see EENV_MEMMAP_EUPDATER_ROMADDR"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eupdater)->size == EENV_MEMMAP_EUPDATER_ROMSIZE, 
    "see EENV_MEMMAP_EUPDATER_ROMSIZE"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eapplication00)->address == EENV_MEMMAP_EAPPLICATION_ROMADDR, 
    "see EENV_MEMMAP_EAPPLICATION_ROMADDR"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eapplication00)->size == EENV_MEMMAP_EAPPLICATION_ROMSIZE, 
    "see EENV_MEMMAP_EAPPLICATION_ROMSIZE"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eapplication01)->address == EENV_MEMMAP_EAPPLICATION_2_ROMADDR, 
    "see EENV_MEMMAP_EAPPLICATION_2_ROMADDR"
);

static_assert
(
    thebsp.thepartitions.get(embot::hw::flash::Partition::ID::eapplication01)->size == EENV_MEMMAP_EAPPLICATION_2_ROMSIZE, 
    "see EENV_MEMMAP_EAPPLICATION_2_ROMSIZE"
);

} // namespace embot::hw::flash::bsp {

#endif // flash

// - support map: end of embot::hw::flash




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

