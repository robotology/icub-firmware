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

#include "embot_hw_timer.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_timer_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif
using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

using namespace embot::hw;

    
#if !defined(HAL_TIM_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_timer)

namespace embot { namespace hw { namespace timer {
    
    bool supported(embot::hw::TIMER t)                                                                  
    { return false; }
    bool initialised(embot::hw::TIMER t)                                                                
    { return false; }
    result_t init(embot::hw::TIMER t, const Config &config)       
    { return resNOK; }
    result_t deinit(embot::hw::TIMER t)       
    { return resNOK; }
    result_t configure(embot::hw::TIMER t, const Config &config)       
    { return resNOK; }    
    bool isrunning(embot::hw::TIMER t)                                                                  
    { return false; }
    result_t start(embot::hw::TIMER t)                                                
    { return resNOK; }
    result_t stop(embot::hw::TIMER t)                                                                   
    { return resNOK; }
    Status status(embot::hw::TIMER t)
    { return Status::none; }
    void execute(embot::hw::TIMER t)
    {}
    
}}} // namespace embot { namespace hw { namespace timer {

#elif defined(EMBOT_ENABLE_hw_timer_emulated)

#warning EMBOT_ENABLE_hw_timer_emulated is defined. it uses embot::os::Timer, so Config::time must be multiple of 1 ms

#include "embot_os_Timer.h"
#include "embot_os_theCallbackManager.h"

namespace embot { namespace hw { namespace timer {
              
    // initialised mask: one variable for all the timers      
    static std::uint32_t initialisedmask = 0;
    
    bool supported(TIMER t)
    {
        return embot::hw::timer::getBSP().supported(t);
    }
    
    bool initialised(TIMER t)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(t));
    }    

    struct TIMERprop
    {        
        bool                isrunning {false};
        TIMER               t {TIMER::none};    
        Config              config {};
        embot::os::Timer*   ostimer {nullptr};
        Status              status {Status::none};
        void deinit()
        {
            isrunning = false;
            t = TIMER::none;
            config = {};    
            if(nullptr != ostimer)
            {
                ostimer->stop();
                delete ostimer;
            }
            status = Status::none;                
        }
    };
    
    struct propsOFalltimers
    {
        TIMERprop prop[embot::core::tointegral(embot::hw::TIMER::maxnumberof)];
        
        TIMERprop & get(TIMER t)
        {
            return prop[embot::core::tointegral(t)];
        }        
    };

    static propsOFalltimers s_properties;
        
    void execute(void *p)
    {
        TIMERprop * prop = reinterpret_cast<TIMERprop*>(p);
        // the timer callback executes this code. now we do actions depending on what is inside s_privatedata.         
        if(false == initialised(prop->t))
        {
            return;
        }
               
        prop->config.onexpiry.execute();
       
        if(Mode::oneshot == prop->config.mode)
        {
            prop->status = Status::expired;
            stop(prop->t); // still required to update teh status of the embot::hw::TIMER
        }
    }    
    
    result_t init(TIMER t, const Config &config)
    {
        if(false == supported(t))
        {
            return resNOK;
        }
        
        if(true == initialised(t))
        {
            return resOK;
        }
        
        // init peripheral... actually it is done inside general bsp
        // the emulated doe not do anything
        // embot::hw::timer::getBSP().init(t);
             
        TIMERprop &prop = s_properties.get(t);    
        prop.t = t;        
        prop.isrunning = false;
        prop.config = config;
        prop.status = Status::idle;
        if(nullptr == prop.ostimer)
        {
            prop.ostimer = new embot::os::Timer;
        }
        
        // VERY IMPORTANT: keep it in here before configure...
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(t));
        
        return configure(t, config);    
    }

    result_t deinit(embot::hw::TIMER t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
    
        if(true == isrunning(t))
        {
            stop(t);
        }

        TIMERprop &prop = s_properties.get(t); 
        prop.deinit();
        
        embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(t));
        
        return resOK;
    }


    result_t configure(TIMER t, const Config &config)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
    
        if(true == isrunning(t))
        {
            stop(t);
        }
        
        TIMERprop &prop = s_properties.get(t); 
        prop.config = config;
        //mx_timx_init(t, config.time);      
        prop.isrunning = false;
        
        return resOK;        
    }
        

    bool isrunning(TIMER t)
    { 
        if(false == initialised(t))
        {
            return false;
        }
        
        TIMERprop &prop = s_properties.get(t); 
        return prop.isrunning;
    }
 
 
    result_t start(TIMER t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
        

        TIMERprop &prop = s_properties.get(t); 
        
        if(true == prop.isrunning)
        {
            stop(t);
        }
        
        prop.isrunning = true;
        prop.status = Status::running;
     
        // ok: the timer starts.
        //const embot::hw::timer::PROP * stm32props = embot::hw::timer::getBSP().getPROP(t);
        //TIM_HandleTypeDef* phandletimx = reinterpret_cast<TIM_HandleTypeDef*>(stm32props->handle);
        //HAL_TIM_Base_Start_IT(phandletimx);
 //       prop.ostimer.
        embot::os::Timer::Mode mode = (prop.config.mode == Mode::periodic) ? (embot::os::Timer::Mode::forever) : (embot::os::Timer::Mode::oneshot);
        embot::os::Action act(embot::os::CallbackToThread(execute, &prop, embot::os::theCallbackManager::getInstance().thread()));
        embot::os::Timer::Config cfg(prop.config.time, act, mode, 0);  
        prop.ostimer->start(cfg);
        return resOK;        
    } 
    
    result_t stop(TIMER t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
        
            
        // stop it anyway        
        TIMERprop &prop = s_properties.get(t);
        prop.ostimer->stop();
        prop.isrunning = false;
        if(prop.status != Status::expired)
        {
            prop.status = Status::idle;       
        }            
        return resOK;              
    }    
    
    Status status(embot::hw::TIMER t)
    { 
        if(false == initialised(t))
        {
            return Status::none;
        }    
         
        TIMERprop &prop = s_properties.get(t);
        
        return prop.status;
    }
    
}}}

#else

#if defined(STM32HAL_STM32H7)
//#error pls verify embot::hw::timer for STM32H7 
#endif


void hal_error_handler()
{
#if defined(STM32HAL_STM32L4) && (STM32HAL_DRIVER_VERSION >= 0x190)
    Error_Handler();
#elif defined(STM32HAL_STM32H7)
    Error_Handler();
#endif
}
 
namespace embot { namespace hw { namespace timer {
        
      
    // initialised mask: one variable for all the timers      
    static std::uint32_t initialisedmask = 0;
    
    bool supported(TIMER t)
    {
        return embot::hw::timer::getBSP().supported(t);
    }
    
    bool initialised(TIMER t)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(t));
    }    
    
    // stm32 specific support
    struct stm32_tim_registervalues
    {   // those ready to be put into registers of stm32
        std::uint16_t   prescaler;   
        std::uint16_t   period;        
    };
        
    
    // retrieves the values to be put inside the stm32 register
    void compute(TIMER t, const embot::hw::timer::PROP *stm32data, const embot::core::relTime time, stm32_tim_registervalues &pars, embot::core::relTime &effectivetime)
    {        
        // for some timers referencespeed could also be HAL_RCC_GetSysClockFreq() or HAL_RCC_GetPCLK1Freq() or HAL_RCC_GetPCLK2Freq()
        // i embed teh choice into that into embot::hw::sys::clock()
        uint32_t referencespeed = (embot::hw::CLOCK::none == stm32data->clock) ? stm32data->speed : embot::hw::sys::clock(stm32data->clock);
        
        effectivetime = time;

        // use prescaler = ((referencespeed/a/1000) )

        if(0 == (time % 1000))
        {   // multiple of 1 ms: use 10 khz, thus a = 10. 1 tick is 100us, max time is 6400 msec = 6.4 s

            if(time > 64000*100) // tick is 100
            {   // adjust time
                effectivetime = 64000*100; // tick is 100
            }

            pars.prescaler      = (referencespeed/10/1000);  // a is 10. the value is 1600 (for strain2): ok, lower than 65k
            pars.period         = time / 100; // tick is 100

        }
        else if(0 == (time % 100))
        {   // multiple of 100 us: use 100 khz, thus a = 100. 1 tick is 10us, max time is 640 msec
            
            if(time > 64000*10) // tick is 10
            {
                effectivetime = 64000*10; // tick is 10
            }

            pars.prescaler      = (referencespeed/100/1000);  // a is 100. the value is 160 (for strain2): ok, lower than 65k
            pars.period         = time / 10; // tick is 10
        }
        else if(0 == (time % 10))
        {   // multiple of 10 us: use 1000 khz, thus a = 1000. 1 tick is 1us, max time is 64 msec
            
            if(time > 64000*1) // tick is 1
            {
                effectivetime = 64000*1; // tick is 1
            }

            pars.prescaler      = (referencespeed/1000/1000);  // a is 1000. the value is 16 (for strain2): ok, lower than 65k
            pars.period         = time / 1; // tick is 1
        }
        else
        {   // multiple of 1 us: use 8000 khz, thus a = 8000. 1 tick is 0.125us, max time is 8 msec
            
            if(time > 8000) // tick is 0.125
            {
                effectivetime = 8000; // tick is 0.125
            }

            pars.prescaler   = (referencespeed/8000/1000);  // a is 8000. the value is 2 (for strain2): ok, lower than 65k
            pars.period      = time * 8; // tick is 0.125
        }

    }        
 
        
    void mx_timx_init(TIMER t, std::uint32_t time)
    {
        std::uint32_t effectivetime = 0;
        stm32_tim_registervalues pars =  {0};
        
        const embot::hw::timer::PROP * stm32props = embot::hw::timer::getBSP().getPROP(t);
        TIM_HandleTypeDef* phandletimx = stm32props->handle;

        
        compute(t, stm32props, time, pars, effectivetime);
        
        
        TIM_MasterConfigTypeDef sMasterConfig;
        
        phandletimx->Instance = stm32props->TIMx;
        phandletimx->Init.Prescaler = pars.prescaler-1;
        phandletimx->Init.CounterMode = TIM_COUNTERMODE_DOWN;
        phandletimx->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        phandletimx->Init.RepetitionCounter = 0;
        phandletimx->Init.Period = pars.period-1;
        #if defined(STM32HAL_STM32L4) && (STM32HAL_DRIVER_VERSION >= 0x183)
        phandletimx->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        #elif defined(STM32HAL_STM32H7)
        #warning TODO: verify it
        phandletimx->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        #endif
        
        
        if(true == stm32props->isonepulse)
        {   // e.g., for tim15: use HAL_TIM_OnePulse_Init() and dont use HAL_TIM_Base_Init().
            if (HAL_TIM_OnePulse_Init(phandletimx, TIM_OPMODE_REPETITIVE) != HAL_OK)
            {
                hal_error_handler();
            }
        } 
        else
        {   // normal case            
            if (HAL_TIM_Base_Init(phandletimx) != HAL_OK)
            {
               hal_error_handler();
            }
        }

        if(true == stm32props->mastermode)
        {
            sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
            sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
            if (HAL_TIMEx_MasterConfigSynchronization(phandletimx, &sMasterConfig) != HAL_OK)
            {
                hal_error_handler();
            }
        }

    }   

           

    struct TIMERprop
    {        
        bool                isrunning;
        Config              config;
    };
    
    struct propsOFalltimers
    {
        TIMERprop prop[embot::core::tointegral(embot::hw::TIMER::maxnumberof)];
        
        TIMERprop & get(TIMER t)
        {
            return prop[embot::core::tointegral(t)];
        }        
    };

    static propsOFalltimers s_properties;

         
    void execute(TIMER t)
    {
        // the timer callback executes this code. now we do actions depending on what is inside s_privatedata.         
        if(false == initialised(t))
        {
            return;
        }
        
        TIMERprop &prop = s_properties.get(t);
        
        prop.config.onexpiry.execute();
       
        if(Mode::oneshot == prop.config.mode)
        {
            stop(t);
        }
    }    
    
    result_t init(TIMER t, const Config &config)
    {
        if(false == supported(t))
        {
            return resNOK;
        }
        
        if(true == initialised(t))
        {
            return resOK;
        }
        
        // init peripheral... actually it is done inside general bsp
        embot::hw::timer::getBSP().init(t);
             
        TIMERprop &prop = s_properties.get(t);        
        prop.isrunning = false;
        prop.config = config;
        
        // VERY IMPORTANT: keep it in here before configure...
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(t));
        
        return configure(t, config);    
    }

    
    result_t configure(TIMER t, const Config &config)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
    
        if(true == isrunning(t))
        {
            stop(t);
        }
        
        TIMERprop &prop = s_properties.get(t); 
        prop.config = config;
        mx_timx_init(t, config.time);      
        prop.isrunning = false;
        
        return resOK;        
    }
        

    bool isrunning(TIMER t)
    { 
        if(false == initialised(t))
        {
            return false;
        }
        
        TIMERprop &prop = s_properties.get(t); 
        return prop.isrunning;
    }
 
 
    result_t start(TIMER t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
        

        TIMERprop &prop = s_properties.get(t); 
        
        if(true == prop.isrunning)
        {
            stop(t);
        }
        
        prop.isrunning = true;
     
        // ok: the timer starts.
        const embot::hw::timer::PROP * stm32props = embot::hw::timer::getBSP().getPROP(t);
        TIM_HandleTypeDef* phandletimx = reinterpret_cast<TIM_HandleTypeDef*>(stm32props->handle);
        HAL_TIM_Base_Start_IT(phandletimx);
  
        return resOK;
        
    } 
    

    result_t stop(TIMER t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
        
            
        // stop it anyway
        const embot::hw::timer::PROP * stm32props = embot::hw::timer::getBSP().getPROP(t);
        HAL_TIM_Base_Stop_IT(stm32props->handle);
        
        TIMERprop &prop = s_properties.get(t);
        prop.isrunning = false;
                
        return resOK;              
    }
 
    
}}} // namespace embot { namespace hw { namespace tim {


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.



#endif // defined(HAL_TIM_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

