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

#include "embot_hw_button.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"
#include "embot_hw_gpio.h"
#include "embot_hw_bsp.h"

#include <cstring>
#include <vector>

#include "embot_binary.h"

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------
  
using namespace embot::hw;

namespace embot { namespace hw { namespace button {
          
    // initialised mask        
    static std::uint32_t initialisedmask = 0;
    
    GPIO_PinState convert(embot::hw::gpio::State s)
    {
        return (s == embot::hw::gpio::State::RESET) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET);
    }
    
    bool supported(BTN btn)
    {
        return embot::hw::bsp::button::getBSP().supported(btn);
    }
    
    bool initialised(BTN btn)
    {
        return embot::binary::bit::check(initialisedmask,embot::common::tointegral(btn));
    }
        
    result_t init(BTN btn)
    {
        if(!supported(btn))
        {
            return resNOK;
        }  
        
        if(initialised(btn))
        {   // dont need to re-init
            return resOK;
        }
        
        // every button initalisation is done in the bsp, but ... 
        embot::hw::bsp::button::getBSP().init(btn);        
       
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        

             
        
        embot::binary::bit::set(initialisedmask, embot::common::tointegral(btn));
                
        return resOK;        
    }

    
    
    
    bool pressed(BTN btn)
    {
        if(!initialised(btn))
        {
            return false;
        }  
        const embot::hw::GPIO gpio = embot::hw::bsp::button::getBSP().getPROP(btn)->gpio;       
        
        embot::hw::gpio::State s = embot::hw::gpio::get(gpio);
        //GPIO_PinState b1state = HAL_GPIO_ReadPin(static_cast<GPIO_TypeDef*>(gpio.port), static_cast<uint16_t>(gpio.pin));    
        //return (convert(embot::hw::bsp::button::getBSP().getPROP(btn)->pressed) == b1state) ? (true) : (false);
        return (embot::hw::bsp::button::getBSP().getPROP(btn)->pressed == s) ? (true) : (false);        
    }
    
    
}}} // namespace embot { namespace hw { namespace button 






    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

