
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ENCODER_BSP_H_
#define __EMBOT_HW_ENCODER_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_gpio_bsp.h"
#include "embot_hw_spi.h"
#include "embot_hw_flash.h"
#include "embot_hw_chip_AS5045.h"
#include "embot_hw_chip_MA730.h"

namespace embot { namespace hw { namespace encoder {
    
    enum class Type { none, chipAS5045, chipMA730, encoderEMULATED, encoderTEST}; //, spiAS5045 };

    struct nonePROP
    {
        uint32_t none {0};
        constexpr nonePROP() = default;           
    };
    
//    struct spiPROP
//    {
//        embot::hw::SPI bus {embot::hw::SPI::none};
//        embot::hw::GPIO hold {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};
//        embot::hw::GPIO sel {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};   
//        embot::hw::GPIO wp {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};   
//        constexpr spiPROP(const embot::hw::GPIO &h,  const embot::hw::GPIO &s, const embot::hw::GPIO &w, embot::hw::SPI b) : bus(b), hold(h), sel(s), wp(w) {}; 
//        constexpr spiPROP() = default;           
//    };
    
    
//    union multiPROP
//    {
//        uint32_t none {0};
//        embot::hw::chip::AS5045::Config chipAS5045cfg;
////        spiPROP spi;
//        constexpr multiPROP() : none(0) {}; 
//        constexpr multiPROP(const embot::hw::chip::AS5045::Config &cc) : chipAS5045cfg(cc) {};
////        constexpr multiPROP(const spiPROP &sp) : spi(sp) {};
//    };
    
    struct PROP
    {   // this one is ok for a spi encoder.
        embot::hw::encoder::Type type {embot::hw::encoder::Type::none};
        embot::hw::SPI spiBus {embot::hw::SPI::none};
//        multiPROP multi {};
        constexpr PROP() = default;
        constexpr PROP(embot::hw::encoder::Type t, const embot::hw::SPI &b)
            : type(t), spiBus(b) {}
        
    };
    
//    struct PROP
//    {   // this one is ok for a spi eeprom. in case of emulated eeprom or i2c based eeprom .... add ctors and fields 
//        embot::hw::eeprom::Type type {embot::hw::eeprom::Type::spiM95512};
//        embot::hw::SPI spi {embot::hw::SPI::none};
//        embot::hw::GPIO enable {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};
//        constexpr PROP() = default;
//        constexpr PROP(embot::hw::eeprom::Type t, embot::hw::SPI s, const embot::hw::GPIO &e) : type(t), spi(s), enable(e) {};
//    };
    
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


