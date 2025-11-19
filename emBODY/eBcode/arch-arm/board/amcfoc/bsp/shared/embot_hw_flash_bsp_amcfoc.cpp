
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_flash_bsp_amcfoc.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"

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
    
} // namespace embot::hw::flash::bsp {

#else
    
namespace embot::hw::flash::bsp {
     
    constexpr theBanks thebanks 
    {
        amcfoc::numbanks, 
        { &amcfoc::bank01, &amcfoc::bank02 }
    }; 
    
    
    constexpr thePartitions thepartitions
    {
        { &amcfoc::ldr, &amcfoc::upd, &amcfoc::a00, &amcfoc::b00, &amcfoc::a01, &amcfoc::b01 }
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

#endif // flash

// - support map: end of embot::hw::flash




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

