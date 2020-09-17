    /*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#include "embot_app_theLEDmanager.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"
#include "embot_hw.h"
#include "embot_hw_gpio.h"
#include "embot_hw_bsp.h"
#include "embot_core_binary.h"
#include "embot_os_Timer.h"

#include "embot_os_theCallbackManager.h"
#include "embot_os_Timer.h"
#include "embot_os_Action.h"

#include <vector>
#include <map>

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace app {
    
    using namespace embot::hw;
    
    GPIO_PinState convert(embot::hw::gpio::State s)
    {
        return (s == embot::hw::gpio::State::RESET) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET);
    }
    
    
#if 0
    // correct but we use another one
    class aLED : public LEDhandle
    {
    public:
        LED led;
        GPIO_PinState stateON;
        GPIO_PinState stateOFF;
        bool supp;
        const embot::hw::stm32GPIO *gpio;
    public:
        
        aLED(LED l = LED::none, const embot::hw::stm32GPIO *g = nullptr, GPIO_PinState ON = GPIO_PIN_SET, GPIO_PinState OFF = GPIO_PIN_RESET) : led(l), gpio(g), stateON(ON), stateOFF(OFF)
        {
            supp = embot::hw::bsp::led::getMAP()->supported(led);
        };
            
        ~aLED() {};
            
        virtual bool supported() const
        {
            return supp;
        }
        
        virtual LED id() const
        {
            return led;
        }
        
        virtual void on()
        {
            if(supp)
            {
                HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), stateON);
            }
        }
        
        virtual void off()
        {
            if(supp)
            {
                HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), stateOFF);
            }
        }
        
        virtual void toggle()
        {
            if(supp)
            {
                HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
            }
        }  

        virtual void pulse(embot::core::relTime period, std::uint32_t times) 
        {
    
        }            
        
    };
    
#endif

#if 0
    // use this class if you know for sure that the led is supported. please check with a call to embot::hw::bsp::led::getMAP()->supported(led) first
    // correct but we use its extension w/ timer
    class aSlimLED : public LEDhandle
    {
    public:
        LED led;
        const embot::hw::stm32GPIO *gpio;
        GPIO_PinState stateON;
        GPIO_PinState stateOFF;
    public:
        
        aSlimLED(LED l = LED::none, const embot::hw::stm32GPIO *g = nullptr, 
                        embot::hw::gpio::State ON = embot::hw::gpio::State::SET, 
                        embot::hw::gpio::State OFF = embot::hw::gpio::State::RESET) 
                        : led(l), gpio(g), stateON(convert(ON)), stateOFF(convert(OFF))
        {
            // assume led is ok.
        };
            
        ~aSlimLED() {};
            
        virtual bool supported() const
        {   // assume it is ok but i give correct result anyway
            return embot::hw::bsp::led::getMAP()->supported(led);
        }
        
        virtual LED id() const
        {
            return led;
        }
        
        virtual void on() const
        {   // assume it is ok, hence gpio is not null
            HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), stateON);
        }
        
        virtual void off() const
        {   // assume it is ok, hence gpio is not null
            HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), stateOFF);
        }
        
        virtual void toggle() const
        {   // assume it is ok, hence gpio is not null
            HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
        }   

        virtual void pulse(embot::core::relTime period, std::uint32_t times) 
        {

        }            
        
    };
    
#endif

    // use this class if you know for sure that the led is supported. please check with a call to embot::hw::bsp::led::getMAP()->supported(led) first
    class aSlimPulsableLED : public LEDhandle
    {
    public:
        LED led;
        const embot::hw::GPIO *gpio;
        embot::hw::gpio::State stateON;
        embot::hw::gpio::State stateOFF;
        embot::os::Timer *tmr;
        LEDwave *_ledw;
        uint64_t _memory[4];
    public:
        
        aSlimPulsableLED(LED l = LED::none, const embot::hw::GPIO *g = nullptr, 
                        embot::hw::gpio::State ON = embot::hw::gpio::State::SET, 
                        embot::hw::gpio::State OFF = embot::hw::gpio::State::RESET) 
                        : led(l), gpio(g), stateON(ON), stateOFF(OFF)
        {
            // assume led is ok.
            // i defer creation of the timer.
            tmr = nullptr;   
            _ledw = nullptr;
            std::memset(_memory, 0xff, sizeof(_memory));
        };
            
        ~aSlimPulsableLED() {};
            
        virtual bool supported() const
        {   // assume it is ok but i give correct result anyway
            return embot::hw::bsp::led::getBSP().supported(led);
        }
        
        virtual LED id() const
        {
            return led;
        }
        
        virtual void on() 
        {   // assume it is ok, hence gpio is not null
            //HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), stateON);
            embot::hw::gpio::set(*gpio, stateON);
        }
        
        virtual void off() 
        {   // assume it is ok, hence gpio is not null
            //HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), stateOFF);
            embot::hw::gpio::set(*gpio, stateOFF);
        }
        
        virtual void toggle() 
        {   // assume it is ok, hence gpio is not null
            //HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
            embot::hw::gpio::toggle(*gpio);
        }   

        static void onexpirypulse(void *p)
        {
            aSlimPulsableLED * l = reinterpret_cast<aSlimPulsableLED*>(p);
            l->toggle();            
        }
        
        virtual void pulse(embot::core::relTime period, std::uint32_t times) 
        {            
            if(nullptr == tmr)
            {
                tmr = new embot::os::Timer;
            }
            
            if(0 == period)
            {
                tmr->stop();
                off();
            }
            else
            {
                tmr->stop();
                off();
                embot::os::Timer::Mode mode = (0 == times) ? (embot::os::Timer::Mode::forever) : (embot::os::Timer::Mode::someshots);
                embot::os::Action act(embot::os::CallbackToThread(onexpirypulse, this, embot::os::theCallbackManager::getInstance().thread()));
                embot::os::Timer::Config cfg(period/2, act, mode, times);
                tmr->start(cfg);
            }
        }

        
        virtual void stop()
        {
            if(nullptr != tmr)
            {
                tmr->stop();
                off();
            }
            
            if(nullptr != _ledw)
            {
                _ledw->~LEDwave();
                _ledw = nullptr;
                std::memset(_memory, 0xff, sizeof(_memory));
            }
        }
        
                        
        static void onexpirywave(void *p)
        {
            aSlimPulsableLED * l = reinterpret_cast<aSlimPulsableLED*>(p);
            bool ledon = l->_ledw->tick();
            
            if(ledon)
            {
                l->on();
            }
            else
            {
                l->off();
            }
        }
        
        virtual void wave(const LEDwave *lw, std::uint32_t times = 0)
        {
            if(nullptr == tmr)
            {
                tmr = new embot::os::Timer;
            }
            
            if(0 == lw->frequency())
            {
                stop();
            }
            else
            {
                stop();
                _ledw = lw->duplicate(_memory, sizeof(_memory));
                embot::os::Timer::Mode mode = (0 == times) ? (embot::os::Timer::Mode::forever) : (embot::os::Timer::Mode::someshots);
                embot::os::Action act(embot::os::CallbackToThread(onexpirywave, this, embot::os::theCallbackManager::getInstance().thread()));
                embot::os::Timer::Config cfg(_ledw->frequency(), act, mode, times*_ledw->length());
                tmr->start(cfg);
            }              
            
        }
        
        
    };    
    
    
    class aDummyLED : public LEDhandle
    {
    public:
        LED led;
    public:
        
        aDummyLED(LED le = LED::none) : led(le) {};
            
        ~aDummyLED() {};
            
        virtual bool supported() const
        {
            return false;
        }
        
        virtual LED id() const
        {
            return led;
        }
        
        virtual void on() {}        
        virtual void off() {}        
        virtual void toggle() {}  
        virtual void pulse(embot::core::relTime period, std::uint32_t times) {}  
        virtual void stop() {}
        virtual void wave(const LEDwave *lw, std::uint32_t times = 0) {}
    };
    
}}

namespace embot { namespace app {
    
    struct theLEDmanager::Impl
    {    
        bool started;
                
        std::uint32_t initialisedmask = 0;
        
        //std::map<LED, aSlimLED> mapofleds;
        std::map<LED, aSlimPulsableLED> mapofleds;
        aDummyLED lednone{LED::none};
        //aSlimLED lednone{LED::none};
                     
        Impl() 
        {              
            started = false;
            initialisedmask = 0;
        }
        
        ~Impl() 
        {              
            started = false;
            initialisedmask = 0;
        }
        
        
        bool initialised(LED led) const
        {
            return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(led));
        }
           

        bool init(LED led)
        {
            if(!embot::hw::bsp::led::getBSP().supported(led))
            {
                return false;
            }  
                        
            if(initialised(led))
            {   // dont need to re-init
                return true;
            }
            
            // the GPIOs of all the LEDs may have been initialised by the cube-mx code
            // called by stm32hal_init() inside embot::hw::init() but ... maybe not. 
            // better to init the LED anyway
            
            if(!embot::hw::initialised())
            {   // requires embot::hw::bsp::init()
                return false;
            }   
            
            embot::hw::led::init(led);
                    
            embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(led));
            
            // i am sure that thebspmap->getgpio(led) is ok 
//            aSlimLED a{led, thebspmap->getgpio(led), thebspmap->on, thebspmap->off};
//            a.off();            
//            mapofleds.insert(std::pair<LED, aSlimLED>(led, a));
            
            aSlimPulsableLED a{led, &embot::hw::bsp::led::getBSP().getPROP(led)->gpio, embot::hw::bsp::led::getBSP().getPROP(led)->on, embot::hw::bsp::led::getBSP().getPROP(led)->off};
            a.off();            
            mapofleds.insert(std::pair<LED, aSlimPulsableLED>(led, a));
                       
            return true;        
        }
        
        bool deinit()
        {           
            initialisedmask = 0;
            mapofleds.clear();                                   
            return true;        
        }
        
        bool deinit(LED led)
        { 
            if(initialised(led))
            {
                mapofleds.erase(led);            
                embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(led));
            }
            return true;
        }

    };
    
    theLEDmanager& theLEDmanager::getInstance()
    {
        static theLEDmanager* p = new theLEDmanager();
        return *p;
    }
    
    theLEDmanager::theLEDmanager()
    {
        pImpl = std::make_unique<Impl>();
    }  
    
    
    theLEDmanager::~theLEDmanager() { }
   

    bool theLEDmanager::init(const std::initializer_list<LED> &leds)
    {
        for(auto a : leds)
        {
            pImpl->init(a);
        }
        
        return true;
    } 

    bool theLEDmanager::deinit()
    {
        pImpl->deinit();
        return true;
    }  

    bool theLEDmanager::deinit(const std::initializer_list<LED> &leds)
    {
        for(auto a : leds)
        {
            pImpl->deinit(a);
        }
        return true;
    }     

    bool theLEDmanager::initialised(LED led) const
    {
        return pImpl->initialised(led);     
    }    
    
    LEDhandle & theLEDmanager::get(LED led) const
    {
        auto a = pImpl->mapofleds.find(led);
        if(a != pImpl->mapofleds.end())
        {
            return a->second;
        }
        else
        {
            return pImpl->lednone;
        }        
    }
    
    
//    LEDhandle * theLEDmanager::obtain(LED led) const
//    {
//        auto a = pImpl->mapofleds.find(led);
//        if(a != pImpl->mapofleds.end())
//        {
//            return &a->second;
//        }
//        else
//        {
//            return &pImpl->lednone;
//        }        
//    }
    
//    LEDhandle * theLEDmanager::release(const LEDhandle *handle) const
//    {
//        //handle = &pImpl->lednone;
//        return &pImpl->lednone;
//    }
//    
//    LEDhandle & theLEDmanager::none() const
//    {
//        return pImpl->lednone;
//    }


}} // namespace embot { namespace app {

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


