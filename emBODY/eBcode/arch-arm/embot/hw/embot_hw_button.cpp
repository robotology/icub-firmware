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

#include "embot_hw_button.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"
#include "embot_hw_gpio.h"
#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"

#include "embot_hw_sys.h"

#include <cstring>
#include <vector>

#include "embot_core_binary.h"


using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//#define DBG_triggertime

// the use of millisecond resolution is just a bit quicker and we dont need filter below 1 ms.
// in any case: the get time when called by the EXTI Handler DOES not call the svc because osal
// functions detect the call from an IRQhandler.

#define USE_millisec_resolution

#if defined(USE_millisec_resolution)
#include "osal_system.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------
  
using namespace embot::hw;

namespace embot { namespace hw { namespace button {
          
    // initialised mask        
    static std::uint32_t initialisedmask = 0;
    
    struct PrivateData
    {        
        Config config[embot::core::tointegral(BTN::maxnumberof)];  
        embot::core::Time pressed[embot::core::tointegral(BTN::maxnumberof)] {0};
        embot::core::Time released[embot::core::tointegral(BTN::maxnumberof)] {0};
        uint32_t countingmask {0};
        PrivateData() = default;
    };
    
    static PrivateData s_privatedata;    
    
    GPIO_PinState convert(embot::hw::gpio::State s)
    {
        return (s == embot::hw::gpio::State::RESET) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET);
    }
    
    bool supported(BTN btn)
    {
        return embot::hw::bsp::button::getBSP().supported(btn);
    }
    
    bool initialised(BTN btn)
    {
        return embot::core::binary::bit::check(initialisedmask,embot::core::tointegral(btn));
    }
        
    result_t init(BTN btn, const Config &cfg)
    {
        if(!supported(btn))
        {
            return resNOK;
        }  
        
        if(initialised(btn))
        {   // dont need to re-init
            return resOK;
        }
                 
        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        s_privatedata.config[embot::core::tointegral(btn)] = cfg;
        
        const embot::hw::bsp::button::BSP &btnbsp = embot::hw::bsp::button::getBSP();
        
        // bsp specific initialization                    
        btnbsp.init(btn);  

        // but ... initialization in here
        if(cfg.mode == embot::hw::button::Mode::Polling)
        {
            // just init it in a simple way
            embot::hw::gpio::configure(btnbsp.getPROP(btn)->gpio, embot::hw::gpio::Mode::INPUT, btnbsp.getPROP(btn)->pull, embot::hw::gpio::Speed::medium);
        }
        else if((cfg.mode == embot::hw::button::Mode::TriggeredOnPress) || 
                (cfg.mode == embot::hw::button::Mode::TriggeredOnRelease) || 
                (cfg.mode == embot::hw::button::Mode::TriggeredOnDebouncedRelease))
        {
            // we have to write code in here which depends on the pin and on PROP.pressed
            
            embot::hw::gpio::Mode mode = embot::hw::gpio::Mode::EXTIrising;
            if(cfg.mode == embot::hw::button::Mode::TriggeredOnPress)
            {
                mode = (btnbsp.getPROP(btn)->pressed == embot::hw::gpio::State::SET) ? embot::hw::gpio::Mode::EXTIrising : embot::hw::gpio::Mode::EXTIfalling;
            }
            else if(cfg.mode == embot::hw::button::Mode::TriggeredOnRelease)
            {
                mode = (btnbsp.getPROP(btn)->pressed == embot::hw::gpio::State::SET) ? embot::hw::gpio::Mode::EXTIfalling : embot::hw::gpio::Mode::EXTIrising;
            }
            else
            {
                mode = embot::hw::gpio::Mode::EXTIrisingfalling;
            }
                        
            embot::hw::gpio::configure(btnbsp.getPROP(btn)->gpio, mode, btnbsp.getPROP(btn)->pull, embot::hw::gpio::Speed::medium);
            
            // and in here we enable the interrupt. 
            HAL_NVIC_SetPriority(static_cast<IRQn_Type>(btnbsp.getPROP(btn)->irqn), 0, 0);
            HAL_NVIC_EnableIRQ(static_cast<IRQn_Type>(btnbsp.getPROP(btn)->irqn));           
        }
                
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(btn));
                
        return resOK;        
    }

    const Config & config(BTN btn)
    {
        return s_privatedata.config[embot::core::tointegral(btn)];
    }
    
    
    bool pressed(BTN btn)
    {
        if(!initialised(btn))
        {
            return false;
        }  
        const embot::hw::GPIO gpio = embot::hw::bsp::button::getBSP().getPROP(btn)->gpio;       
        
        embot::hw::gpio::State s = embot::hw::gpio::get(gpio);
        return (embot::hw::bsp::button::getBSP().getPROP(btn)->pressed == s) ? (true) : (false);        
    }
    
    
    embot::core::Time _timenow()
    {
#if defined(USE_millisec_resolution)      
        return osal_system_ticks_abstime_get();
#else
        return embot::core::now();
#endif        
    }

#if defined(DBG_triggertime)    
    struct time_t
    {
        uint64_t t0 {0};
        uint64_t t1 {0};
    };
#endif    
    void onexti(BTN btn)
    {
        if(!initialised(btn))
        {
            return;
        }
        
        const embot::hw::button::Config & cfg = s_privatedata.config[embot::core::tointegral(btn)];
        
        if((cfg.mode == embot::hw::button::Mode::TriggeredOnPress) || (cfg.mode == embot::hw::button::Mode::TriggeredOnRelease))
        {
            // i call it w/out checking if we are still pressed or released because ... exti is configured on one direction only
            cfg.callback.execute(); 
        }
        else if(cfg.mode == embot::hw::button::Mode::TriggeredOnDebouncedRelease)
        {
            
            // exti is configured for two directions... i must sample. then there may be spurious pulses both risong and falling. i try to filter them out
            if(false == embot::core::binary::bit::check(s_privatedata.countingmask, embot::core::tointegral(btn)))
            {   
                // i am not counting time. i see if i can start the counting of time by checking if i am in pressed state                
                if(true == embot::hw::button::pressed(btn))
                {
                    //  we begin counting: we are in pressed state
                    embot::core::binary::bit::set(s_privatedata.countingmask, embot::core::tointegral(btn));
                    s_privatedata.pressed[embot::core::tointegral(btn)] = _timenow();
#if defined(DBG_triggertime)                    
                    embot::hw::sys::puts(std::string("PRE @") + embot::core::TimeFormatter(s_privatedata.pressed[embot::core::tointegral(btn)]).to_string());
#endif
                }
            }
            else 
            { 
                // i am counting. i must stop counting if i am not in pressed state anymore 
                if(false == embot::hw::button::pressed(btn))
                {
                    embot::core::binary::bit::clear(s_privatedata.countingmask, embot::core::tointegral(btn));
                    s_privatedata.released[embot::core::tointegral(btn)] = _timenow();
                                    
                    int64_t delta = static_cast<int64_t>(s_privatedata.released[embot::core::tointegral(btn)]) - static_cast<int64_t>(s_privatedata.pressed[embot::core::tointegral(btn)]);                
#if defined(DBG_triggertime) 
                    embot::hw::sys::puts(std::string("REL @") + 
                                         embot::core::TimeFormatter(s_privatedata.released[embot::core::tointegral(btn)]).to_string() + 
                                         " after " + 
                                         embot::core::TimeFormatter(delta).to_string());  
#endif                    
                    if((delta > 0) && (delta > cfg.debouncetime))
                    {
#if defined(DBG_triggertime)                         
                        time_t *tt = reinterpret_cast<time_t*>(cfg.callback.arg);
                        if(nullptr != tt)
                        {
                            tt->t0 = s_privatedata.pressed[embot::core::tointegral(btn)];
                            tt->t1 = s_privatedata.released[embot::core::tointegral(btn)];
                        }
#endif                        
                        cfg.callback.execute(); 
                    }                       
                    s_privatedata.pressed[embot::core::tointegral(btn)] = s_privatedata.released[embot::core::tointegral(btn)];
                }
                else
                {
//                    // it must be a spurious trigger.
#if defined(DBG_triggertime)                     
                    embot::hw::sys::puts(std::string("SPU @") + embot::core::TimeFormatter(embot::core::now()).to_string());
#endif                    
                }                
            }
        }                
    }
    
    
}}} // namespace embot { namespace hw { namespace button 




    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

