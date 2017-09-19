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
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_sys.h"

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(HAL_TIM_MODULE_ENABLED)


namespace embot { namespace hw { namespace timer {
    
    bool supported(Timer t)                                                                  { return false; }
    bool initialised(Timer t)                                                                { return false; }
    result_t init(Timer t, const Config &config)                                             { return resNOK; }
    bool isrunning(Timer t)                                                                  { return false; }
    result_t start(Timer t, const Mode &mode)                                                { return resNOK; }
    result_t stop(Timer t)                                                                   { return resNOK; }
    
}}} // namespace embot { namespace hw { namespace timer {

#else


// there are two parts: 
// 1. the cpp driver under namespace embot::hw::onewire
// 2. the functions which stm library required to be redefined: IRQ handlers and callbacks 

// the implementation uses just a few timers: TIM6 and TIM7 which are compacted in a small array
 
namespace embot { namespace hw { namespace timer {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
#if     defined(STM32HAL_BOARD_STRAIN2)    
    
    static const std::uint8_t numberofsupported = 4;
    
    static const bspmap_t bspmap = 
    {
        (1 << static_cast<std::uint32_t>(Timer::six))       | (1 << static_cast<std::uint32_t>(Timer::seven)) | 
        (1 << static_cast<std::uint32_t>(Timer::fifteen))   | (1 << static_cast<std::uint32_t>(Timer::sixteen))
    };

    static const std::uint8_t map2compactarray[static_cast<unsigned int>(Timer::maxnumberof)] = 
    {   // each pos keeps either 255 or a valid index to the set of valid timers without holes 
        255, 255, 255, 255, 255,
        0,  // Timer::six
        1,  // Timer::seven
        255, 255, 255, 255, 255, 255, 255, 
        2,  // Timer::fifteen 
        3   // Timer::sixteen        
    };
   
#else
    
    static const std::uint8_t numberofsupported = 1; // cannot be 1 because ...
    
    static const bspmap_t bspmap = 
    {
        0x00000000
    };
    
    static const std::uint8_t map2compactarray[1] = 
    { 
        255 
    };
    
#endif
      
    // initialised mask: one variable for all the timers      
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t timer2index(Timer t)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(t);
    }
        
    bool supported(Timer t)
    {
        if((Timer::none == t) || (Timer::maxnumberof == t))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, timer2index(t));
    }
    
    bool initialised(Timer t)
    {
        if(Timer::none == t)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, timer2index(t));
    } 

    
    std::uint8_t timer2indexofcompactarray(Timer t)
    {
        if(false == supported(t))
        {
            return 255;
        }
        
        return map2compactarray[timer2index(t)];        
    }

    
    // stm32 specific support
    struct stm32_tim_registervalues
    {   // those ready to be put into registers of stm32
        std::uint16_t   prescaler;   
        std::uint16_t   period;        
    };
        
    struct stm32_tim_mapping
    {
        embot::hw::sys::CLOCK   clock;        
        TIM_TypeDef*            TIMx;
        TIM_HandleTypeDef*      phandletimx;
        bool                    isonepulse;
        bool                    mastermode;
    };

#if     defined(STM32HAL_BOARD_STRAIN2)    
    static const stm32_tim_mapping s_stm32_tim_mapping[numberofsupported] = 
    { 
        { embot::hw::sys::CLOCK::syscore, TIM6,  &htim6,  false, true }, 
        { embot::hw::sys::CLOCK::syscore, TIM7,  &htim7,  false, true },
        { embot::hw::sys::CLOCK::syscore, TIM15, &htim15, true,  false },
        { embot::hw::sys::CLOCK::syscore, TIM16, &htim16, false, false }        
    };   
#else
    static const stm32_tim_mapping s_stm32_tim_mapping[1] = { {embot::hw::sys::CLOCK::none, nullptr, nullptr } };
#endif 
    
    // retrieves the values to be put inside the stm32 register
    void compute(Timer t, const stm32_tim_mapping *stm32data, const embot::common::relTime time, stm32_tim_registervalues &pars, embot::common::relTime &effectivetime)
    {        
        // for some timers referencespeed could also be HAL_RCC_GetSysClockFreq() or HAL_RCC_GetPCLK1Freq() or HAL_RCC_GetPCLK2Freq()
        // i embed teh choice into that into embot::hw::sys::clock()
        uint32_t referencespeed = embot::hw::sys::clock(stm32data->clock);
        
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

         
        
    void mx_timx_init(Timer t, std::uint32_t time)
    {
        std::uint32_t effectivetime = 0;
        stm32_tim_registervalues pars =  {0};
        
        const stm32_tim_mapping *stm32data = &s_stm32_tim_mapping[timer2indexofcompactarray(t)];
        
        compute(t, stm32data, time, pars, effectivetime);
        
        
        TIM_MasterConfigTypeDef sMasterConfig;
        
        stm32data->phandletimx->Instance = stm32data->TIMx;
        stm32data->phandletimx->Init.Prescaler = pars.prescaler-1;
        stm32data->phandletimx->Init.CounterMode = TIM_COUNTERMODE_DOWN;
        stm32data->phandletimx->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        stm32data->phandletimx->Init.RepetitionCounter = 0;
        stm32data->phandletimx->Init.Period = pars.period-1;

        
        if(true == stm32data->isonepulse)
        {   // e.g., for tim15: use HAL_TIM_OnePulse_Init() and dont use HAL_TIM_Base_Init().
            if (HAL_TIM_OnePulse_Init(stm32data->phandletimx, TIM_OPMODE_REPETITIVE) != HAL_OK)
            {
                _Error_Handler(__FILE__, __LINE__);
            }
        } 
        else
        {   // normal case            
            if (HAL_TIM_Base_Init(stm32data->phandletimx) != HAL_OK)
            {
                _Error_Handler(__FILE__, __LINE__);
            }
        }

        if(true == stm32data->mastermode)
        {
            sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
            sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
            if (HAL_TIMEx_MasterConfigSynchronization(stm32data->phandletimx, &sMasterConfig) != HAL_OK)
            {
                _Error_Handler(__FILE__, __LINE__);
            }
        }

    }   

    
//#if     defined(STM32HAL_BOARD_STRAIN2)  
//     
//    // this is a modified version from the one generated by cube-mx which ... was wrong because did not use the -1 in prescaler and period
//    void mx_tim6_init(Timer t, TIM_HandleTypeDef *htim, std::uint32_t time)
//    {   
//        std::uint32_t effectivetime = 0;
//        stm32_tim_registervalues pars =  {0};
//        
//        compute(t, time, pars, effectivetime);
//        
//        TIM_MasterConfigTypeDef sMasterConfig;

//        htim6.Instance = TIM6;
//        htim6.Init.Prescaler = pars.prescaler-1;
//        htim6.Init.CounterMode = TIM_COUNTERMODE_DOWN;
//        htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//        htim6.Init.RepetitionCounter = 0;
//        htim6.Init.Period = pars.period-1;
//        
//        if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
//        {
//            _Error_Handler(__FILE__, __LINE__);
//        }

//        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//        if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
//        {
//            _Error_Handler(__FILE__, __LINE__);
//        }

//    }  

//    // need: TIMx, htimx

//    void mx_tim7_init(Timer t, TIM_HandleTypeDef *htim, std::uint32_t time)
//    {
//        std::uint32_t effectivetime = 0;
//        stm32_tim_registervalues pars =  {0};
//        
//        compute(t, time, pars, effectivetime);
//        
//        TIM_MasterConfigTypeDef sMasterConfig;
//        
//        htim7.Instance = TIM7;
//        htim7.Init.Prescaler = pars.prescaler-1;
//        htim7.Init.CounterMode = TIM_COUNTERMODE_DOWN;
//        htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//        htim7.Init.RepetitionCounter = 0;
//        htim7.Init.Period = pars.period-1;


//        if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
//        {
//            _Error_Handler(__FILE__, __LINE__);
//        }

//        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//        if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
//        {
//            _Error_Handler(__FILE__, __LINE__);
//        }

//    }
//     
//      
//     
//#endif    
           


    struct PrivateData
    {        
        bool                isrunning;
        Config              config;
    };
    
    static PrivateData s_privatedata[numberofsupported]; 
         
    void callbackOnTick(Timer t)
    {
        // the timer callback executes this code. now we do actions depending on what is inside s_privatedata. 
        
        std::uint8_t compindex = timer2indexofcompactarray(t);
       
        if(nullptr != s_privatedata[compindex].config.onexpiry.callback)
        {
            s_privatedata[compindex].config.onexpiry.callback(s_privatedata[compindex].config.onexpiry.arg);
        }
       
        if(Mode::oneshot == s_privatedata[compindex].config.mode)
        {
            stop(t);
        }

    }    
    
    result_t init(Timer t, const Config &config)
    {
        if(false == supported(t))
        {
            return resNOK;
        }
        
        if(true == initialised(t))
        {
            return resOK;
        }
        
        std::uint8_t compindex = timer2indexofcompactarray(t);
          
        s_privatedata[compindex].isrunning = false;
        s_privatedata[compindex].config = config;
        
        embot::common::bit::set(initialisedmask, timer2index(t));
        
        configure(t, config);    

        return resOK;
    }

    
    result_t configure(Timer t, const Config &config)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
    
        if(true == isrunning(t))
        {
            stop(t);
        }
        
        std::uint8_t compindex = timer2indexofcompactarray(t);
        
        s_privatedata[compindex].config = config;
        mx_timx_init(t, config.time);      
        s_privatedata[compindex].isrunning = false;
        
        return resOK;        
    }
        

    bool isrunning(Timer t)
    { 
        if(false == initialised(t))
        {
            return false;
        }
        
        std::uint8_t compindex = timer2indexofcompactarray(t);
        
        return s_privatedata[compindex].isrunning;
    }
 
 
    result_t start(Timer t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
        
        std::uint8_t compindex = timer2indexofcompactarray(t);
        
        if(true == s_privatedata[compindex].isrunning)
        {
            stop(t);
        }
        
        s_privatedata[compindex].isrunning = true;
     
        // ok: the timer starts.
        const stm32_tim_mapping *stm32data = &s_stm32_tim_mapping[compindex];
        HAL_TIM_Base_Start_IT(stm32data->phandletimx);
  
        return resOK;
        
    } 
    

    result_t stop(Timer t)
    {
        if(false == initialised(t))
        {
            return resNOK;
        }
        
        std::uint8_t compindex = timer2indexofcompactarray(t);
     
        // stop it anyway
        const stm32_tim_mapping *stm32data = &s_stm32_tim_mapping[compindex];
        HAL_TIM_Base_Stop_IT(stm32data->phandletimx);
        
        s_privatedata[compindex].isrunning = false;
                
        return resOK;              
    }
 
    
}}} // namespace embot { namespace hw { namespace tim {


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.


#define USE_QUICKER_MODE

#if defined(USE_QUICKER_MODE)

// in here it is implemented in the way the good old hal2 was doing: the handler directly manages the callback
// instead the stm hal make a lot of calls before actually calling the callback code, hence it is slower.

void manageInterrupt(embot::hw::timer::Timer t, TIM_HandleTypeDef *htim)
{
    if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
    {
        if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) !=RESET)
        {
            __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
            embot::hw::timer::callbackOnTick(t);
        }
    }   
} 

void TIM6_DAC_IRQHandler(void)
{
    manageInterrupt(embot::hw::timer::Timer::six, &htim6);
}

void TIM7_IRQHandler(void)
{
    manageInterrupt(embot::hw::timer::Timer::seven, &htim7);    
}

void TIM1_BRK_TIM15_IRQHandler(void)
{
    manageInterrupt(embot::hw::timer::Timer::fifteen, &htim15);
}

void TIM1_UP_TIM16_IRQHandler(void)
{
    manageInterrupt(embot::hw::timer::Timer::sixteen, &htim16);
}


#else

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim7);
}

void TIM1_BRK_TIM15_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim15);
}

void TIM1_UP_TIM16_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&htim16);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(TIM6 == htim->Instance)
    { 
        embot::hw::timer::callbackOnTick(embot::hw::timer::Timer::six); 
    }
    else if(TIM7 == htim->Instance)
    { 
        embot::hw::timer::callbackOnTick(embot::hw::timer::Timer::seven); 
    } 
    else if(TIM15 == htim->Instance)
    { 
        embot::hw::timer::callbackOnTick(embot::hw::timer::Timer::fifteen); 
    }
    else if(TIM16 == htim->Instance)
    { 
        embot::hw::timer::callbackOnTick(embot::hw::timer::Timer::sixteen); 
    }     
}

#endif




#endif 


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

