
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_GPIO_BSP_H_
#define _EMBOT_HW_GPIO_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"


namespace embot { namespace hw { namespace gpio {
    
    #if defined(HAL_GPIO_MODULE_ENABLED)
        using GPIO_t = GPIO_TypeDef;
    #else
        using GPIO_t = void;
    #endif
    
    struct PROP
    {
        enum : uint16_t {  stmPIN00 = 0x0001, stmPIN01 = 0x0002, stmPIN02 = 0x0004, stmPIN03 = 0x0008, 
                stmPIN04 = 0x0010, stmPIN05 = 0x0020, stmPIN06 = 0x0040, stmPIN07 = 0x0080,
                stmPIN08 = 0x0100, stmPIN09 = 0x0200, stmPIN10 = 0x0400, stmPIN11 = 0x0800,
                stmPIN12 = 0x1000, stmPIN13 = 0x2000, stmPIN14 = 0x4000, stmPIN15 = 0x8000,
                stmPINall = 0xFFFF, stmPINnone = 0x0000
        }; // they are GPIO_PIN_0, GPIO_PIN_1, etc. without the need of including stm32hal.h

        GPIO_t* stmport {nullptr};  // GPIOA, etc.
        std::uint16_t stmpin {PROP::stmPINnone};   // GPIO_PIN_0, GPIO_PIN_1, etc.  or ... stmPIN00xx    
        embot::core::fpWorker clkena {nullptr};    // [](){__HAL_RCC_GPIOA_CLK_ENABLE();} etc.    
        constexpr PROP() : stmport(nullptr), stmpin(0) {}
        constexpr PROP(GPIO_t* po, std::uint16_t pi, embot::core::fpWorker ce) : stmport(po), stmpin(pi), clkena(ce) {}
        constexpr bool isvalid() const { return (nullptr == stmport) ? false : true; }
        constexpr void clockenable() const { if(nullptr != clkena) { clkena(); } }
    };
    
    struct BSP
    {
        constexpr static std::uint8_t maxnumberofPORTs = embot::core::tointegral(embot::hw::GPIO::PORT::maxnumberof);
        constexpr BSP(std::array<std::uint16_t, maxnumberofPORTs> msk, 
                      std::array<GPIO_t*, maxnumberofPORTs> po, 
                      std::array<embot::core::fpWorker, maxnumberofPORTs> sc) 
                        : supportmask2d(msk), ports(po), startclocks(sc) {}
        constexpr BSP() : supportmask2d({0}), ports({0}), startclocks({0}) {}            
          
        std::array<std::uint16_t, maxnumberofPORTs> supportmask2d;
        std::array<GPIO_t*, maxnumberofPORTs> ports;  
        std::array<embot::core::fpWorker, maxnumberofPORTs> startclocks;

        constexpr bool supported(embot::hw::GPIO h) const
        { 
            if(embot::core::tointegral(h.port) >= maxnumberofPORTs) 
            {
                return false; 
            }
            if(embot::core::tointegral(h.pin) >= embot::core::tointegral(embot::hw::GPIO::PIN::maxnumberof)) 
            {
                return false; 
            }
            return embot::core::binary::bit::check(supportmask2d[embot::core::tointegral(h.port)], embot::core::tointegral(h.pin));
        }
        
        constexpr PROP getPROP(embot::hw::GPIO h) const 
        { 
            PROP p{}; 
            if(supported(h)) { p.stmport = ports[embot::core::tointegral(h.port)]; p.stmpin = 1 << embot::core::tointegral(h.pin); p.clkena = startclocks[embot::core::tointegral(h.port)]; }
            return p;
        }
        
//        constexpr embot::hw::GPIO getGPIO(const PROP &p) const;
        constexpr embot::hw::GPIO getGPIO(const embot::hw::gpio::PROP &p) const
        {
            
            embot::hw::GPIO gpio {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};
            switch(p.stmpin)
            {
                case PROP::stmPIN00:  { gpio.pin = embot::hw::GPIO::PIN::zero; } break;
                case PROP::stmPIN01:  { gpio.pin = embot::hw::GPIO::PIN::one; } break;
                case PROP::stmPIN02:  { gpio.pin = embot::hw::GPIO::PIN::two; } break;
                case PROP::stmPIN03:  { gpio.pin = embot::hw::GPIO::PIN::three; } break;
                case PROP::stmPIN04:  { gpio.pin = embot::hw::GPIO::PIN::four; } break;
                case PROP::stmPIN05:  { gpio.pin = embot::hw::GPIO::PIN::five; } break;
                case PROP::stmPIN06:  { gpio.pin = embot::hw::GPIO::PIN::six; } break;
                case PROP::stmPIN07:  { gpio.pin = embot::hw::GPIO::PIN::seven; } break;
                case PROP::stmPIN08:  { gpio.pin = embot::hw::GPIO::PIN::eight; } break;
                case PROP::stmPIN09:  { gpio.pin = embot::hw::GPIO::PIN::nine; } break;
                case PROP::stmPIN10:  { gpio.pin = embot::hw::GPIO::PIN::ten; } break;
                case PROP::stmPIN11:  { gpio.pin = embot::hw::GPIO::PIN::eleven; } break;
                case PROP::stmPIN12:  { gpio.pin = embot::hw::GPIO::PIN::twelve; } break;
                case PROP::stmPIN13:  { gpio.pin = embot::hw::GPIO::PIN::thirteen; } break;
                case PROP::stmPIN14:  { gpio.pin = embot::hw::GPIO::PIN::fourteen; } break;
                case PROP::stmPIN15:  { gpio.pin = embot::hw::GPIO::PIN::fifteen; } break;
                case PROP::stmPINall: { gpio.pin = embot::hw::GPIO::PIN::none; } break;
                default: { gpio.pin = embot::hw::GPIO::PIN::none; } break;
            }
            
            if(nullptr != p.stmport)
            {
                for(int i=0; i<ports.size(); i++)
                {
                    if(ports[i] == p.stmport) 
                    {
                        gpio.port = static_cast<embot::hw::GPIO::PORT>(i);
                        break;
                    }
                }
            }
            
            return gpio;
        }    
                        
        void init(embot::hw::GPIO h) const;
    };
    
    const BSP& getBSP();
                                     
        
}}} // namespace embot { namespace hw { namespace gpio { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


