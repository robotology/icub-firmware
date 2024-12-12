
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

// - support map: begin of embot::hw::encoder

#include "embot_hw_encoder.h"
#include "embot_hw_encoder_bsp.h"

#if !defined(EMBOT_ENABLE_hw_encoder)

namespace embot::hw::encoder {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#elif defined(EMBOT_ENABLE_hw_encoder)

namespace embot::hw::encoder {
    
    // encoder one --> SPI1
    constexpr PROP e1p = { embot::hw::SPI::one };

    // encoder two --> SPI2
    constexpr PROP e2p = { embot::hw::SPI::two };
//    
//    // encoder one --> SPI3
//    constexpr PROP e3p = { embot::hw::SPI::three };
//        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ENCODER::one) | mask::pos2mask<uint32_t>(ENCODER::two) | mask::pos2mask<uint32_t>(ENCODER::three),
        // properties
        {{ &e1p, &e2p, nullptr }}
    };
        
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}    
       
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#endif // defined(EMBOT_ENABLE_hw_encoder)

// - support map: end of embot::hw::encoder




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

