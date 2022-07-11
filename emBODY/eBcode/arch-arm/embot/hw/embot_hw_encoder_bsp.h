
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ENCODER_BSP_H_
#define __EMBOT_HW_ENCODER_BSP_H_


#include "embot_hw_chip_AS5045.h"
#include "embot_hw_chip_MA730.h"

namespace embot { namespace hw { namespace encoder {
    
    struct nonePROP
    {
        uint32_t none {0};
        constexpr nonePROP() = default;           
    };
    
    struct PROP
    {   // this one is ok for a spi encoder.
        embot::hw::SPI spiBus {embot::hw::SPI::none};
        constexpr PROP() = default;
        constexpr PROP(const embot::hw::SPI &b)
            : spiBus(b) {}
        
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::ENCODER::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;
        constexpr const PROP * getPROP(embot::hw::ENCODER h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::ENCODER h) const;
        void deinit(embot::hw::ENCODER h) const;
    };
    
    const BSP& getBSP();
        
}}} // namespace embot { namespace hw { namespace encoder { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


