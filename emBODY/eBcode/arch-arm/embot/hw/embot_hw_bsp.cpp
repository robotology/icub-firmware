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
#include "embot_hw_pga308.h"
#include "embot_hw_timer.h"
#include "embot_hw_adc.h"
#include "embot_hw_si7051.h"
#include "embot_hw_tlv493d.h"

#include "embot_binary.h"

using namespace std;
using namespace embot::binary;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



namespace embot { namespace hw { namespace bsp {
    
    static bool initted = false; 
        
    static embot::common::fpGetU64 s_timenow = nullptr;
    
    static embot::common::Time s_timenowstm32hal()
    {
        return static_cast<embot::common::Time>(embot::common::time1millisec)*HAL_GetTick();
    }

    bool initialised()
    {
        return initted;
    }

    result_t init(const Config &config)
    {
        if(true == embot::hw::bsp::initialised())
        {
            return resOK;
        }
        
        // put whatwever is required for ...        
        stm32hal_config_t cfg = {0};
        cfg.tick1ms_init = config.stm32hal.init1millitick;
        cfg.tick1ms_get = config.stm32hal.get1millitick;
        
        stm32hal_init(&cfg);
        
        s_timenow = config.get1microtime;
        
        if(nullptr == s_timenow)
        {
            s_timenow = s_timenowstm32hal;
        }
        
        initted = true;
        return resOK;
    }
    
    
    embot::common::Time now()
    {
        return s_timenow();
    }
    

}}} // namespace embot { namespace hw { namespace bsp {



namespace embot { namespace hw { namespace bsp { namespace gpio {
        
    // sadly we cannot use constepr because of the reinterpret_cast<> inside GPIOA etc.
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
    
    void BSP::init(embot::hw::GPIO h) const {}
        
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace gpio {


namespace embot { namespace hw { namespace bsp { namespace led {
           
    static_assert(embot::common::tointegral(embot::hw::LED::none) < 8*sizeof(SUPP::supportedmask), "LED::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::LED::maxnumberof) < 8*sizeof(SUPP::supportedmask), "LED::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::LED::maxnumberof) < embot::common::tointegral(embot::hw::LED::none), "LED::maxnumberof must be higher that LED::none, so that we can optimise code");

    
    #if     defined(STM32HAL_BOARD_NUCLEO64)    
           
    constexpr PROP led1p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {LD2_GPIO_Port, LD2_Pin}  };  
        
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
    
    #else
        #error embot::hw::bsp::led::theMAP must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace led {


namespace embot { namespace hw { namespace bsp { namespace button {
           
    static_assert(embot::common::tointegral(embot::hw::BTN::none) < 8*sizeof(SUPP::supportedmask), "BTN::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::BTN::maxnumberof) < 8*sizeof(SUPP::supportedmask), "BTN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::BTN::maxnumberof) < embot::common::tointegral(embot::hw::BTN::none), "BTN::maxnumberof must be higher that BTN::none, so that we can optimise code");


    #if     defined(STM32HAL_BOARD_NUCLEO64)    
        
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::RESET, .gpio = {B1_GPIO_Port, B1_Pin}  };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one),        
        // properties
        {{
            &btn1p            
        }}        
    };
    
    void BSP::init(embot::hw::BTN h) const {}
        

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    
    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    
    #elif   defined(STM32HAL_BOARD_RFE)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}
    
    #elif   defined(STM32HAL_BOARD_PSC)
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::BTN h) const {}

    #else
        #error embot::hw::bsp::button::theMAP must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace button {



#if   defined(HAL_CAN_MODULE_ENABLED)

namespace embot { namespace hw { namespace bsp { namespace can {
           
    static_assert(embot::common::tointegral(embot::hw::CAN::none) < 8*sizeof(SUPP::supportedmask), "CAN::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::CAN::maxnumberof) < 8*sizeof(SUPP::supportedmask), "CAN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::CAN::maxnumberof) < embot::common::tointegral(embot::hw::CAN::none), "CAN::maxnumberof must be higher that CAN::none, so that we can optimise code");

    
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
    
    #else
        #error embot::hw::bsp::can::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace can {
    
// ---------------------------------- IRQhandlers --------------------------------------------------------------------------

void CAN1_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}


#endif // defined(HAL_CAN_MODULE_ENABLED)



namespace embot { namespace hw { namespace bsp { namespace flash {
 
    static_assert(embot::common::tointegral(embot::hw::FLASH::none) < 8*sizeof(SUPP::supportedmask), "FLASH::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::FLASH::maxnumberof) < 8*sizeof(SUPP::supportedmask), "FLASH::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::FLASH::maxnumberof) < embot::common::tointegral(embot::hw::FLASH::none), "FLASH::maxnumberof must be higher that FLASH::none, so that we can optimise code");

    
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               256*1024,           2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application
   
    #elif   defined(STM32HAL_BOARD_MTB4)
           
        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               256*1024,           2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application


    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
        #if defined(STRAIN2_APP_AT_128K)
        
        // strain legacy: application @ 128k and application storage together with sharedstorage
        constexpr PROP whole                {{0x08000000,               256*1024,           2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP applicationstorage   {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // applicationstorage: together with sharedstorage
        constexpr PROP application          {{0x08000000+(128*1024),    (128)*1024,         2*1024}};   // application @ 128k
        
        #else 
        
        // strain2: application @ 080k
        constexpr PROP whole                {{0x08000000,               256*1024,           2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (78)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(78*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(80*1024),     (172)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application    
        
        #endif
        

    #elif   defined(STM32HAL_BOARD_RFE)
    
        // any: application @ 128k but ... applicationstorage on top
        constexpr PROP whole                {{0x08000000,               256*1024,           2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (124)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(124*1024),    (4)*1024,           2*1024}};   // sharedstorage: 4k on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (124)*1024,         2*1024}};   // application @ 128k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: 4k on top of application

    #elif   defined(STM32HAL_BOARD_PSC)
           
        // psc: application @ 080k
        constexpr PROP whole                {{0x08000000,               256*1024,           2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (78)*1024,          2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(78*1024),     (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(80*1024),     (172)*1024,         2*1024}};   // application @ 080k
        constexpr PROP applicationstorage   {{0x08000000+(252*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            
       
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



namespace embot { namespace hw { namespace bsp { namespace pga308 {
           
    static_assert(embot::common::tointegral(embot::hw::PGA308::none) < 8*sizeof(SUPP::supportedmask), "PGA308::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::PGA308::maxnumberof) < 8*sizeof(SUPP::supportedmask), "PGA308::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::PGA308::maxnumberof) < embot::common::tointegral(embot::hw::PGA308::none), "PGA308::maxnumberof must be higher that PGA308::none, so that we can optimise code");


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
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::PGA308 h) const {}

    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace pga308 {



namespace embot { namespace hw { namespace bsp { namespace si7051 {
           
    static_assert(embot::common::tointegral(embot::hw::SI7051::none) < 8*sizeof(SUPP::supportedmask), "SI7051::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::SI7051::maxnumberof) < 8*sizeof(SUPP::supportedmask), "SI7051::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::SI7051::maxnumberof) < embot::common::tointegral(embot::hw::SI7051::none), "SI7051::maxnumberof must be higher that SI7051::none, so that we can optimise code");


    #if defined(STM32HAL_BOARD_STRAIN2) || defined(STM32HAL_BOARD_MTB4)
        
    constexpr PROP prop01 { .i2caddress = 0x80 }; 

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(SI7051::one),        
        // properties
        {{
            &prop01             
        }}        
    };

    void BSP::init(embot::hw::SI7051 h) const {}

    #else
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::SI7051 h) const {}

    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace si7051 {


namespace embot { namespace hw { namespace bsp { namespace onewire {
           
    static_assert(embot::common::tointegral(embot::hw::ONEWIRE::none) < 8*sizeof(SUPP::supportedmask), "ONEWIRE::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::ONEWIRE::maxnumberof) < 8*sizeof(SUPP::supportedmask), "ONEWIRE::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::ONEWIRE::maxnumberof) < embot::common::tointegral(embot::hw::ONEWIRE::none), "ONEWIRE::maxnumberof must be higher that ONEWIRE::none, so that we can optimise code");

    

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
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ONEWIRE h) const {}

    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}} // namespace embot { namespace hw { namespace bsp {  namespace onewire {



#if   defined(HAL_ADC_MODULE_ENABLED)

namespace embot { namespace hw { namespace bsp { namespace adc {
           
    static_assert(embot::common::tointegral(embot::hw::ADC::none) < 8*sizeof(SUPP::supportedmask), "ADC::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::ADC::maxnumberof) < 8*sizeof(SUPP::supportedmask), "ADC::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::ADC::maxnumberof) < embot::common::tointegral(embot::hw::ADC::none), "ADC::maxnumberof must be higher that ADC::none, so that we can optimise code");

    
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
    
    #else
    
    constexpr BSP thebsp { };    
    void BSP::init(embot::hw::ADC h) const {}

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

#endif // defined(HAL_ADC_MODULE_ENABLED)


#if defined(HAL_TIM_MODULE_ENABLED)

namespace embot { namespace hw { namespace bsp { namespace timer {
               
    static_assert(embot::common::tointegral(embot::hw::TIMER::none) < 8*sizeof(SUPP::supportedmask), "TIMER::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::TIMER::maxnumberof) < 8*sizeof(SUPP::supportedmask), "TIMER::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::TIMER::maxnumberof) < embot::common::tointegral(embot::hw::TIMER::none), "TIMER::maxnumberof must be higher that CAN::none, so that we can optimise code");

    
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

    constexpr BSP thebsp { };    
    void BSP::init(embot::hw::TIMER h) const {}
   
    #elif   defined(STM32HAL_BOARD_MTB4)
       
    // sadly we cannot use constepr because of the reinterpret_cast<> inside TIM6 etc.
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

    // sadly we cannot use constepr because of the reinterpret_cast<> inside TIM6 etc.
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

    // sadly we cannot use constepr because of the reinterpret_cast<> inside TIM6 etc.
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

#endif // defined(HAL_TIM_MODULE_ENABLED)


#if defined(HAL_I2C_MODULE_ENABLED)

namespace embot { namespace hw { namespace bsp { namespace i2c {
           
    static_assert(embot::common::tointegral(embot::hw::I2C::none) < 8*sizeof(SUPP::supportedmask), "I2C::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::I2C::maxnumberof) < 8*sizeof(SUPP::supportedmask), "I2C::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::I2C::maxnumberof) < embot::common::tointegral(embot::hw::I2C::none), "I2C::maxnumberof must be higher that I2C::none, so that we can optimise code");

    
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
        
    
    #else 
       
    constexpr BSP thebsp { };    
    void BSP::init(embot::hw::I2C h) const {}
    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace i2c {

// ---------------------------------- IRQhandlers --------------------------------------------------------------------------


#if defined(STM32HAL_BOARD_MTB4) | defined(STM32HAL_BOARD_STRAIN2) | defined(STM32HAL_BOARD_RFE)

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


#endif

#endif // defined(HAL_I2C_MODULE_ENABLED)




namespace embot { namespace hw { namespace bsp { namespace bno055 {
           
    static_assert(embot::common::tointegral(embot::hw::BNO055::none) < 8*sizeof(SUPP::supportedmask), "BNO055::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::BNO055::maxnumberof) < 8*sizeof(SUPP::supportedmask), "BNO055::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::BNO055::maxnumberof) < embot::common::tointegral(embot::hw::BNO055::none), "BNO055::maxnumberof must be higher that BNO055::none, so that we can optimise code");

    
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
        
    #else
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BNO055 h) const {}
        
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace bno055 {



namespace embot { namespace hw { namespace bsp { namespace tlv493d {
           
    static_assert(embot::common::tointegral(embot::hw::TLV493D::none) < 8*sizeof(SUPP::supportedmask), "TLV493D::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::TLV493D::maxnumberof) < 8*sizeof(SUPP::supportedmask), "TLV493D::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(embot::common::tointegral(embot::hw::TLV493D::maxnumberof) < embot::common::tointegral(embot::hw::TLV493D::none), "TLV493D::maxnumberof must be higher that TLV493D::none, so that we can optimise code");


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

    #else
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TLV493D h) const {}

    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace tlv493d {


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

