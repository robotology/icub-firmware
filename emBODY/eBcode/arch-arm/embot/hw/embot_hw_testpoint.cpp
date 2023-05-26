/*
 * Copyright (C) 2023 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_testpoint.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_testpoint_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_gpio.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

using namespace embot::hw;

#if !defined(EMBOT_ENABLE_hw_testpoint)

namespace embot { namespace hw { namespace testpoint {
    
    #error fill it
}}}

#else

namespace embot { namespace hw { namespace testpoint {

    // initialised mask
    static std::uint32_t initialisedmask = 0;
            
    GPIO_PinState convert(embot::hw::gpio::State s)
    {
        return (s == embot::hw::gpio::State::RESET) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET);
    }
    
    
    bool supported(TESTPOINT testpoint)
    {
        return embot::hw::testpoint::getBSP().supported(testpoint);
    }
    
    bool initialised(TESTPOINT testpoint)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(testpoint));
    }
            
    result_t init(TESTPOINT testpoint)
    {
        if(!supported(testpoint))
        {
            return resNOK;
        }
        
        if(initialised(testpoint))
        {   // dont need to re-init
            return resOK;
        }
        
        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        // we do specific init of the peripheral
        embot::hw::testpoint::getBSP().init(testpoint);
       
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(testpoint));
        
        // we just switch it off
        embot::hw::testpoint::off(testpoint);
        
        return resOK;

    }

          
    result_t on(TESTPOINT testpoint)
    {
        if(!initialised(testpoint))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &embot::hw::testpoint::getBSP().getPROP(testpoint)->gpio;
        //HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), convert(embot::hw::testpoint::getBSP().getPROP(testpoint)->on));
        embot::hw::gpio::set(*gpio, embot::hw::testpoint::getBSP().getPROP(testpoint)->on);
        return resOK;        
    }
    
    result_t off(TESTPOINT testpoint)
    {
        if(!initialised(testpoint))
        {
            return resNOK;
        }
        const embot::hw::GPIO *gpio = &embot::hw::testpoint::getBSP().getPROP(testpoint)->gpio;
        //HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), convert(embot::hw::testpoint::getBSP().getPROP(testpoint)->off));
        embot::hw::gpio::set(*gpio, embot::hw::testpoint::getBSP().getPROP(testpoint)->off);
        return resOK;
    }
    
    result_t toggle(TESTPOINT testpoint)
    {
        if(!initialised(testpoint))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &embot::hw::testpoint::getBSP().getPROP(testpoint)->gpio;
        //HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
        embot::hw::gpio::toggle(*gpio);
        return resOK;
    }
    
    
}}} // namespace embot { namespace hw { namespace testpoint


#endif //#if defined(EMBOT_ENABLE_hw_testpoint)



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

