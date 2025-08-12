/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:    Marco Accame
 * email:     marco.accame@iit.it
 * Coauthor:  Kevin Sangalli
 * email:     kevin.sangalli@iit.it
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

#include "embot_hw_bsp_amc_config.h"
// - support map: begin of embot::hw::encoder

#include "embot_hw_encoder.h"
#include "embot_hw_encoder_bsp.h"

#if !defined(EMBOT_ENABLE_hw_encoder)

namespace embot embot::hw::encoder::bsp{
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else

#include "embot_hw_chip_AS5045.h"
#include "embot_hw_chip_MA730.h"
#include "embot_hw_chip_MB049.h"

#include "embot_hw_spi.h"
#include "embot_hw_spi_bsp.h"

namespace embot::hw::encoder::bsp{
    
    #if defined(STM32HAL_BOARD_AMC)
    
    
    constexpr embot::hw::spi::Config spiCFGchipMA730  {embot::hw::chip::MA730::standardspiconfig};
    constexpr embot::hw::spi::Config spiCFGchipAS5045 {embot::hw::chip::AS5045::standardspiconfig};
    constexpr embot::hw::spi::Config spiCFGchipMB049  {embot::hw::chip::MB049::standardspiconfig};
        
    
    // encoder one --> SPI1
    constexpr PROP e1p = { embot::hw::SPI::one, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };

    // encoder one --> SPI2
    constexpr PROP e2p = { embot::hw::SPI::two, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };
    
    // encoder one --> SPI3
    constexpr PROP e3p = { embot::hw::SPI::three, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ENCODER::one) | mask::pos2mask<uint32_t>(ENCODER::two) | mask::pos2mask<uint32_t>(ENCODER::three),
        // properties
        {{ &e1p, &e2p, &e3p }}
    };
    
    
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}    
    
    #else
        #error embot::hw::bsp::encoder::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#endif // encoder


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


