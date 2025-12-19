
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

#include "embot_hw_bsp_config.h"


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
        mask::pos2mask<uint32_t>(TIMER::five) | mask::pos2mask<uint32_t>(TIMER::six),        
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
    TIM_HandleTypeDef htim7;
    static const PROP tim7p = { .TIMx = TIM7,  .handle = &htim7,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    TIM_HandleTypeDef htim16;
    static const PROP tim16p = { .TIMx = TIM16,  .handle = &htim16,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    TIM_HandleTypeDef htim17;
    static const PROP tim17p = { .TIMx = TIM17,  .handle = &htim17,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::one) | mask::pos2mask<uint32_t>(TIMER::two) | mask::pos2mask<uint32_t>(TIMER::three),        
        // properties
        {{
            &tim7p, &tim16p, &tim17p, nullptr, nullptr, nullptr, nullptr, nullptr,     // from 1 to 8
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
    
    void TIM7_IRQHandler(void)
    {
        //#warning TODO: cambiare il modo in cui si chiama la callback. usare le callback di stm32
        manageInterrupt(embot::hw::TIMER::one, &embot::hw::timer::htim7);
    }
    
    void TIM16_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::two, &embot::hw::timer::htim16);
    }
    
    void TIM17_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::three, &embot::hw::timer::htim17);
    }
}


extern "C"
{
    void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
    {
      if(tim_baseHandle->Instance==TIM7)
      {
      /* USER CODE BEGIN TIM7_MspInit 0 */

      /* USER CODE END TIM7_MspInit 0 */
        /* TIM7 clock enable */
        __HAL_RCC_TIM7_CLK_ENABLE();

        /* TIM7 interrupt Init */
        HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM7_IRQn);
      /* USER CODE BEGIN TIM7_MspInit 1 */

      /* USER CODE END TIM7_MspInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM16)
      {
      /* USER CODE BEGIN TIM16_MspInit 0 */

      /* USER CODE END TIM16_MspInit 0 */
        /* TIM16 clock enable */
        __HAL_RCC_TIM16_CLK_ENABLE();

        /* TIM16 interrupt Init */
        HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM16_IRQn);
      /* USER CODE BEGIN TIM16_MspInit 1 */

      /* USER CODE END TIM16_MspInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM17)
      {
      /* USER CODE BEGIN TIM17_MspInit 0 */

      /* USER CODE END TIM17_MspInit 0 */
        /* TIM17 clock enable */
        __HAL_RCC_TIM17_CLK_ENABLE();

        /* TIM17 interrupt Init */
        HAL_NVIC_SetPriority(TIM17_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM17_IRQn);
      /* USER CODE BEGIN TIM17_MspInit 1 */

      /* USER CODE END TIM17_MspInit 1 */
      }
    }

    void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
    {

      if(tim_baseHandle->Instance==TIM7)
      {
      /* USER CODE BEGIN TIM7_MspDeInit 0 */

      /* USER CODE END TIM7_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM7_CLK_DISABLE();

        /* TIM7 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM7_IRQn);
      /* USER CODE BEGIN TIM7_MspDeInit 1 */

      /* USER CODE END TIM7_MspDeInit 1 */
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
      else if(tim_baseHandle->Instance==TIM17)
      {
      /* USER CODE BEGIN TIM17_MspDeInit 0 */

      /* USER CODE END TIM17_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM17_CLK_DISABLE();

        /* TIM17 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM17_IRQn);
      /* USER CODE BEGIN TIM17_MspDeInit 1 */

      /* USER CODE END TIM17_MspDeInit 1 */
      }
    }
}

#endif // timer


// - support map: end of embot::hw::timer



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

