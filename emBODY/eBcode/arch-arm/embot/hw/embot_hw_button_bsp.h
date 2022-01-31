
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BUTTON_BSP_H_
#define __EMBOT_HW_BUTTON_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_gpio_bsp.h"

namespace embot { namespace hw { namespace button {
    
    struct PROP
    { 
        static constexpr int32_t irqNONE {-1000}; 
        embot::hw::gpio::State pressed {embot::hw::gpio::State::SET};
        embot::hw::GPIO gpio {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};
        embot::hw::gpio::Pull pull {embot::hw::gpio::Pull::nopull};
        int32_t irqn {irqNONE};
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::BTN::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::BTN h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::BTN h) const;
        void onEXTI(const embot::hw::gpio::PROP &p) const; // it must be called by IRQhandler or its callback with for example {nullptr, GPIO_PIN_5} 
    };
    
    const BSP& getBSP();
        
}}} // namespace embot { namespace hw { namespace button { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


