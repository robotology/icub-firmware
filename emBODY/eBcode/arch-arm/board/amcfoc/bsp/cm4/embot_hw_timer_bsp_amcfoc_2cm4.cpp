
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
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

#include "embot_hw_bsp_amc2cm4_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


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

#elif defined(EMBOT_ENABLE_hw_timer_emulated)

namespace embot { namespace hw { namespace timer {
        
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

namespace embot { namespace hw { namespace timer {

    // sadly we cannot use constexpr because of the reinterpret_cast<> inside TIM6 etc.
    TIM_HandleTypeDef htim13;
    static const PROP tim13p = { .TIMx = TIM13,  .handle = &htim13,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    TIM_HandleTypeDef htim15;
    static const PROP tim15p = { .TIMx = TIM15,  .handle = &htim15,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    TIM_HandleTypeDef htim16;
    static const PROP tim16p = { .TIMx = TIM16,  .handle = &htim16,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::one) | mask::pos2mask<uint32_t>(TIMER::two) | mask::pos2mask<uint32_t>(TIMER::three),        
        // properties
        {{
            &tim13p, &tim15p, &tim16p, nullptr, nullptr, nullptr, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16            
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}    

        
    const BSP& getBSP() 
    {
        return thebsp;
    }    
}}}

// in here it is implemented in the way the good old hal2 was doing: the handler directly manages the callback
// instead the stm hal make a lot of calls before actually calling the callback code, hence it is slower.

#include "embot_hw_timer.h"

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


extern "C" {
    
    void TIM8_UP_TIM13_IRQHandler(void)
    {
        //#warning TODO: cambiare il modo in cui si chiama la callback. usare le callback di stm32
        manageInterrupt(embot::hw::TIMER::one, &embot::hw::timer::htim13);
    }
    
    void TIM15_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::two, &embot::hw::timer::htim15);
    }
    
    void TIM16_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::three, &embot::hw::timer::htim16);
    }
}


extern "C"
{
    void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
    {
      if(tim_baseHandle->Instance==TIM13)
      {
      /* USER CODE BEGIN TIM13_MspInit 0 */

      /* USER CODE END TIM13_MspInit 0 */
        /* TIM13 clock enable */
        __HAL_RCC_TIM13_CLK_ENABLE();

        /* TIM13 interrupt Init */
        HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
      /* USER CODE BEGIN TIM13_MspInit 1 */

      /* USER CODE END TIM13_MspInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM15)
      {
      /* USER CODE BEGIN TIM15_MspInit 0 */

      /* USER CODE END TIM15_MspInit 0 */
        /* TIM15 clock enable */
        __HAL_RCC_TIM15_CLK_ENABLE();

        /* TIM15 interrupt Init */
        HAL_NVIC_SetPriority(TIM15_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM15_IRQn);
      /* USER CODE BEGIN TIM15_MspInit 1 */

      /* USER CODE END TIM15_MspInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM16)
      {
      /* USER CODE BEGIN TIM16_MspInit 0 */

      /* USER CODE END TIM16_MspInit 0 */
        /* TIM15 clock enable */
        __HAL_RCC_TIM16_CLK_ENABLE();

        /* TIM16 interrupt Init */
        HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM16_IRQn);
      /* USER CODE BEGIN TIM15_MspInit 1 */

      /* USER CODE END TIM15_MspInit 1 */
      }
    }

    void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
    {

      if(tim_baseHandle->Instance==TIM13)
      {
      /* USER CODE BEGIN TIM13_MspDeInit 0 */

      /* USER CODE END TIM13_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM13_CLK_DISABLE();

        /* TIM13 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
      /* USER CODE BEGIN TIM13_MspDeInit 1 */

      /* USER CODE END TIM13_MspDeInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM15)
      {
      /* USER CODE BEGIN TIM15_MspDeInit 0 */

      /* USER CODE END TIM15_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM15_CLK_DISABLE();

        /* TIM15 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM15_IRQn);
      /* USER CODE BEGIN TIM15_MspDeInit 1 */

      /* USER CODE END TIM15_MspDeInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM16)
      {
      /* USER CODE BEGIN TIM16_MspDeInit 0 */

      /* USER CODE END TIM16_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM16_CLK_DISABLE();

        /* TIM16 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM16_IRQn);
      /* USER CODE BEGIN TIM16_MspDeInit 1 */

      /* USER CODE END TIM16_MspDeInit 1 */
      }
    }
}

#endif // timer


// - support map: end of embot::hw::timer



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

