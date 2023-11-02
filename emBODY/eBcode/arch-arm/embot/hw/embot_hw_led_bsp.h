
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_LED_BSP_H_
#define _EMBOT_HW_LED_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_gpio.h"

namespace embot { namespace hw { namespace led {

//namespace embot { namespace hw { namespace bsp { namespace led {
    
    struct PROP
    { 
        embot::hw::gpio::State on {embot::hw::gpio::State::SET};
        embot::hw::gpio::State off {embot::hw::gpio::State::SET};
        embot::hw::GPIO gpio {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};  
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::LED::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::LED h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::LED h) const;
    };
    
    const BSP& getBSP();
    
              
//}}}} // namespace embot { namespace hw { namespace bsp {  namespace led {        
        
}}} // namespace embot { namespace hw { namespace led 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


