
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


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_stm32g4eval_config.h"


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
 
    #if defined(STM32HAL_BOARD_STM32G4EVAL)
    static const BSP thebsp {        
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
    
    #if   defined(STM32HAL_BOARD_STM32G4EVAL)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::nine}  };  
    constexpr PROP led3p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::eleven}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::three),        
        // properties
        {{
            &led1p, nullptr, &led3p, nullptr, nullptr, nullptr, nullptr, nullptr            
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
               
    #if   defined(STM32HAL_BOARD_STM32G4EVAL)
        
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

namespace embot { namespace hw { namespace flash { namespace bsp {
    
    constexpr BSP thebsp { };
    void BSP::init() const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else
 
namespace embot { namespace hw { namespace flash { namespace bsp {
     
#if   defined(STM32HAL_BOARD_STM32G4EVAL)
         
    #if defined(EMBOT_ENABLE_hw_flash_DUALBANK)
        #error EMBOT_ENABLE_hw_flash_DUALBANK should not be enabled
        // must configure differently the bps, but ... dont define this mcro
    #endif
    // note:
    // the stm32g4 has a page size of:
    // - 4k when its flash is configured as single bank
    // - 2k when its flash is configured as two banks
    // we have used 4k because at the time did not have support for dual bank added only in nov 2022
    
    constexpr uint8_t numbanks {1};
    constexpr uint32_t banksize {512*1024};
    constexpr uint32_t pagesize {4*1024};
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr theBanks thebanks 
    {
        numbanks, 
        { &bank01, nullptr }
    }; 
    
    // on on top of each other, with sizes:
    constexpr std::array<uint32_t, 4> ss = {124*1024, 4*1024, 380*1024, 4*1024};
    constexpr Partition btl {Partition::ID::bootloader,         &bank01,    bank01.address,         ss[0]}; 
    constexpr Partition sha {Partition::ID::sharedstorage,      &bank01,    btl.address+btl.size,   ss[1]};
    constexpr Partition app {Partition::ID::application,        &bank01,    sha.address+sha.size,   ss[2]}; 
    constexpr Partition stg {Partition::ID::applicationstorage, &bank01,    app.address+app.size,   ss[3]}; 
    
    constexpr thePartitions thepartitions
    {
        { &btl, &sha, &app, &stg }
    };
        
    constexpr BSP thebsp {        
        thebanks,
        thepartitions
    };        
    
    void BSP::init() const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }        
        
#else
    #error embot::hw::flash::thebsp must be defined    
#endif   
              
}}}} // namespace embot { namespace hw { namespace flash { namespace bsp {

#endif // flash

// - support map: end of embot::hw::flash


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
                  
    #if   defined(STM32HAL_BOARD_STM32G4EVAL)
    
    constexpr PROP i2c3p { &hi2c3, embot::hw::i2c::Speed::fast400 };
        
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
        #error embot::hw::i2c::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace i2c {


// irq handlers of i2c

#if defined(STM32HAL_BOARD_STM32G4EVAL)

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



// - support map: begin of embot::hw::i2ce

#include "embot_hw_i2ce.h"
#include "embot_hw_i2ce_bsp.h"
#include "embot_hw_gpio.h"

#if !defined(EMBOT_ENABLE_hw_i2ce)

namespace embot { namespace hw { namespace i2ce { namespace bsp {
    
    constexpr BSP thebsp { };
    bool BSP::init(embot::hw::I2CE h, const Config &config) const { return true;}
    bool BSP::deinit(embot::hw::I2CE h) const { return true;}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace i2ce { namespace bsp {
    
    #if defined(STM32HAL_BOARD_STM32G4EVAL)
    
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinout1 = { {
        {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::eight},        // scl
        {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::nine}          // sda
    } };
    constexpr PROP i2ce1p = { Speed::standard100, pinout1 }; 
    
 #if 0   
 
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinout2 = { {
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four},     // scl
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::seven}     // sda
    } };
    constexpr PROP i2ce2p = { Speed::standard100, pinout2 };     
    
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinout3 = { {
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four},     // scl
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::six}       // sda
    } };
    constexpr PROP i2ce3p = { Speed::standard100, pinout3 };     
    
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinout4 = { {
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four},     // scl
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five}      // sda
    } };
    constexpr PROP i2ce4p = { Speed::standard100, pinout4 }; 
    
#endif   

    
    constexpr BSP thebsp {        
        // maskofsupported
//        mask::pos2mask<uint32_t>(I2CE::one) | mask::pos2mask<uint32_t>(I2CE::two) | 
//        mask::pos2mask<uint32_t>(I2CE::three) | mask::pos2mask<uint32_t>(I2CE::four),    
        mask::pos2mask<uint32_t>(I2CE::one),
        // properties
        {{
//            &i2ce1p, &i2ce2p, &i2ce3p, &i2ce4p  
        &i2ce1p, nullptr, nullptr, nullptr           
        }}        
    };
     
    
    
    bool BSP::init(embot::hw::I2CE h, const Config &config) const
    { 
        static constexpr embot::hw::gpio::Config out 
        { 
            embot::hw::gpio::Mode::OUTPUTpushpull, 
            embot::hw::gpio::Pull::nopull, 
            embot::hw::gpio::Speed::veryhigh 
        };    
        
        embot::hw::GPIO scl = embot::hw::i2ce::bsp::getBSP().getPROP(h)->pinout[0];
        embot::hw::GPIO sda = embot::hw::i2ce::bsp::getBSP().getPROP(h)->pinout[1];
        
        switch(h)
        {
            case I2CE::one:
            case I2CE::two:
            case I2CE::three:
            case I2CE::four:
            {
                embot::hw::gpio::init(scl, out);
                embot::hw::gpio::init(sda, out);
            } break;
            
            default: {} break;                            
        }
        
        return true; 
    }

    bool BSP::deinit(embot::hw::I2CE h) const
    { 

        embot::hw::GPIO scl = embot::hw::i2ce::bsp::getBSP().getPROP(h)->pinout[0];
        embot::hw::GPIO sda = embot::hw::i2ce::bsp::getBSP().getPROP(h)->pinout[1];
        
        switch(h)
        {
            case I2CE::one:
            case I2CE::two:
            case I2CE::three:
            case I2CE::four:
            {
                embot::hw::gpio::deinit(scl);
                embot::hw::gpio::deinit(sda);
            } break;
            
            default: {} break;                            
        }        
        
        return true;        
    }
    
    #else
        #error embot::hw::bsp::spi::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

extern "C"
{
    // TODO: add in here any C function we may need
    
}

extern "C"
{
    // TODO: add in here any irq handler we may beed
}

#endif // i2ce

// - support map: end of embot::hw::i2ce



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
           
    #if defined(STM32HAL_BOARD_STM32G4EVAL)

#if defined(EMBOT_ENABLE_hw_tlv493d_i2ceMODE)
    // we use emulated i2c on bus one
    constexpr PROP prop01 { embot::hw::I2Cdescriptor{embot::hw::ANY::one,   0xBC} };


    constexpr BSP thebsp {     

        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one),        
        // properties
        {{
            &prop01
        }}    
    };
    
#else
    // we use a proper I2C    
    constexpr PROP prop01 { embot::hw::I2Cdescriptor{embot::hw::ANY::three,   0xBC} };
    constexpr PROP prop02fake { embot::hw::I2Cdescriptor{embot::hw::ANY::three,   0x02} };
    constexpr PROP prop03fake { embot::hw::I2Cdescriptor{embot::hw::ANY::three,   0x03} };
    constexpr PROP prop04fake { embot::hw::I2Cdescriptor{embot::hw::ANY::three,   0x04} };
    constexpr PROP prop05fake { embot::hw::I2Cdescriptor{embot::hw::ANY::three,   0x05} };
    constexpr PROP prop06fake { embot::hw::I2Cdescriptor{embot::hw::ANY::three,   0x06} };


    constexpr BSP thebsp {     
#if !defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)     
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one),        
        // properties
        {{
            &prop01
        }}
#else
        // maskofsupported
        mask::pos2mask<uint32_t>(TLV493D::one) |  mask::pos2mask<uint32_t>(TLV493D::two) |  mask::pos2mask<uint32_t>(TLV493D::three) |
        mask::pos2mask<uint32_t>(TLV493D::four) |  mask::pos2mask<uint32_t>(TLV493D::five) |  mask::pos2mask<uint32_t>(TLV493D::six),        
        // properties
        {{
            &prop01, &prop02fake, &prop03fake, &prop04fake, &prop05fake, &prop06fake 
        }}        
#endif        
    };
    
#endif    
    
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




// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------


void leds_init_off()
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

void leds_set(bool on)
{
    using namespace embot::hw;
    const led::BSP &b = led::getBSP();
    constexpr std::array<LED, embot::core::tointegral(LED::maxnumberof)> leds {LED::one, LED::two, LED::three, LED::four, LED::five, LED::six, LED::seven , LED::eight};
    for(const auto &l : leds)
    {
        const led::PROP *p = b.getPROP(l);
        if(nullptr != p)
        {
            gpio::set(p->gpio, on ? p->on : p->off);
        }
    }    
}

void verify_flash_bank()
{
    FLASH_OBProgramInitTypeDef odb = {0};   
    HAL_FLASHEx_OBGetConfig(&odb);
    
    uint8_t detectedbanks = ((odb.USERConfig & FLASH_OPTR_DBANK) == FLASH_OPTR_DBANK) ? 2 : 1;

#if defined(EMBOT_ENABLE_hw_flash_DUALBANK)
    constexpr uint8_t expectedbanks = 2;
#else
    constexpr uint8_t expectedbanks = 1;
#endif
    
    if(expectedbanks != detectedbanks)
    {
        embot::core::print("number of banks is not as expected: detected " + std::to_string(detectedbanks));
        embot::core::print("cannot continue");
        
        leds_init_off();
        for(;;)
        {
            leds_set(true);
            HAL_Delay(250);
            leds_set(false);
            HAL_Delay(250);
        }
    }        
}

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() 
{ 
    leds_init_off();
    
    verify_flash_bank();
    
    return true; 
}
#else   
bool embot::hw::bsp::specialize() { return true; }
#endif  //EMBOT_ENABLE_hw_bsp_specialize



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

