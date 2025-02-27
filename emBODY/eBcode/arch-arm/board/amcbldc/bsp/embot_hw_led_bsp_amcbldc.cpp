/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Kevin Sangalli
 * email:   kevin.sangalli@iit.it
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
#include "embot_hw_sys.h"

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

#include "embot_hw_bsp_amcbldc_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - support maps for the supported drivers
// --------------------------------------------------------------------------------------------------------------------

// - support map: begin of embot::hw::led

#include "embot_hw_bsp_amcbldc.h"

#include "embot_hw_led_bsp.h"


#if !defined(EMBOT_ENABLE_hw_led)

namespace embot::hw::led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else

namespace embot::hw::led {         
           
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::two}  };
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::five}  };
//  board revision b removes PB15 as LED and uses it for the break. so i remove led3p
//  constexpr PROP led3p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fifteen}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two),     
        // properties
        {{
            &led1p, &led2p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 

    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::led {

#endif // led
    
// - support map: end of embot::hw::led


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

    
    
