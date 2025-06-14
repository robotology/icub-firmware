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


using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


using namespace embot::hw;

#if !defined(EMBOT_ENABLE_hw_gpio)

namespace embot { namespace hw { namespace gpio {

    bool supported(GPIO &g) { return false; }
    bool initialised(GPIO &g) { return false; }
    result_t init(const embot::hw::GPIO &g, const Config &config) { return resNOK; }
    result_t configure(const embot::hw::GPIO &g, Mode m, Pull p, Speed s) { return resNOK; }      
    result_t set(const embot::hw::GPIO &g, State s) { return resNOK; }
    result_t toggle(const embot::hw::GPIO &g) { return resNOK; }
    State get(const embot::hw::GPIO &g) { return State::RESET; }
    
}}}
    
#else  

namespace embot::hw::gpio::DRIVER {    
    // these depend on the hal layer beneath
    bool _clockenable(const embot::hw::gpio::PROP &gg);
    bool _configure(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg, Mode m, Pull p, Speed s);   
    bool _deconfigure(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg);      
    bool _set(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg, State s);
    bool _toggle(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg);    
    State _get(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg);    
}

namespace embot::hw::gpio {
     
    // initialised mask. there must be one for each of PORT::maxnumberof     
    static std::array<std::uint32_t, embot::core::tointegral(embot::hw::GPIO::PORT::maxnumberof)> initialised2dmask = {0};
    
    bool supported(const GPIO &g)
    {
        return embot::hw::gpio::getBSP().supported(g);
    }
    
    bool initialised(const GPIO &g)
    {
        uint8_t i = embot::core::tointegral(g.port);
        if(i < embot::core::tointegral(embot::hw::GPIO::PORT::maxnumberof))
        {   // i dont do control vs j < embot::core::tointegral(embot::hw::GPIO::PIN::maxnumberof)
            // because initialised2dmask is an array of uint32_t and embot::hw::GPIO::PIN at max is 31 by design  
            uint8_t j = embot::core::tointegral(g.pin);
            return embot::core::binary::bit::check(initialised2dmask[i], j);
        }
        return false; 
    }   


    result_t init(const embot::hw::GPIO &g, const Config &config)
    {
        return configure(g, config.mode, config.pull, config.speed);
    }  
 
    
    result_t deinit(const embot::hw::GPIO &g)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);    
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        embot::hw::gpio::DRIVER::_deconfigure(g, gg); 
        
        embot::core::binary::bit::clear(initialised2dmask[embot::core::tointegral(g.port)], embot::core::tointegral(g.pin));
        return resOK;
    }


    result_t clockenable(const embot::hw::GPIO &g)
    { 
        result_t r {resNOK};
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);    
        if(!gg.isvalid())
        {
            return resNOK;
        }            
        if(true == embot::hw::gpio::DRIVER::_clockenable(gg))
        {
            r = resOK;
        }
        return r;
    }  
    
    result_t configure(const embot::hw::GPIO &g, Mode m, Pull p, Speed s)
    { 
        result_t r {resNOK};
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);    
        if(!gg.isvalid())
        {
            return resNOK;
        }            
        if(true == embot::hw::gpio::DRIVER::_configure(g, gg, m, p, s))
        {
            embot::core::binary::bit::set(initialised2dmask[embot::core::tointegral(g.port)], embot::core::tointegral(g.pin));
            r = resOK;
        }
        return r;
    }  
 
    
    result_t set(const embot::hw::GPIO &g, State s)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        embot::hw::gpio::DRIVER::_set(g, gg, s);
        return resOK;        
    }
 
    
    result_t toggle(const embot::hw::GPIO &g)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return resNOK;
        }  
        embot::hw::gpio::DRIVER::_toggle(g, gg);
        return resOK;        
    }
 
    
    State get(const embot::hw::GPIO &g)
    {
        embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(g);
        if(!gg.isvalid())
        {
            return State::RESET;
        }          
        return embot::hw::gpio::DRIVER::_get(g, gg);        
    }  

} // namespace embot::hw::gpio     

   

// and now comes what depends on the hal

#if defined(USE_HAL_DRIVER)   

#include "stm32hal.h"

namespace embot::hw::gpio::DRIVER {  
    
    constexpr uint32_t _convert_to_stm32mode(Mode m)
    {
        constexpr uint32_t _map2stm32mode[] = {
            GPIO_MODE_INPUT, 
            GPIO_MODE_OUTPUT_PP, 
            GPIO_MODE_OUTPUT_OD, 
            GPIO_MODE_AF_PP, 
            GPIO_MODE_AF_OD, 
            GPIO_MODE_ANALOG, 
#if defined(GPIO_MODE_ANALOG_ADC_CONTROL)            
            GPIO_MODE_ANALOG_ADC_CONTROL,   // as for instance in l4 architecture
#else
            GPIO_MODE_ANALOG,               // as for instance in g4 and h7 architectures
#endif            
            GPIO_MODE_IT_RISING, 
            GPIO_MODE_IT_FALLING, 
            GPIO_MODE_IT_RISING_FALLING, 
            GPIO_MODE_EVT_RISING, 
            GPIO_MODE_EVT_FALLING, 
            GPIO_MODE_EVT_RISING_FALLING
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

    bool _clockenable(const embot::hw::gpio::PROP &gg)
    {
        gg.clockenable();
        return true;       
    }      
   
    bool _configure(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg, Mode m, Pull p, Speed s)
    {
        bool r {false};
       
        switch(m)
        {
            case Mode::INPUT:
            case Mode::OUTPUTpushpull:
            case Mode::OUTPUTopendrain:
            case Mode::EXTIrising:
            case Mode::EXTIfalling:
            case Mode::EXTIrisingfalling:
            {
                gg.clockenable();
                GPIO_InitTypeDef GPIO_InitStruct = {0};       
                GPIO_InitStruct.Pin = gg.stmpin;
                GPIO_InitStruct.Mode = _convert_to_stm32mode(m); 
                GPIO_InitStruct.Pull = _convert_to_stm32pull(p);
                GPIO_InitStruct.Speed = _convert_to_stm32speed(s);
                HAL_GPIO_Init(gg.stmport, &GPIO_InitStruct);
                r = true;                
            } break;
            
            default: 
            {
            } break;
        }
        
        return r;
       
    }  

    bool _deconfigure(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg)
    {
        HAL_GPIO_DeInit(gg.stmport, gg.stmpin);  
        return true;
    }    
    
    bool _set(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg, State s)
    {
        HAL_GPIO_WritePin(gg.stmport, gg.stmpin, static_cast<GPIO_PinState>(s));    
        return true;        
    }
    
    bool _toggle(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg)
    {
        HAL_GPIO_TogglePin(gg.stmport, gg.stmpin);    
        return true;        
    }
    
    State _get(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg)
    {
        GPIO_PinState s = HAL_GPIO_ReadPin(gg.stmport, gg.stmpin);            
        return static_cast<State>(s);        
    }

} // namespace embot::hw::gpio::DRIVER {  

#elif defined(USE_hal2_DRIVER) // in case we use legacy hal2

namespace embot::hw::gpio::DRIVER {  
    
    namespace hal2tools {
        
        hal_gpio_pin_t topin(GPIO::PIN p)       { return static_cast<hal_gpio_pin_t>(0x0001 << embot::core::tointegral(p)); }
        hal_gpio_port_t toport(GPIO::PORT p)    { return static_cast<hal_gpio_port_t>(p); }
        hal_gpio_t togpio(GPIO g)               { return {toport(g.port), topin(g.pin)}; }
        hal_gpio_val_t toval(State s)           { return static_cast<hal_gpio_val_t>(s); }
        hal_gpio_cfg_t toconfig(Mode m, Pull p, Speed s) 
        { 
            hal_gpio_dir_t dd { Mode::INPUT == m ? hal_gpio_dirINP : hal_gpio_dirOUT};
            hal_gpio_speed_t ss { Speed::none == s ? hal_gpio_speed_NONE : (static_cast<hal_gpio_speed_t>(static_cast<uint8_t>(s)+1))};   
            return {dd, ss, nullptr}; 
        }
        
    }    

    bool _clockenable(const embot::hw::gpio::PROP &gg)
    {
        return true;
    }
    
    bool _configure(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg, Mode m, Pull p, Speed s)
    {
        bool r {false};
       
        switch(m)
        {
            case Mode::INPUT:
            case Mode::OUTPUTpushpull:
            case Mode::OUTPUTopendrain:
            {
                const hal_gpio_cfg_t cfg = hal2tools::toconfig(m, p, s);
                hal_gpio_init(hal2tools::togpio(g), &cfg);
                r = true;                
            } break;
            
            case Mode::EXTIrising:
            case Mode::EXTIfalling:
            case Mode::EXTIrisingfalling:            
            default: 
            {
            } break;
        }
        
        return r;       
    }  

    bool _deconfigure(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg)
    {
        return true;
    }  
    
    bool _set(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg, State s)
    {
        hal_gpio_setval(hal2tools::togpio(g), hal2tools::toval(s));
        return true;        
    }
    
    bool _toggle(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg)
    {
        hal_gpio_val_t v = hal_gpio_getval(hal2tools::togpio(g));
        State s = (hal_gpio_valLOW == v) ? (State::SET) : (State::RESET);
        _set(g, gg, s);
        return true;        
    }
    
    State _get(const embot::hw::GPIO &g, const embot::hw::gpio::PROP &gg)
    {
        hal_gpio_val_t v = hal_gpio_getval(hal2tools::togpio(g));         
        return static_cast<State>(v);        
    } 

} // namespace embot::hw::gpio::DRIVER {  

#else
    #error either HAL or hal2
#endif    
       


#endif // !defined(EMBOT_ENABLE_hw_gpio)

    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

