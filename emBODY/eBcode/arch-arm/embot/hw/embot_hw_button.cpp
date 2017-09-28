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
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

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
  

namespace embot { namespace hw { namespace button {
    
    struct bspmap_t
    {
        std::uint32_t           mask;
        GPIO_PinState           pressed;
        embot::hw::gpio::GPIO   gpio[static_cast<uint8_t>(BTN::maxnumberof)];
    };

    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64)  

    static const bspmap_t bspmap = 
    {
        0x00000001,
        GPIO_PIN_RESET,
        {
            {B1_GPIO_Port, B1_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };    

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const bspmap_t bspmap = 
    {
        0x00000000,
        GPIO_PIN_RESET,
        {
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    static const bspmap_t bspmap = 
    {
        0x00000000,
        GPIO_PIN_RESET,
        {
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
    
    #else
    
    static const bspmap_t bspmap = 
    {
        0x00000000,
        {
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    }; 
    
    #endif
      
    // initialised mask
        
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t btn2index(BTN btn)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(btn);
    }
        
    bool supported(BTN btn)
    {
        if((BTN::none == btn) || (BTN::maxnumberof == btn))
        {
            return false;
        }
        return embot::binary::bit::check(bspmap.mask, btn2index(btn));
    }
    
    bool initialised(BTN btn)
    {
        if(BTN::none == btn)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, btn2index(btn));
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
        
        // every button initalisation is done in the bsp ...
        
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                
        embot::binary::bit::set(initialisedmask, btn2index(btn));
                
        return resOK;        
    }

    
    
    
    bool pressed(BTN btn)
    {
        if(!initialised(btn))
        {
            return false;
        }  
        const embot::hw::gpio::GPIO *gpio = &bspmap.gpio[btn2index(btn)];       
        
        GPIO_PinState b1state = HAL_GPIO_ReadPin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));    
        return (bspmap.pressed == b1state) ? (true) : (false);         
    }
    
    
}}} // namespace embot { namespace hw { namespace button 






    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

