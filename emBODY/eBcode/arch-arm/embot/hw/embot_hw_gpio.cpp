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

#include "embot_hw_gpio.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;

#include "embot_hw_bsp.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace gpio {

#if     !defined(HAL_GPIO_MODULE_ENABLED)

    result_t configure(const embot::hw::GPIO &g, Mode m)      { return resNOK; }      
    result_t set(const embot::hw::GPIO &g, State s)        { return resNOK; }
    
#else    
   
    result_t configure(const embot::hw::GPIO &g, Mode m)
    {
        // caveat: HAL_GPIO_* use u16, and all macros are u16, whereas LL_GPIO_* use u32 
        if(m == Mode::OUTPUTopendrain)
        {
            LL_GPIO_SetPinMode(static_cast<GPIO_TypeDef *>(g.port), static_cast<std::uint32_t>(g.pin), LL_GPIO_MODE_OUTPUT);		
            LL_GPIO_SetPinOutputType(static_cast<GPIO_TypeDef *>(g.port), static_cast<std::uint32_t>(g.pin), LL_GPIO_OUTPUT_OPENDRAIN);
        }
        else if(m == Mode::OUTPUTpushpull)
        {
            LL_GPIO_SetPinMode(static_cast<GPIO_TypeDef *>(g.port), static_cast<std::uint32_t>(g.pin), LL_GPIO_MODE_OUTPUT);		
            LL_GPIO_SetPinOutputType(static_cast<GPIO_TypeDef *>(g.port), static_cast<std::uint32_t>(g.pin), LL_GPIO_OUTPUT_PUSHPULL);            
        }
        return resOK;
    }  
    
    result_t set(const embot::hw::GPIO &g, State s)
    {
        if(false == embot::hw::bsp::gpio::getMAP()->isvalid(g))
        {
            return resNOK;
        }
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef *>(g.port), g.pin, static_cast<GPIO_PinState>(s));    
        return resOK;        
    }
    
    State get(const embot::hw::GPIO &g)
    {
        if(false == embot::hw::bsp::gpio::getMAP()->isvalid(g))
        {
            return State::RESET;
        }

        GPIO_PinState s = HAL_GPIO_ReadPin(static_cast<GPIO_TypeDef *>(g.port), g.pin);    
        
        return static_cast<State>(s);        
    }
    
#endif
       
}}} // namespace embot { namespace hw { namespace gpio     



    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

