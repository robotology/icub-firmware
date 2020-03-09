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
#include "embot_hw_bsp_config.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;

#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


using namespace embot::hw;

#if !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

namespace embot { namespace hw { namespace gpio {

    result_t configure(const embot::hw::stm32GPIO &g, Mode m) { return resNOK; }      
    result_t set(const embot::hw::stm32GPIO &g, State s) { return resNOK; }
    
}}}
    
#else  

namespace embot { namespace hw { namespace gpio {
    
    
    result_t configure(const embot::hw::bsp::gpio::PROP &g, Mode m);    
    result_t set(const embot::hw::bsp::gpio::PROP &g, State s);
    result_t toggle(const embot::hw::bsp::gpio::PROP &g);    
    State get(const embot::hw::bsp::gpio::PROP &g);
    
    
    // initialised mask. there must be one for each of PORT::maxnumberof     
    //static std::array<std::uint16_t, embot::core::tointegral(PORT::maxnumberof)> initialised2dmask = {0};
    

    bool supported(GPIO &g)
    {
        return embot::hw::bsp::gpio::getBSP().supported(g);
    }
    
    bool initialised(GPIO &g)
    {
        return true; 
    }   


    result_t init(embot::hw::GPIO &g, const Config &config)
    {
        return resOK;
    }  


    result_t configure(const embot::hw::GPIO &g, Mode m)
    {        
        embot::hw::bsp::gpio::PROP gg = embot::hw::bsp::gpio::getBSP().getPROP(g);    
        if(!gg.isvalid())
        {
            return resNOK;
        }            
        configure(gg, m);
        return resOK;
    }  
    
    result_t set(const embot::hw::GPIO &g, State s)
    {
        embot::hw::bsp::gpio::PROP gg = embot::hw::bsp::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        set(gg, s);
        return resOK;        
    }
    
    result_t toggle(const embot::hw::GPIO &g)
    {
        embot::hw::bsp::gpio::PROP gg = embot::hw::bsp::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        toggle(gg);
        return resOK;        
    }
    
    State get(const embot::hw::GPIO &g)
    {
        embot::hw::bsp::gpio::PROP gg = embot::hw::bsp::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return State::RESET;
        }          
        return get(gg);        
    }    
   
    result_t configure(const embot::hw::bsp::gpio::PROP &g, Mode m)
    {
        // caveat: HAL_GPIO_* use u16, and all macros are u16, whereas LL_GPIO_* use u32 
        if(m == Mode::OUTPUTopendrain)
        {
            LL_GPIO_SetPinMode(g.stmport, static_cast<std::uint32_t>(g.stmpin), LL_GPIO_MODE_OUTPUT);		
            LL_GPIO_SetPinOutputType(g.stmport, static_cast<std::uint32_t>(g.stmpin), LL_GPIO_OUTPUT_OPENDRAIN);
        }
        else if(m == Mode::OUTPUTpushpull)
        {
            LL_GPIO_SetPinMode(g.stmport, static_cast<std::uint32_t>(g.stmpin), LL_GPIO_MODE_OUTPUT);		
            LL_GPIO_SetPinOutputType(g.stmport, static_cast<std::uint32_t>(g.stmpin), LL_GPIO_OUTPUT_PUSHPULL);            
        }
        return resOK;
    }  
    
    result_t set(const embot::hw::bsp::gpio::PROP &g, State s)
    {
        HAL_GPIO_WritePin(g.stmport, g.stmpin, static_cast<GPIO_PinState>(s));    
        return resOK;        
    }
    
    result_t toggle(const embot::hw::bsp::gpio::PROP &g)
    {
        HAL_GPIO_TogglePin(g.stmport, g.stmpin);    
        return resOK;        
    }
    
    State get(const embot::hw::bsp::gpio::PROP &g)
    {
        GPIO_PinState s = HAL_GPIO_ReadPin(g.stmport, g.stmpin);            
        return static_cast<State>(s);        
    }
    
       
}}} // namespace embot { namespace hw { namespace gpio     

#endif // !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

