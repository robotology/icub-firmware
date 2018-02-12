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
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

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



namespace embot { namespace hw { namespace led {

    struct bspmap_t
    {
        std::uint32_t               mask;
        GPIO_PinState               on;
        GPIO_PinState               off;
        embot::hw::gpio::GPIO       gpio[static_cast<uint8_t>(LED::maxnumberof)];
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64)    
        
    static const bspmap_t bspmap = 
    {
        0x00000001,
        GPIO_PIN_SET, GPIO_PIN_RESET,
        {
            {LD2_GPIO_Port, LD2_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
   

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const bspmap_t bspmap = 
    {
        0x00000003,
        GPIO_PIN_RESET, GPIO_PIN_SET,
        {
            {LED_RED_GPIO_Port, LED_RED_Pin},
            {LED_BLUE_GPIO_Port, LED_BLUE_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    static const bspmap_t bspmap = 
    {
        0x00000001,
        GPIO_PIN_RESET, GPIO_PIN_SET,
        {
            {LED_GPIO_Port, LED_Pin}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, 
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
    
    #else
        #error embot::hw::led::mask and embot::hw::led::gpiomap[] must be filled    
    #endif
      
    // initialised mask
        
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t led2index(LED led)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(led);
    }
        
    bool supported(LED led)
    {
        if((LED::none == led) || (LED::maxnumberof == led))
        {
            return false;
        }
        return embot::binary::bit::check(bspmap.mask, led2index(led));
    }
    
    bool initialised(LED led)
    {
        if(LED::none == led)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, led2index(led));
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
                
        embot::binary::bit::set(initialisedmask, led2index(led));
        
        // we just switch it off        
        embot::hw::led::off(led);
        
        return resOK;        

    }

    
    result_t init_legacy(LED led, const void *par)
    {
        return init(led);
    }
    
    
    result_t on(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::gpio::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), bspmap.on);
        return resOK;        
    }
    
    result_t off(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::gpio::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), bspmap.off);
        return resOK;          
    }
    
    result_t toggle(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::gpio::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
        return resOK;          
    }
    
    
}}} // namespace embot { namespace hw { namespace led 





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

