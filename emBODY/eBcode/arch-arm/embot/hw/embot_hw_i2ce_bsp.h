
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_I2CE_BSP_H_
#define _EMBOT_HW_I2CE_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_i2ce.h"

namespace embot { namespace hw { namespace i2ce { namespace bsp {
    
    enum class Signal { SCL = 0, SDA = 1, NumberOf = 2 };
    constexpr size_t SignalsNumberOf {embot::core::tointegral(Signal::NumberOf)};
    
    struct PROP
    {     
        embot::hw::i2ce::Speed speed {embot::hw::i2ce::Speed::standard100};
        std::array<embot::hw::GPIO, SignalsNumberOf> pinout { {} };
        constexpr PROP() = default;
        constexpr PROP(embot::hw::i2ce::Speed s, std::array<embot::hw::GPIO, SignalsNumberOf> p) : speed(s), pinout(p) {} 
        constexpr const embot::hw::GPIO& scl() const { return pinout[0]; }  
        constexpr const embot::hw::GPIO& sda() const { return pinout[1]; }        
    };
       
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::I2CE::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::I2CE h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
   
        bool init(embot::hw::I2CE h, const Config &config) const;
        bool deinit(embot::hw::I2CE h) const;
        
        constexpr embot::hw::I2CE toID(const PROP& p) const
        { 
            // look for the (scl, sda) gpios, aka p.pinout[0] and p.pinout[1]
            for(uint8_t i=0; i<maxnumberof; i++) 
            { 
                if((p.pinout[0] == properties[i]->pinout[0]) && (p.pinout[1] == properties[i]->pinout[1])) 
                    return static_cast<embot::hw::I2CE>(i); 
            }
            return embot::hw::I2CE::none;
        }
    };
    
    const BSP& getBSP();
                                     
        
}}}} // namespace embot { namespace hw { namespace i2ce { namepwce bsp



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


