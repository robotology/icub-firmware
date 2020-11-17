
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
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


#if defined(STM32HAL_BOARD_RFE)
#else
    #error this file is only for STM32HAL_BOARD_RFE
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_rfe_config.h"


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
 
    #if defined(STM32HAL_BOARD_RFE)
    const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, nullptr, nullptr, nullptr
        }},
        {{
            [](){__HAL_RCC_GPIOA_CLK_ENABLE();}, [](){__HAL_RCC_GPIOB_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOC_CLK_ENABLE();}, [](){__HAL_RCC_GPIOD_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOE_CLK_ENABLE();}, nullptr,
            nullptr, nullptr
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
    
    #if   defined(STM32HAL_BOARD_RFE)
       
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

#include "embot_hw_button_bsp.h"


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
               
    #if   defined(STM32HAL_BOARD_RFE)
    
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
    
}}}}

#else

namespace embot { namespace hw { namespace flash {
     
    #if   defined(STM32HAL_BOARD_RFE)

        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               (256)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application

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



// - support map: begin of embot::hw::pga308

#include "embot_hw_pga308_bsp.h"

#if !defined(EMBOT_ENABLE_hw_pga308)

namespace embot { namespace hw { namespace pga308 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::PGA308 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else
    
namespace embot { namespace hw { namespace pga308 {
    
    #if defined(STM32HAL_BOARD_RFE)            
    #else
        #error embot::hw::pga308::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#endif 


// - support map: end of embot::hw::pga308



// - support map: begin of embot::hw::si7051

#include "embot_hw_si7051_bsp.h"

#if !defined(EMBOT_ENABLE_hw_si7051)

namespace embot { namespace hw { namespace si7051 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::SI7051 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

#include "embot_hw_sys.h"

namespace embot { namespace hw { namespace si7051 {
    
    #if defined(STM32HAL_BOARD_RFE)        
    #else
        #error embot::hw::can::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }    
}}}


#endif // si7051


// - support map: end of embot::hw::si7051


// - support map: begin of embot::hw::onewire

#include "embot_hw_onewire_bsp.h"

#if !defined(EMBOT_ENABLE_hw_onewire)

namespace embot { namespace hw { namespace onewire {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ONEWIRE h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace onewire {
    
    #if defined(STM32HAL_BOARD_RFE)           
    #else
        #error embot::hw::onewire::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#endif

// - support map: end of embot::hw::onewire



// - support map: begin of embot::hw::adc

#include "embot_hw_adc_bsp.h"

#if   !defined(HAL_ADC_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_adc)

namespace embot { namespace hw { namespace adc {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ADC h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace adc {

    #if defined(STM32HAL_BOARD_RFE)
    #else
        #error embot::hw::bsp::adc::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#endif // adc

// - support map: end of embot::hw::adc



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
    
    #if   defined(STM32HAL_BOARD_RFE)    
    
    constexpr BSP thebsp { };
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

#undef STM32HAL_HAS_TIM6

#if defined(STM32HAL_HAS_TIM6)
void TIM6_DAC_IRQHandler(void)
{
    manageInterrupt(embot::hw::TIMER::six, &htim6);
}
#endif

#endif // timer


// - support map: end of embot::hw::timer


// - support map: begin of embot::hw::i2c

#include "embot_hw_i2c_bsp.h"


#if !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_i2c)

namespace embot { namespace hw { namespace i2c {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::I2C h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace i2c {
                  
    #if   defined(STM32HAL_BOARD_RFE)
    
    constexpr PROP i2c1p { &hi2c1, embot::hw::i2c::Speed::fast400 };
    //constexpr PROP i2c2p { .handle = &hi2c2 }; 
        
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
    
    #else
        #error embot::hw::i2c::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace i2c {


// irq handlers of i2c

#if defined(STM32HAL_BOARD_MTB4) | defined(STM32HAL_BOARD_STRAIN2) | defined(STM32HAL_BOARD_RFE)

void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void DMA1_Channel6_IRQHandler(void)
{   
    HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}

void DMA1_Channel7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

void DMA1_Channel4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c2_tx);
}

void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c2_rx);
}

void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c2);
}

void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c2);
}


#endif // irq handlers

#endif // i2c

// - support map: end of embot::hw::i2c    
    
    
    
// - support map: begin of embot::hw::bno055
    
#include "embot_hw_bno055_bsp.h"

#if   !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_bno055)

namespace embot { namespace hw { namespace bno055 {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BNO055 h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace bno055 {
    
    #if defined(STM32HAL_BOARD_RFE)
    
    // .boot = { BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin }, .reset = { BNO055_RESET_GPIO_Port, BNO055_RESET_Pin } 
    constexpr PROP prop01 {
        { embot::hw::I2C::two, 0x50 },
        { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::fourteen },   // .boot
        { embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::fifteen }      // .reset
    };
    
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
        #error embot::hw::bno055::thebsp must be defined    
    #endif
 
    const BSP& getBSP() 
    {
        return thebsp;
    }        
}}}

#endif // bno055

// - support map: end of embot::hw::bno055



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

