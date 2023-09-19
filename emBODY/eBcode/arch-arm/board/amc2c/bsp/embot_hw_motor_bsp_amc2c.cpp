
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bsp_amc2c.h"


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


#include "embot_hw_motor_bsp.h"
#include "motorhal_config.h"


#if !defined(EMBOT_ENABLE_hw_motor)

namespace embot::hw::motor::bsp {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::MOTOR h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else

namespace embot::hw::motor::bsp {
                  
    constexpr PROP propM1 { 0 };
    
    constexpr BSP thebsp {     

        // maskofsupported
        mask::pos2mask<uint32_t>(MOTOR::one),        
        // properties
        {{
            &propM1
        }}
  
    };
    

    void BSP::init(embot::hw::MOTOR h) const { // marker1
        
        amc2c::Init_MOTOR(h);
 
    }
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
        
              
} // namespace embot::hw::motor::bsp {

 

// the motorhal requires:
// ADC1, ADC2
// TIM1, TIM4, TIM5


namespace embot::hw::motor::bsp::amc2c {
    
    ADC_HandleTypeDef hADC1;
    ADC_HandleTypeDef hADC2;   
    DMA_HandleTypeDef hdma_adc1;    
    TIM_HandleTypeDef hTIM1;
    TIM_HandleTypeDef hTIM4;
    TIM_HandleTypeDef hTIM5;
}

namespace embot::hw::motor::bsp::amc2c {
    
    void MX_GPIO_Init();
    void MX_TIM4_Init();
    void MX_TIM5_Init();
    void MX_TIM1_Init();
    void MX_DMA_Init();
    void MX_ADC1_Init();
    void MX_ADC2_Init();
    
    
    void Init_MOTOR(embot::hw::MOTOR h)
    {
        // add in here the initialisation of the GPIO that are relevant to the motor.
        // keept out the break which is managed inside embot::hw::button::bsp         
        
        MX_GPIO_Init();
        MX_DMA_Init();
        MX_TIM4_Init();
//        MX_TIM5_Init(); // we start the TIM5 elsewhere
        MX_ADC1_Init();
        MX_ADC2_Init();
//        MX_DAC1_Init();
        MX_TIM1_Init();
        
        // something else
    }
    
    void Init_MOTORHAL_testmode(embot::hw::MOTOR h)
    {
        // add in here the initialisation of the GPIO that are relevant to the motor.
        // keept out the break which is managed inside embot::hw::button::bsp         
        
        MX_GPIO_Init();
        MX_DMA_Init();
        MX_TIM4_Init();
        MX_TIM5_Init(); // we start the TIM5 elsewhere
        MX_ADC1_Init();
        MX_ADC2_Init();
//        MX_DAC1_Init();
        MX_TIM1_Init();
        
        // something else
    }
    
    void MX_GPIO_Init()
    {
        // add in here the initialisation of the GPIO that are relevant to the motor.
        // keept out the break which is managed inside embot::hw::button::bsp             
        #warning TODO: fill MX_GPIO_Init() w/ code required by motor
        
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        // marco.accame: already done inside MX_GPIO_Init() in global scope
#if 0        
        /* GPIO Ports Clock Enable */
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
#endif
        /*Configure GPIO pin Output Level */
#if defined(MOTORHALCONFIG_TESTPOINT_DEBUG_remove) 
    HAL_GPIO_WritePin(GPIOC, MOT_EN3_Pin|MOT_EN2_Pin|MOT_EN1_Pin, GPIO_PIN_RESET);
#else        
        HAL_GPIO_WritePin(GPIOC, TP4_Pin|MOT_EN3_Pin|MOT_EN2_Pin|MOT_EN1_Pin
                              |TP3_Pin, GPIO_PIN_RESET);
#endif        

//        /*Configure GPIO pin Output Level */
//        HAL_GPIO_WritePin(GPIOH, nLED1_Pin|nLED2_Pin|nLED3_Pin|nLED5_Pin
//                              |nLED4_Pin|nLED6_Pin, GPIO_PIN_SET);

//        /*Configure GPIO pin Output Level */
//        HAL_GPIO_WritePin(GPIOG, I2C3_XEN_Pin|I2C2_XEN_Pin|I2C1_XEN_Pin, GPIO_PIN_RESET);

//        /*Configure GPIO pin Output Level */
//        HAL_GPIO_WritePin(GPIOD, SPI2_nXEN_Pin|SPI3_nXEN_Pin|SPI1_nXEN_Pin, GPIO_PIN_RESET);

        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(GPIOE, MOT_nRESET_Pin|MOT_nSLEEP_Pin, GPIO_PIN_RESET);

#if defined(MOTORHALCONFIG_TESTPOINT_DEBUG_remove) 
#else
        /*Configure GPIO pins : TP4_Pin TP3_Pin */
        GPIO_InitStruct.Pin = TP4_Pin|TP3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

//        /*Configure GPIO pins : nLED1_Pin nLED2_Pin nLED3_Pin nLED5_Pin
//                               nLED4_Pin nLED6_Pin */
//        GPIO_InitStruct.Pin = nLED1_Pin|nLED2_Pin|nLED3_Pin|nLED5_Pin
//                              |nLED4_Pin|nLED6_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        /*Configure GPIO pins : MOT_EN3_Pin MOT_EN2_Pin MOT_EN1_Pin */
        GPIO_InitStruct.Pin = MOT_EN3_Pin|MOT_EN2_Pin|MOT_EN1_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//        /*Configure GPIO pins : VER1_Pin VER0_Pin VER2_Pin */
//        GPIO_InitStruct.Pin = VER1_Pin|VER0_Pin|VER2_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//        /*Configure GPIO pins : I2C3_XEN_Pin I2C2_XEN_Pin I2C1_XEN_Pin */
//        GPIO_InitStruct.Pin = I2C3_XEN_Pin|I2C2_XEN_Pin|I2C1_XEN_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//        /*Configure GPIO pins : SPI2_nXEN_Pin SPI3_nXEN_Pin SPI1_nXEN_Pin */
//        GPIO_InitStruct.Pin = SPI2_nXEN_Pin|SPI3_nXEN_Pin|SPI1_nXEN_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


        /*Configure GPIO pins : MOT_nRESET_Pin MOT_nSLEEP_Pin */
        GPIO_InitStruct.Pin = MOT_nRESET_Pin|MOT_nSLEEP_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

//        /*Configure GPIO pin : BOOT1_Pin */
//        GPIO_InitStruct.Pin = BOOT1_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);
    }
    
    void MX_DMA_Init()
    {

//        if (ResMgr_Request(RESMGR_ID_DMA1, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMA1 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMA1 */
//        }
//        if (ResMgr_Request(RESMGR_ID_DMA1_Stream2, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMA1_Stream2 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMA1_Stream2 */
//        }
//        if (ResMgr_Request(RESMGR_ID_DMA1_Stream3, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMA1_Stream3 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMA1_Stream3 */
//        }
//        if (ResMgr_Request(RESMGR_ID_DMA2, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMA2 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMA2 */
//        }
//        if (ResMgr_Request(RESMGR_ID_DMAMUX1, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMAMUX1 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMAMUX1 */
//        }
//        if (ResMgr_Request(RESMGR_ID_DMAMUX1_Channel2, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMAMUX1_Channel2 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMAMUX1_Channel2 */
//        }
//        if (ResMgr_Request(RESMGR_ID_DMAMUX1_Channel3, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_DMAMUX1_Channel3 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_DMAMUX1_Channel3 */
//        }

        /* DMA controller clock enable */
        __HAL_RCC_DMA2_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* DMA interrupt init */
        /* DMA2_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    }   
    
    void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
    {
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      if(htim->Instance==TIM1)
      {
      /* USER CODE BEGIN TIM1_MspPostInit 0 */

      /* USER CODE END TIM1_MspPostInit 0 */

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**TIM1 GPIO Configuration
        PA10     ------> TIM1_CH3
        PA9     ------> TIM1_CH2
        PE9     ------> TIM1_CH1
        */
        GPIO_InitStruct.Pin = MOT_PWM3_Pin|MOT_PWM2_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOT_PWM1_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(MOT_PWM1_GPIO_Port, &GPIO_InitStruct);

      /* USER CODE BEGIN TIM1_MspPostInit 1 */

      /* USER CODE END TIM1_MspPostInit 1 */
      }

    }    

    void MX_TIM1_Init(void)
    {

//        if (ResMgr_Request(RESMGR_ID_TIM1, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_TIM1 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_TIM1 */
//        }
//        /* USER CODE BEGIN TIM1_Init 0 */

//        /* USER CODE END TIM1_Init 0 */

        TIM_ClockConfigTypeDef sClockSourceConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIMEx_BreakInputConfigTypeDef sBreakInputConfig = {0};
        TIM_OC_InitTypeDef sConfigOC = {0};
        TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

        /* USER CODE BEGIN TIM1_Init 1 */

        /* USER CODE END TIM1_Init 1 */
        hTIM1.Instance = TIM1;
        hTIM1.Init.Prescaler = 0;
        hTIM1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
        hTIM1.Init.Period = 1219;
        hTIM1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        hTIM1.Init.RepetitionCounter = 0;
        hTIM1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
        if (HAL_TIM_Base_Init(&hTIM1) != HAL_OK)
        {
        Error_Handler();
        }
        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(&hTIM1, &sClockSourceConfig) != HAL_OK)
        {
        Error_Handler();
        }
        if (HAL_TIM_PWM_Init(&hTIM1) != HAL_OK)
        {
        Error_Handler();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_OC6REF;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&hTIM1, &sMasterConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sBreakInputConfig.Source = TIM_BREAKINPUTSOURCE_BKIN;
        sBreakInputConfig.Enable = TIM_BREAKINPUTSOURCE_ENABLE;
        sBreakInputConfig.Polarity = TIM_BREAKINPUTSOURCE_POLARITY_LOW;
        if (HAL_TIMEx_ConfigBreakInput(&hTIM1, TIM_BREAKINPUT_BRK, &sBreakInputConfig) != HAL_OK)
        {
        Error_Handler();
        }
        if (HAL_TIMEx_ConfigBreakInput(&hTIM1, TIM_BREAKINPUT_BRK2, &sBreakInputConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 0;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
        sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        if (HAL_TIM_PWM_ConfigChannel(&hTIM1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
        {
        Error_Handler();
        }
        if (HAL_TIM_PWM_ConfigChannel(&hTIM1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
        {
        Error_Handler();
        }
        if (HAL_TIM_PWM_ConfigChannel(&hTIM1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
        {
        Error_Handler();
        }
        sConfigOC.OCMode = TIM_OCMODE_PWM2;
        sConfigOC.Pulse = 895; // was 752, changed due to different clock source from amc-bldc
        if (HAL_TIM_PWM_ConfigChannel(&hTIM1, &sConfigOC, TIM_CHANNEL_6) != HAL_OK)
        {
        Error_Handler();
        }
        sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
        sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
        sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
        sBreakDeadTimeConfig.DeadTime = 0;
        sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
        sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
        sBreakDeadTimeConfig.BreakFilter = 15;
        sBreakDeadTimeConfig.Break2State = TIM_BREAK2_ENABLE;
        sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
        sBreakDeadTimeConfig.Break2Filter = 0;
        sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
        if (HAL_TIMEx_ConfigBreakDeadTime(&hTIM1, &sBreakDeadTimeConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /* USER CODE BEGIN TIM1_Init 2 */

        /* USER CODE END TIM1_Init 2 */
        HAL_TIM_MspPostInit(&hTIM1);

    }

    void MX_TIM4_Init(void)
    {
//        if (ResMgr_Request(RESMGR_ID_TIM4, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_TIM4 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_TIM4 */
//        }
//        /* USER CODE BEGIN TIM4_Init 0 */

//        /* USER CODE END TIM4_Init 0 */

        TIM_ClockConfigTypeDef sClockSourceConfig = {0};
        TIM_HallSensor_InitTypeDef sConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};

        /* USER CODE BEGIN TIM4_Init 1 */

        /* USER CODE END TIM4_Init 1 */
        hTIM4.Instance = TIM4;
        hTIM4.Init.Prescaler = 200;
        hTIM4.Init.CounterMode = TIM_COUNTERMODE_UP;
        hTIM4.Init.Period = 65535;
        hTIM4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        hTIM4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_Base_Init(&hTIM4) != HAL_OK)
        {
        Error_Handler();
        }
        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(&hTIM4, &sClockSourceConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC1Filter = 4;
        sConfig.Commutation_Delay = 0;
        if (HAL_TIMEx_HallSensor_Init(&hTIM4, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&hTIM4, &sMasterConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /* USER CODE BEGIN TIM4_Init 2 */

        /* USER CODE END TIM4_Init 2 */

    }


    void MX_TIM5_Init(void)
    {

//        if (ResMgr_Request(RESMGR_ID_TIM5, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_TIM5 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_TIM5 */
//        }
//        /* USER CODE BEGIN TIM5_Init 0 */

//        /* USER CODE END TIM5_Init 0 */

        TIM_Encoder_InitTypeDef sConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIM_IC_InitTypeDef sConfigIC = {0};

        /* USER CODE BEGIN TIM5_Init 1 */

        /* USER CODE END TIM5_Init 1 */
        hTIM5.Instance = TIM5;
        hTIM5.Init.Prescaler = 0;
        hTIM5.Init.CounterMode = TIM_COUNTERMODE_UP;
        hTIM5.Init.Period = 4294967295;
        hTIM5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
        hTIM5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_IC_Init(&hTIM5) != HAL_OK)
        {
        Error_Handler();
        }
        sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
        sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
        sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC1Filter = 8;
        sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
        sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC2Filter = 15;
        if (HAL_TIM_Encoder_Init(&hTIM5, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&hTIM5, &sMasterConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
        sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
        sConfigIC.ICFilter = 8;
        if (HAL_TIM_IC_ConfigChannel(&hTIM5, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
        {
        Error_Handler();
        }
        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
        sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
        sConfigIC.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&hTIM5, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
        {
        Error_Handler();
        }
        /* USER CODE BEGIN TIM5_Init 2 */

        /* USER CODE END TIM5_Init 2 */

    }    
    
    
    void DeInit_TIM5()
    {
        // HAL_TIM_IC_ConfigChannel() inversa for TIM_CHANNEL_3 and TIM_CHANNEL_4
        // HAL_TIMEx_MasterConfigSynchronization() inversa
        HAL_TIM_Encoder_DeInit(&hTIM5);
        
        HAL_TIM_IC_DeInit(&hTIM5);
        
    }

    void Init_TIM5(int16_t _resolution, uint8_t _num_polar_couples)
    {
        uint32_t _encodermode = TIM_ENCODERMODE_TI12;
        uint32_t _countermode = TIM_COUNTERMODE_UP;
        uint32_t _clockdivision = TIM_CLOCKDIVISION_DIV1; // originally = TIM_CLOCKDIVISION_DIV4, amcbldc = TIM_CLOCKDIVISION_DIV1
        uint32_t _ic1filter = 4; // amcbldc is = 4, in here amc is originally = 8 
        uint32_t _ic2filter = 4; // amcbldc is = 4, in here amc is originally = 15 
        if(_resolution < 0)
        {
            _resolution = - _resolution;
            _countermode = TIM_COUNTERMODE_DOWN;            
        }

        uint32_t _period = (_resolution/_num_polar_couples + 0.5) - 1;
       

    //        if (ResMgr_Request(RESMGR_ID_TIM5, RESMGR_FLAGS_ACCESS_NORMAL | \
    //                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
    //        {
    //        /* USER CODE BEGIN RESMGR_UTILITY_TIM5 */
    //        Error_Handler();
    //        /* USER CODE END RESMGR_UTILITY_TIM5 */
    //        }
    //        /* USER CODE BEGIN TIM5_Init 0 */

    //        /* USER CODE END TIM5_Init 0 */

        TIM_Encoder_InitTypeDef sConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIM_IC_InitTypeDef sConfigIC = {0};

        /* USER CODE BEGIN TIM5_Init 1 */

        /* USER CODE END TIM5_Init 1 */
        hTIM5.Instance = TIM5;
        hTIM5.Init.Prescaler = 0;
        hTIM5.Init.CounterMode = _countermode;
        hTIM5.Init.Period = _period;
        hTIM5.Init.ClockDivision = _clockdivision;
        hTIM5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_IC_Init(&hTIM5) != HAL_OK)
        {
        Error_Handler();
        }
        sConfig.EncoderMode = _encodermode;
        sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
        sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC1Filter = _ic1filter;
        sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
        sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC2Filter = _ic2filter;
        if (HAL_TIM_Encoder_Init(&hTIM5, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&hTIM5, &sMasterConfig) != HAL_OK)
        {
        Error_Handler();
        }
        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
        sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
        sConfigIC.ICFilter = 8;
        if (HAL_TIM_IC_ConfigChannel(&hTIM5, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
        {
        Error_Handler();
        }
        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
        sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
        sConfigIC.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&hTIM5, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
        {
        Error_Handler();
        }
        /* USER CODE BEGIN TIM5_Init 2 */

        /* USER CODE END TIM5_Init 2 */

    }      
    
    void MX_ADC1_Init(void)
    {

//        if (ResMgr_Request(RESMGR_ID_ADC1, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_ADC1 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_ADC1 */
//        }
//        if (ResMgr_Request(RESMGR_ID_ADC12_COMMON, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_ADC12_COMMON */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_ADC12_COMMON */
//        }
//        /* USER CODE BEGIN ADC1_Init 0 */

//        /* USER CODE END ADC1_Init 0 */

        ADC_MultiModeTypeDef multimode = {0};
        ADC_ChannelConfTypeDef sConfig = {0};

        /* USER CODE BEGIN ADC1_Init 1 */

        /* USER CODE END ADC1_Init 1 */
        /** Common config
        */
        hADC1.Instance = ADC1;
        hADC1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
        hADC1.Init.Resolution = ADC_RESOLUTION_16B;
        hADC1.Init.ScanConvMode = ADC_SCAN_ENABLE;
        hADC1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hADC1.Init.LowPowerAutoWait = DISABLE;
        hADC1.Init.ContinuousConvMode = DISABLE;
        hADC1.Init.NbrOfConversion = 3;
        hADC1.Init.DiscontinuousConvMode = ENABLE;
        hADC1.Init.NbrOfDiscConversion = 1;
        hADC1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO2;
        hADC1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
        hADC1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
        hADC1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
        hADC1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        hADC1.Init.OversamplingMode = DISABLE;
        if (HAL_ADC_Init(&hADC1) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure the ADC multi-mode
        */
        multimode.Mode = ADC_DUALMODE_REGSIMULT;
        multimode.DualModeData = ADC_DUALMODEDATAFORMAT_32_10_BITS;
        multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_9CYCLES;
        if (HAL_ADCEx_MultiModeConfigChannel(&hADC1, &multimode) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        
#if 0
        
        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_1;
//        sConfig.Offset = 29789;
//        sConfig.OffsetRightShift = DISABLE;
//        sConfig.OffsetSignedSaturation = ENABLE;
//        sConfig.Offset = 0;
//        sConfig.OffsetRightShift = DISABLE;
//        sConfig.OffsetSignedSaturation = DISABLE;
        sConfig.Offset = 29496;
        sConfig.OffsetRightShift = DISABLE;
        sConfig.OffsetSignedSaturation = ENABLE;
        if (HAL_ADC_ConfigChannel(&hADC1, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = ADC_REGULAR_RANK_2;
        sConfig.OffsetNumber = ADC_OFFSET_2;
        sConfig.Offset = 29634;
        if (HAL_ADC_ConfigChannel(&hADC1, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = ADC_REGULAR_RANK_3;
        sConfig.OffsetNumber = ADC_OFFSET_3;
        sConfig.Offset = 29020;
        if (HAL_ADC_ConfigChannel(&hADC1, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        
#else
// zero offset in ADC        
        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        sConfig.OffsetRightShift = DISABLE;
        sConfig.OffsetSignedSaturation = DISABLE;
        if (HAL_ADC_ConfigChannel(&hADC1, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = ADC_REGULAR_RANK_2;


        if (HAL_ADC_ConfigChannel(&hADC1, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = ADC_REGULAR_RANK_3;

        if (HAL_ADC_ConfigChannel(&hADC1, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
#endif        
        /* USER CODE BEGIN ADC1_Init 2 */

        /* USER CODE END ADC1_Init 2 */

    }


    void MX_ADC2_Init(void)
    {

//        if (ResMgr_Request(RESMGR_ID_ADC2, RESMGR_FLAGS_ACCESS_NORMAL | \
//                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//        {
//        /* USER CODE BEGIN RESMGR_UTILITY_ADC2 */
//        Error_Handler();
//        /* USER CODE END RESMGR_UTILITY_ADC2 */
//        }
//        /* USER CODE BEGIN ADC2_Init 0 */

//        /* USER CODE END ADC2_Init 0 */

        ADC_ChannelConfTypeDef sConfig = {0};

        /* USER CODE BEGIN ADC2_Init 1 */

        /* USER CODE END ADC2_Init 1 */
        /** Common config
        */
        hADC2.Instance = ADC2;
        hADC2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
        hADC2.Init.Resolution = ADC_RESOLUTION_16B;
        hADC2.Init.ScanConvMode = ADC_SCAN_ENABLE;
        hADC2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hADC2.Init.LowPowerAutoWait = DISABLE;
        hADC2.Init.ContinuousConvMode = DISABLE;
        hADC2.Init.NbrOfConversion = 3;
        hADC2.Init.DiscontinuousConvMode = ENABLE;
        hADC2.Init.NbrOfDiscConversion = 1;
        hADC2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
        hADC2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
        hADC2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        hADC2.Init.OversamplingMode = DISABLE;
        if (HAL_ADC_Init(&hADC2) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_9;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_16CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        sConfig.OffsetSignedSaturation = DISABLE;
        if (HAL_ADC_ConfigChannel(&hADC2, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_12;
        sConfig.Rank = ADC_REGULAR_RANK_2;
        if (HAL_ADC_ConfigChannel(&hADC2, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_13;
        sConfig.Rank = ADC_REGULAR_RANK_3;
        if (HAL_ADC_ConfigChannel(&hADC2, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /* USER CODE BEGIN ADC2_Init 2 */

        /* USER CODE END ADC2_Init 2 */

    }    

} // namespace embot::hw::motor::bsp {


// in here the required IRQ handlers, MspInit and MspDeInit 

// ADC1, ADC2 and assciated DMA
extern "C" {
    
    // - irq handlers
    
    // ADC1 and ADC2 global interrupts
    void ADC_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(&embot::hw::motor::bsp::amc2c::hADC1);
        HAL_ADC_IRQHandler(&embot::hw::motor::bsp::amc2c::hADC2);
    }   

    // DMA2 stream0 global interrupt
    void DMA2_Stream0_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::motor::bsp::amc2c::hdma_adc1);
    }  

    // - msp functions    
    
    static uint32_t HAL_RCC_ADC12_CLK_ENABLED=0;
    
    void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        
        if(hadc->Instance==ADC1)
        {
            /* USER CODE BEGIN ADC1_MspInit 0 */

            /* USER CODE END ADC1_MspInit 0 */
            /* Peripheral clock enable */
            HAL_RCC_ADC12_CLK_ENABLED++;
            if(HAL_RCC_ADC12_CLK_ENABLED==1){
              __HAL_RCC_ADC12_CLK_ENABLE();
            }

            __HAL_RCC_GPIOC_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            __HAL_RCC_GPIOB_CLK_ENABLE();
            /**ADC1 GPIO Configuration
            PC2     ------> ADC1_INP12
            PC3     ------> ADC1_INP13
            PA1_C     ------> ADC1_INP1
            PA0_C     ------> ADC1_INP0
            PA6     ------> ADC1_INP3
            PB0     ------> ADC1_INP9
            */
            GPIO_InitStruct.Pin = MOT_aVPH2_Pin|MOT_aVPH3_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = MOT_aCPH3_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(MOT_aCPH3_GPIO_Port, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = MOT_aVPH1_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(MOT_aVPH1_GPIO_Port, &GPIO_InitStruct);

            HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_OPEN);

            HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_OPEN);

            /* ADC1 DMA Init */
            /* ADC1 Init */
            embot::hw::motor::bsp::amc2c::hdma_adc1.Instance = DMA2_Stream0;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.Mode = DMA_CIRCULAR;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
            embot::hw::motor::bsp::amc2c::hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&embot::hw::motor::bsp::amc2c::hdma_adc1) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_LINKDMA(hadc,DMA_Handle,embot::hw::motor::bsp::amc2c::hdma_adc1);

            /* ADC1 interrupt Init */
            HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(ADC_IRQn);
            /* USER CODE BEGIN ADC1_MspInit 1 */

            /* USER CODE END ADC1_MspInit 1 */
        }
        else if(hadc->Instance==ADC2)
        {
            /* USER CODE BEGIN ADC2_MspInit 0 */

            /* USER CODE END ADC2_MspInit 0 */
            /* Peripheral clock enable */
            HAL_RCC_ADC12_CLK_ENABLED++;
            if(HAL_RCC_ADC12_CLK_ENABLED==1){
              __HAL_RCC_ADC12_CLK_ENABLE();
            }

            __HAL_RCC_GPIOC_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            __HAL_RCC_GPIOB_CLK_ENABLE();
            /**ADC2 GPIO Configuration
            PC2     ------> ADC2_INP12
            PC3     ------> ADC2_INP13
            PA1_C     ------> ADC2_INP1
            PA0_C     ------> ADC2_INP0
            PA6     ------> ADC2_INP3
            PB0     ------> ADC2_INP9
            */
            GPIO_InitStruct.Pin = MOT_aVPH2_Pin|MOT_aVPH3_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = MOT_aCPH3_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(MOT_aCPH3_GPIO_Port, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = MOT_aVPH1_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(MOT_aVPH1_GPIO_Port, &GPIO_InitStruct);

            HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_OPEN);

            HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_OPEN);

            /* ADC2 interrupt Init */
            HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(ADC_IRQn);
            /* USER CODE BEGIN ADC2_MspInit 1 */

            /* USER CODE END ADC2_MspInit 1 */
        }

    }

    void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
    {
        if(hadc->Instance==ADC1)
        {
            /* USER CODE BEGIN ADC1_MspDeInit 0 */

            /* USER CODE END ADC1_MspDeInit 0 */
            /* Peripheral clock disable */
            HAL_RCC_ADC12_CLK_ENABLED--;
            if(HAL_RCC_ADC12_CLK_ENABLED==0){
              __HAL_RCC_ADC12_CLK_DISABLE();
            }

            /**ADC1 GPIO Configuration
            PC2     ------> ADC1_INP12
            PC3     ------> ADC1_INP13
            PA1_C     ------> ADC1_INP1
            PA0_C     ------> ADC1_INP0
            PA6     ------> ADC1_INP3
            PB0     ------> ADC1_INP9
            */
            HAL_GPIO_DeInit(GPIOC, MOT_aVPH2_Pin|MOT_aVPH3_Pin);

            HAL_GPIO_DeInit(MOT_aCPH3_GPIO_Port, MOT_aCPH3_Pin);

            HAL_GPIO_DeInit(MOT_aVPH1_GPIO_Port, MOT_aVPH1_Pin);

            /* ADC1 DMA DeInit */
            HAL_DMA_DeInit(hadc->DMA_Handle);

            /* ADC1 interrupt DeInit */
            /* USER CODE BEGIN ADC1:ADC_IRQn disable */
            /**
            * Uncomment the line below to disable the "ADC_IRQn" interrupt
            * Be aware, disabling shared interrupt may affect other IPs
            */
            /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
            /* USER CODE END ADC1:ADC_IRQn disable */

            /* USER CODE BEGIN ADC1_MspDeInit 1 */

            /* USER CODE END ADC1_MspDeInit 1 */
        }
        else if(hadc->Instance==ADC2)
        {
            /* USER CODE BEGIN ADC2_MspDeInit 0 */

            /* USER CODE END ADC2_MspDeInit 0 */
            /* Peripheral clock disable */
            HAL_RCC_ADC12_CLK_ENABLED--;
            if(HAL_RCC_ADC12_CLK_ENABLED==0){
              __HAL_RCC_ADC12_CLK_DISABLE();
            }

            /**ADC2 GPIO Configuration
            PC2     ------> ADC2_INP12
            PC3     ------> ADC2_INP13
            PA1_C     ------> ADC2_INP1
            PA0_C     ------> ADC2_INP0
            PA6     ------> ADC2_INP3
            PB0     ------> ADC2_INP9
            */
            HAL_GPIO_DeInit(GPIOC, MOT_aVPH2_Pin|MOT_aVPH3_Pin);

            HAL_GPIO_DeInit(MOT_aCPH3_GPIO_Port, MOT_aCPH3_Pin);

            HAL_GPIO_DeInit(MOT_aVPH1_GPIO_Port, MOT_aVPH1_Pin);

            /* ADC2 interrupt DeInit */
            /* USER CODE BEGIN ADC2:ADC_IRQn disable */
            /**
            * Uncomment the line below to disable the "ADC_IRQn" interrupt
            * Be aware, disabling shared interrupt may affect other IPs
            */
            /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
            /* USER CODE END ADC2:ADC_IRQn disable */

            /* USER CODE BEGIN ADC2_MspDeInit 1 */

            /* USER CODE END ADC2_MspDeInit 1 */
        }

    }    


}

// TIM1, TIM4, TIM5

extern "C" {
    
    // - irq handlers
    
    // TIM1 break interrupt
    void TIM1_BRK_IRQHandler(void)
    {
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove) 
        // this mode is not enabled and we get the external fault w/ polling
        for(;;);
#else        
        HAL_TIM_IRQHandler(&embot::hw::motor::bsp::amc2c::hTIM1);
#endif
    }

    // TIM1 update interrupt
    void TIM1_UP_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bsp::amc2c::hTIM1);
    }

    // TIM1 trigger and commutation interrupts
    void TIM1_TRG_COM_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bsp::amc2c::hTIM1);
    }

    // TIM1 capture compare interrupt
    void TIM1_CC_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bsp::amc2c::hTIM1);
    }   
    
    // TIM4 global interrupt
    void TIM4_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bsp::amc2c::hTIM4);
    }
    
    // TIM5 global interrupt
    void TIM5_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bsp::amc2c::hTIM5);
    }

    // msp functions

    
#if 0
    for TIM1 and TIM4, we need
    void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);
    void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
    But they are inside the embot::hw::timer section and need only
    - embot::hw::motor::bsp::amc2c::TIM_base_MspInit()
    - embot::hw::motor::bsp::amc2c::TIM_base_MspDeInit()
#endif
    
    void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim_ic)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        
        if(htim_ic->Instance==TIM5)
        {
            /* USER CODE BEGIN TIM5_MspInit 0 */

            /* USER CODE END TIM5_MspInit 0 */
            /* Peripheral clock enable */
            __HAL_RCC_TIM5_CLK_ENABLE();

            __HAL_RCC_GPIOH_CLK_ENABLE();
            /**TIM5 GPIO Configuration
            PH11     ------> TIM5_CH2
            PH10     ------> TIM5_CH1
            PH12     ------> TIM5_CH3
            */
            GPIO_InitStruct.Pin = MOT_ENCB_Pin|MOT_ENCA_Pin|MOT_ENCZ_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
            HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

            /* TIM5 interrupt Init */
            HAL_NVIC_SetPriority(TIM5_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(TIM5_IRQn);
            /* USER CODE BEGIN TIM5_MspInit 1 */

            /* USER CODE END TIM5_MspInit 1 */
        }
    } 

    void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* htim_ic)
    {
        if(htim_ic->Instance==TIM5)
        {
            /* USER CODE BEGIN TIM5_MspDeInit 0 */

            /* USER CODE END TIM5_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_TIM5_CLK_DISABLE();

            /**TIM5 GPIO Configuration
            PH11     ------> TIM5_CH2
            PH10     ------> TIM5_CH1
            PH12     ------> TIM5_CH3
            */
            HAL_GPIO_DeInit(GPIOH, MOT_ENCB_Pin|MOT_ENCA_Pin|MOT_ENCZ_Pin);

            /* TIM5 interrupt DeInit */
            HAL_NVIC_DisableIRQ(TIM5_IRQn);
            /* USER CODE BEGIN TIM5_MspDeInit 1 */

            /* USER CODE END TIM5_MspDeInit 1 */
        }
    }
   
}


namespace embot::hw::motor::bsp::amc2c {
    
    void TIM_base_MspInit(TIM_HandleTypeDef* htim_base)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        
        if(htim_base->Instance==TIM1)
        {
            /* USER CODE BEGIN TIM1_MspInit 0 */

            /* USER CODE END TIM1_MspInit 0 */
            /* Peripheral clock enable */
            __HAL_RCC_TIM1_CLK_ENABLE();

            __HAL_RCC_GPIOG_CLK_ENABLE();
            __HAL_RCC_GPIOE_CLK_ENABLE();
            /**TIM1 GPIO Configuration
            PG4     ------> TIM1_BKIN2
            PE15     ------> TIM1_BKIN
            */
            GPIO_InitStruct.Pin = MOT_nFAULT_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
            HAL_GPIO_Init(MOT_nFAULT_GPIO_Port, &GPIO_InitStruct);
            
#warning marco.accame: removed the BREAK on IRQ
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
#else
            GPIO_InitStruct.Pin = MOT_BREAK_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
            HAL_GPIO_Init(MOT_BREAK_GPIO_Port, &GPIO_InitStruct);
#endif
            /* TIM1 interrupt Init */
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove) 
#else
            HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);
#endif            
//            HAL_NVIC_SetPriority(TIM1_UP_IRQn, 5, 0);
//            HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
            HAL_NVIC_SetPriority(TIM1_TRG_COM_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);
//            HAL_NVIC_SetPriority(TIM1_CC_IRQn, 5, 0);
//            HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
            /* USER CODE BEGIN TIM1_MspInit 1 */

            /* USER CODE END TIM1_MspInit 1 */
        }
        else if(htim_base->Instance==TIM4)
        {
            /* USER CODE BEGIN TIM4_MspInit 0 */

            /* USER CODE END TIM4_MspInit 0 */
            /* Peripheral clock enable */
            __HAL_RCC_TIM4_CLK_ENABLE();

            __HAL_RCC_GPIOD_CLK_ENABLE();
            /**TIM4 GPIO Configuration
            PD14     ------> TIM4_CH3
            PD13     ------> TIM4_CH2
            PD12     ------> TIM4_CH1
            */
            GPIO_InitStruct.Pin = MOT_HALL3_Pin|MOT_HALL2_Pin|MOT_HALL1_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
            HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

            /* TIM4 interrupt Init */
            HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(TIM4_IRQn);
            /* USER CODE BEGIN TIM4_MspInit 1 */

            /* USER CODE END TIM4_MspInit 1 */
        }     
    }

    void TIM_base_MspDeInit(TIM_HandleTypeDef* htim_base)
    {
        if(htim_base->Instance==TIM1)
        {
            /* USER CODE BEGIN TIM1_MspDeInit 0 */

            /* USER CODE END TIM1_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_TIM1_CLK_DISABLE();

            /**TIM1 GPIO Configuration
            PA10     ------> TIM1_CH3
            PA9     ------> TIM1_CH2
            PG4     ------> TIM1_BKIN2
            PE15     ------> TIM1_BKIN
            PE9     ------> TIM1_CH1
            */
            HAL_GPIO_DeInit(GPIOA, MOT_PWM3_Pin|MOT_PWM2_Pin);

            HAL_GPIO_DeInit(MOT_nFAULT_GPIO_Port, MOT_nFAULT_Pin);
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove) 
#else
            HAL_GPIO_DeInit(GPIOE, MOT_BREAK_Pin);
#endif            
            HAL_GPIO_DeInit(GPIOE, MOT_PWM1_Pin);

            /* TIM1 interrupt DeInit */
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove) 
#else            
            HAL_NVIC_DisableIRQ(TIM1_BRK_IRQn);
#endif            
//            HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
            HAL_NVIC_DisableIRQ(TIM1_TRG_COM_IRQn);
//            HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
            /* USER CODE BEGIN TIM1_MspDeInit 1 */

            /* USER CODE END TIM1_MspDeInit 1 */
        }
        else if(htim_base->Instance==TIM4)
        {
            /* USER CODE BEGIN TIM4_MspDeInit 0 */

            /* USER CODE END TIM4_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_TIM4_CLK_DISABLE();

            /**TIM4 GPIO Configuration
            PD14     ------> TIM4_CH3
            PD13     ------> TIM4_CH2
            PD12     ------> TIM4_CH1
            */
            HAL_GPIO_DeInit(GPIOD, MOT_HALL3_Pin|MOT_HALL2_Pin|MOT_HALL1_Pin);

            /* TIM4 interrupt DeInit */
            HAL_NVIC_DisableIRQ(TIM4_IRQn);
            /* USER CODE BEGIN TIM4_MspDeInit 1 */

            /* USER CODE END TIM4_MspDeInit 1 */
        }  
    }    
} // namespace embot::hw::motor::bsp::amc2c {



#endif // motor





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

