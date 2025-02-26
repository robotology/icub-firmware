
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

namespace embot::hw::encoder::bsp {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#elif defined(EMBOT_ENABLE_hw_encoder)

#include "embot_hw_chip_AS5045.h"
#include "embot_hw_chip_MA730.h"
#include "embot_hw_chip_MB049.h"


#if 0
    //in general, the standardspiconfig should work on every board,
    //if you need some more customization you can change the SPI configuration
    //example for a SPI config for an encoder
    constexpr embot::hw::spi::Config spiCFGchip..._spix
    { 
    embot::hw::spi::Prescaler::onehundredtwentyeigth,         //prescaler depends on SPI BUS freq and wanted clock freq for the chip used
    embot::hw::spi::DataSize::eight,                          //depends on chip used
    embot::hw::spi::Mode::one,                                //depends on chip used
    { {embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull,      // | miso | mosi |  //depends on board and mode used
       embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull} }    // | sclk | sel  |  //depends on board and mode used
    };
    
#endif


namespace embot::hw::encoder::bsp {
       
    constexpr embot::hw::spi::Config spiCFGchipAS5045 {embot::hw::chip::AS5045::standardspiconfig};
    constexpr embot::hw::spi::Config spiCFGchipMA730  {embot::hw::chip::MA730::standardspiconfig};
    constexpr embot::hw::spi::Config spiCFGchipMB049  {embot::hw::chip::MB049::standardspiconfig};
    
    //amcfoc supports 2 encoders
    
    // encoder one --> SPI1
    constexpr PROP e1p = { embot::hw::SPI::one, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };

    // encoder two --> SPI2
    constexpr PROP e2p = { embot::hw::SPI::two, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };

   
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ENCODER::one) | mask::pos2mask<uint32_t>(ENCODER::two),
        // properties
        {{ &e1p, &e2p}}
    };
        
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}    
       
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#endif // defined(EMBOT_ENABLE_hw_encoder)

// - support map: end of embot::hw::encoder::bsp




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

