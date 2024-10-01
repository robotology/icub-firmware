
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


// - support map: begin of embot::hw::eeprom

#include "embot_hw_eeprom.h"
#include "embot_hw_eeprom_bsp.h"

#if !defined(EMBOT_ENABLE_hw_eeprom)

namespace embot::hw::eeprom {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::EEPROM h) const {}
    void BSP::deinit(embot::hw::EEPROM h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

//    {EEPR_nSEL_GPIO_Port, EEPR_nSEL_Pin},   /* EEP_DEV */
//    {ETH_nSEL_GPIO_Port, ETH_nSEL_Pin},     /* PHY_DEV */
//#define ETH_nSEL_Pin GPIO_PIN_4
//#define ETH_nSEL_GPIO_Port GPIOA
//#define EEPR_nSEL_Pin GPIO_PIN_0
//#define EEPR_nSEL_GPIO_Port GPIOG

#elif defined(EMBOT_ENABLE_hw_eeprom)

namespace embot::hw::eeprom {
           
    constexpr PROP ee1p = { embot::hw::eeprom::Type::chipM95512DF, 
                            {   
                                embot::hw::SPI::three, 
                                { 
                                    embot::hw::spi::Prescaler::eight, 
                                    embot::hw::spi::DataSize::eight, 
                                    embot::hw::spi::Mode::zero,
                                    { {embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::none} }
                                },
                                {
                                    {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::zero},    // nS
                                    {}, // nW ?? schematics does not show
                                    {}, // it is always high
//                                    {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::twelve},   // nW
//                                    {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::thirteen}, // nHOLD
                                    {
                                        embot::hw::gpio::Mode::OUTPUTpushpull,
                                        embot::hw::gpio::Pull::nopull,
                                        embot::hw::gpio::Speed::veryhigh
                                    }
                                }
                            }   
                          };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(EEPROM::one),        
        // properties
        {{
            &ee1p, nullptr            
        }}        
    };
    
    
    void BSP::init(embot::hw::EEPROM h) const {}
    void BSP::deinit(embot::hw::EEPROM h) const {}    
    
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
} // namespace embot::hw::eeprom {

#endif // defined(EMBOT_ENABLE_hw_eeprom)

// - support map: end of embot::hw::eeprom




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

