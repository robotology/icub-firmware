
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_MULTISDA_BSP_H_
#define _EMBOT_HW_MULTISDA_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"


#include "embot_hw_gpio_bsp.h"

namespace embot { namespace hw { namespace multisda {
             
    struct PROP
    {
        constexpr static std::uint8_t numberof = 4;
        embot::hw::gpio::PROP clk {};
        std::array<const embot::hw::gpio::PROP, numberof> sda;
        constexpr PROP() = default;
        constexpr PROP(const embot::hw::gpio::PROP &c, const std::array<const embot::hw::gpio::PROP, PROP::numberof> &s) :clk(c), sda(s) {}
    };
    
    struct BSP
    {
        constexpr BSP(const embot::hw::gpio::PROP &c, const std::array<const embot::hw::gpio::PROP, PROP::numberof> &s) : prop(c, s) {}
        constexpr BSP() : prop({}, {}) {}            
          
        PROP prop;
    };
    
    const BSP& getBSP();
    
}}} // namespace embot { namespace hw { namespace multisda { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


