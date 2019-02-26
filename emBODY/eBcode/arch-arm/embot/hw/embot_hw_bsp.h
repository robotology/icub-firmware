
/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_H_
#define _EMBOT_HW_BSP_H_

#include "embot_common.h"
#include "embot_hw.h"
#include "embot_binary.h"
#include "embot_hw_gpio.h"
#include "embot_hw_button.h"
#include "embot_hw_can.h"
#include "embot_hw_flash.h"
#include "embot_hw_pga308.h"
#include "embot_hw_timer.h"
#include "embot_hw_adc.h"
#include "embot_hw_si7051.h"
#include <array>

#include "stm32hal.h"

namespace embot { namespace hw { namespace bsp {
        
    struct stm32halConfig
    {   // what the the stm32 hal requires to operate it internals. if nullptr, the stm32hal uses fake implemention.
        embot::common::fpWorker     init1millitick;    
        embot::common::fpGetU32     get1millitick;  
        stm32halConfig() : init1millitick(nullptr), get1millitick(nullptr) {}  
        stm32halConfig(embot::common::fpWorker ini, embot::common::fpGetU32 get) : init1millitick(ini), get1millitick(get) {}             
    };
    
    struct Config
    {
        stm32halConfig              stm32hal; 
        embot::common::fpGetU64     get1microtime;  // in usec. it is used as extra precision
        Config() : get1microtime(nullptr) {}
        Config(const stm32halConfig &_stm32, embot::common::fpGetU64 _tmicro) : stm32hal(_stm32), get1microtime(_tmicro) {}
    }; 
    
        
    bool initialised();
    
    result_t init(const Config &config);
    
    // it returns time in microseconds. the precision is usec or ms and depends on how the bsp was configured
    embot::common::Time now();
              
}}} // namespace embot { namespace hw { namespace bsp {


namespace embot { namespace hw { namespace bsp {
    
    struct SUPP
    {
        std::uint32_t       supportedmask;
        
        constexpr SUPP(std::uint32_t m) : supportedmask(m) {}      
            
        template <typename T>
        constexpr bool supported(T v) const
        {
            return embot::binary::bit::check(supportedmask, embot::common::tointegral(v));
        }        
    };

}}} // namespace embot { namespace hw { namespace bsp {
  

namespace embot { namespace hw { namespace bsp { namespace gpio {
    
    struct PROP
    {
        GPIO_TypeDef* stmport;
        std::uint16_t stmpin;        
        constexpr PROP() : stmport(nullptr), stmpin(0) {}
        constexpr PROP(GPIO_TypeDef* po, std::uint16_t pi) : stmport(po), stmpin(pi) {}
        constexpr bool isvalid() const { return (nullptr == stmport) ? false : true; }
    };
    
    struct BSP
    {
        constexpr static std::uint8_t maxnumberofPORTs = embot::common::tointegral(embot::hw::GPIO::PORT::maxnumberof);
        constexpr BSP(std::array<std::uint16_t, maxnumberofPORTs> msk, std::array<GPIO_TypeDef*, maxnumberofPORTs> po) : supportmask2d(msk), ports(po) {}
        constexpr BSP() : supportmask2d({0}), ports({0}) {}            
          
        std::array<std::uint16_t, maxnumberofPORTs> supportmask2d;
        std::array<GPIO_TypeDef*, maxnumberofPORTs> ports;  

        constexpr bool supported(embot::hw::GPIO h) const
        { 
            if(embot::common::tointegral(h.port) >= maxnumberofPORTs) 
            {
                return false; 
            }
            if(embot::common::tointegral(h.pin) >= embot::common::tointegral(embot::hw::GPIO::PIN::maxnumberof)) 
            {
                return false; 
            }
            return embot::binary::bit::check(supportmask2d[embot::common::tointegral(h.port)], embot::common::tointegral(h.pin));
        }
        
        constexpr PROP getPROP(embot::hw::GPIO h) const 
        { 
            PROP p{}; 
            if(supported(h)) { p.stmport = ports[embot::common::tointegral(h.port)]; p.stmpin = 1 << embot::common::tointegral(h.pin); }
            return p;
        }
                
        void init(embot::hw::GPIO h) const;
    };
    
    const BSP& getBSP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace gpio {


namespace embot { namespace hw { namespace bsp { namespace led {
    
    struct PROP
    { 
        embot::hw::gpio::State  on;
        embot::hw::gpio::State  off;
        embot::hw::GPIO         gpio;  
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::LED::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::LED h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::LED h) const;
    };
    
    const BSP& getBSP();
    
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace led {


namespace embot { namespace hw { namespace bsp { namespace button {
    
    struct PROP
    { 
        embot::hw::gpio::State  pressed;
        embot::hw::GPIO         gpio;
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::BTN::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::BTN h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::BTN h) const;
    };
    
    const BSP& getBSP();
                    
}}}} // namespace embot { namespace hw { namespace bsp {  namespace button {


namespace embot { namespace hw { namespace bsp { namespace can {
    
    
    struct PROP
    { 
        CAN_HandleTypeDef* handle;   
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::CAN::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::CAN h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::CAN h) const;
    };
    
    const BSP& getBSP();
   
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace can {


namespace embot { namespace hw { namespace bsp { namespace flash {


    struct PROP
    { 
        embot::hw::flash::Partition partition;   
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::FLASH::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::FLASH h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::FLASH h) const;
    };
    
    const BSP& getBSP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace flash {


namespace embot { namespace hw { namespace bsp { namespace pga308 {
        
    struct PROP
    {
        embot::hw::GPIO poweron;        
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::PGA308::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::PGA308 h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::PGA308 h) const;
    };
    
    const BSP& getBSP();
                 
}}}} // namespace embot { namespace hw { namespace bsp {  namespace pga308 {


namespace embot { namespace hw { namespace bsp { namespace si7051 {
       
    struct PROP
    {   
        std::uint8_t i2caddress;;  
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::SI7051::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::SI7051 h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::SI7051 h) const;
    };
    
    const BSP& getBSP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace si7051 {


namespace embot { namespace hw { namespace bsp { namespace onewire {

    struct PROP
    {   
        embot::hw::GPIO gpio;  
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::ONEWIRE::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}        
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::ONEWIRE h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::ONEWIRE h) const;
    };
    
    const BSP& getBSP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace onewire {


namespace embot { namespace hw { namespace bsp { namespace adc {
        
    struct PROP
    { 
        ADC_HandleTypeDef*  handle;
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::ADC::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::ADC h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::ADC h) const;
    };
    
    const BSP& getBSP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace adc {


namespace embot { namespace hw { namespace bsp { namespace timer {
    
    struct PROP
    {   
        TIM_TypeDef*        TIMx;
        TIM_HandleTypeDef*  handle;  
        embot::hw::CLOCK    clock;      // the clock used by the timer      
        bool                isonepulse; // others
        bool                mastermode; // others        
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::TIMER::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::TIMER h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::TIMER h) const;
    };
    
    const BSP& getBSP();
                  
}}}} // namespace embot { namespace hw { namespace bsp {  namespace timer {

namespace embot { namespace hw { namespace bsp { namespace i2c {
    
    
    struct PROP
    {   
        I2C_HandleTypeDef*  handle;  
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::I2C::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::I2C h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::I2C h) const;
    };
    
    const BSP& getBSP();
                  
}}}} // namespace embot { namespace hw { namespace bsp {  namespace i2c {


namespace embot { namespace hw { namespace bsp { namespace bno055 {
        
    struct PROP
    { 
        std::uint8_t i2caddress;
        embot::hw::GPIO boot;        
        embot::hw::GPIO reset;   
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::common::tointegral(embot::hw::BNO055::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::BNO055 h) const { return supported(h) ? properties[embot::common::tointegral(h)] : nullptr; }
        void init(embot::hw::BNO055 h) const;
    };
    
    const BSP& getBSP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace bno055 {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


