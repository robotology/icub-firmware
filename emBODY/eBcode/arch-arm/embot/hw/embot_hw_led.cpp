/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

#include "embot_hw_led.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"
#include <cstring>
#include <vector>

#include "embot_binary.h"
#include "embot_hw_gpio.h"
#include "embot_hw_bsp.h"

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

using namespace embot::hw;

namespace embot { namespace hw { namespace led {

    // initialised mask
    static std::uint32_t initialisedmask = 0;
            
    GPIO_PinState convert(embot::hw::gpio::State s)
    {
        return (s == embot::hw::gpio::State::RESET) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET);
    }
    
    
    bool supported(LED led)
    {
        return embot::hw::bsp::led::getMAP()->supported(led);
    }
    
    bool initialised(LED led)
    {
        return embot::binary::bit::check(initialisedmask, embot::hw::bsp::led::MAP::toindex(led));
    }
            
    result_t init(LED led)
    {
        if(!supported(led))
        {
            return resNOK;
        }  
        
        if(initialised(led))
        {   // dont need to re-init
            return resOK;
        }
        
        // every led initalisation is done in the bsp ...
        
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                
        embot::binary::bit::set(initialisedmask, embot::hw::bsp::led::MAP::toindex(led));
        
        // we just switch it off        
        embot::hw::led::off(led);
        
        return resOK;        

    }

          
    result_t on(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = embot::hw::bsp::led::getMAP()->getgpio(led);      
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), convert(embot::hw::bsp::led::getMAP()->on));
        return resOK;        
    }
    
    result_t off(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = embot::hw::bsp::led::getMAP()->getgpio(led);       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), convert(embot::hw::bsp::led::getMAP()->off));
        return resOK;          
    }
    
    result_t toggle(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = embot::hw::bsp::led::getMAP()->getgpio(led);       
        HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
        return resOK;          
    }
    
    
}}} // namespace embot { namespace hw { namespace led 





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

