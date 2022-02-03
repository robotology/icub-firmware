
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
#include "embot_hw_gpio_bsp.h"

namespace embot { namespace hw { namespace spi { namespace bsp {
    
#if   defined(HAL_SPI_MODULE_ENABLED)        
    using SPI_Handle = SPI_HandleTypeDef;
    using SPI_Device = SPI_TypeDef;
#else
    using SPI_Handle = void;
#endif
        
    struct PROP
    {
        SPI_Handle* handle {nullptr}; 
        uint32_t clockrate {0};
        std::array<embot::hw::GPIO, SignalsNumberOf> pinout { {} };
        constexpr PROP() = default;
        constexpr PROP(SPI_Handle *h, uint32_t c, std::array<embot::hw::GPIO, SignalsNumberOf> p) : handle(h), clockrate(c), pinout(p) {}
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
        std::array<const PROP*, maxnumberof> properties {}; 
        
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
               
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
    
    // sadly I cannot put a SPI_TypeDef* inside a constexpr data staructure, so i need this funtion to get SPI1 / SPI2 etc
    SPI_Device* getDEVICE(embot::hw::SPI h);
        
namespace utils {
        
    struct ExtendedConfig
    {
        std::array<embot::hw::gpio::Pull, SignalsNumberOf> _pulls { embot::hw::gpio::Pull::none, embot::hw::gpio::Pull::none, embot::hw::gpio::Pull::none };
        std::array<embot::hw::GPIO, SignalsNumberOf> _pinout {};
        ExtendedConfig() = default;
        
        void load(embot::hw::SPI h, const Config &cfg)
        {
            _pulls = cfg.gpiospecials.pulls;   
            _pinout = embot::hw::spi::bsp::getBSP().getPROP(h)->pinout;           
        }
        
        void clear() 
        {
            for(auto &v : _pulls) v = embot::hw::gpio::Pull::none;
            for(auto &g : _pinout) g = {};
        }
        
        uint32_t pull(const embot::hw::spi::Signal s, const uint32_t defvalue = 0) const
        {
            if(embot::core::tointegral(s) >= SignalsNumberOf)
            {
                return defvalue;
            }                   
            embot::hw::gpio::Pull pu = _pulls[embot::core::tointegral(s)];
            return (embot::hw::gpio::Pull::none != pu) ? embot::core::tointegral(pu) : defvalue;
        }
        
        uint32_t pin(const embot::hw::spi::Signal s, const uint32_t defvalue = 0) const
        {
            if(embot::core::tointegral(s) >= SignalsNumberOf)
            {
                return defvalue;
            }              
            embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(_pinout[embot::core::tointegral(s)]);
            return (gg.isvalid()) ? gg.stmpin : defvalue;
        }

        embot::hw::gpio::GPIO_t* port(const embot::hw::spi::Signal s, embot::hw::gpio::GPIO_t* defvalue = nullptr) const
        {
            if(embot::core::tointegral(s) >= SignalsNumberOf)
            {
                return defvalue;
            }              
            embot::hw::gpio::PROP gg = embot::hw::gpio::getBSP().getPROP(_pinout[embot::core::tointegral(s)]);
            return (gg.isvalid()) ? gg.stmport : defvalue;
        } 
        
        embot::hw::GPIO gpio(const embot::hw::spi::Signal s) const
        {
            if(embot::core::tointegral(s) >= SignalsNumberOf)
            {
                return {};
            }  
            return _pinout[embot::core::tointegral(s)];
        }            
    }; 

#if defined(HAL_SPI_MODULE_ENABLED)    

namespace stm32 {  
    
    constexpr uint32_t toCLKpolarity(const Mode m)
    {
//        if(Mode::none == m) { return SPI_POLARITY_LOW; }
        bool polhigh = embot::core::binary::bit::check(embot::core::tointegral(m), 1);
        return (true == polhigh) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
    }
    
    constexpr uint32_t toCLKphase(const Mode m)
    {
//        if(Mode::none == m) { return SPI_PHASE_1EDGE; }
        bool phaedge2 = embot::core::binary::bit::check(embot::core::tointegral(m), 0);
        return (true == phaedge2) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;;
    }
    
    constexpr uint32_t todatasize(DataSize d)
    {
        return embot::core::tointegral(d);
    }
    
    constexpr uint32_t tobaudrateprescaler(Prescaler p)
    {
        return static_cast<uint32_t>(p) << 28;
    }    
} // namespace stm32 {  

#endif // #if defined(HAL_SPI_MODULE_ENABLED)     
    
} // namespace utils {    

    
}}}} //namespace embot { namespace hw { namespace spi { namespace bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


