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


#include "embot_hw_testpoint_bsp.h"

#if !defined(EMBOT_ENABLE_hw_testpoint)

namespace embot::hw::testpoint {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TESTPOINT h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else

namespace embot::hw::testpoint {
    
    #if   defined(STM32HAL_BOARD_AMC)

    
    constexpr PROP tp1 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five}  };
    constexpr PROP tp2 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::eight}  };
    constexpr PROP tp3 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::zero}  };
    constexpr PROP tp4 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}}; 
    
    constexpr BSP thebsp {
        // maskofsupported
        mask::pos2mask<uint32_t>(TESTPOINT::one) | mask::pos2mask<uint32_t>(TESTPOINT::two) | 
        mask::pos2mask<uint32_t>(TESTPOINT::three) | mask::pos2mask<uint32_t>(TESTPOINT::four) 
        ,
        // properties
        {{
            &tp1, &tp2, &tp3, &tp4
        }}
    };
    
    void clock_enable_A() { __HAL_RCC_GPIOA_CLK_ENABLE(); }
    void clock_enable_C() { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    void BSP::init(embot::hw::TESTPOINT h) const 
    {
        // activate the clock if cube-mx didn't do that
        uint8_t i = embot::core::tointegral(h);

        clock_enable_A();
        clock_enable_C();
        
        // init the gpio
        const embot::hw::GPIO &g = thebsp.properties[i]->gpio;
        embot::hw::gpio::configure(g, embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::high);
    } 

    #else
        #error embot::hw::testpoint::thebsp must be defined
    #endif
    
    const BSP& getBSP()
    {
        return thebsp;
    }

} // namespace embot:hw::testpoint {

#endif // testpoint


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


