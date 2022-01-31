
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
    
    constexpr uint32_t mode2stm32clkpolarity(const Mode m)
    {
        if(Mode::none == m) { return SPI_POLARITY_LOW; }
        bool polhigh = embot::core::binary::bit::check(embot::core::tointegral(m), 1);
        return (true == polhigh) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
    }
    
    constexpr uint32_t mode2stm32clkphase(const Mode m)
    {
        if(Mode::none == m) { return SPI_PHASE_1EDGE; }
        bool phaedge2 = embot::core::binary::bit::check(embot::core::tointegral(m), 0);
        return (true == phaedge2) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;;
    }
    
    constexpr uint32_t datasize2stm32(DataSize d)
    {
        return embot::core::tointegral(d);
    }
    
    constexpr uint32_t prescaler2stm32baudrateprescaler(Prescaler p)
    {
        return static_cast<uint32_t>(p) << 28;
    }
    
#else
    using SPI_Handle = void;
#endif
    
    struct PROP
    {     
        SPI_Handle* handle {nullptr}; 
        uint32_t clockrate {0};
        constexpr PROP() = default;
        constexpr PROP(SPI_Handle *h, uint32_t c) : handle(h), clockrate(c) {}
        constexpr PROP(SPI_Handle *h) : handle(h) {} 
        constexpr Speed prescalertospeed(Prescaler p) const { return clockrate >> (1+embot::core::tointegral(p)); } 
        constexpr Prescaler speedtoprescaler(Speed s) const
        { 
            if(s > clockrate/2) return Prescaler::none; 
            for(uint8_t i=0; i<8; i++) { if((clockrate >> (i+1))<=s) return static_cast<Prescaler>(i); }
            return Prescaler::none;
        }; 
    };
       
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::SPI::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::SPI h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        bool init(embot::hw::SPI h, const Config &config) const;
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


