
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BNO055_BSP_H_
#define _EMBOT_HW_BNO055_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "embot_hw_bno055.h"

namespace embot { namespace hw { namespace bno055 {
                                  
    struct PROP
    { 
        embot::hw::i2c::Descriptor i2cdes {embot::hw::I2C::none, 0};
        embot::hw::GPIO boot {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};        
        embot::hw::GPIO reset {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};   
        constexpr PROP(const embot::hw::i2c::Descriptor &d, const embot::hw::GPIO &b, const embot::hw::GPIO &r) : i2cdes(d), boot(b), reset(r) {}
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::BNO055::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::BNO055 h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::BNO055 h) const;
    };
    
    const BSP& getBSP();
    
}}} // namespace embot { namespace hw { namespace bno055 { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


