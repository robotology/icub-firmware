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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"
#include <cstring>
#include <vector>
#include <array>

#include "embot_hw_gpio.h"
#include "embot_hw_button.h"
#include "embot_hw_can.h"
#include "embot_hw_flash.h"
#include "embot_hw_timer.h"
#include "embot_hw_adc.h"
#include "embot_hw_si7051.h"
#include "embot_hw_tlv493d.h"
#include "embot_hw_sys.h"

#include "embot_core_binary.h"
#include "embot_core.h"


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and attached chips. it is done board by board
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------



// - support map: begin of embot::hw::gpio

#if   !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

#error CAVEAT: embot::hw requires GPIO. pls enable it!

namespace embot { namespace hw { namespace bsp { namespace gpio {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::GPIO h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else
    
namespace embot { namespace hw { namespace bsp { namespace gpio {
 
    #if defined(STM32HAL_BOARD_STM32G4EVAL)
    static const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, nullptr
        }}
    };    
    #else
    // sadly we cannot use constexpr because of the reinterpret_cast<> inside GPIOA etc.
    static const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, nullptr, nullptr, nullptr
        }}
    };
    #endif
    
    constexpr embot::hw::GPIO BSP::getGPIO(const PROP &p) const
    {
        embot::hw::GPIO gpio {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};
        switch(p.stmpin)
        {
            case GPIO_PIN_0:  { gpio.pin = embot::hw::GPIO::PIN::zero; } break;
            case GPIO_PIN_1:  { gpio.pin = embot::hw::GPIO::PIN::one; } break;
            case GPIO_PIN_2:  { gpio.pin = embot::hw::GPIO::PIN::two; } break;
            case GPIO_PIN_3:  { gpio.pin = embot::hw::GPIO::PIN::three; } break;
            case GPIO_PIN_4:  { gpio.pin = embot::hw::GPIO::PIN::four; } break;
            case GPIO_PIN_5:  { gpio.pin = embot::hw::GPIO::PIN::five; } break;
            case GPIO_PIN_6:  { gpio.pin = embot::hw::GPIO::PIN::six; } break;
            case GPIO_PIN_7:  { gpio.pin = embot::hw::GPIO::PIN::seven; } break;
            case GPIO_PIN_8:  { gpio.pin = embot::hw::GPIO::PIN::eight; } break;
            case GPIO_PIN_9:  { gpio.pin = embot::hw::GPIO::PIN::nine; } break;
            case GPIO_PIN_10: { gpio.pin = embot::hw::GPIO::PIN::ten; } break;
            case GPIO_PIN_11: { gpio.pin = embot::hw::GPIO::PIN::eleven; } break;
            case GPIO_PIN_12: { gpio.pin = embot::hw::GPIO::PIN::twelve; } break;
            case GPIO_PIN_13: { gpio.pin = embot::hw::GPIO::PIN::thirteen; } break;
            case GPIO_PIN_14: { gpio.pin = embot::hw::GPIO::PIN::fourteen; } break;
            case GPIO_PIN_15: { gpio.pin = embot::hw::GPIO::PIN::fifteen; } break;
            case GPIO_PIN_All: { gpio.pin = embot::hw::GPIO::PIN::none; } break;
            default: { gpio.pin = embot::hw::GPIO::PIN::none; } break;
        }
        
        if(nullptr != p.stmport)
        {
            for(int i=0; i<thebsp.ports.size(); i++)
            {
                if(thebsp.ports[i] == p.stmport) 
                {
                    gpio.port = static_cast<embot::hw::GPIO::PORT>(i);
                    break;
                }
            }
        }
        
        return gpio;
    }
    
    void BSP::init(embot::hw::GPIO h) const {}        
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace gpio {

#endif // gpio

// - support map: end of embot::hw::gpio



// - support map: begin of embot::hw::led

namespace embot { namespace hw { namespace bsp { namespace led {
           
    static_assert(embot::core::tointegral(embot::hw::LED::none) < 8*sizeof(SUPP::supportedmask), "LED::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::LED::maxnumberof) < 8*sizeof(SUPP::supportedmask), "LED::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::LED::maxnumberof) < embot::core::tointegral(embot::hw::LED::none), "LED::maxnumberof must be higher that LED::none, so that we can optimise code");

}}}}

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot { namespace hw { namespace bsp { namespace led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace led {         
    
    #if     defined(STM32HAL_BOARD_NUCLEO64)    
           
    constexpr PROP led1p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {}
   

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::nine}  };  
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::eight}  };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two),        
        // properties
        {{
            &led1p, &led2p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {}
    
    
    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::eleven}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {}    
    
    
    #elif   defined(STM32HAL_BOARD_RFE)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::zero}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {}   
    
    #elif   defined(STM32HAL_BOARD_PSC)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::seven}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 
    
    #elif   defined(STM32HAL_BOARD_SG3)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::seven}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 
    
    #elif   defined(STM32HAL_BOARD_NUCLEOH7)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::zero}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 
       
    #elif   defined(STM32HAL_BOARD_SG3)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::seven}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 
    
    #elif   defined(STM32HAL_BOARD_STM32G4EVAL)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::nine}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 

    #elif   defined(STM32HAL_BOARD_PMC)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::six}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one),        
        // properties
        {{
            &led1p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void BSP::init(embot::hw::LED h) const {} 
        
    #else
        #error embot::hw::bsp::led::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace led {

#endif // led

// - support map: end of embot::hw::led



// - support map: begin of embot::hw::button

namespace embot { namespace hw { namespace bsp { namespace button {
           
    static_assert(embot::core::tointegral(embot::hw::BTN::none) < 8*sizeof(SUPP::supportedmask), "BTN::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::BTN::maxnumberof) < 8*sizeof(SUPP::supportedmask), "BTN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::BTN::maxnumberof) < embot::core::tointegral(embot::hw::BTN::none), "BTN::maxnumberof must be higher that BTN::none, so that we can optimise code");

}}}}

#if !defined(EMBOT_ENABLE_hw_button)

namespace embot { namespace hw { namespace bsp { namespace button {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace button {
           
    #if     defined(STM32HAL_BOARD_NUCLEO64)    
        
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one),        
        // properties
        {{
            &btn1p            
        }}        
    };
    
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}
        

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}
    
    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}
    
    #elif   defined(STM32HAL_BOARD_RFE)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}
    
    #elif   defined(STM32HAL_BOARD_PSC)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}

    #elif   defined(STM32HAL_BOARD_SG3)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}

    #elif   defined(STM32HAL_BOARD_NUCLEOH7)
    
    // this button is the blue one on the board
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}, 
                             .pull = embot::hw::gpio::Pull::nopull, .irqn = EXTI15_10_IRQn  };  
    // this button is attached directly to PB8 w/out any filter. it is pressed when PB8 is connected to 3V3
    constexpr PROP btn2p = { .pressed = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::eight}, 
                             .pull = embot::hw::gpio::Pull::pulldown, .irqn = EXTI9_5_IRQn  };  
 
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one) | mask::pos2mask<uint32_t>(BTN::two),        
        // properties
        {{
            &btn1p, &btn2p            
        }}        
    };
    
    
    void BSP::init(embot::hw::BTN h) const {}
    
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const
    {
        const embot::hw::GPIO gpio = embot::hw::bsp::gpio::getBSP().getGPIO(p);
        switch(gpio.pin)
        {
            case embot::hw::GPIO::PIN::thirteen:
            {
                embot::hw::button::onexti(BTN::one);
            } break;
            
            case embot::hw::GPIO::PIN::eight:
            {
                embot::hw::button::onexti(BTN::two);
            } break;    

            default:
            {
            } break;           
        }              
    }
    
    // we put in here the IRQHandlers + the exti callback


    extern "C" {
    
        void EXTI9_5_IRQHandler(void)
        {
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
        }
        
        void EXTI15_10_IRQHandler(void)
        {
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
        }
        
        void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
        {
             embot::hw::bsp::button::getBSP().onEXTI({nullptr, GPIO_Pin});            
        }        
    }
   
    #elif   defined(STM32HAL_BOARD_STM32G4EVAL)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::bsp::gpio::PROP &p) const {}    
        
    #else
        #error embot::hw::bsp::button::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace button {


#endif // button

// - support map: end of embot::hw::button



// - support map: begin of embot::hw::can

namespace embot { namespace hw { namespace bsp { namespace can {
    static_assert(embot::core::tointegral(embot::hw::CAN::none) < 8*sizeof(SUPP::supportedmask), "CAN::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::CAN::maxnumberof) < 8*sizeof(SUPP::supportedmask), "CAN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::CAN::maxnumberof) < embot::core::tointegral(embot::hw::CAN::none), "CAN::maxnumberof must be higher that CAN::none, so that we can optimise code");
}}}}


#if   !defined(EMBOT_ENABLE_hw_can)

namespace embot { namespace hw { namespace bsp { namespace can {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::CAN h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace can {
               
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

    constexpr BSP thebsp {};    
    void BSP::init(embot::hw::CAN h) const {}
   
    #elif   defined(STM32HAL_BOARD_MTB4)
    
    constexpr PROP can1p = { .handle = &hcan1 };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p            
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }        
    }

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr PROP can1p = { .handle = &hcan1 };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p            
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }        
    }
    
    #elif   defined(STM32HAL_BOARD_RFE)
    
    constexpr PROP can1p = { .handle = &hcan1 };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p            
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }        
    }

    #elif   defined(STM32HAL_BOARD_PSC)
    
    constexpr PROP can1p = { .handle = &hcan1 };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p            
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }        
    }
    
    #elif   defined(STM32HAL_BOARD_SG3)
    
    constexpr PROP can1p = { .handle = &hcan1 };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p            
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }        
    }

    #elif   defined(STM32HAL_BOARD_STM32G4EVAL)
    
    // it has HAL_FDCAN_MODULE_ENABLED
    
    constexpr PROP can1p = { .handle = &hfdcan1 }; 
    constexpr PROP can2p = { .handle = &hfdcan2 };    
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one) | mask::pos2mask<uint32_t>(CAN::two),        
        // properties
        {{
            &can1p, &can2p           
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            MX_FDCAN1_Init();
        }
        else if(h == CAN::two)
        {            
            MX_FDCAN2_Init();
        }
    }
    
    #else
        #error embot::hw::bsp::can::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace can {
    
#if defined(HAL_CAN_MODULE_ENABLED)

// irq handlers for can: they are common to every board which has can

void CAN1_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

#elif defined(HAL_FDCAN_MODULE_ENABLED)


#warning -> fill handlers for fdcan
        
void FDCAN1_IT0_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan1);
}  

void FDCAN1_IT1_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan1);
}   

#endif //

#endif // EMBOT_ENABLE_hw_can


// - support map: end of embot::hw::can



// - support map: begin of embot::hw::flash

namespace embot { namespace hw { namespace bsp { namespace flash {
 
    static_assert(embot::core::tointegral(embot::hw::FLASH::none) < 8*sizeof(SUPP::supportedmask), "FLASH::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::FLASH::maxnumberof) < 8*sizeof(SUPP::supportedmask), "FLASH::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::FLASH::maxnumberof) < embot::core::tointegral(embot::hw::FLASH::none), "FLASH::maxnumberof must be higher that FLASH::none, so that we can optimise code");

}}}}

#if !defined(EMBOT_ENABLE_hw_flash)

namespace embot { namespace hw { namespace bsp { namespace flash {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::FLASH h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace flash {
 
    #if defined(EMBOT_APPL_ZEROOFFSET)
    
        // used for special debug. in this case we use the whole 256k of flash even if there is more
        #if defined(STM32HAL_BOARD_MTB4) || defined(STM32HAL_BOARD_STRAIN2) || defined(STM32HAL_BOARD_RFE) || defined(STM32HAL_BOARD_PSC) || defined(STM32HAL_BOARD_SG3) || defined(STM32HAL_BOARD_NUCLEOH7)
    
            #warning CAVEAT: EMBOT_APPL_ZEROOFFSET is defined. use a proper scatter file to place the application in the correct place

            constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
            constexpr PROP bootloader           {{0x08000000,               (250)*1024,         2*1024}};   // bootloader; we dont use it ... but ok, give it same space as appl
            constexpr PROP sharedstorage        {{0x08000000+(250*1024),    (2)*1024,           2*1024}};   // sharedstorage: on top of a 2K-smaller application
            constexpr PROP application          {{0x08000000,               (250)*1024,         2*1024}};   // application @ 000k, 250k big
            constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application    
        
        #else
            #error this board does not support macro EMBOT_APPL_ZEROOFFSET yet
        #endif
    
    #elif   defined(STM32HAL_BOARD_NUCLEO64) 

        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application
   
    #elif   defined(STM32HAL_BOARD_MTB4)
           
        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application


    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
        #if defined(STRAIN2_APP_AT_128K)
        
        // strain legacy: application @ 128k and application storage together with sharedstorage
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP applicationstorage   {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // applicationstorage: together with sharedstorage
        constexpr PROP application          {{0x08000000+(128*1024),    (128)*1024,         2*1024}};   // application @ 128k
        
        #else 
        
        // strain2: application @ 080k
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (78)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(78*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(80*1024),     (172)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application    
        
        #endif
        

    #elif   defined(STM32HAL_BOARD_RFE)
            
        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application
            
    #elif   defined(STM32HAL_BOARD_PSC)
               
        // psc: application @ 080k
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (78)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(78*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(80*1024),     (172)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application   
                  
   #elif   defined(STM32HAL_BOARD_SG3)
           
        // psc: application @ 080k
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (78)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(78*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(80*1024),     (172)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            
       
    #elif   defined(STM32HAL_BOARD_NUCLEOH7)          
        #error pls define    

   #elif   defined(STM32HAL_BOARD_STM32G4EVAL )

// acemor          
        // application @ 128k
        constexpr PROP whole                {{0x08000000,               (512)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (126)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(126*1024),    (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (256+124)*1024,     2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(508*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            
 
#if 0     
        constexpr PROP whole                {{0x08000000,               (512)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000+(256+2)*1014,               (128)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(256*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(0*1024),     (252)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            
#endif
        
    #else
        #error embot::hw::bsp::flash::thebsp must be defined    
    #endif   


    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
        mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),        
        // properties
        {{
            &whole, &bootloader, &application, &sharedstorage, &applicationstorage            
        }}        
    };
    
    void BSP::init(embot::hw::FLASH h) const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace flash {

#endif // flash

// - support map: end of embot::hw::flash



// - support map: begin of embot::hw::pga308

namespace embot { namespace hw { namespace bsp { namespace pga308 {
           
    static_assert(embot::core::tointegral(embot::hw::PGA308::none) < 8*sizeof(SUPP::supportedmask), "PGA308::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::PGA308::maxnumberof) < 8*sizeof(SUPP::supportedmask), "PGA308::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::PGA308::maxnumberof) < embot::core::tointegral(embot::hw::PGA308::none), "PGA308::maxnumberof must be higher that PGA308::none, so that we can optimise code");

}}}}


#if !defined(EMBOT_ENABLE_hw_pga308)

namespace embot { namespace hw { namespace bsp { namespace pga308 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::PGA308 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

#include "embot_hw_pga308.h"

namespace embot { namespace hw { namespace bsp { namespace pga308 {
           

    #if defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr PROP prop { .poweron = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fifteen} }; // EN_2V8_GPIO_Port, EN_2V8_Pin

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(PGA308::one) | mask::pos2mask<uint32_t>(PGA308::two) | mask::pos2mask<uint32_t>(PGA308::three) |
        mask::pos2mask<uint32_t>(PGA308::four) | mask::pos2mask<uint32_t>(PGA308::five) | mask::pos2mask<uint32_t>(PGA308::six),        
        // properties
        {{
            &prop, &prop, &prop, &prop, &prop, &prop            
        }}        
    };

    void BSP::init(embot::hw::PGA308 h) const {}
        
    #else
        #error embot::hw::bsp::pga308::thebsp must be defined    
    #endif

    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace pga308 {

#endif 


// - support map: end of embot::hw::pga308



// - support map: begin of embot::hw::si7051

namespace embot { namespace hw { namespace bsp { namespace si7051 {
           
    static_assert(embot::core::tointegral(embot::hw::SI7051::none) < 8*sizeof(SUPP::supportedmask), "SI7051::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::SI7051::maxnumberof) < 8*sizeof(SUPP::supportedmask), "SI7051::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::SI7051::maxnumberof) < embot::core::tointegral(embot::hw::SI7051::none), "SI7051::maxnumberof must be higher that SI7051::none, so that we can optimise code");

}}}}

#if !defined(EMBOT_ENABLE_hw_si7051)

namespace embot { namespace hw { namespace bsp { namespace si7051 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::SI7051 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace si7051 {
           

    #if defined(STM32HAL_BOARD_STRAIN2)
        
    // actually the strain2 has another chip mounted, which is in the same i2c bus as the bno.
    // for now we don't support it
    constexpr PROP prop01 { .i2caddress = 0x80 }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(SI7051::one),        
        // properties
        {{
            &prop01             
        }}        
    };

    void BSP::init(embot::hw::SI7051 h) const { }

    #elif defined(STM32HAL_BOARD_MTB4)
        
    // the mtb4 has only one si7051 chip
    constexpr PROP prop01 { .i2caddress = 0x80 }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(SI7051::one),        
        // properties
        {{
            &prop01             
        }}        
    };

    void BSP::init(embot::hw::SI7051 h) const 
    {
    	// the mtb4 board has a pin for power up of the chip SI7051::one 
        HAL_GPIO_WritePin(POWER_TSENSOR1_GPIO_Port, POWER_TSENSOR1_Pin, GPIO_PIN_SET);
        embot::hw::sys::delay(25*embot::core::time1millisec);
    }
    
    #else
        #error embot::hw::bsp::si7051::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace si7051 {


#endif // si7051


// - support map: end of embot::hw::si7051



// - support map: begin of embot::hw::onewire

namespace embot { namespace hw { namespace bsp { namespace onewire {
           
    static_assert(embot::core::tointegral(embot::hw::ONEWIRE::none) < 8*sizeof(SUPP::supportedmask), "ONEWIRE::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::ONEWIRE::maxnumberof) < 8*sizeof(SUPP::supportedmask), "ONEWIRE::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::ONEWIRE::maxnumberof) < embot::core::tointegral(embot::hw::ONEWIRE::none), "ONEWIRE::maxnumberof must be higher that ONEWIRE::none, so that we can optimise code");

}}}}

#if !defined(EMBOT_ENABLE_hw_onewire)

namespace embot { namespace hw { namespace bsp { namespace onewire {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ONEWIRE h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace onewire {  

    #if defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr PROP onewi1 { .gpio = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::four} };  // W_STRAIN1_GPIO_Port, W_STRAIN1_Pin
    constexpr PROP onewi2 { .gpio = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::five} };  // W_STRAIN2_GPIO_Port, W_STRAIN2_Pin
    constexpr PROP onewi3 { .gpio = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::six} };   // W_STRAIN3_GPIO_Port, W_STRAIN3_Pin
    constexpr PROP onewi4 { .gpio = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::seven} }; // W_STRAIN4_GPIO_Port, W_STRAIN4_Pin
    constexpr PROP onewi5 { .gpio = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::eight} }; // W_STRAIN5_GPIO_Port, W_STRAIN5_Pin
    constexpr PROP onewi6 { .gpio = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::nine} };  // W_STRAIN6_GPIO_Port, W_STRAIN6_Pin
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ONEWIRE::one) | mask::pos2mask<uint32_t>(ONEWIRE::two) | mask::pos2mask<uint32_t>(ONEWIRE::three) |
        mask::pos2mask<uint32_t>(ONEWIRE::four) | mask::pos2mask<uint32_t>(ONEWIRE::five) | mask::pos2mask<uint32_t>(ONEWIRE::six),       
        // properties
        {{
            &onewi1, &onewi2, &onewi3, &onewi4, &onewi5, &onewi6
        }}        
    };

    void BSP::init(embot::hw::ONEWIRE h) const {}
                
    #else
        #error embot::hw::bsp::onewire::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}} // namespace embot { namespace hw { namespace bsp {  namespace onewire {

#endif

// - support map: end of embot::hw::onewire



// - support map: begin of embot::hw::adc

namespace embot { namespace hw { namespace bsp { namespace adc {
           
    static_assert(embot::core::tointegral(embot::hw::ADC::none) < 8*sizeof(SUPP::supportedmask), "ADC::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::ADC::maxnumberof) < 8*sizeof(SUPP::supportedmask), "ADC::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::ADC::maxnumberof) < embot::core::tointegral(embot::hw::ADC::none), "ADC::maxnumberof must be higher that ADC::none, so that we can optimise code");

}}}}


#if   !defined(HAL_ADC_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_adc)

namespace embot { namespace hw { namespace bsp { namespace adc {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ADC h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace adc {
           
    #if defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr PROP adc1p { .handle = &hadc1 }; 
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ADC::one),        
        // properties
        {{
            &adc1p             
        }}        
    };

    void BSP::init(embot::hw::ADC h) const
    {
        // init peripherals: adc1 and dma1
        MX_ADC1_Init();
        // dma is globally initted by stm32hal_bsp_init() because it holds all dma peripherals
        //MX_DMA_Init();     
    }
    
    #elif defined(STM32HAL_BOARD_SG3)
    
    constexpr PROP adc1p { .handle = &hadc1 }; 
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ADC::one),        
        // properties
        {{
            &adc1p             
        }}        
    };

    void BSP::init(embot::hw::ADC h) const
    {
        // init peripherals: adc1 and dma1
        MX_ADC1_Init();
        // dma is globally initted by stm32hal_bsp_init() because it holds all dma peripherals
        //MX_DMA_Init();     
    }
    
    #else
        #error embot::hw::bsp::adc::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace adc {



#if defined(STM32HAL_BOARD_STRAIN2)

void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

// this IRQHandler is not used if we have DMA ....
//void ADC1_IRQHandler(void)
//{
//    HAL_ADC_IRQHandler(&hadc1);
//}

#endif


#endif // adc

// - support map: end of embot::hw::adc



// - support map: begin of embot::hw::timer

namespace embot { namespace hw { namespace bsp { namespace timer {
               
    static_assert(embot::core::tointegral(embot::hw::TIMER::none) < 8*sizeof(SUPP::supportedmask), "TIMER::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::TIMER::maxnumberof) < 8*sizeof(SUPP::supportedmask), "TIMER::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::TIMER::maxnumberof) < embot::core::tointegral(embot::hw::TIMER::none), "TIMER::maxnumberof must be higher that CAN::none, so that we can optimise code");

}}}}


#if   !defined(HAL_TIM_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_timer)

namespace embot { namespace hw { namespace bsp { namespace timer {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TIMER h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace timer {
                   
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

    constexpr BSP thebsp { };    
    void BSP::init(embot::hw::TIMER h) const {}
   
    #elif   defined(STM32HAL_BOARD_MTB4)
       
    // sadly we cannot use constexpr because of the reinterpret_cast<> inside TIM6 etc.
    static const PROP tim06p = { .TIMx = TIM6,  .handle = &htim6,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };   
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::six),        
        // properties
        {{
            nullptr, nullptr, nullptr, nullptr, nullptr, &tim06p, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16            
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}

    #elif   defined(STM32HAL_BOARD_STRAIN2)    

    // sadly we cannot use constexpr because of the reinterpret_cast<> inside TIM6 etc.
    static const PROP tim06p = { .TIMx = TIM6,  .handle = &htim6,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
    static const PROP tim07p = { .TIMx = TIM7,  .handle = &htim7,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
    static const PROP tim15p = { .TIMx = TIM15, .handle = &htim15, .clock = embot::hw::CLOCK::syscore, .isonepulse = true,  .mastermode = false };
    static const PROP tim16p = { .TIMx = TIM16, .handle = &htim16, .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = false };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::six) | mask::pos2mask<uint32_t>(TIMER::seven) | 
        mask::pos2mask<uint32_t>(TIMER::fifteen) | mask::pos2mask<uint32_t>(TIMER::sixteen),        
        // properties
        {{
            nullptr, nullptr, nullptr, nullptr, nullptr, &tim06p, &tim07p, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &tim15p, &tim16p      // from 9 to 16             
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}

    #elif   defined(STM32HAL_BOARD_RFE)
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TIMER h) const {} 

    #elif   defined(STM32HAL_BOARD_PSC)    

    // sadly we cannot use constexpr because of the reinterpret_cast<> inside TIM6 etc.
    static const PROP tim06p = { .TIMx = TIM6,  .handle = &htim6,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
    static const PROP tim07p = { .TIMx = TIM7,  .handle = &htim7,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::six) | mask::pos2mask<uint32_t>(TIMER::seven),        
        // properties
        {{
            nullptr, nullptr, nullptr, nullptr, nullptr, &tim06p, &tim07p, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16             
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}

    #elif   defined(STM32HAL_BOARD_SG3)    

    // sadly we cannot use constexpr because of the reinterpret_cast<> inside TIM6 etc.
    static const PROP tim06p = { .TIMx = TIM6,  .handle = &htim6,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
    static const PROP tim07p = { .TIMx = TIM7,  .handle = &htim7,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::six) | mask::pos2mask<uint32_t>(TIMER::seven),        
        // properties
        {{
            nullptr, nullptr, nullptr, nullptr, nullptr, &tim06p, &tim07p, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16             
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}

    #else
        #error embot::hw::bsp::timer::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace timer {

#if defined(STM32HAL_BOARD_STRAIN2) 
    #define STM32HAL_HAS_TIM6
    #define STM32HAL_HAS_TIM7
    #define STM32HAL_HAS_TIM15
    #define STM32HAL_HAS_TIM16
#elif defined(STM32HAL_BOARD_MTB4)
    #define STM32HAL_HAS_TIM6
#elif defined(STM32HAL_BOARD_PSC)
    #define STM32HAL_HAS_TIM6
    #define STM32HAL_HAS_TIM7
#endif

#define USE_QUICKER_MODE

#if defined(USE_QUICKER_MODE)

// in here it is implemented in the way the good old hal2 was doing: the handler directly manages the callback
// instead the stm hal make a lot of calls before actually calling the callback code, hence it is slower.

void manageInterrupt(embot::hw::TIMER t, TIM_HandleTypeDef *htim)
{
    if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
    {
        if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) !=RESET)
        {
            __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
            embot::hw::timer::execute(t);
        }
    }   
} 


#if defined(STM32HAL_HAS_TIM6)
void TIM6_DAC_IRQHandler(void)
{
    manageInterrupt(embot::hw::TIMER::six, &htim6);
}
#endif

#if defined(STM32HAL_HAS_TIM7)
void TIM7_IRQHandler(void)
{
    manageInterrupt(embot::hw::TIMER::seven, &htim7);    
}
#endif

#if defined(STM32HAL_HAS_TIM15)
void TIM1_BRK_TIM15_IRQHandler(void)
{
    manageInterrupt(embot::hw::TIMER::fifteen, &htim15);
}
#endif

#if defined(STM32HAL_HAS_TIM16)
void TIM1_UP_TIM16_IRQHandler(void)
{
    manageInterrupt(embot::hw::TIMER::sixteen, &htim16);
}
#endif

#else   // !defined(USE_QUICKER_MODE)

#if defined(STM32HAL_HAS_TIM6)
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}
#endif

#if defined(STM32HAL_HAS_TIM7)
void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim7);
}
#endif

#if defined(STM32HAL_HAS_TIM15)
void TIM1_BRK_TIM15_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim15);
}
#endif

#if defined(STM32HAL_HAS_TIM16)
void TIM1_UP_TIM16_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&htim16);
}
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(TIM6 == htim->Instance)
    { 
        embot::hw::timer::execute(embot::hw::TIMER::six); 
    }
    else if(TIM7 == htim->Instance)
    { 
        embot::hw::timer::execute(embot::hw::TIMER::seven); 
    } 
    else if(TIM15 == htim->Instance)
    { 
        embot::hw::timer::execute(embot::hw::TIMER::fifteen); 
    }
    else if(TIM16 == htim->Instance)
    { 
        embot::hw::timer::execute(embot::hw::TIMER::sixteen); 
    }     
}

#endif // !defined(USE_QUICKER_MODE)


#endif // timer


// - support map: end of embot::hw::timer



// - support map: begin of embot::hw::i2c

namespace embot { namespace hw { namespace bsp { namespace i2c {
           
    static_assert(embot::core::tointegral(embot::hw::I2C::none) < 8*sizeof(SUPP::supportedmask), "I2C::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::I2C::maxnumberof) < 8*sizeof(SUPP::supportedmask), "I2C::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::I2C::maxnumberof) < embot::core::tointegral(embot::hw::I2C::none), "I2C::maxnumberof must be higher that I2C::none, so that we can optimise code");

}}}}




#if !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_i2c)

namespace embot { namespace hw { namespace bsp { namespace i2c {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::I2C h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace i2c {
               
    #if defined(STM32HAL_BOARD_MTB4)
    
    constexpr PROP i2c1p { .handle = &hi2c1 }; //, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    constexpr PROP i2c2p { .handle = &hi2c2 }; //, .handledmatx = &hdma_i2c2_tx, .handledmarx = &hdma_i2c2_rx };
                
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two),        
        // properties
        {{
            &i2c1p, &i2c2p, nullptr             
        }}        
    };
    
    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::one)
        {            
            MX_I2C1_Init();
        }
        else if(h == I2C::two)
        {
            MX_I2C2_Init();
        }         
    }

    #elif   defined(STM32HAL_BOARD_STRAIN2)
     
    constexpr PROP i2c1p { .handle = &hi2c1 }; //, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    constexpr PROP i2c2p { .handle = &hi2c2 }; //, .handledmatx = &hdma_i2c2_tx, .handledmarx = &hdma_i2c2_rx };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two),        
        // properties
        {{
            &i2c1p, &i2c2p, nullptr             
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::one)
        {            
            MX_I2C1_Init();
        }
        else if(h == I2C::two)
        {
            MX_I2C2_Init();
        }         
    }
    
    
    #elif   defined(STM32HAL_BOARD_PSC)
     
    constexpr PROP i2c1p { .handle = &hi2c1 }; //, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    constexpr PROP i2c3p { .handle = &hi2c3 }; //, .handledmatx = &hdma_i2c3_tx, .handledmarx = &hdma_i2c3_rx };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::three),        
        // properties
        {{
            &i2c1p, nullptr, &i2c3p             
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::one)
        {            
            MX_I2C1_Init();
        }
        else if(h == I2C::three)
        {
            MX_I2C3_Init();
        }         
    }
    
    #elif   defined(STM32HAL_BOARD_SG3)
     
    constexpr PROP i2c1p { .handle = &hi2c1 }; //, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    constexpr PROP i2c3p { .handle = &hi2c3 }; //, .handledmatx = &hdma_i2c3_tx, .handledmarx = &hdma_i2c3_rx };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::three),        
        // properties
        {{
            &i2c1p, nullptr, &i2c3p             
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::one)
        {            
            MX_I2C1_Init();
        }
        else if(h == I2C::three)
        {
            MX_I2C3_Init();
        }         
    }
    
    #elif   defined(STM32HAL_BOARD_RFE)
    
    constexpr PROP i2c1p { .handle = &hi2c1 }; //, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    //constexpr PROP i2c2p { .handle = &hi2c2 }; //, .handledmatx = &hdma_i2c2_tx, .handledmarx = &hdma_i2c2_rx };
        
    constexpr BSP thebsp {        
        // maskofsupported
        //mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two),   
        mask::pos2mask<uint32_t>(I2C::one),         
        // properties
        {{
//            &i2c1p, &i2c2p, nullptr             
            &i2c1p, nullptr, nullptr
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::one)
        {            
            MX_I2C1_Init();
        }
//        else if(h == I2C::two)
//        {
//            MX_I2C2_Init();
//        }         
    }
        
    #elif   defined(STM32HAL_BOARD_NUCLEOH7)
    
    constexpr PROP i2c1p { .handle = &hi2c1 };

        
    constexpr BSP thebsp {        
        // maskofsupported   
        mask::pos2mask<uint32_t>(I2C::one),         
        // properties
        {{       
            &i2c1p, nullptr, nullptr
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::one)
        {            
            MX_I2C1_Init();
        }        
    }
    
    #elif   defined(STM32HAL_BOARD_STM32G4EVAL)
    
    constexpr PROP i2c3p { .handle = &hi2c3 };

        
    constexpr BSP thebsp {        
        // maskofsupported   
        mask::pos2mask<uint32_t>(I2C::three),         
        // properties
        {{       
            nullptr, nullptr, &i2c3p
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::three)
        {            
            MX_I2C3_Init();
        }        
    }
    #else
        #error embot::hw::bsp::i2c::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace i2c {


// irq handlers of i2c

#if defined(STM32HAL_BOARD_MTB4) | defined(STM32HAL_BOARD_STRAIN2) | defined(STM32HAL_BOARD_RFE)

// they all have i2c1 and i2c2 w/ same name of variables, hence i group them together

    /**
* @brief This function handles I2C1 event interrupt.
*/
void I2C1_EV_IRQHandler(void)
{
//    I2C_HandleTypeDef* h1 = embot::hw::i2c::s_privatedata.handles[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::one)];
//    HAL_I2C_EV_IRQHandler(h1);
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
* @brief This function handles I2C1 error interrupt.
*/
void I2C1_ER_IRQHandler(void)
{
//    I2C_HandleTypeDef* h1 = embot::hw::i2c::s_privatedata.handles[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::one)];
//    HAL_I2C_ER_IRQHandler(h1);
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}


/**
* @brief This function handles DMA1 channel6 global interrupt.
*/
void DMA1_Channel6_IRQHandler(void)
{   
//    DMA_HandleTypeDef* h1dmatx = embot::hw::i2c::s_privatedata.handlesdmatx[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::one)];
//    HAL_DMA_IRQHandler(h1dmatx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel7 global interrupt.
*/
void DMA1_Channel7_IRQHandler(void)
{
//    DMA_HandleTypeDef* h1dmarx = embot::hw::i2c::s_privatedata.handlesdmarx[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::one)];
//    HAL_DMA_IRQHandler(h1dmarx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}


/**
* @brief This function handles DMA1 channel4 global interrupt.
*/
void DMA1_Channel4_IRQHandler(void)
{
//    DMA_HandleTypeDef* h2dmatx = embot::hw::i2c::s_privatedata.handlesdmatx[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::two)];
//    HAL_DMA_IRQHandler(h2dmatx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c2_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel5 global interrupt.
*/
void DMA1_Channel5_IRQHandler(void)
{
//    DMA_HandleTypeDef* h2dmarx = embot::hw::i2c::s_privatedata.handlesdmarx[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::two)];
//    HAL_DMA_IRQHandler(h2dmarx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c2_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
* @brief This function handles I2C2 event interrupt.
*/
void I2C2_EV_IRQHandler(void)
{
//    I2C_HandleTypeDef* h2 = embot::hw::i2c::s_privatedata.handles[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::two)];
//    HAL_I2C_EV_IRQHandler(h2);
  /* USER CODE BEGIN I2C2_EV_IRQn 0 */

  /* USER CODE END I2C2_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_EV_IRQn 1 */

  /* USER CODE END I2C2_EV_IRQn 1 */
}

/**
* @brief This function handles I2C2 error interrupt.
*/
void I2C2_ER_IRQHandler(void)
{
//    I2C_HandleTypeDef* h2 = embot::hw::i2c::s_privatedata.handles[embot::hw::bsp::i2c::MAP::toindex(embot::hw::I2C::two)];
//    HAL_I2C_ER_IRQHandler(h2);
  /* USER CODE BEGIN I2C2_ER_IRQn 0 */

  /* USER CODE END I2C2_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_ER_IRQn 1 */

  /* USER CODE END I2C2_ER_IRQn 1 */
}

#elif   defined(STM32HAL_BOARD_PSC)

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}


/**
  * @brief This function handles I2C3 event interrupt.
  */
void I2C3_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C3_EV_IRQn 0 */

  /* USER CODE END I2C3_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c3);
  /* USER CODE BEGIN I2C3_EV_IRQn 1 */

  /* USER CODE END I2C3_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C3 error interrupt.
  */
void I2C3_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C3_ER_IRQn 0 */

  /* USER CODE END I2C3_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c3);
  /* USER CODE BEGIN I2C3_ER_IRQn 1 */

  /* USER CODE END I2C3_ER_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c3_tx);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c3_rx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}


#elif defined(STM32HAL_BOARD_SG3)

// the sg3 has different code for i2c: i2c1 and i2c3

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c3_tx);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c3_rx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles ADC1 global interrupt.
  */
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */

  /* USER CODE END ADC1_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_IRQn 1 */

  /* USER CODE END ADC1_IRQn 1 */
}

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}


/**
  * @brief This function handles I2C3 event interrupt.
  */
void I2C3_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C3_EV_IRQn 0 */

  /* USER CODE END I2C3_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c3);
  /* USER CODE BEGIN I2C3_EV_IRQn 1 */

  /* USER CODE END I2C3_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C3 error interrupt.
  */
void I2C3_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C3_ER_IRQn 0 */

  /* USER CODE END I2C3_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c3);
  /* USER CODE BEGIN I2C3_ER_IRQn 1 */

  /* USER CODE END I2C3_ER_IRQn 1 */
}


#elif defined(STM32HAL_BOARD_NUCLEOH7)


void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}


void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}

#elif defined(STM32HAL_BOARD_STM32G4EVAL)


void I2C3_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c3);
}

void I2C3_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c3);
}

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c3_rx);
}

void DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c3_tx);
}

#endif // irq handlers

#endif // i2c

// - support map: end of embot::hw::i2c    
    
    
    
// - support map: begin of embot::hw::bno055
    
namespace embot { namespace hw { namespace bsp { namespace bno055 {
       
    static_assert(embot::core::tointegral(embot::hw::BNO055::none) < 8*sizeof(SUPP::supportedmask), "BNO055::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::BNO055::maxnumberof) < 8*sizeof(SUPP::supportedmask), "BNO055::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::BNO055::maxnumberof) < embot::core::tointegral(embot::hw::BNO055::none), "BNO055::maxnumberof must be higher that BNO055::none, so that we can optimise code");

}}}}

#if   !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_bno055)

namespace embot { namespace hw { namespace bsp { namespace bno055 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BNO055 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace bno055 {
               
    #if defined(STM32HAL_BOARD_STRAIN2) || defined(STM32HAL_BOARD_MTB4)
    
    // .boot = { BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin }, .reset = { BNO055_RESET_GPIO_Port, BNO055_RESET_Pin } 
    constexpr PROP prop01 { .i2caddress = 0x52, .boot = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen }, .reset = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::twelve } }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BNO055::one),        
        // properties
        {{
            &prop01             
        }}        
    };

    void BSP::init(embot::hw::BNO055 h) const {}
    
    #elif defined(STM32HAL_BOARD_RFE)
       
    // .boot = { BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin }, .reset = { BNO055_RESET_GPIO_Port, BNO055_RESET_Pin } 
    constexpr PROP prop01 { .i2caddress = 0x50, .boot = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::fourteen }, .reset = { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::fifteen } }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BNO055::one),        
        // properties
        {{
            &prop01             
        }}        
    };

    void BSP::init(embot::hw::BNO055 h) const {}
                
    #elif defined(STM32HAL_BOARD_NUCLEOH7)
    
    constexpr PROP prop01 { .i2caddress = 0x52, .boot = { embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none }, .reset = { embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none } }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BNO055::one),        
        // properties
        {{
            &prop01             
        }}        
    };

    void BSP::init(embot::hw::BNO055 h) const {}
        
    #else
        #error embot::hw::bsp::bno055::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace bno055 {

#endif // bno055

// - support map: end of embot::hw::bno055



// - support map: begin of embot::hw::tlv493d

namespace embot { namespace hw { namespace bsp { namespace tlv493d {
           
    static_assert(embot::core::tointegral(embot::hw::TLV493D::none) < 8*sizeof(SUPP::supportedmask), "TLV493D::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::TLV493D::maxnumberof) < 8*sizeof(SUPP::supportedmask), "TLV493D::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::TLV493D::maxnumberof) < embot::core::tointegral(embot::hw::TLV493D::none), "TLV493D::maxnumberof must be higher that TLV493D::none, so that we can optimise code");

}}}}

#if   !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_tlv493d)

namespace embot { namespace hw { namespace bsp { namespace tlv493d {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TLV493D h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace tlv493d {
           
    #if defined(STM32HAL_BOARD_PSC)
        
    constexpr PROP prop01 { .i2cbus = embot::hw::I2C::one, .i2caddress = 0x3E }; 
    constexpr PROP prop02 { .i2cbus = embot::hw::I2C::three, .i2caddress = 0x3E }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one) | mask::pos2mask<uint32_t>(TLV493D::two),        
        // properties
        {{
            &prop01, &prop02
        }}        
    };

    void BSP::init(embot::hw::TLV493D h) const {}
    
    #elif defined(STM32HAL_BOARD_NUCLEOH7)
    
    constexpr PROP prop01 { .i2cbus = embot::hw::I2C::one, .i2caddress = 0xBC }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one),        
        // properties
        {{
            &prop01
        }}        
    };
    
    void BSP::init(embot::hw::TLV493D h) const {}

    #elif defined(STM32HAL_BOARD_STM32G4EVAL)
    
    constexpr PROP prop01 { .i2cbus = embot::hw::I2C::three, .i2caddress = 0xBC }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one),        
        // properties
        {{
            &prop01
        }}        
    };
    
    void BSP::init(embot::hw::TLV493D h) const {}
    
    #else
        #error embot::hw::bsp::tlv493d::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace tlv493d {

#endif // tlv493d

// - support map: end of embot::hw::tlv493d


// - support map: begin of embot::hw::multisda

#if   !defined(EMBOT_ENABLE_hw_multisda)

namespace embot { namespace hw { namespace bsp { namespace multisda {
    
    constexpr BSP thebsp { };

    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace multisda {
  
    #if     defined(STM32HAL_BOARD_PSC)

    static const BSP thebsp { 
        // clk = 
        { I2C_CLK_GPIO_Port, I2C_CLK_Pin },
        // sda
        {{  
            { I2C_SDA0_GPIO_Port, I2C_SDA0_Pin }, { I2C_SDA1_GPIO_Port, I2C_SDA1_Pin }, { I2C_SDA2_GPIO_Port, I2C_SDA2_Pin }, { I2C_SDA3_GPIO_Port, I2C_SDA3_Pin }            
        }}
    };
    
    #elif   defined(STM32HAL_BOARD_MTB4)

    static const BSP thebsp { 
        // clk = 
        { SCK0_GPIO_Port, SCK0_Pin },
        // sda
        {{  
            { SDA0_GPIO_Port, SDA0_Pin }, { SDA1_GPIO_Port, SDA1_Pin }, { SDA2_GPIO_Port, SDA2_Pin }, { SDA3_GPIO_Port, SDA3_Pin }            
        }}
    };
    
    #else
        #error embot::hw::bsp::multisda::thebsp must be defined    
    #endif
 
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}} // namespace embot { namespace hw { namespace bsp {  namespace multisda {

#endif // multisda

// - support map: end of embot::hw::multisda


// - support map: begin of embot::hw::ads122c04

namespace embot { namespace hw { namespace bsp { namespace ads122c04 {
           
    static_assert(embot::core::tointegral(embot::hw::ADS122C04::none) < 8*sizeof(SUPP::supportedmask), "ADS122C04::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::ADS122C04::maxnumberof) < 8*sizeof(SUPP::supportedmask), "ADS122C04::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::core::tointegral(embot::hw::ADS122C04::maxnumberof) < embot::core::tointegral(embot::hw::ADS122C04::none), "ADS122C04::maxnumberof must be higher that ADS122C04::none, so that we can optimise code");

}}}}

#if   !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_ads122c04)

namespace embot { namespace hw { namespace bsp { namespace ads122c04 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ADS122C04 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace bsp { namespace ads122c04 {
           
    #if defined(STM32HAL_BOARD_NUCLEOH7)
        
    constexpr PROP prop01 { .i2cbus = embot::hw::I2C::one, .i2caddress = (0x40<<1) }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ADS122C04::one),        
        // properties
        {{
            &prop01
        }}        
    };

    void BSP::init(embot::hw::ADS122C04 h) const {}

    #else
        #error embot::hw::bsp::ads122c04::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace ads122c04 {

#endif // ads122c04

// - support map: end of embot::hw::ads122c04


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

