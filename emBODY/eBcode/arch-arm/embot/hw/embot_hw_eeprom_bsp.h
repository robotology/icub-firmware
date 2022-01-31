
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_EEPROM_BSP_H_
#define __EMBOT_HW_EEPROM_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_gpio_bsp.h"
#include "embot_hw_spi.h"
#include "embot_hw_flash.h"
#include "embot_hw_chip_M95512DF.h"

namespace embot { namespace hw { namespace eeprom {
    
    enum class Type { none, chipM95512DF, flashEMULATED, heapTEST}; //, spiM95512DF };

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

    
    struct flashPROP
    {
        embot::hw::Partition partition {};   
        constexpr flashPROP(const embot::hw::Partition &p) : partition(p) {}; 
        constexpr flashPROP() = default;             
    };

    struct heapPROP
    {
        size_t size {32};   
        constexpr heapPROP(size_t s) : size(s) {}; 
        constexpr heapPROP() = default;            
    };
    
    union multiPROP
    {
        uint32_t none {0};
        embot::hw::chip::M95512DF::Config chipM95512DFcfg;
        flashPROP flash;    
        heapPROP heap;
//        spiPROP spi;
        constexpr multiPROP() : none(0) {}; 
        constexpr multiPROP(const embot::hw::chip::M95512DF::Config &cc) : chipM95512DFcfg(cc) {};
        constexpr multiPROP(const flashPROP &fl) : flash(fl) {}; 
        constexpr multiPROP(const heapPROP &hp) : heap(hp) {};
//        constexpr multiPROP(const spiPROP &sp) : spi(sp) {};
    };
    
    struct PROP
    {   // this one is ok for a spi eeprom. in case of emulated eeprom or i2c based eeprom .... add ctors and fields 
        embot::hw::eeprom::Type type {embot::hw::eeprom::Type::none};
        multiPROP multi {};
        constexpr PROP() = default;
        constexpr PROP(embot::hw::eeprom::Type t, const embot::hw::chip::M95512DF::Config &cc)
            : type(Type::chipM95512DF), multi(cc) {}
//        constexpr PROP(embot::hw::eeprom::Type t, const embot::hw::GPIO &h,  const embot::hw::GPIO &s, const embot::hw::GPIO &w, embot::hw::SPI b) 
//            : type(Type::spiM95512DF), multi({h, s, w, b}) {};
        constexpr PROP(embot::hw::eeprom::Type t, const embot::hw::Partition &pa) 
            : type(Type::flashEMULATED), multi(pa) {};
        constexpr PROP(embot::hw::eeprom::Type t, size_t s) 
            : type(Type::heapTEST), multi(s) {};
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
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::EEPROM::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::EEPROM h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::EEPROM h) const;
    };
    
    const BSP& getBSP();
        
}}} // namespace embot { namespace hw { namespace eeprom { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


