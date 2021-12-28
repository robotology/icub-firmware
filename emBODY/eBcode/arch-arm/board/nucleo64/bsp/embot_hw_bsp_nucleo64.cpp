
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


#if defined(STM32HAL_BOARD_NUCLEO64)
#else
    #error this file is only for STM32HAL_BOARD_NUCLEO64
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_nucleo64_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   
bool embot::hw::bsp::specialize() { return true; }
#endif  //EMBOT_ENABLE_hw_bsp_specialize


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------



// - support map: begin of embot::hw::gpio

#include "embot_hw_gpio_bsp.h"

#if   !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)
    #error CAVEAT: embot::hw requires GPIO. pls enable it!
#else
    
namespace embot { namespace hw { namespace gpio {
 
    #if defined(STM32HAL_BOARD_NUCLEO64)
    const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH
        }},
        {{
            [](){__HAL_RCC_GPIOA_CLK_ENABLE();}, [](){__HAL_RCC_GPIOB_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOC_CLK_ENABLE();}, [](){__HAL_RCC_GPIOD_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOE_CLK_ENABLE();}, [](){__HAL_RCC_GPIOF_CLK_ENABLE();},
            [](){__HAL_RCC_GPIOG_CLK_ENABLE();}, [](){__HAL_RCC_GPIOH_CLK_ENABLE();}
        }}        
    };      
    #else
        #error embot::hw::gpio::thebsp must be defined
    #endif
        
    void BSP::init(embot::hw::GPIO h) const {}        
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace gpio {

#endif // gpio

// - support map: end of embot::hw::gpio



// - support map: begin of embot::hw::led

#include "embot_hw_led_bsp.h"

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot { namespace hw { namespace led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace led {         
    
    #if   defined(STM32HAL_BOARD_NUCLEO64)
       
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

    #else
        #error embot::hw::led::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw {  namespace led {

#endif // led

// - support map: end of embot::hw::led



// - support map: begin of embot::hw::button

#include "embot_hw_button.h"
#include "embot_hw_button_bsp.h"
#include "embot_hw_gpio_bsp.h"

#if !defined(EMBOT_ENABLE_hw_button)

namespace embot { namespace hw { namespace button {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace button {
    
    #if   defined(STM32HAL_BOARD_NUCLEO64)
    
    // this button is the blue one on the board
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}, 
                             .pull = embot::hw::gpio::Pull::nopull, .irqn = EXTI15_10_IRQn  };  
    // this button is attached directly to PB9 w/out any filter. it is pressed when PB9 is connected to 3V3
    // on the board, PB9 can also be SDA.
    constexpr PROP btn2p = { .pressed = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::nine}, 
                             .pull = embot::hw::gpio::Pull::pulldown, .irqn = EXTI9_5_IRQn  };  
 
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one) | mask::pos2mask<uint32_t>(BTN::two),
//        mask::pos2mask<uint32_t>(BTN::one),        
        // properties
        {{
            &btn1p, &btn2p
//            &btn1p              
        }}        
    };
    
    
    void BSP::init(embot::hw::BTN h) const {}
    
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const
    {
        const embot::hw::GPIO gpio = embot::hw::gpio::getBSP().getGPIO(p);
        switch(gpio.pin)
        {
            case embot::hw::GPIO::PIN::thirteen:
            {
                embot::hw::button::onexti(BTN::one);
            } break;
            
            case embot::hw::GPIO::PIN::nine:
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
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
        }
        
        void EXTI15_10_IRQHandler(void)
        {
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
        }
        
        void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
        {
             embot::hw::button::getBSP().onEXTI({nullptr, GPIO_Pin, nullptr});            
        }        
    }
           
    #else
        #error embot::hw::bsp::button::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#endif // button

// - support map: end of embot::hw::button



// - support map: begin of embot::hw::can

#include "embot_hw_can_bsp.h"

#if   !defined(EMBOT_ENABLE_hw_can)

namespace embot { namespace hw { namespace can {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::CAN h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace can {
               
    #if   defined(STM32HAL_BOARD_NUCLEOH7)        
    #else
        #error embot::hw::can::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace can {
    

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
#endif //

#endif // EMBOT_ENABLE_hw_can


// - support map: end of embot::hw::can



// - support map: begin of embot::hw::flash

#include "embot_hw_flash_bsp.h"

#if !defined(EMBOT_ENABLE_hw_flash)

namespace embot { namespace hw { namespace flash {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::FLASH h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace flash {
     
    #if   defined(STM32HAL_BOARD_NUCLEO64)
        #error embot::hw::flash::thebsp is not supported for STM32HAL_BOARD_NUCLEOH7 
    #else
        #error embot::hw::flash::thebsp must be defined    
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
              
}}} // namespace embot { namespace hw { namespace flash {

#endif // flash

// - support map: end of embot::hw::flash



// - support map: begin of embot::hw::timer

#include "embot_hw_timer_bsp.h"

#if   !defined(HAL_TIM_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_timer)

namespace embot { namespace hw { namespace timer {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TIMER h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace timer {
    
    #if   defined(STM32HAL_BOARD_NUCLEO64)    

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
        #error embot::hw::timer::thebsp must be defined    
    #endif
        
    const BSP& getBSP() 
    {
        return thebsp;
    }    
}}}

// in here it is implemented in the way the good old hal2 was doing: the handler directly manages the callback
// instead the stm hal make a lot of calls before actually calling the callback code, hence it is slower.

#include "embot_hw_timer.h"

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

#define STM32HAL_HAS_TIM6
#define STM32HAL_HAS_TIM7


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


#endif // timer


// - support map: end of embot::hw::timer





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

