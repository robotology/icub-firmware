
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
#include "embot_hw_gpio.h"
#include "embot_hw_button.h"
#include "embot_hw_can.h"
#include "embot_hw_flash.h"
#include "embot_hw_pga308.h"
#include "embot_hw_timer.h"
#include "embot_hw_adc.h"
#include "embot_hw_si7051.h"

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


  

namespace embot { namespace hw { namespace bsp { namespace gpio {
        
    struct MAP
    {   // so far we simplify and say that alla gpio are supported (as long as they are not {nullptr, 0]
        std::uint32_t dummy;       
    public:                  
        bool supported(const embot::hw::GPIO &h) const;
        bool isvalid(const embot::hw::GPIO &h) const;   
    }; 
    
    
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace gpio {


namespace embot { namespace hw { namespace bsp { namespace led {
        
    struct MAP
    {
        std::uint32_t               mask;
        embot::hw::gpio::State      on;
        embot::hw::gpio::State      off;
        const embot::hw::GPIO       gpio[static_cast<std::uint8_t>(embot::hw::LED::maxnumberof)];
    public:                  
        bool supported(embot::hw::LED h) const;
        bool isvalid(embot::hw::LED h) const;
        const embot::hw::GPIO * getgpio(embot::hw::LED h) const;
    public:
        static std::uint8_t toindex(embot::hw::LED h);      
    }; 
    
    
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace led {


namespace embot { namespace hw { namespace bsp { namespace button {
        
    struct MAP
    {
        std::uint32_t               mask;
        embot::hw::gpio::State      pressed;
        embot::hw::GPIO             gpio[static_cast<std::uint8_t>(embot::hw::BTN::maxnumberof)];
       
    public:                  
        bool supported(embot::hw::BTN h) const;
        bool isvalid(embot::hw::BTN h) const;
        const embot::hw::GPIO * getgpio(embot::hw::BTN h) const;
    public:
        static std::uint8_t toindex(embot::hw::BTN h);      
    }; 
    
    
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace button {


namespace embot { namespace hw { namespace bsp { namespace can {
   

    struct STM32PROPS
    {
        void* handle;       // a CAN_HandleTypeDef* (they are the handles created by cube-mx) 
    };
    
    struct MAP
    {
        std::uint32_t mask;  
        const STM32PROPS * const stm32props[static_cast<std::uint8_t>(embot::hw::CAN::maxnumberof)];        
    public:     
        bool supported(embot::hw::CAN h) const;
        bool isvalid(embot::hw::CAN h) const;    
        const STM32PROPS * const getprops(embot::hw::CAN h) const;  
        void init(embot::hw::CAN h) const;    
        static std::uint8_t toindex(embot::hw::CAN h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace can {


namespace embot { namespace hw { namespace bsp { namespace flash {
     
    struct MAP
    {
        std::uint32_t mask;    
        embot::hw::flash::Partition partitions[static_cast<std::uint8_t>(embot::hw::FLASH::maxnumberof)];    
    public:     
        bool supported(embot::hw::FLASH h) const;
        bool isvalid(embot::hw::FLASH h) const; 
        const embot::hw::flash::Partition& getpartition(embot::hw::FLASH h) const;
        static std::uint8_t toindex(embot::hw::FLASH h);      
    }; 
    
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace flash {


namespace embot { namespace hw { namespace bsp { namespace pga308 {
        
    struct MAP
    {
        std::uint32_t mask;       
    public:     
        bool supported(embot::hw::PGA308 h) const;
        bool isvalid(embot::hw::PGA308 h) const;        
        static std::uint8_t toindex(embot::hw::PGA308 h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace pga308 {


namespace embot { namespace hw { namespace bsp { namespace si7051 {
        
    struct MAP
    {
        std::uint32_t mask;       
    public:     
        bool supported(embot::hw::SI7051 h) const;
        bool isvalid(embot::hw::SI7051 h) const;        
        static std::uint8_t toindex(embot::hw::SI7051 h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace si7051 {


namespace embot { namespace hw { namespace bsp { namespace onewire {
        
    struct MAP
    {
        std::uint32_t mask;       
    public:     
        bool supported(embot::hw::ONEWIRE h) const;
        bool isvalid(embot::hw::ONEWIRE h) const;        
        static std::uint8_t toindex(embot::hw::ONEWIRE h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace pga308 {


namespace embot { namespace hw { namespace bsp { namespace adc {

    struct STM32PROPS
    {
        void*               handle;     // a ADC_HandleTypeDef* (they are the handles created by cube-mx)
    };
    
    struct MAP
    {
        std::uint32_t mask;  
        const STM32PROPS * const stm32props[static_cast<std::uint8_t>(embot::hw::ADC::maxnumberof)];        
    public:     
        bool supported(embot::hw::ADC h) const;
        bool isvalid(embot::hw::ADC h) const;  
        const STM32PROPS * const getprops(embot::hw::ADC h) const;      
        void init(embot::hw::ADC h) const;    
        static std::uint8_t toindex(embot::hw::ADC h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace adc {


namespace embot { namespace hw { namespace bsp { namespace timer {
    
    struct STM32PROPS
    {
        void*               TIMx;       // a TIM_TypeDef* (they are TIM1, TIM2, .. TIM16)
        void*               handle;     // a TIM_HandleTypeDef* (they are the handles created by cube-mx)
        embot::hw::CLOCK    clock;      // the clock used by the timer      
        bool                isonepulse; // others
        bool                mastermode; // others        
    };
    
    struct MAP
    {
        std::uint32_t mask; 
        const STM32PROPS * const stm32props[static_cast<std::uint8_t>(embot::hw::TIMER::maxnumberof)];
    public:     
        bool supported(embot::hw::TIMER h) const;
        bool isvalid(embot::hw::TIMER h) const;  
        const STM32PROPS * const getprops(embot::hw::TIMER h) const;    
        static std::uint8_t toindex(embot::hw::TIMER h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace timer {


namespace embot { namespace hw { namespace bsp { namespace i2c {
    
    struct STM32PROPS
    {
        void* handle;       // a I2C_HandleTypeDef* (they are the handles created by cube-mx) 
        void* handledmatx;  // a DMA_HandleTypeDef*
        void* handledmarx;  // a DMA_HandleTypeDef*
    };
        
    struct MAP
    {
        std::uint32_t mask;     
        const STM32PROPS * const stm32props[static_cast<std::uint8_t>(embot::hw::I2C::maxnumberof)];        
    public:     
        bool supported(embot::hw::I2C h) const;
        bool isvalid(embot::hw::I2C h) const;        
        const STM32PROPS * const getprops(embot::hw::I2C h) const;
        void init(embot::hw::I2C h) const;
        static std::uint8_t toindex(embot::hw::I2C h);      
    }; 
        
    const MAP * const getMAP();
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace i2c {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


