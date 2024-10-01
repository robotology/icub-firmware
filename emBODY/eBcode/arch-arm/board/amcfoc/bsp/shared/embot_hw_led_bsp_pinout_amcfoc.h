
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_LED_BSP_PINOUT_AMCFOC_H_
#define __EMBOT_HW_LED_BSP_PINOUT_AMCFOC_H_


#include "embot_hw_bsp_config.h"

#include "embot_hw_led_bsp.h"

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot::hw::led { }

#else

namespace embot::hw::led {         
    
    constexpr PROP pled1 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::four}  };  
    constexpr PROP pled2 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::five}  };  
    constexpr PROP pled3 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eight}  }; 
    constexpr PROP pled4 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::nine}  };     
    constexpr PROP pled5 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::ten}  }; 
    constexpr PROP pled6 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eleven}  }; 
        
} // namespace embot::hw::led { 

#endif // led

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


