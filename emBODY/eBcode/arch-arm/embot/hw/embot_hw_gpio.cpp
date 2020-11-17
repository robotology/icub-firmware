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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_gpio_bsp.h"

#include <cstring>
#include <vector>
#include <math.h>
#include "embot_core_binary.h"


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

#if !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

namespace embot { namespace hw { namespace gpio {

    bool supported(GPIO &g) { return false; }
    bool initialised(GPIO &g) { return false; }
    result_t init(const embot::hw::GPIO &g, const Config &config) { return resNOK; }
    result_t configure(const embot::hw::stm32GPIO &g, Mode m, Pull p, Speed s) { return resNOK; }      
    result_t set(const embot::hw::stm32GPIO &g, State s) { return resNOK; }
    result_t toggle(const embot::hw::GPIO &g) { return resNOK; }
    State get(const embot::hw::GPIO &g) { return State::RESET; }
    
}}}
    
#else  

namespace embot { namespace hw { namespace gpio {
    
    result_t _configure(const embot::hw::gpio::PROP &g, Mode m, Pull p, Speed s);    
    result_t _set(const embot::hw::gpio::PROP &g, State s);
    result_t _toggle(const embot::hw::gpio::PROP &g);    
    State _get(const embot::hw::gpio::PROP &g);
    
    
    // initialised mask. there must be one for each of PORT::maxnumberof     
    //static std::array<std::uint16_t, embot::core::tointegral(PORT::maxnumberof)> initialised2dmask = {0};
    

    bool supported(GPIO &g)
    {
        return embot::hw::gpio::getBSP().supported(g);
    }
    
    bool initialised(GPIO &g)
    {
        return true; 
    }   


    result_t init(const embot::hw::GPIO &g, const Config &config)
    {
        return configure(g, config.mode, config.pull, config.speed);
    }  

    result_t configure(const embot::hw::GPIO &g, Mode m, Pull p, Speed s)
    {        
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);    
        if(!gg.isvalid())
        {
            return resNOK;
        }            
        _configure(gg, m, p, s);
        return resOK;
    }  
    
    result_t set(const embot::hw::GPIO &g, State s)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        _set(gg, s);
        return resOK;        
    }
    
    result_t toggle(const embot::hw::GPIO &g)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        _toggle(gg);
        return resOK;        
    }
    
    State get(const embot::hw::GPIO &g)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return State::RESET;
        }          
        return _get(gg);        
    }  


    constexpr uint32_t _convert_to_stm32mode(Mode m)
    {
        constexpr uint32_t _map2stm32mode[] = {
            GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD, GPIO_MODE_AF_PP, 
            GPIO_MODE_AF_OD, GPIO_MODE_ANALOG, GPIO_MODE_IT_RISING, GPIO_MODE_IT_FALLING, 
            GPIO_MODE_IT_RISING_FALLING, GPIO_MODE_EVT_RISING, GPIO_MODE_EVT_FALLING, GPIO_MODE_EVT_RISING_FALLING
        };
        
        return _map2stm32mode[embot::core::tointegral(m)];
    }
    
    constexpr uint32_t _convert_to_stm32pull(Pull p)
    {
        constexpr uint32_t _map2stm32pull[] = {
            GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN
        };
        
        return _map2stm32pull[embot::core::tointegral(p)];
    }
    
    constexpr uint32_t _convert_to_stm32speed(Speed s)
    {
        constexpr uint32_t _map2stm32speed[] = {
            GPIO_SPEED_FREQ_LOW, GPIO_SPEED_FREQ_MEDIUM, GPIO_SPEED_FREQ_HIGH, GPIO_SPEED_FREQ_VERY_HIGH
        };
        
        return _map2stm32speed[embot::core::tointegral(s)];
    }
        
   
    result_t _configure(const embot::hw::gpio::PROP &g, Mode m, Pull p, Speed s)
    {

        g.clockenable();
        
        // caveat: for pins HAL_GPIO_* use u16, and all macros are u16, whereas LL_GPIO_* use u32 
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
        else if(m == Mode::INPUT)
        {
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            GPIO_InitStruct.Pin = g.stmpin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = _convert_to_stm32pull(p);
            GPIO_InitStruct.Speed = _convert_to_stm32speed(s);
            HAL_GPIO_Init(g.stmport, &GPIO_InitStruct);            
        }
        else if((m == Mode::EXTIrising) || (m == Mode::EXTIfalling) || (m == Mode::EXTIrisingfalling))
        {
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            GPIO_InitStruct.Pin = g.stmpin;
            GPIO_InitStruct.Mode = _convert_to_stm32mode(m);
            GPIO_InitStruct.Pull = _convert_to_stm32pull(p);
            HAL_GPIO_Init(g.stmport, &GPIO_InitStruct);               
        }
        else
        {
            return resNOK;
        }

        return resOK;
    }  
    
    result_t _set(const embot::hw::gpio::PROP &g, State s)
    {
        HAL_GPIO_WritePin(g.stmport, g.stmpin, static_cast<GPIO_PinState>(s));    
        return resOK;        
    }
    
    result_t _toggle(const embot::hw::gpio::PROP &g)
    {
        HAL_GPIO_TogglePin(g.stmport, g.stmpin);    
        return resOK;        
    }
    
    State _get(const embot::hw::gpio::PROP &g)
    {
        GPIO_PinState s = HAL_GPIO_ReadPin(g.stmport, g.stmpin);            
        return static_cast<State>(s);        
    }
    
       
}}} // namespace embot { namespace hw { namespace gpio     

#endif // !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

