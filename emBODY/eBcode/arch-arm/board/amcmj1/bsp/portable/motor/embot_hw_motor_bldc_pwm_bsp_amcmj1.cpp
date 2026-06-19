
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_pwm_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_bldc_pwm)

namespace embot::hw::motor::bldc::pwm::bsp {
    
    constexpr BSP thebsp { };   
     
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        return false;       
    } 
    
    void BSP::set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &pwm) const
    {
    }     

    void BSP::enable(embot::hw::MOTOR m, bool on) const 
    {   
    } 
    
    const TIMconfig & BSP::timconfig() const
    {
        static constexpr TIMconfig tc {};
        return tc;
    }
    
    const BSP& getBSP() 
    {
        return thebsp;
    } 

} // namespace embot::hw::motor::bldc::pwm::bsp {

namespace embot::hw::motor::bldc::pwm::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p) {};          
    void HAL_ADC_MspDeInit(void* p) {};        
    
    void HAL_TIM_Base_MspInit(void* p) {};     
    void HAL_TIM_Base_MspDeInit(void* p) {};       
        
} // namespace embot::hw::motor::bldc::pwm::bsp::stm32 {
    
#elif defined(EMBOT_ENABLE_hw_motor_bldc_pwm)

#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_pwm)

namespace embot::hw::motor::bldc::pwm::bsp {
    
    constexpr BSP thebsp { };   
     
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        return false;       
    } 
    
    void BSP::set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &pwm) const
    {
    }     

    void BSP::enable(embot::hw::MOTOR m, bool on) const 
    {   
    } 
    
    const TIMconfig & BSP::timconfig() const
    {
        static constexpr TIMconfig tc {};
        return tc;
    }
    
    const BSP& getBSP() 
    {
        return thebsp;
    }    

} // namespace embot::hw::motor::bldc::pwm::bsp {


namespace embot::hw::motor::bldc::pwm::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p) {};          
    void HAL_ADC_MspDeInit(void* p) {};        
    
    void HAL_TIM_Base_MspInit(void* p) {};     
    void HAL_TIM_Base_MspDeInit(void* p) {};       
        
} // namespace embot::hw::motor::bldc::pwm::bsp::stm32 {

#else

namespace embot::hw::motor::bldc::pwm::bsp::impl {
    
    void init();
    void deinit();
    void set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &pwm);
    void enable(embot::hw::MOTOR m, bool on);    
    
} // namespace embot::hw::motor::bldc::pwm::bsp::impl {

namespace embot::hw::motor::bldc::pwm::bsp {
        
    constexpr uint32_t supportedmask
    {
        embot::core::binary::mask::pos2mask<uint32_t>(embot::hw::MOTOR::one)
    };

    constexpr BSP thebsp { supportedmask };   
  
    
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::pwm::bsp::impl::init();
        return true;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::pwm::bsp::impl::deinit();
        return true;       
    } 
    
    void BSP::set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &pwm) const
    {
        embot::hw::motor::bldc::pwm::bsp::impl::set(m, pwm);
    }     

    void BSP::enable(embot::hw::MOTOR m, bool on) const
    {
        embot::hw::motor::bldc::pwm::bsp::impl::enable(m, on);
    }  
    
    const TIMconfig & BSP::timconfig() const
    {
        constexpr pwmFREQ pwmfreq {pwmFREQ::kHz050dot0};
        //constexpr pwmFREQ pwmfreq {pwmFREQ::kHz066dot6};
        //constexpr pwmFREQ pwmfreq {pwmFREQ::kHz097dot6};        
        //constexpr pwmFREQ pwmfreq {pwmFREQ::kHz100dot0};
        
        switch(pwmfreq)
        {
            case pwmFREQ::kHz050dot0: { return timCFG050000Hz; } break;
            case pwmFREQ::kHz066dot6: { return timCFG066666Hz; } break;
            case pwmFREQ::kHz097dot6: { return timCFGgzini; } break;
            case pwmFREQ::kHz100dot0: { return timCFG100000Hz; } break;
            default: { return timCFG066666Hz; } break;           
        }
        
        return timCFG066666Hz;
    }    
    
    const BSP& getBSP() 
    {
        return thebsp;
    }    
     
    
} // namespace embot::hw::motor::bldc::pwm::bsp { 



namespace embot::hw::motor::bldc::pwm::bsp::impl {
    
#if 0

    we use ADC1 to measure the voltage of the three motor phases and ADC2 to measure their currents
    and we use TIM8 for the PWM
    
    however, in here we manage only the TIM8 and we manage the ADC elsewhere
    
#endif 
    
    
    #define REMOVE_ADC
    #if defined(REMOVE_ADC)
    void HAL_ADC_MspInit(void* p) {};          
    void HAL_ADC_MspDeInit(void* p) {};  
    #else
    #endif

        
    void Error_Handler() { for(;;); }    
    


    // timer TIM8
    
    TIM_HandleTypeDef htim8 {};
    
    #define EXT_FAULT_Pin GPIO_PIN_3
    #define EXT_FAULT_GPIO_Port GPIOG
    #define MOT_BREAK_Pin GPIO_PIN_2
    #define MOT_BREAK_GPIO_Port GPIOG
        
    #define MOT_PWM2H_Pin GPIO_PIN_7
    #define MOT_PWM2H_GPIO_Port GPIOC
    #define MOT_PWM4_Pin GPIO_PIN_9
    #define MOT_PWM4_GPIO_Port GPIOC
    #define MOT_PWM3H_Pin GPIO_PIN_8
    #define MOT_PWM3H_GPIO_Port GPIOC
    #define MOT_PWM1H_Pin GPIO_PIN_6
    #define MOT_PWM1H_GPIO_Port GPIOC
    #define MOT_PWM1L_Pin GPIO_PIN_5
    #define MOT_PWM1L_GPIO_Port GPIOA
    #define MOT_PWM3L_Pin GPIO_PIN_15
    #define MOT_PWM3L_GPIO_Port GPIOB
    #define MOT_PWM2L_Pin GPIO_PIN_14
    #define MOT_PWM2L_GPIO_Port GPIOB

    void HAL_TIM_Base_MspInit(void *t)
    {
        TIM_HandleTypeDef* tim_baseHandle = reinterpret_cast<TIM_HandleTypeDef*>(t);

      GPIO_InitTypeDef GPIO_InitStruct = {0};
      if(tim_baseHandle->Instance==TIM8)
      {
      /* USER CODE BEGIN TIM8_MspInit 0 */

      /* USER CODE END TIM8_MspInit 0 */
        /* TIM8 clock enable */
        __HAL_RCC_TIM8_CLK_ENABLE();

        __HAL_RCC_GPIOG_CLK_ENABLE();
        /**TIM8 GPIO Configuration
        PG3     ------> TIM8_BKIN2
        PG2     ------> TIM8_BKIN
        */
        GPIO_InitStruct.Pin = EXT_FAULT_Pin|MOT_BREAK_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        /* TIM8 interrupt Init */
        HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
        HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
//        HAL_NVIC_SetPriority(TIM8_CC_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
      /* USER CODE BEGIN TIM8_MspInit 1 */

      /* USER CODE END TIM8_MspInit 1 */
      }
    }
    
    void HAL_TIM_MspPostInit(void *t)
    {
        TIM_HandleTypeDef* timHandle = reinterpret_cast<TIM_HandleTypeDef*>(t);

      GPIO_InitTypeDef GPIO_InitStruct = {0};
      if(timHandle->Instance==TIM8)
      {
      /* USER CODE BEGIN TIM8_MspPostInit 0 */

      /* USER CODE END TIM8_MspPostInit 0 */

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**TIM8 GPIO Configuration
        PC7     ------> TIM8_CH2
        PC9     ------> TIM8_CH4
        PC8     ------> TIM8_CH3
        PC6     ------> TIM8_CH1
        PA5     ------> TIM8_CH1N
        PB15     ------> TIM8_CH3N
        PB14     ------> TIM8_CH2N
        */
        GPIO_InitStruct.Pin = MOT_PWM2H_Pin|MOT_PWM4_Pin|MOT_PWM3H_Pin|MOT_PWM1H_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOT_PWM1L_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(MOT_PWM1L_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOT_PWM3L_Pin|MOT_PWM2L_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      /* USER CODE BEGIN TIM8_MspPostInit 1 */

      /* USER CODE END TIM8_MspPostInit 1 */
      }

    }

    void HAL_TIM_Base_MspDeInit(void *t)
    {
        TIM_HandleTypeDef* tim_baseHandle = reinterpret_cast<TIM_HandleTypeDef*>(t);

      if(tim_baseHandle->Instance==TIM8)
      {
      /* USER CODE BEGIN TIM8_MspDeInit 0 */

      /* USER CODE END TIM8_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM8_CLK_DISABLE();

        /**TIM8 GPIO Configuration
        PC7     ------> TIM8_CH2
        PC9     ------> TIM8_CH4
        PC8     ------> TIM8_CH3
        PC6     ------> TIM8_CH1
        PG3     ------> TIM8_BKIN2
        PG2     ------> TIM8_BKIN
        PA5     ------> TIM8_CH1N
        PB15     ------> TIM8_CH3N
        PB14     ------> TIM8_CH2N
        */
        HAL_GPIO_DeInit(GPIOC, MOT_PWM2H_Pin|MOT_PWM4_Pin|MOT_PWM3H_Pin|MOT_PWM1H_Pin);

        HAL_GPIO_DeInit(GPIOG, EXT_FAULT_Pin|MOT_BREAK_Pin);

        HAL_GPIO_DeInit(MOT_PWM1L_GPIO_Port, MOT_PWM1L_Pin);

        HAL_GPIO_DeInit(GPIOB, MOT_PWM3L_Pin|MOT_PWM2L_Pin);

        /* TIM8 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
        HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
        HAL_NVIC_DisableIRQ(TIM8_CC_IRQn);
      /* USER CODE BEGIN TIM8_MspDeInit 1 */

      /* USER CODE END TIM8_MspDeInit 1 */
      }
    }    
    
    
} // namespace embot::hw::motor::bldc::pwm::bsp::impl {

namespace embot::hw::motor::bldc::pwm::bsp::impl {

    /* TIM8 init function */
    void MX_TIM8_Init(void)
    {

      /* USER CODE BEGIN TIM8_Init 0 */

      /* USER CODE END TIM8_Init 0 */

      TIM_ClockConfigTypeDef sClockSourceConfig = {0};
      TIM_MasterConfigTypeDef sMasterConfig = {0};
      TIMEx_BreakInputConfigTypeDef sBreakInputConfig = {0};
      TIM_OC_InitTypeDef sConfigOC = {0};
      TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

      /* USER CODE BEGIN TIM8_Init 1 */

      /* USER CODE END TIM8_Init 1 */
      htim8.Instance = TIM8;
      htim8.Init.Prescaler = 0;
      htim8.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
      // htim8.Init.Period = 1024;
      htim8.Init.Period = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().period();
      htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim8.Init.RepetitionCounter = 1;
      htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
      if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
      {
        Error_Handler();
      }
      sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
      if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
      {
        Error_Handler();
      }
      sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
      sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_OC5REF_RISING_OC6REF_RISING;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
      if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sBreakInputConfig.Source = TIM_BREAKINPUTSOURCE_BKIN;
      sBreakInputConfig.Enable = TIM_BREAKINPUTSOURCE_ENABLE;
      sBreakInputConfig.Polarity = TIM_BREAKINPUTSOURCE_POLARITY_HIGH;
      if (HAL_TIMEx_ConfigBreakInput(&htim8, TIM_BREAKINPUT_BRK, &sBreakInputConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sBreakInputConfig.Polarity = TIM_BREAKINPUTSOURCE_POLARITY_LOW;
      if (HAL_TIMEx_ConfigBreakInput(&htim8, TIM_BREAKINPUT_BRK2, &sBreakInputConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigOC.OCMode = TIM_OCMODE_PWM1;
      sConfigOC.Pulse = 0;
      sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
      sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
      sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
      sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
      sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
      if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
      if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigOC.OCMode = TIM_OCMODE_PWM2;
      //sConfigOC.Pulse = 1000;
      sConfigOC.Pulse = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().hightrigger();
      if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_5) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigOC.OCMode = TIM_OCMODE_PWM1;
      //sConfigOC.Pulse = 24;
      sConfigOC.Pulse = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().lowtrigger();
      if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_6) != HAL_OK)
      {
        Error_Handler();
      }
      sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
      sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
      sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
      sBreakDeadTimeConfig.DeadTime = 10;
      sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
      sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
      sBreakDeadTimeConfig.BreakFilter = 15;
      sBreakDeadTimeConfig.Break2State = TIM_BREAK2_ENABLE;
      sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
      sBreakDeadTimeConfig.Break2Filter = 15;
      sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
      if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
      {
        Error_Handler();
      }
      /* USER CODE BEGIN TIM8_Init 2 */

      /* USER CODE END TIM8_Init 2 */
      HAL_TIM_MspPostInit(&htim8);

    }

} // namespace embot::hw::motor::bldc::pwm::bsp::impl {


namespace embot::hw::motor::bldc::pwm::bsp::impl {
    
 
#define hMot1 (htim8)    
    
    /* Motor numbers */
    #define PWM_MOTOR_NONE              (0x00000000UL)
    #define PWM_MOTOR_1                 (0x00000001UL)
    #define PWM_MOTOR_ALL               (0x00000003UL)    
        
    /* PWM phase mask */
    #define PWM_PHASE1                  (0x00000001U)
    #define PWM_PHASE2                  (0x00000002U)
    #define PWM_PHASE3                  (0x00000004U)
    #define PWM_SYNC                    (0x00000008U)

    #define PWM_FAULT_EMERGENCY_BUTTON  (0x00002000UL)
    #define PWM_FAULT_SYSTEM            (0x00008000UL)
    
    static uint32_t Pwm1Status = 0;
    
    uint32_t maxPWMsat {1000};
    
        
    enum class Saturation { NONE, toMINIMUM };
    uint32_t PwmSetWidth(uint32_t mot, int32_t ph1, int32_t ph2, int32_t ph3, Saturation sat);
    
    
    /*******************************************************************************************************************//**
     * @brief Call back function. Called by the TIM interrupt manager following the BRAEAK input activation
     * @param   *htim   pointer to the TIM peripheral handler
     * @return  void
     */
    static void Pwm1_break_cb(TIM_HandleTypeDef *htim)
    {
        if (0 != (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_SYSTEM_BREAK)||(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK ))))
            Pwm1Status |= PWM_FAULT_SYSTEM; 
    }    

    /*******************************************************************************************************************//**
     * @brief Call back function. Called by the TIM interrupt manager following the BREAK2 input activation
     * @param   *htim   pointer to the TIM peripheral handler
     * @return  void
     */
    static void Pwm1_break2_cb(TIM_HandleTypeDef *htim)
    {
        if (0 != __HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK2)) Pwm1Status |= PWM_FAULT_EMERGENCY_BUTTON; 
    }    

    /*******************************************************************************************************************//**
     * @brief Register TIM8 call-back functions and start TIM8 
     *        out-of-phase, synchronous PWM generators.
     * @param   void
     * @return  HAL_OK
     */
    HAL_StatusTypeDef PwmInit(void)
    {
        
        maxPWMsat = static_cast<uint32_t>(0.99*embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().period());
        
        /* Stop Motor 1 PWM (if not already stopped) */
        HAL_TIMEx_PWMN_Stop_IT(&hMot1, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop_IT(&hMot1, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop_IT(&hMot1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_4);
        HAL_TIM_PWM_Stop(&hMot1, TIM_CHANNEL_6);

        /* Stop counters */
        HAL_TIM_Base_Stop(&hMot1);
        
        /* 0% duty cycle */
        PwmSetWidth(PWM_MOTOR_1, 0, 0, 0, Saturation::NONE);
        
        /* CAUTION
         * =======
         * The following statement stops the PWM generators and forces their outputs LOW (lower transistor ON) whenever the
         * debugger triggers any breakpoint on CM4. Remove the statement if you prefer to leave the PWM outputs always
         * active during debugging. Both solutions have drawbacks that must be carefully evaluated by the programmer
         */
        DBGMCU->APB2FZ2 |= 0x00000003;
        
        /* Configure motor 1 PWM */
        /* Start PWM counter from 512 in up direction. First sample when PWM pulse is LOW */
        // not 512 but ... embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().period()/2
        uint32_t startpwm = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().period()/2;
        __HAL_TIM_SetCounter(&hMot1, startpwm);
        hMot1.Instance->CR1 = ((hMot1.Instance->CR1 & ~TIM_CR1_CMS) | TIM_CR1_CMS_0) & ~TIM_CR1_DIR;
        
        /* Clear the status registers */
        Pwm1Status = 0;
        
        /* Register the callback functions */
        HAL_TIM_RegisterCallback(&hMot1, HAL_TIM_BREAK_CB_ID,  Pwm1_break_cb);
        HAL_TIM_RegisterCallback(&hMot1, HAL_TIM_BREAK2_CB_ID, Pwm1_break2_cb);
        
        // HAL_TIM_PWM_PULSE_FINISHED_CB_ID
        //HAL_TIM_RegisterCallback(&hMot1, HAL_TIM_PWM_PULSE_FINISHED_CB_ID,  Pwm1_pulse_finished_cb);
        
        /* Start Motor 1 (TIM8 slave timer) */
        HAL_TIMEx_PWMN_Start_IT(&hMot1, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Start_IT(&hMot1, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Start_IT(&hMot1, TIM_CHANNEL_3);
        
        HAL_TIM_PWM_Start_IT(&hMot1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start_IT(&hMot1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Start_IT(&hMot1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start_IT(&hMot1, TIM_CHANNEL_4);
        
        
        HAL_TIM_PWM_Start(&hMot1, TIM_CHANNEL_5);
        HAL_TIM_PWM_Start(&hMot1, TIM_CHANNEL_6);
            
        /* Start counters */
        HAL_TIM_Base_Start(&hMot1);
        
        
//        HAL_NVIC_SetPriority(TIM8_CC_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);

#warning DEBUG: WHY TIM_IT_UPDATE in PWM ??? maybe just to see its timings but not for else
        __HAL_TIM_ENABLE_IT(&hMot1, TIM_IT_UPDATE);
        
//         __HAL_TIM_ENABLE_IT(&hMot1, TIM_IT_CC4);
        
        return HAL_OK;
    }


    /*******************************************************************************************************************//**
     * @brief Performs the reverse function of PwmInit().
     * @param   void
     * @return  void
     */
    void PwmDeInit(void)
    {
        /* Disable main outputs */
        __HAL_TIM_MOE_DISABLE(&hMot1);
        
        /* 0% duty cycle */
        PwmSetWidth(PWM_MOTOR_1, 0, 0, 0, Saturation::NONE);

        /* Stop Motor 1 PWM */
        HAL_TIMEx_PWMN_Stop_IT(&hMot1, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop_IT(&hMot1, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop_IT(&hMot1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop(&hMot1, TIM_CHANNEL_6);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&hMot1, TIM_CHANNEL_4);

        
        /* Remove the registered functions */
        HAL_TIM_UnRegisterCallback(&hMot1, HAL_TIM_BREAK_CB_ID);
        HAL_TIM_UnRegisterCallback(&hMot1, HAL_TIM_BREAK2_CB_ID);

    }    
    
    
    /*******************************************************************************************************************//**
     * @brief Set the PWM width value for motor 1
     * @param   ph1, ph2, ph3   Relative pulse widths in TIM8 clock periods. The function sets to zero the pulse width
     *                          of the phase with the algebrically minumum argument value. The other two phases are set in
     *                          order to mantain the phase differences, or to saturate to the maximum allowable PWM value.
     * @return  uint32_t    Zero if all PWM values were within the allowed range, otherwise any combination of the following
     *                      constants
     *                      PWM_PHASE1  Saturation detected on PHASE 1 driver
     *                      PWM_PHASE2  Saturation detected on PHASE 2 driver
     *                      PWM_PHASE3  Saturation detected on PHASE 3 driver
     */
    uint32_t PwmSetWidth(uint32_t mot, int32_t ph1, int32_t ph2, int32_t ph3, Saturation satmode)
    {
        if(Saturation::toMINIMUM == satmode)
        {
            int32_t min;
            uint32_t sat = 0;
            /* Select the minumum value */
            min = (ph1 <= ph2)? ((ph1 <= ph3)? ph1 : ph3) : ((ph2 <= ph3)? ph2 : ph3);
            /* Shift to minimum and saturate to maximum */
            ph1 -= min; if (ph1 > maxPWMsat) {ph1 = maxPWMsat; sat |= PWM_PHASE1;}
            ph2 -= min; if (ph2 > maxPWMsat) {ph2 = maxPWMsat; sat |= PWM_PHASE2;}
            ph3 -= min; if (ph3 > maxPWMsat) {ph3 = maxPWMsat; sat |= PWM_PHASE3;}
        }
        /* Set motor #1 */
        if (PWM_MOTOR_1 & mot)
        {
            /* Must be atomic */
    //        taskDISABLE_INTERRUPTS();
            __HAL_TIM_SetCompare(&hMot1, TIM_CHANNEL_1, ph1);
            __HAL_TIM_SetCompare(&hMot1, TIM_CHANNEL_2, ph2);
            __HAL_TIM_SetCompare(&hMot1, TIM_CHANNEL_3, ph3);
            __HAL_TIM_SetCompare(&hMot1, TIM_CHANNEL_4, (int32_t)100);  // SYNC FOR SPEEDGOAT
    //        taskENABLE_INTERRUPTS();
        }
        /* Saturation status */
        return 0;
    }



    void init()
    {
        MX_TIM8_Init();        
        PwmInit();        
    }
    
    void deinit()
    {
        PwmDeInit();
    }
    
    
    void set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &pwm)
    {
        // must transform pwm.u, .v, .w from [0.0, 100.0] into [0, maxval], where original gzini code is maxval = 1024
        
        int32_t u = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().ccr(pwm.u); 
        int32_t v = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().ccr(pwm.v);
        int32_t w = embot::hw::motor::bldc::pwm::bsp::getBSP().timconfig().ccr(pwm.w);       
        PwmSetWidth(PWM_MOTOR_1, u, v, w, Saturation::toMINIMUM);        
    }
    
    
    void enable(embot::hw::MOTOR m, bool on)
    {
        #warning ... dont we init???
    }        
    
} // namespace embot::hw::motor::bldc::pwm::bsp::impl {

// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------



namespace embot::hw::motor::bldc::pwm::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p)           { embot::hw::motor::bldc::pwm::bsp::impl::HAL_ADC_MspInit(p); }
    void HAL_ADC_MspDeInit(void* p)         { embot::hw::motor::bldc::pwm::bsp::impl::HAL_ADC_MspDeInit(p); }
    
    void HAL_TIM_Base_MspInit(void* p)      { embot::hw::motor::bldc::pwm::bsp::impl::HAL_TIM_Base_MspInit(p); }
    void HAL_TIM_Base_MspDeInit(void* p)    { embot::hw::motor::bldc::pwm::bsp::impl::HAL_TIM_Base_MspDeInit(p); }            
        
} // namespace embot::hw::motor::bldc::pwm::bsp::stm32 {


extern "C"
{
    void TIM8_BRK_TIM12_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::pwm::bsp::impl::htim8);
    }

    void TIM8_UP_TIM13_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::pwm::bsp::impl::htim8);
    }

    void TIM8_CC_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::pwm::bsp::impl::htim8);
    }
}

#endif // #if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_pwm)


#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_pwm)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

