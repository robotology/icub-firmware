
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_timer_bsp_amc2c.h"

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

#include "embot_hw_bsp_amc2c_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::timer

#include "embot_hw_timer_bsp.h"
#include "embot_hw_timer.h"

#if   !defined(HAL_TIM_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_timer)

namespace embot::hw::timer {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TIMER h) const {}    
    const BSP& getBSP() { return thebsp; }

}

#elif defined(EMBOT_ENABLE_hw_timer_emulated)

namespace embot::hw::timer {
    
    constexpr PROP tim01p = { };
    constexpr PROP tim02p = { };
    constexpr PROP tim03p = { };
    constexpr PROP tim04p = { };  
    constexpr PROP tim05p = { };
    constexpr PROP tim06p = { };  
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::one) | mask::pos2mask<uint32_t>(TIMER::two) | 
        mask::pos2mask<uint32_t>(TIMER::three) | mask::pos2mask<uint32_t>(TIMER::four) |
        mask::pos2mask<uint32_t>(TIMER::four) | mask::pos2mask<uint32_t>(TIMER::five),        
        // properties
        {{
            &tim01p, &tim02p, &tim03p, &tim04p, &tim05p, &tim06p, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16             
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}
        
    const BSP& getBSP() 
    {
        return thebsp;
    }    
}}}

#else

namespace embot::hw::timer {


//    TIM_HandleTypeDef htim1;
//    static const PROP tim1p = { .TIMx = TIM1,  .handle = &htim1,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };        
//    TIM_HandleTypeDef htim4;
//    static const PROP tim4p = { .TIMx = TIM4,  .handle = &htim4,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };        
//    TIM_HandleTypeDef htim5;
//    static const PROP tim5p = { .TIMx = TIM5,  .handle = &htim5,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
            
    
    // we use TIM1, TIM4, TIM5 for the motorhal and the amc uses 13, 15 and 16.
    // so, we can use at least TIM8 for embot::hw::timer

    TIM_HandleTypeDef htim8;
    static const PROP tim8p = { .TIMx = TIM8,  .handle = &htim8,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::one),        
        // properties
        {{
            &tim8p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,      // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16            
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}    
        

    const BSP& getBSP() 
    {
        return thebsp;
    }    

} // namespace embot::hw::timer {


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

// in here the required C functions

// as we share some timers w/ embot::hw::motor so we place HAL_TIM_Base_MspInit() in here and
// we manage TIMx (TIM1, TIM4, TIM5) with a dedicated embot::hw::motor::bsp::TIM_base_MspInit()
#include "embot_hw_motor_bsp_amc2c.h"

extern "C" {
    
    // the IRQ handler used by embot::hw::timer
    
    void TIM8_UP_TIM13_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::one, &embot::hw::timer::htim8);
    }

    // the MspInit / MspDeInit for all the timers
    
    void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
    {       
        if(TIM8 == htim_base->Instance)
        {
            __HAL_RCC_TIM8_CLK_ENABLE();
            HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
        }
        else
            {   // the others are used by embot::hw::motor::bsp        
            embot::hw::motor::bsp::TIM_base_MspInit(htim_base);
        }
    }
//      }
//      else if(htim_base->Instance==TIM16)
//      {
//      /* USER CODE BEGIN TIM16_MspInit 0 */
//
//      /* USER CODE END TIM16_MspInit 0 */
//        /* TIM15 clock enable */
//        __HAL_RCC_TIM16_CLK_ENABLE();
//
//        /* TIM16 interrupt Init */
//        HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
//        HAL_NVIC_EnableIRQ(TIM16_IRQn);
//      /* USER CODE BEGIN TIM15_MspInit 1 */
//
//      /* USER CODE END TIM15_MspInit 1 */
//      }
//    }

    void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
    {
        if(TIM8 == htim_base->Instance)
        {
            __HAL_RCC_TIM8_CLK_DISABLE();
            HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
        }
        else
        {   // the others are used by the motor
            embot::hw::motor::bsp::TIM_base_MspDeInit(htim_base);
        }
//      else if(tim_baseHandle->Instance==TIM16)
//      {
//      /* USER CODE BEGIN TIM16_MspDeInit 0 */

//      /* USER CODE END TIM16_MspDeInit 0 */
//        /* Peripheral clock disable */
//        __HAL_RCC_TIM16_CLK_DISABLE();

//        /* TIM16 interrupt Deinit */
//        HAL_NVIC_DisableIRQ(TIM16_IRQn);
//      /* USER CODE BEGIN TIM16_MspDeInit 1 */

//      /* USER CODE END TIM16_MspDeInit 1 */
//      }
    }
}

#endif // timer


// - support map: end of embot::hw::timer


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

