/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
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
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_led_bsp.h"


#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

void leds_init_off()
{
    using namespace embot::hw;
    const led::BSP &b = led::getBSP();
    constexpr std::array<LED, embot::core::tointegral(LED::maxnumberof)> leds {LED::one, LED::two, LED::three, LED::four, LED::five, LED::six, LED::seven , LED::eight};
    for(const auto &l : leds)
    {
        const led::PROP *p = b.getPROP(l);
        if(nullptr != p)
        {
            gpio::init(p->gpio, {gpio::Mode::OUTPUTpushpull, gpio::Pull::nopull, gpio::Speed::veryhigh});  
            gpio::set(p->gpio, p->off);
        }
    }    
}

void leds_set(bool on)
{
    using namespace embot::hw;
    const led::BSP &b = led::getBSP();
    constexpr std::array<LED, embot::core::tointegral(LED::maxnumberof)> leds {LED::one, LED::two, LED::three, LED::four, LED::five, LED::six, LED::seven , LED::eight};
    for(const auto &l : leds)
    {
        const led::PROP *p = b.getPROP(l);
        if(nullptr != p)
        {
            gpio::set(p->gpio, on ? p->on : p->off);
        }
    }    
}

void verify_flash_bank()
{
    FLASH_OBProgramInitTypeDef odb = {0};   
    HAL_FLASHEx_OBGetConfig(&odb);
    
    uint8_t detectedbanks = ((odb.USERConfig & FLASH_OPTR_DBANK) == FLASH_OPTR_DBANK) ? 2 : 1;

#if defined(EMBOT_ENABLE_hw_flash_DUALBANK)
    constexpr uint8_t expectedbanks = 2;
#else
    constexpr uint8_t expectedbanks = 1;
#endif
    
    if(expectedbanks != detectedbanks)
    {
        embot::core::print("number of banks is not as expected: detected " + std::to_string(detectedbanks));
        embot::core::print("cannot continue. program Option bytes / User Configuration / DBANK to be 1 or 2 banks");
        
        leds_init_off();
        for(;;)
        {
            leds_set(true);
            HAL_Delay(250);
            leds_set(false);
            HAL_Delay(250);
        }
    }        
}

bool embot::hw::bsp::specialize() 
{   

    leds_init_off();
    
    verify_flash_bank();
    
    return true; 
}

#endif  //EMBOT_ENABLE_hw_bsp_specialize


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

