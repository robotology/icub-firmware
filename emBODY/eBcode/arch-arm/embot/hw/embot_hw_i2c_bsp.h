
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_I2C_BSP_H_
#define _EMBOT_HW_I2C_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_i2c.h"

namespace embot { namespace hw { namespace i2c {
    
#if   defined(HAL_I2C_MODULE_ENABLED)    
    using I2C_Handle = I2C_HandleTypeDef;
#else
    using I2C_Handle = void;
#endif
    
    struct PROP
    {     
        I2C_Handle* handle {nullptr}; 
        embot::hw::i2c::Speed speed {embot::hw::i2c::Speed::standard100};
        constexpr PROP() = default;
        constexpr PROP(I2C_Handle *h, embot::hw::i2c::Speed s) : handle(h), speed(s) {}          
    };
       
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::I2C::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::I2C h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::I2C h) const;
        constexpr embot::hw::I2C toID(const PROP& p) const
        { 
            if(nullptr == p.handle) { return embot::hw::I2C::none; }
            for(uint8_t i=0; i<maxnumberof; i++) { if(p.handle == properties[i]->handle) return static_cast<embot::hw::I2C>(i); }
            return embot::hw::I2C::none;
        }
    };
    
    const BSP& getBSP();
                                     
        
}}} // namespace embot { namespace hw { namespace i2c 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


