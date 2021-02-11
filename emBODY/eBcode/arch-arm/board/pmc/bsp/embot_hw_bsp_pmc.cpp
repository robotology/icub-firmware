
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

#include "embot_hw_bsp_pmc_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

void i2c_address_assignment_to_tlv493d_chips_stm32hal()
{
    // i2c address assignment for fap boards and internal tlv493d.
    // faps on J4, J5, J6 and J7 shall have 0xBC. they are respectively on I2C1, I2C2, I2C3 and I2C4
    // fap on J11 and internal tlv493d U27 shall have 0x3E. they are respectively on I2C1 and I2C2
    
    // i proceed as if the call of MX_GPIO_Init() was not done.
    // 1. init of PE10 (power of J11) and of PE11 (power of U27). cube-mx uses MAGVCC1_Pin and MAGVCC2_Pin respectively
    // 2. power them off ...
    // 3. init the sda-i2c1 PB9 and sda-i2c2 PA8
    // ...
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // init power control pins for J11 and U27. they are PE10 and PE11
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);  

    // power off both J13 and U27 and wait
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_Delay(10);
    
    // init i2c1-sda (PB9) as output and put it low. then power up J11
    __HAL_RCC_GPIOB_CLK_ENABLE();    
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_Delay(10);
    // power up J13 so that it can have address 0x3E
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);    
    HAL_Delay(10);  

    // init i2c2-sda (PA8) as output and put it low. then power up J11
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(10);
    // power up u27 so that it can have address 0x3E
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);    
    HAL_Delay(10);      
    
}

#include "embot_hw_led_bsp.h"
void i2c_address_assignment_to_tlv493d_chips()
{
    // i2c address assignment for fap boards and internal tlv493d.
    // faps on J4, J5, J6 and J7 shall have 0xBC. they are respectively on I2C1, I2C2, I2C3 and I2C4
    // fap on J11 and internal tlv493d U27 shall have 0x3E. they are respectively on I2C1 and I2C2
    
    // i proceed as if the call of MX_GPIO_Init() was not done.
    // 1. init of PE10 (power of J11) and of PE11 (power of U27). cube-mx uses MAGVCC1_Pin and MAGVCC2_Pin respectively
    // 2. power them off ...
    // 3. init the sda-i2c1 PB9 and sda-i2c2 PA8
    // ...
    
#if defined(EMBOT_ENABLE_hw_tlv493d_U27off) & defined(EMBOT_ENABLE_hw_tlv493d_J13off)
    
#else    
    using namespace embot::hw;
    
    constexpr GPIO gpioMAGVCC1 {GPIO::PORT::E, GPIO::PIN::ten};
    constexpr GPIO gpioMAGVCC2 {GPIO::PORT::E, GPIO::PIN::eleven};    
    constexpr GPIO gpioI2C1sda {GPIO::PORT::B, GPIO::PIN::nine};
    constexpr GPIO gpioI2C2sda {GPIO::PORT::A, GPIO::PIN::eight};
    
    
//    constexpr embot::hw::led::PROP LED7p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::nine}  };        
//    gpio::init(LED7p.gpio, {gpio::Mode::OUTPUTpushpull, gpio::Pull::nopull, gpio::Speed::veryhigh});  
//    gpio::set(LED7p.gpio, LED7p.off);
//    gpio::set(LED7p.gpio, LED7p.on);
    
    // init the pins which controls power on/off of the two sensors
    gpio::init(gpioMAGVCC1, {gpio::Mode::OUTPUTpushpull, gpio::Pull::nopull, gpio::Speed::veryhigh});    
    gpio::init(gpioMAGVCC2, {gpio::Mode::OUTPUTpushpull, gpio::Pull::nopull, gpio::Speed::veryhigh});
    // set them low to power off both J11 and U27
    gpio::set(gpioMAGVCC1, gpio::State::RESET);
    gpio::set(gpioMAGVCC2, gpio::State::RESET);
        
    // init the sda pins for i2c1 and i2c2
    gpio::init(gpioI2C1sda, {gpio::Mode::OUTPUTopendrain, gpio::Pull::nopull, gpio::Speed::veryhigh});
    gpio::init(gpioI2C2sda, {gpio::Mode::OUTPUTopendrain, gpio::Pull::nopull, gpio::Speed::veryhigh});
    // and set them low
    gpio::set(gpioI2C1sda, gpio::State::RESET);
    gpio::set(gpioI2C2sda, gpio::State::RESET);
    
    // wait a bit to stabilize ... 
    // cannot use embot::core::wait() now because the rtos is not started yet. 
    sys::delay(10*embot::core::time1millisec);
    
    // and now ... power up J11 and U27 so that they can have address 0x3E
#if defined(EMBOT_ENABLE_hw_tlv493d_J11off)
#else    
    gpio::set(gpioMAGVCC1, gpio::State::SET);
#endif
#if defined(EMBOT_ENABLE_hw_tlv493d_U27off)
#else    
    gpio::set(gpioMAGVCC2, gpio::State::SET);
#endif    
    // wait a bit so that the chips are ready to use.
    sys::delay(10*embot::core::time1millisec);
        
#endif
}

void leds_off()
{
    using namespace embot::hw;
    const led::BSP &b = led::getBSP();
    constexpr std::array<LED, embot::core::tointegral(LED::maxnumberof)> leds {LED::one, LED::two, LED::three, LED::four, LED::five, LED::six, LED::seven , LED::eight};
    for(const auto &l : leds)
    {
        const led::PROP *p = b.getPROP(l);
        if(nullptr != p)
        {
            gpio::init(p->gpio, {gpio::Mode::OUTPUTpushpull, gpio::Pull::nopull, gpio::Speed::veryhigh});  
            gpio::set(p->gpio, p->off);
        }
    }    
}

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   
bool embot::hw::bsp::specialize() 
{ 
#if !defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
    i2c_address_assignment_to_tlv493d_chips();
#endif    
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
 
    #if defined(STM32HAL_BOARD_PMC)
    
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
    
    #if   defined(STM32HAL_BOARD_PMC)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::six}  };
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::seven}  };
    constexpr PROP led3p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eight}  };  
    constexpr PROP led4p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::nine}  }; 
    constexpr PROP led5p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::ten}  }; 
    constexpr PROP led6p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::eight}  }; 
    constexpr PROP led7p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::nine}  };        
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two) | mask::pos2mask<uint32_t>(LED::three) | mask::pos2mask<uint32_t>(LED::four) |
        mask::pos2mask<uint32_t>(LED::five) | mask::pos2mask<uint32_t>(LED::six) | mask::pos2mask<uint32_t>(LED::seven),        
        // properties
        {{
            &led1p, &led2p, &led3p, &led4p, &led5p, &led6p, &led7p, nullptr            
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
               
    #if   defined(STM32HAL_BOARD_PMC)
        
    constexpr PROP can1p = { .handle = &hfdcan1 };  
        
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
            // at first make sure these pins are reset
            HAL_GPIO_WritePin(GPIOD, CAN1_STDBY_Pin|CAN1_SHDN_Pin, GPIO_PIN_RESET);
            HAL_Delay(1);

            MX_FDCAN1_Init();
                        
//            // for 80 mhz (from a project by st)
//            hfdcan1.Instance = FDCAN1;
//            hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV2;
//            hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
//            hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
//            hfdcan1.Init.AutoRetransmission = ENABLE;
//            hfdcan1.Init.TransmitPause = ENABLE;
//            hfdcan1.Init.ProtocolException = DISABLE;
//            hfdcan1.Init.NominalPrescaler = 1;
//            hfdcan1.Init.NominalSyncJumpWidth = 16;
//            hfdcan1.Init.NominalTimeSeg1 = 63;
//            hfdcan1.Init.NominalTimeSeg2 = 16;
//            hfdcan1.Init.DataPrescaler = 1;
//            hfdcan1.Init.DataSyncJumpWidth = 4;
//            hfdcan1.Init.DataTimeSeg1 = 5;
//            hfdcan1.Init.DataTimeSeg2 = 4;
//            hfdcan1.Init.StdFiltersNbr = 1;
//            hfdcan1.Init.ExtFiltersNbr = 0;
//            hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
//            if(HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
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
    void FDCAN1_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan1);
    }  
    
    void FDCAN1_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan1);
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
     
   #if   defined(STM32HAL_BOARD_PMC)

// acemor          
        // application @ 128k
        constexpr PROP whole                {{0x08000000,               (512)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (126)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(126*1024),    (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (256+124)*1024,     2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(508*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            
 
#if 0     
        constexpr PROP whole                {{0x08000000,               (512)*1024,         2*1024}}; 
        constexpr PROP bootloader           {{0x08000000+(256+2)*1014,               (128)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(256*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(0*1024),     (252)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            
#endif


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
    #error embot::hw::pga308::thebsp must be defined
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
                  
    #if   defined(STM32HAL_BOARD_PMC)
    
    constexpr PROP i2c1p { &hi2c1, embot::hw::i2c::Speed::standard100 };
    constexpr PROP i2c2p { &hi2c2, embot::hw::i2c::Speed::standard100 };
    constexpr PROP i2c3p { &hi2c3, embot::hw::i2c::Speed::standard100 };
    constexpr PROP i2c4p { &hi2c4, embot::hw::i2c::Speed::standard100 };

        
    constexpr BSP thebsp {        
        // maskofsupported   
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two) | mask::pos2mask<uint32_t>(I2C::three) | mask::pos2mask<uint32_t>(I2C::four),         
        // properties
        {{       
            &i2c1p, &i2c2p, &i2c3p, &i2c4p
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
        else if(h == I2C::three)
        {            
            MX_I2C3_Init();
        } 
        else if(h == I2C::four)
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

#if defined(STM32HAL_BOARD_PMC)

extern "C"
{

void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c2);
}

void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c2);
}

void I2C3_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c3);
}

void I2C3_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c3);
}

void I2C4_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c4);
}

void I2C4_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c4);
}

void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

void DMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}

void DMA1_Channel6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c2_rx);
}

void DMA1_Channel7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c2_tx);
}

void DMA1_Channel8_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c3_rx);
}

void DMA2_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c3_tx);
}

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
#endif // bno055

// - support map: end of embot::hw::bno055



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
           
    #if defined(STM32HAL_BOARD_PMC)
    
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




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

