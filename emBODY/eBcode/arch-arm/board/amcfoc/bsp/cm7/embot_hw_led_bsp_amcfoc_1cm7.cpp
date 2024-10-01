
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


// - support map: begin of embot::hw::led

#include "embot_hw_led_bsp.h"

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot::hw::led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() { return thebsp; }
    
}

#else

#include "embot_hw_led_bsp_pinout_amcfoc.h"

namespace embot::hw::led {         
    
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two) | mask::pos2mask<uint32_t>(LED::three) 
        ,        
        // properties
        {{
            &pled4, &pled5, &pled6,
            nullptr, nullptr, nullptr, nullptr, nullptr         
        }}        
    };
    
    void clock_enable_D() { __HAL_RCC_GPIOD_CLK_ENABLE(); }
    void BSP::init(embot::hw::LED h) const 
    {
        uint8_t i = embot::core::tointegral(h);
        // activate the clock as cube-mx may not do that
        //constexpr std::array<embot::core::fpWorker, 6> clockenable { clock_enable_D, clock_enable_D, clock_enable_D, clock_enable_D, clock_enable_D, clock_enable_D};        
        //if(i < clockenable.size())
        //{
        //    clockenable[i]();
        //}
        clock_enable_D(); // no need of the above as every LED is on bus H
        
        // init the gpio
        const embot::hw::GPIO &g = thebsp.properties[i]->gpio;
        embot::hw::gpio::configure(g, embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::low);                        
    } 

    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::led { 

#endif // led

// - support map: end of embot::hw::led



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

