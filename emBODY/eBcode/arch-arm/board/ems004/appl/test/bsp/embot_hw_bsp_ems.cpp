
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"
#include "embot_hw_bsp_ems.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL) && defined(USE_hal2_DRIVER)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal over hal2
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_ems_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::flash

// it must be declared

#include "embot_hw_flash_bsp.h"

namespace embot::hw::flash::bsp {
         
    constexpr uint8_t numbanks {1};
    constexpr uint32_t banksize {1024*1024};
    constexpr uint32_t pagesize {128*1024}; // actually the pages have different sizes but in the application space 128k is ok
    // of 
    #if 0
    16*1024,    
    16*1024,
    16*1024,
    16*1024,
    64*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024    
    #endif
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr theBanks thebanks 
    {
        numbanks, 
        { &bank01, nullptr }
    }; 
    
    // on Bank::one
    constexpr Partition ldr {Partition::ID::eloader,        &bank01,    bank01.address,         32*1024}; 
    constexpr Partition upd {Partition::ID::eupdater,       &bank01,    ldr.address+ldr.size,   96*1024};
    constexpr Partition a00 {Partition::ID::eapplication00, &bank01,    upd.address+upd.size,   384*1024};  

    
    constexpr thePartitions thepartitions
    {
        { &ldr, &upd, &a00 }
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



// - support map: end of embot::hw::flash


// - support map: begin of embot::hw::gpio


#include "embot_hw_gpio_bsp.h"

#if   !defined(EMBOT_ENABLE_hw_gpio)

namespace embot::hw::gpio {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::GPIO h) const {}    
    const BSP& getBSP() { return thebsp; }    
}

#else
    
namespace embot::hw::gpio {
 
    static uint32_t fake {0};
    void *GPIOfake {&fake};
    static const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff 
        }},            
        // ports
        {{
            GPIOfake, GPIOfake, GPIOfake, GPIOfake, GPIOfake, GPIOfake, GPIOfake // until G
        }},
        {{
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            // clock init is done inside hal2 
        }}
    };      

    
    void BSP::init(embot::hw::GPIO h) const {}        
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::gpio {

#endif // gpio

// - support map: end of embot::hw::gpio


// - support map: begin of embot::hw::led

#include "embot_hw_led_bsp.h"

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot::hw::led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() { return thebsp; }
    
}

#else

namespace embot::hw::led {     
    
    // the gpio are taken from: hal_led_boardconfig_t hal_led__theboardconfig;
    
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::eight}  };  
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::ten}  };  
    constexpr PROP led3p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::twelve}  }; 
    constexpr PROP led4p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::fifteen}  };     
    constexpr PROP led5p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eleven}  }; 
    constexpr PROP led6p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::seven}  }; 
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two) | mask::pos2mask<uint32_t>(LED::three) |
        mask::pos2mask<uint32_t>(LED::four) | mask::pos2mask<uint32_t>(LED::five) | mask::pos2mask<uint32_t>(LED::six),        
        // properties
        {{
            &led1p, &led2p, &led3p, &led4p, &led5p, &led6p
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const 
    {
        uint8_t i = embot::core::tointegral(h);
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



// - support map: begin of embot::hw::can

#include "embot_hw_can_bsp.h"

#if !defined(EMBOT_ENABLE_hw_can)

namespace embot::hw::can {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::CAN h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
} // namespace embot::hw::can {

#else

namespace embot::hw::can {
                          
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one) | mask::pos2mask<uint32_t>(CAN::two),        
        // properties
        {{
            nullptr, nullptr           
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const { }
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::can {

#endif


// --------------------------------------------------------------------------------------------------------------------
// - board specific methods
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------


#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   


bool embot::hw::bsp::specialize()
{
    // do something
    return true;
}


#endif  //EMBOT_ENABLE_hw_bsp_specialize


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

