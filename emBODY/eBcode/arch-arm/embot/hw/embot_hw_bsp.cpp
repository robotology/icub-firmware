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
        
    bool MAP::supported(const embot::hw::GPIO &h) const 
    { 
        return isvalid(h);
    }
    
    
    bool MAP::isvalid(const embot::hw::GPIO &h) const 
    { 
        if((nullptr == h.port) || (0 == h.pin)) 
        { 
            return false; 
        } 
        else 
        {
            return true;
        }
    }
    
    static const MAP theMAP = { 0 };
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace gpio {


namespace embot { namespace hw { namespace bsp { namespace led {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::LED::none) < 8*sizeof(MAP::mask), "LED::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::LED::maxnumberof) < 8*sizeof(MAP::mask), "LED::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::LED::maxnumberof) < static_cast<std::uint32_t>(embot::hw::LED::none), "LED::maxnumberof must be higher that LED::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::LED h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    
    const embot::hw::GPIO * MAP::getgpio(embot::hw::LED h) const
    {
        return &gpio[toindex(h)];
    }
    
    bool MAP::isvalid(embot::hw::LED h) const
    {
        if(supported(h))
        {   // i am sure that getgpio() returns something inside gpio[]...
            return embot::hw::bsp::gpio::getMAP()->isvalid(*getgpio(h));
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::LED h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    
    // and now the maps

    #if     defined(STM32HAL_BOARD_NUCLEO64)    
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(LED::one),
        .on   = embot::hw::gpio::State::SET, 
        .off  = embot::hw::gpio::State::RESET,
        .gpio = {   
            {LD2_GPIO_Port, LD2_Pin}, 
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
   

    #elif   defined(STM32HAL_BOARD_MTB4)
    
        static const MAP theMAP = 
        {
            .mask = mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two),
            .on   = embot::hw::gpio::State::RESET, 
            .off  = embot::hw::gpio::State::SET,
            .gpio = {   
                {LED_RED_GPIO_Port, LED_RED_Pin}, 
                {LED_BLUE_GPIO_Port, LED_BLUE_Pin},
                {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, 
            }        
        };
    
    
    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    static const MAP theMAP = 
    {        
        .mask = mask::pos2mask<uint32_t>(LED::one),
        .on   = embot::hw::gpio::State::RESET, 
        .off  = embot::hw::gpio::State::SET,
        .gpio = {   
            {LED_GPIO_Port, LED_Pin}, 
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
    
    #elif   defined(STM32HAL_BOARD_RFE)
    
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(LED::one),
        .on   = embot::hw::gpio::State::RESET, 
        .off  = embot::hw::gpio::State::SET,
        .gpio = {   
            {LED_BLUE_GPIO_Port, LED_BLUE_Pin}, 
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
    
    #else
        #error embot::hw::bsp::led::theMAP must be defined    
    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace led {


namespace embot { namespace hw { namespace bsp { namespace button {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::BTN::none) < 8*sizeof(MAP::mask), "BTN::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::BTN::maxnumberof) < 8*sizeof(MAP::mask), "BTN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::BTN::maxnumberof) < static_cast<std::uint32_t>(embot::hw::BTN::none), "BTN::maxnumberof must be higher that BTN::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::BTN h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    
    const embot::hw::GPIO * MAP::getgpio(embot::hw::BTN h) const
    {
        return &gpio[toindex(h)];
    }
    
    bool MAP::isvalid(embot::hw::BTN h) const
    {
        if(supported(h))
        {   // i am sure that getgpio() returns something inside gpio[]...
            return embot::hw::bsp::gpio::getMAP()->isvalid(*getgpio(h));
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::BTN h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    
    // and now the maps

    #if     defined(STM32HAL_BOARD_NUCLEO64)    
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(BTN::one),
        .pressed = embot::hw::gpio::State::RESET,
        .gpio = {
            {B1_GPIO_Port, B1_Pin},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0}
        } 
    };
   

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const MAP theMAP = 
    {
        .mask = 0,
        .pressed = embot::hw::gpio::State::RESET,
        .gpio = {
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0}
        }       
    };
    
    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    static const MAP theMAP = 
    {
        .mask = 0,
        .pressed = embot::hw::gpio::State::RESET,
        .gpio = {
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0}
        }        
    };
    
    #elif   defined(STM32HAL_BOARD_RFE)
    
    static const MAP theMAP = 
    {
        .mask = 0,
        .pressed = embot::hw::gpio::State::RESET,
        .gpio = {
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0},
            {nullptr, 0}
        }      
    };
    
    #else
        #error embot::hw::bsp::button::theMAP must be defined    
    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace button {



#if   defined(HAL_CAN_MODULE_ENABLED)

namespace embot { namespace hw { namespace bsp { namespace can {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::CAN::none) < 8*sizeof(MAP::mask), "CAN::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::CAN::maxnumberof) < 8*sizeof(MAP::mask), "CAN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::CAN::maxnumberof) < static_cast<std::uint32_t>(embot::hw::CAN::none), "CAN::maxnumberof must be higher that CAN::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::CAN h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::CAN h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::CAN h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    const STM32PROPS * const MAP::getprops(embot::hw::CAN h) const
    {
        if(false == supported(h))
        {
            return nullptr;
        }
        return stm32props[toindex(h)];        
    }
    
    
    // and now the maps

    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

    static const MAP theMAP = 
    {
        .mask = 0,
        .stm32props = { nullptr }
    };
    
    void MAP::init(embot::hw::CAN h) const {}
   
    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const STM32PROPS can1p = { .handle = &hcan1 };    
    
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(CAN::one),
        .stm32props = { &can1p }
    };
    

    void MAP::init(embot::hw::CAN h) const
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }         
    } 

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    static const STM32PROPS can1p = { .handle = &hcan1 };    
    
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(CAN::one),
        .stm32props = { &can1p }
    };
    

    void MAP::init(embot::hw::CAN h) const
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }         
    }    

    #elif   defined(STM32HAL_BOARD_RFE)
    
    static const STM32PROPS can1p = { .handle = &hcan1 };    
    
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(CAN::one),
        .stm32props = { &can1p }
    };
    

    void MAP::init(embot::hw::CAN h) const
    {
        if(h == CAN::one)
        {            
            MX_CAN1_Init();
        }         
    } 

    #else
        #error embot::hw::bsp::can::theMAP must be defined    
    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
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
 
    static_assert(static_cast<std::uint32_t>(embot::hw::FLASH::none) < 8*sizeof(MAP::mask), "FLASH::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::FLASH::maxnumberof) < 8*sizeof(MAP::mask), "FLASH::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::FLASH::maxnumberof) < static_cast<std::uint32_t>(embot::hw::FLASH::none), "FLASH::maxnumberof must be higher that FLASH::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::FLASH h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::FLASH h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::FLASH h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    const embot::hw::flash::Partition& MAP::getpartition(embot::hw::FLASH h) const
    {
        if(false == supported(h))
        {
            static const embot::hw::flash::Partition dummy { 0, 0, 0};  
            return dummy;
        }
        
        return partitions[toindex(h)];        
    }
    
    
    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

        static const MAP theMAP = 
        {
            .mask = mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
                    mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),
            .partitions = { 
                {0x08000000, 1024*1024, 2*1024},         // whole
                {0x08000000, (124)*1024, 2*1024},       // bootloader
                {0x08020000, (128)*1024, 2*1024},       // application           
                {0x0801F000, (4)*1024,   4*1024},       // sharedstorage: on top of bootloader
                {0x0801F000, (4)*1024,   4*1024},       // applicationstorage: on top of bootloader
            }
        };
   
    #elif   defined(STM32HAL_BOARD_MTB4)
           
        static const MAP theMAP = 
        {
            .mask = mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
                    mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),
            .partitions = { 
                {0x08000000, 256*1024, 2*1024},         // whole
                {0x08000000, (124)*1024, 2*1024},       // bootloader
                {0x08020000, (128)*1024, 2*1024},       // application           
                {0x0801F000, (4)*1024,   4*1024},       // sharedstorage: on top of bootloader
                {0x0801F000, (4)*1024,   4*1024},       // applicationstorage: on top of bootloader
            }
        };

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
        #if defined(STRAIN2_APP_AT_128K)
         // at 128k
        static const MAP theMAP = 
        {
            .mask = mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
                    mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),
            .partitions = { 
                {0x08000000, 256*1024, 2*1024},         // whole
                {0x08000000, (124)*1024, 2*1024},       // bootloader
                {0x08020000, (128)*1024, 2*1024},       // application           
                {0x0801F000, (4)*1024,   4*1024},       // sharedstorage: on top of bootloader
                {0x0801F000, (4)*1024,   4*1024},       // applicationstorage: on top of bootloader
            }
        };      
        #else 
        // at 80k
        static const MAP theMAP = 
        {
            .mask = mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
                    mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),
            .partitions = { 
                {0x08000000, 256*1024, 2*1024},         // whole
                {0x08000000, (80-2)*1024, 2*1024},      // bootloader
                {0x08014000, (256-80-4)*1024, 2*1024},  // application           
                {0x08013800, 2*1024,   2*1024},         // sharedstorage: on top of bootloader
                {0x0803F000, 4*1024,   2*1024},         // applicationstorage: on top of applciation
            }
        };
        #endif
        

    #elif   defined(STM32HAL_BOARD_RFE)
    
        static const MAP theMAP = 
        {
            .mask = mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
                    mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),
            .partitions = { 
                {0x08000000, 256*1024, 2*1024},         // whole
                {0x08000000, (124)*1024, 2*1024},       // bootloader
                {0x08020000, (128)*1024, 2*1024},       // application           
                {0x0801F000, (4)*1024,   4*1024},       // sharedstorage: on top of bootloader
                {0x0801F000, (4)*1024,   4*1024},       // applicationstorage: on top of bootloader
            }
        };

    #else
        #error embot::hw::bsp::flash::theMAP must be defined    
    #endif    
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace flash {



namespace embot { namespace hw { namespace bsp { namespace pga308 {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::PGA308::none) < 8*sizeof(MAP::mask), "PGA308::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::PGA308::maxnumberof) < 8*sizeof(MAP::mask), "PGA308::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::PGA308::maxnumberof) < static_cast<std::uint32_t>(embot::hw::PGA308::none), "PGA308::maxnumberof must be higher that PGA308::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::PGA308 h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::PGA308 h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::PGA308 h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    
    // and now the maps

    // const support maps

    #if defined(STM32HAL_BOARD_STRAIN2)
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(PGA308::one) | mask::pos2mask<uint32_t>(PGA308::two) | mask::pos2mask<uint32_t>(PGA308::three) |
                mask::pos2mask<uint32_t>(PGA308::four) | mask::pos2mask<uint32_t>(PGA308::five) | mask::pos2mask<uint32_t>(PGA308::six)
    };

    #else
    
    static const MAP theMAP = 
    {
        .mask = 0
    };

    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace pga308 {



namespace embot { namespace hw { namespace bsp { namespace si7051 {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::SI7051::none) < 8*sizeof(MAP::mask), "SI7051::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::SI7051::maxnumberof) < 8*sizeof(MAP::mask), "SI7051::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::SI7051::maxnumberof) < static_cast<std::uint32_t>(embot::hw::SI7051::none), "SI7051::maxnumberof must be higher that SI7051::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::SI7051 h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::SI7051 h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::SI7051 h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    
    // and now the maps

    // const support maps

    #if defined(STM32HAL_BOARD_STRAIN2) || defined(STM32HAL_BOARD_MTB4)
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(SI7051::one) | mask::pos2mask<uint32_t>(SI7051::two)
    };

    #else
    
    static const MAP theMAP = 
    {
        .mask = 0
    };

    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace si7051 {


namespace embot { namespace hw { namespace bsp { namespace onewire {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::ONEWIRE::none) < 8*sizeof(MAP::mask), "ONEWIRE::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::ONEWIRE::maxnumberof) < 8*sizeof(MAP::mask), "ONEWIRE::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::ONEWIRE::maxnumberof) < static_cast<std::uint32_t>(embot::hw::ONEWIRE::none), "ONEWIRE::maxnumberof must be higher that ONEWIRE::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::ONEWIRE h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::ONEWIRE h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::ONEWIRE h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    
    // and now the maps

    // const support maps

    #if defined(STM32HAL_BOARD_STRAIN2)
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(ONEWIRE::one) | mask::pos2mask<uint32_t>(ONEWIRE::two) | mask::pos2mask<uint32_t>(ONEWIRE::three) |
                mask::pos2mask<uint32_t>(ONEWIRE::four) | mask::pos2mask<uint32_t>(ONEWIRE::five) | mask::pos2mask<uint32_t>(ONEWIRE::six)
    };

    #else
    
    static const MAP theMAP = 
    {
        .mask = 0
    };

    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace onewire {



#if   defined(HAL_ADC_MODULE_ENABLED)

namespace embot { namespace hw { namespace bsp { namespace adc {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::ADC::none) < 8*sizeof(MAP::mask), "ADC::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::ADC::maxnumberof) < 8*sizeof(MAP::mask), "ADC::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::ADC::maxnumberof) < static_cast<std::uint32_t>(embot::hw::ADC::none), "ADC::maxnumberof must be higher that ADC::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::ADC h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::ADC h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::ADC h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    const STM32PROPS * const MAP::getprops(embot::hw::ADC h) const
    {
        if(false == supported(h))
        {
            return nullptr;
        }
        return stm32props[toindex(h)];        
    }
    
    
    // and now the maps

    // const support maps

    #if defined(STM32HAL_BOARD_STRAIN2)
    
    static const STM32PROPS adc1p = { .handle = &hadc1 };
           
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(ADC::one),
        .stm32props = { &adc1p }
    };
    
    void MAP::init(embot::hw::ADC h) const
    {
        // init peripherals: adc1 and dma1
        MX_ADC1_Init();
        // dma is globally initted by stm32hal_bsp_init() because it holds all dma peripherals
        //MX_DMA_Init();         
    }

    #else
    
    static const MAP theMAP = 
    {
        .mask = 0,
        .stm32props = { nullptr }
    };
    
    void MAP::init(embot::hw::ADC h) const {}

    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
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
           
    static_assert(static_cast<std::uint32_t>(embot::hw::TIMER::none) < 8*sizeof(MAP::mask), "TIMER::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::TIMER::maxnumberof) < 8*sizeof(MAP::mask), "TIMER::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::TIMER::maxnumberof) < static_cast<std::uint32_t>(embot::hw::TIMER::none), "TIMER::maxnumberof must be higher that CAN::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::TIMER h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::TIMER h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::TIMER h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    const STM32PROPS * const MAP::getprops(embot::hw::TIMER h) const
    {
        if(false == supported(h))
        {
            return nullptr;
        }
        return stm32props[toindex(h)];        
    }
    
    
    // and now the maps

    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

    static const MAP theMAP = 
    {
        .mask       = 0,
        .stm32props = { 
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16   
        }    
    };
   
    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const STM32PROPS tim06p = { .TIMx = TIM6,  .handle = &htim6,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
           
    static const MAP theMAP = 
    {
        .mask       = mask::pos2mask<uint32_t>(TIMER::six),
        .stm32props = { 
            nullptr, nullptr, nullptr, nullptr, nullptr, &tim06p, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16  
        }            
    };

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    

    static const STM32PROPS tim06p = { .TIMx = TIM6,  .handle = &htim6,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
    static const STM32PROPS tim07p = { .TIMx = TIM7,  .handle = &htim7,  .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = true };
    static const STM32PROPS tim15p = { .TIMx = TIM15, .handle = &htim15, .clock = embot::hw::CLOCK::syscore, .isonepulse = true,  .mastermode = false };
    static const STM32PROPS tim16p = { .TIMx = TIM16, .handle = &htim16, .clock = embot::hw::CLOCK::syscore, .isonepulse = false, .mastermode = false };
           
    static const MAP theMAP = 
    {
        .mask       = mask::pos2mask<uint32_t>(TIMER::six) | mask::pos2mask<uint32_t>(TIMER::seven) | 
                      mask::pos2mask<uint32_t>(TIMER::fifteen) | mask::pos2mask<uint32_t>(TIMER::sixteen),
        .stm32props = {
            nullptr, nullptr, nullptr, nullptr, nullptr, &tim06p, &tim07p, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &tim15p, &tim16p      // from 9 to 16 
        }           
    };

    #elif   defined(STM32HAL_BOARD_RFE)
    
    static const MAP theMAP = 
    {
        .mask       = 0,
        .stm32props = { 
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16   
        }         
    };

    #else
        #error embot::hw::bsp::timer::theMAP must be defined    
    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace timer {

#if defined(STM32HAL_BOARD_STRAIN2) 
    #define STM32HAL_HAS_TIM6
    #define STM32HAL_HAS_TIM7
    #define STM32HAL_HAS_TIM15
    #define STM32HAL_HAS_TIM16
#elif defined(STM32HAL_BOARD_MTB4)
    #define STM32HAL_HAS_TIM6
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
           
    static_assert(static_cast<std::uint32_t>(embot::hw::I2C::none) < 8*sizeof(MAP::mask), "I2C::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::I2C::maxnumberof) < 8*sizeof(MAP::mask), "I2C::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::I2C::maxnumberof) < static_cast<std::uint32_t>(embot::hw::I2C::none), "I2C::maxnumberof must be higher that I2C::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::I2C h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::I2C h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::I2C h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    const STM32PROPS * const MAP::getprops(embot::hw::I2C h) const
    {
        if(false == supported(h))
        {
            return nullptr;
        }
        return stm32props[toindex(h)];        
    }
    
    
    // and now the maps

    // const support maps
    #if defined(STM32HAL_BOARD_MTB4)
    
    static const STM32PROPS i2c1p = { .handle = &hi2c1, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    static const STM32PROPS i2c2p = { .handle = &hi2c2, .handledmatx = &hdma_i2c2_tx, .handledmarx = &hdma_i2c2_rx };
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two),
        .stm32props = { &i2c1p, &i2c2p }       
    };
    
    void MAP::init(embot::hw::I2C h) const
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
         
    static const STM32PROPS i2c1p = { .handle = &hi2c1, .handledmatx = &hdma_i2c1_tx, .handledmarx = &hdma_i2c1_rx };
    static const STM32PROPS i2c2p = { .handle = &hi2c2, .handledmatx = &hdma_i2c2_tx, .handledmarx = &hdma_i2c2_rx };
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two),
        .stm32props = { &i2c1p, &i2c2p }       
    };
    
    void MAP::init(embot::hw::I2C h) const
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

    #else 
    
    static const MAP theMAP = 
    {
        .mask = 0,
        .stm32props = { nullptr, nullptr }
    };
    
    void MAP::init(embot::hw::I2C h) const {}
    
    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace i2c {

// ---------------------------------- IRQhandlers --------------------------------------------------------------------------


#if defined(STM32HAL_BOARD_MTB4) | defined(STM32HAL_BOARD_STRAIN2)

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

#endif

#endif // defined(HAL_I2C_MODULE_ENABLED)




namespace embot { namespace hw { namespace bsp { namespace bno055 {
           
    static_assert(static_cast<std::uint32_t>(embot::hw::BNO055::none) < 8*sizeof(MAP::mask), "BNO055::none must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::BNO055::maxnumberof) < 8*sizeof(MAP::mask), "BNO055::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
    static_assert(static_cast<std::uint32_t>(embot::hw::BNO055::maxnumberof) < static_cast<std::uint32_t>(embot::hw::BNO055::none), "BNO055::maxnumberof must be higher that BNO055::none, so that we can optimise code");

    
    bool MAP::supported(embot::hw::BNO055 h) const 
    { 
        return embot::binary::bit::check(mask, toindex(h)); 
    }
    

    bool MAP::isvalid(embot::hw::BNO055 h) const
    {
        if(supported(h))
        {   
            return true;
        }
        return false;        
    }
    
    std::uint8_t MAP::toindex(embot::hw::BNO055 h) 
    { 
        return static_cast<uint8_t>(h); 
    }
    
    
    // and now the maps

    // const support maps

    #if defined(STM32HAL_BOARD_STRAIN2) || defined(STM32HAL_BOARD_MTB4)
        
    static const MAP theMAP = 
    {
        .mask = mask::pos2mask<uint32_t>(BNO055::one)
    };

    #else
    
    static const MAP theMAP = 
    {
        .mask = 0
    };

    #endif
    
    const MAP * const getMAP()
    {
        return &theMAP;
    }
              
}}}} // namespace embot { namespace hw { namespace bsp {  namespace bno055 {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

