
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
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amcbldc_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_led_bsp.h"

void leds_off()
{
    using namespace embot::hw;
    const led::BSP &b = led::getBSP();
    constexpr std::array<LED, embot::core::tointegral(LED::maxnumberof)> leds {LED::one, LED::two, LED::three};
    for(const auto &l : leds)
    {
        const led::PROP *p = b.getPROP(l);
        if(nullptr != p)
        {
            gpio::init(p->gpio, {gpio::Mode::OUTPUTpushpull, gpio::Pull::nopull, gpio::Speed::medium});  
            gpio::set(p->gpio, p->on);
            gpio::set(p->gpio, p->off);
        }
    }    
}

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   
bool embot::hw::bsp::specialize() 
{   

    leds_off();
    
    return true; 
}
#endif  //EMBOT_ENABLE_hw_bsp_specialize


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------



// - support map: begin of embot::hw::gpio

#include "embot_hw_gpio_bsp.h"

#if   !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

#error CAVEAT: embot::hw requires GPIO. pls enable it!

namespace embot { namespace hw { namespace gpio {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::GPIO h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else
    
namespace embot { namespace hw { namespace gpio {
 
    #if defined(STM32HAL_BOARD_AMCBLDC)
    
    const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, nullptr
        }},
        {{
            [](){__HAL_RCC_GPIOA_CLK_ENABLE();}, [](){__HAL_RCC_GPIOB_CLK_ENABLE();}, [](){__HAL_RCC_GPIOC_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOD_CLK_ENABLE();}, [](){__HAL_RCC_GPIOE_CLK_ENABLE();}, [](){__HAL_RCC_GPIOF_CLK_ENABLE();},
            [](){__HAL_RCC_GPIOG_CLK_ENABLE();}, nullptr
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
    
    #if   defined(STM32HAL_BOARD_AMCBLDC)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::two}  };
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::five}  };
    constexpr PROP led3p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fifteen}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two) | mask::pos2mask<uint32_t>(LED::three),        
        // properties
        {{
            &led1p, &led2p, &led3p, nullptr, nullptr, nullptr, nullptr, nullptr            
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
    
    #if   defined(STM32HAL_BOARD_AMCBLDC)
    
    // this button is attached directly to PB7 (SDA connector).
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::seven}, 
                             .pull = embot::hw::gpio::Pull::pulldown, .irqn = EXTI9_5_IRQn  };  
 
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one),        
        // properties
        {{
            &btn1p           
        }}        
    };
    
    
    void BSP::init(embot::hw::BTN h) const {}
    
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const
    {
        const embot::hw::GPIO gpio = embot::hw::gpio::getBSP().getGPIO(p);
        switch(gpio.pin)
        {
            case embot::hw::GPIO::PIN::seven:
            {
                embot::hw::button::onexti(BTN::one);
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
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
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
               
    #if   defined(STM32HAL_BOARD_AMCBLDC)
        
    constexpr PROP can1p = { .handle = &hfdcan2 };  
        
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
//            // at first make sure these pins are reset
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            HAL_Delay(1);

            // yes: CAN::one is mapped into FDCAN2
            MX_FDCAN2_Init();
                        
//            // for 80 mhz (from a project by st)
//            hfdcan2.Instance = FDCAN2;
//            hfdcan2.Init.ClockDivider = FDCAN_CLOCK_DIV2;
//            hfdcan2.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
//            hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
//            hfdcan2.Init.AutoRetransmission = ENABLE;
//            hfdcan2.Init.TransmitPause = ENABLE;
//            hfdcan2.Init.ProtocolException = DISABLE;
//            hfdcan2.Init.NominalPrescaler = 1;
//            hfdcan2.Init.NominalSyncJumpWidth = 16;
//            hfdcan2.Init.NominalTimeSeg1 = 63;
//            hfdcan2.Init.NominalTimeSeg2 = 16;
//            hfdcan2.Init.DataPrescaler = 1;
//            hfdcan2.Init.DataSyncJumpWidth = 4;
//            hfdcan2.Init.DataTimeSeg1 = 5;
//            hfdcan2.Init.DataTimeSeg2 = 4;
//            hfdcan2.Init.StdFiltersNbr = 1;
//            hfdcan2.Init.ExtFiltersNbr = 0;
//            hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
//            if(HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
//            {
//                Error_Handler();
//            }

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
    

#if defined(HAL_FDCAN_MODULE_ENABLED)

extern "C"
{    
    void FDCAN2_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan2);
    }  
    
    void FDCAN2_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan2);
    }  
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
     
   #if   defined(STM32HAL_BOARD_AMCBLDC)

   #if defined(EMBOT_ENABLE_hw_flash_SINGLEBANK)
        // the stm32g4 has a page size of 4k when its flash is configured as single bank
        constexpr uint32_t pagesize = 4*1024;
   #else
        // the stm32g4 has a page size of 2k when its flash is configured as two banks
        constexpr uint32_t pagesize = 2*1024;
   #endif
        // application @ 128k, single bank
        constexpr PROP whole                {{0x08000000,               (512)*1024,         pagesize}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         pagesize}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           pagesize}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (256+124)*1024,     pagesize}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(508*1024),    (4)*1024,           pagesize}};   // applicationstorage: on top of application            

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
                  
    #if   defined(STM32HAL_BOARD_AMCBLDC)
    
    constexpr PROP i2c4p { &hi2c4, embot::hw::i2c::Speed::standard100 };

        
    constexpr BSP thebsp {        
        // maskofsupported   
        mask::pos2mask<uint32_t>(I2C::four),         
        // properties
        {{       
            nullptr, nullptr, nullptr, &i2c4p
        }}        
    }; 

    void BSP::init(embot::hw::I2C h) const
    {
        if(h == I2C::four)
        {            
            MX_I2C4_Init();
        }        
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

#if defined(STM32HAL_BOARD_AMCBLDC)

extern "C"
{

void I2C4_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c4);
}

void I2C4_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c4);
}

#error TODO: add DMA for I2C in cubemx
void DMA2_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c4_rx);
}

void DMA2_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c4_tx);
}

}

#endif // irq handlers

#endif // i2c

// - support map: end of embot::hw::i2c    
    
    
// - support map: begin of embot::hw::tlv493d

#include "embot_hw_tlv493d_bsp.h"

#if   !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_tlv493d)

namespace embot { namespace hw { namespace tlv493d {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TLV493D h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace tlv493d {
           
    #if defined(STM32HAL_BOARD_AMCBLDC)
    
#if !defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)

    constexpr PROP propJ4  { embot::hw::i2c::Descriptor{embot::hw::I2C::one,   0xBC} };
    constexpr PROP propJ5  { embot::hw::i2c::Descriptor{embot::hw::I2C::two,   0xBC} };
    constexpr PROP propJ6  { embot::hw::i2c::Descriptor{embot::hw::I2C::three, 0xBC} }; 
    constexpr PROP propJ7  { embot::hw::i2c::Descriptor{embot::hw::I2C::four,  0xBC} };  
    constexpr PROP propJ11 { embot::hw::i2c::Descriptor{embot::hw::I2C::one,   0x3E} };
    constexpr PROP propU27 { embot::hw::i2c::Descriptor{embot::hw::I2C::two,   0x3E} };
    
#else
    constexpr PROP propJ4  { embot::hw::i2c::Descriptor{embot::hw::I2C::one,   0xBC} };
    constexpr PROP propJ5  { embot::hw::i2c::Descriptor{embot::hw::I2C::two,   0xBC} };
    constexpr PROP propJ6  { embot::hw::i2c::Descriptor{embot::hw::I2C::three, 0xBC} }; 
    constexpr PROP propJ7  { embot::hw::i2c::Descriptor{embot::hw::I2C::four,  0xBC} };  
    constexpr PROP propJ11 { embot::hw::i2c::Descriptor{embot::hw::I2C::one,   0x3E} };
    constexpr PROP propU27 { embot::hw::i2c::Descriptor{embot::hw::I2C::two,   0x3E} };
#endif
    
    
    constexpr BSP thebsp {     
#if !defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one) | mask::pos2mask<uint32_t>(TLV493D::two) |
        mask::pos2mask<uint32_t>(TLV493D::three) | mask::pos2mask<uint32_t>(TLV493D::four) |
        mask::pos2mask<uint32_t>(TLV493D::five) | mask::pos2mask<uint32_t>(TLV493D::six),        
        // properties
        {{
            &propJ4, &propJ5, &propJ6, &propJ7, &propJ11, &propU27
        }}
#else
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one) | mask::pos2mask<uint32_t>(TLV493D::two) |
        mask::pos2mask<uint32_t>(TLV493D::three) | mask::pos2mask<uint32_t>(TLV493D::four) |
        mask::pos2mask<uint32_t>(TLV493D::five) | mask::pos2mask<uint32_t>(TLV493D::six),        
        // properties
        {{
            &propJ4, &propJ5, &propJ6, &propJ7, &propJ11, &propU27
        }}
#endif        
    };
    
    void BSP::init(embot::hw::TLV493D h) const {}
        
    #else
        #error embot::hw::tlv493d::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace tlv493d {

#endif // tlv493d

// - support map: end of embot::hw::tlv493d

// - support map: begin of embot::hw::motor

#include "embot_hw_motor_bsp.h"

#if !defined(EMBOT_ENABLE_hw_motor)

namespace embot { namespace hw { namespace motor {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::MOTOR h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace motor {
           
#if defined(STM32HAL_BOARD_AMCBLDC)
    
    
    constexpr PROP propM1  { 0 };
    
    constexpr BSP thebsp {     

        // maskofsupported
        mask::pos2mask<uint32_t>(MOTOR::one),        
        // properties
        {{
            &propM1
        }}
  
    };
    
    void BSP::init(embot::hw::MOTOR h) const {
    // step 1: what cube mx does
        MX_ADC1_Init();
        MX_ADC2_Init();
        
        MX_TIM1_Init();
        MX_TIM3_Init();
        MX_TIM2_Init();
        MX_CORDIC_Init();
        MX_FMAC_Init();
        MX_CRC_Init();   
        
        HAL_GPIO_WritePin(VAUXEN_GPIO_Port, VAUXEN_Pin, GPIO_PIN_SET);
        HAL_Delay(10); 
    
    }
        
    #else
        #error embot::hw::motor::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace motor {

#endif // motor

// - support map: end of embot::hw::motor


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

