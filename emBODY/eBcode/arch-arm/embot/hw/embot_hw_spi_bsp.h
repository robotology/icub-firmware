
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_SPI_BSP_H_
#define __EMBOT_HW_SPI_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_spi.h"

namespace embot { namespace hw { namespace spi {
    
#if   defined(HAL_SPI_MODULE_ENABLED)    
    using SPI_Handle = SPI_HandleTypeDef;
#else
    using SPI_Handle = void;
#endif
    
    struct PROP
    {     
        SPI_Handle* handle {nullptr}; 
        embot::hw::spi::Speed speed {12500};
        constexpr PROP() = default;
        constexpr PROP(SPI_Handle *h, embot::hw::spi::Speed s) : handle(h), speed(s) {}
        constexpr PROP(SPI_Handle *h) : handle(h) {}            
    };
       
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::SPI::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::SPI h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        bool init(embot::hw::SPI h) const;
        bool deinit(embot::hw::SPI h) const;
        constexpr embot::hw::SPI toID(const PROP& p) const
        { 
            if(nullptr == p.handle) { return embot::hw::SPI::none; }
            for(uint8_t i=0; i<maxnumberof; i++) { if(p.handle == properties[i]->handle) return static_cast<embot::hw::SPI>(i); }
            return embot::hw::SPI::none;
        }
    };
    
    const BSP& getBSP();
                                            
}}} // namespace embot { namespace hw { namespace spi 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


