
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_adc_bsp.h"


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


#if !defined(EMBOT_ENABLE_hw_motor_bldc_adc)

namespace embot::hw::motor::bldc::adc::bsp {
    
    constexpr BSP thebsp { };   
     
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        return false;       
    }  

    bool BSP::set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk) const
    {
        return false;
    }

    const BSP& getBSP() 
    {
        return thebsp;
    }    

} // namespace embot::hw::motor::bldc::adc::bsp {

namespace embot::hw::motor::bldc::adc::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p) {};          
    void HAL_ADC_MspDeInit(void* p) {};     

    void HAL_TIM_Base_MspInit(void* p) {}  
    void HAL_TIM_Base_MspDeInit(void* p) {} 
        
} // namespace embot::hw::motor::bldc::adc::bsp::stm32 {

#elif defined(EMBOT_ENABLE_hw_motor_bldc_adc)



#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_adc)

namespace embot::hw::motor::bldc::adc::bsp {
    
    constexpr BSP thebsp { };   
     
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        return false;       
    }  

    bool BSP::set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk) const
    {
        return false;
    }

    const BSP& getBSP() 
    {
        return thebsp;
    }    

} // namespace embot::hw::motor::bldc::adc::bsp {

namespace embot::hw::motor::bldc::adc::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p) {};          
    void HAL_ADC_MspDeInit(void* p) {};     

    void HAL_TIM_Base_MspInit(void* p) {}  
    void HAL_TIM_Base_MspDeInit(void* p) {} 
        
} // namespace embot::hw::motor::bldc::adc::bsp::stm32 {

#else


#define EMBOT_HW_MOTOR_BLDC_ADC_LOG_values    


#if defined(EMBOT_HW_MOTOR_BLDC_ADC_LOG_values)    
    int32_t curr1low {0};
    int32_t curr2low {0};
    int32_t curr3low {0};
    int32_t curr0high {0};
    int32_t curr1high {0};
    int32_t curr2high {0};
#endif    


namespace embot::hw::motor::bldc::adc::bsp::impl {
    
    void init();
    void deinit();
    
    std::array<embot::hw::motor::bldc::OnCurrents, embot::hw::motor::bldc::MAXnumber> oncurrents {};
    
} // namespace embot::hw::motor::bldc::adc::bsp::impl {


namespace embot::hw::motor::bldc::adc::bsp {
    
    constexpr BSP thebsp { }; 
    
    const BSP& getBSP() 
    {
        return thebsp;
    }   
    
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::adc::bsp::impl::init();
        return true;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::adc::bsp::impl::deinit();
        return true;       
    } 

    bool BSP::set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk) const
    {
        embot::hw::motor::bldc::adc::bsp::impl::oncurrents[embot::core::tointegral(m)] = cbk;
        return true;
    }       

} // namespace embot::hw::motor::bldc::adc::bsp {


#undef EMBOT_HW_MOTOR_BLDC_ADC_USE_internaltrigger

namespace embot::hw::motor::bldc::adc::bsp::impl {
    
#if 0

    we use ADC2 to measure the current of the three motor phases 
    the acquision is triggered by ADC_EXTERNALTRIG_T8_TRGO2
    
    if we want acquisition to be trigegred by something else we must:
    - define EMBOT_HW_MOTOR_BLDC_ADC_USE_internaltrigger
    - choose a TIMx
    - create the MX_TIMx_Init()
    - fill in HAL_TIM_Base_MspInit() and HAL_TIM_Base_MspDeInit()
    
#endif 
    
    void HAL_TIM_Base_MspInit(void* p) {}  
    void HAL_TIM_Base_MspDeInit(void* p) {} 
    
    void Error_Handler() { for(;;); }     

    ADC_HandleTypeDef hadc1 {};         // phase voltage (not used at date of 20 may 2026)
    ADC_HandleTypeDef hadc2 {};         // phase current

    DMA_HandleTypeDef hdma_adc1 {};
    DMA_HandleTypeDef hdma_adc2 {};

    #define MOT_VPHASE3_Pin GPIO_PIN_12
    #define MOT_VPHASE3_GPIO_Port GPIOF
    #define MOT_VPHASE1_Pin GPIO_PIN_11
    #define MOT_VPHASE1_GPIO_Port GPIOF
    #define MOT_VPHASE2_Pin GPIO_PIN_6
    #define MOT_VPHASE2_GPIO_Port GPIOA
    #define MOT_CPHASE1_Pin GPIO_PIN_13
    #define MOT_CPHASE1_GPIO_Port GPIOF
    #define MOT_CPHASE3_Pin GPIO_PIN_3
    #define MOT_CPHASE3_GPIO_Port GPIOA
    #define MOT_CPHASE2_Pin GPIO_PIN_14
    #define MOT_CPHASE2_GPIO_Port GPIOF            
        
    static uint32_t HAL_RCC_ADC12_CLK_ENABLED = 0;
    
    void HAL_ADC_MspInit(void* p)
    {
        ADC_HandleTypeDef *adcHandle = reinterpret_cast<ADC_HandleTypeDef*>(p);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        if(adcHandle->Instance==ADC1)
        {
            
        /* ADC1 clock enable */
        HAL_RCC_ADC12_CLK_ENABLED++;
        if(HAL_RCC_ADC12_CLK_ENABLED==1){
          __HAL_RCC_ADC12_CLK_ENABLE();
        }

        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PF12     ------> ADC1_INP6
        PF11     ------> ADC1_INP2
        PA6     ------> ADC1_INP3
        */
        GPIO_InitStruct.Pin = MOT_VPHASE3_Pin|MOT_VPHASE1_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOT_VPHASE2_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(MOT_VPHASE2_GPIO_Port, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        /* ADC1 Init */
        hdma_adc1.Instance = DMA2_Stream7;
        hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_CIRCULAR;
        hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        {
          Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);

        }
        else if(adcHandle->Instance==ADC2)
        {

        /* ADC2 clock enable */
        HAL_RCC_ADC12_CLK_ENABLED++;
        if(HAL_RCC_ADC12_CLK_ENABLED==1){
          __HAL_RCC_ADC12_CLK_ENABLE();
        }

        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC2 GPIO Configuration
        PF13     ------> ADC2_INP2
        PA3     ------> ADC2_INP15
        PF14     ------> ADC2_INP6
        */
        GPIO_InitStruct.Pin = MOT_CPHASE1_Pin|MOT_CPHASE2_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOT_CPHASE3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(MOT_CPHASE3_GPIO_Port, &GPIO_InitStruct);

        /* ADC2 DMA Init */
        /* ADC2 Init */
        hdma_adc2.Instance = DMA2_Stream0;
        hdma_adc2.Init.Request = DMA_REQUEST_ADC2;
        hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc2.Init.Mode = DMA_CIRCULAR;
        hdma_adc2.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
        {
          Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc2);

        /* ADC2 interrupt Init */
        HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
        /* USER CODE BEGIN ADC2_MspInit 1 */

        /* USER CODE END ADC2_MspInit 1 */
        }

    }

    void HAL_ADC_MspDeInit(void* p)
    {        
        ADC_HandleTypeDef *adcHandle = reinterpret_cast<ADC_HandleTypeDef*>(p);

        if(adcHandle->Instance==ADC1)
        {

        /* Peripheral clock disable */
        HAL_RCC_ADC12_CLK_ENABLED--;
        if(HAL_RCC_ADC12_CLK_ENABLED==0){
          __HAL_RCC_ADC12_CLK_DISABLE();
        }

        /**ADC1 GPIO Configuration
        PF12     ------> ADC1_INP6
        PF11     ------> ADC1_INP2
        PA6     ------> ADC1_INP3
        */
        HAL_GPIO_DeInit(GPIOF, MOT_VPHASE3_Pin|MOT_VPHASE1_Pin);

        HAL_GPIO_DeInit(MOT_VPHASE2_GPIO_Port, MOT_VPHASE2_Pin);

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);

        /* ADC1 interrupt Deinit */
        /* USER CODE BEGIN ADC1:ADC_IRQn disable */
        /**
        * Uncomment the line below to disable the "ADC_IRQn" interrupt
        * Be aware, disabling shared interrupt may affect other IPs
        */
        /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
        /* USER CODE END ADC1:ADC_IRQn disable */

        }
        else if(adcHandle->Instance==ADC2)
        {

        /* Peripheral clock disable */
        HAL_RCC_ADC12_CLK_ENABLED--;
        if(HAL_RCC_ADC12_CLK_ENABLED==0){
          __HAL_RCC_ADC12_CLK_DISABLE();
        }

        /**ADC2 GPIO Configuration
        PF13     ------> ADC2_INP2
        PA3     ------> ADC2_INP15
        PF14     ------> ADC2_INP6
        */
        HAL_GPIO_DeInit(GPIOF, MOT_CPHASE1_Pin|MOT_CPHASE2_Pin);

        HAL_GPIO_DeInit(MOT_CPHASE3_GPIO_Port, MOT_CPHASE3_Pin);

        /* ADC2 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);

        /* ADC2 interrupt Deinit */
        /* USER CODE BEGIN ADC2:ADC_IRQn disable */
        /**
        * Uncomment the line below to disable the "ADC_IRQn" interrupt
        * Be aware, disabling shared interrupt may affect other IPs
        */
        /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
        /* USER CODE END ADC2:ADC_IRQn disable */

        }

    }         
        
} // namespace embot::hw::motor::bldc::adc::bsp::impl {



namespace embot::hw::motor::bldc::adc::bsp::impl {
    
#if defined(EMBOT_HW_MOTOR_BLDC_ADC_USE_internaltrigger)    
    void MX_TIMx_Init() { put code in here }
#endif
    

    // only dma2 is used 
    // for ADC2 (currents) we use DMA2_Stream0
    // for ADC1 (voltages) we use DMA2_Stream7
    
    
    void MX_DMA_Init(void)
    {

        /* DMA controller clock enable */
        __HAL_RCC_DMA2_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* DMA interrupt init */
        /* DMA2_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        
        /* DMA2_Stream2_IRQn interrupt configuration */
//        HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
//        /* DMA2_Stream3_IRQn interrupt configuration */
//        HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
//        /* DMA2_Stream4_IRQn interrupt configuration */
//        HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
//        /* DMA2_Stream7_IRQn interrupt configuration */
//        HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

    }  
    
    void MX_ADC2_Init(void)
    {

      ADC_ChannelConfTypeDef sConfig = {0};

      /** Common config
      */
      hadc2.Instance = ADC2;
      hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
      hadc2.Init.Resolution = ADC_RESOLUTION_12B;
      hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
      hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
      hadc2.Init.LowPowerAutoWait = DISABLE;
      hadc2.Init.ContinuousConvMode = DISABLE;
      hadc2.Init.NbrOfConversion = 6;
      hadc2.Init.DiscontinuousConvMode = ENABLE;
      hadc2.Init.NbrOfDiscConversion = 1;
      hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO2;
      hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
      hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
      hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
      hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
      hadc2.Init.OversamplingMode = DISABLE;
      hadc2.Init.Oversampling.Ratio = 1;
      if (HAL_ADC_Init(&hadc2) != HAL_OK)
      {
        Error_Handler();
      }

      /** Configure Regular Channel
      */
      sConfig.Channel = ADC_CHANNEL_2;
      sConfig.Rank = ADC_REGULAR_RANK_1;
      sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
      sConfig.SingleDiff = ADC_SINGLE_ENDED;
      sConfig.OffsetNumber = ADC_OFFSET_1;
      sConfig.Offset = 2048;
      sConfig.OffsetRightShift = DISABLE;
      sConfig.OffsetSignedSaturation = DISABLE;
      if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }

      /** Configure Regular Channel
      */
      sConfig.Rank = ADC_REGULAR_RANK_2;
      if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }

      /** Configure Regular Channel
      */
      sConfig.Channel = ADC_CHANNEL_6;
      sConfig.Rank = ADC_REGULAR_RANK_3;
      sConfig.OffsetNumber = ADC_OFFSET_2;
      if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }

      /** Configure Regular Channel
      */
      sConfig.Rank = ADC_REGULAR_RANK_4;
      if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }

      /** Configure Regular Channel
      */
      sConfig.Channel = ADC_CHANNEL_15;
      sConfig.Rank = ADC_REGULAR_RANK_5;
      sConfig.OffsetNumber = ADC_OFFSET_3;
      if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }

      /** Configure Regular Channel
      */
      sConfig.Rank = ADC_REGULAR_RANK_6;
      if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }

    }
    
    
    /* Utilities */
    #ifndef lengthof
    #define lengthof(array) (sizeof(array)/sizeof((array)[0]))
    #endif    
    
    
    // actually: ADC2 .... 3 channels
    #define ADC1_NUMBER_OF_CHANNELS (3u)
    #define ADC_VREF                (2.5/*V*/)
    #define ADC_NUMBER_OF_BITS      (12)
    #define ADC_RESOLUTION          (1U<<ADC_NUMBER_OF_BITS)
    #define ADC_LSB                 (ADC_VREF/(double)ADC_RESOLUTION)
    #define U3_GAIN             (50.0)
    #define R105                (0.001)    
    
    
    /* DMA circular buffers for ADC2: Dual samples per phase, double buffer */
    volatile int16_t AinDma2Buffer[4 * ADC1_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));
//    volatile int16_t AinAdc1Buffer[ADC1_NUMBER_OF_CHANNELS];  /* Motor 1 currents */
    volatile int16_t AinAdc1BufferF[ADC1_NUMBER_OF_CHANNELS];  /* Motor 1 currents first sample */
    volatile int16_t AinAdc1BufferS[ADC1_NUMBER_OF_CHANNELS];  /* Motor 1 currents second sample */

    
    /* LSB current of ADC2 */
    constexpr float AinAdc2Lsb = ADC_LSB/(U3_GAIN * R105);
    
    // ADC_LSB = (2.5 / 4096) = 0.0006103515625 
    // (U3_GAIN * R105) = (50 * 0.001) =  0.05
    // AinAdc2Lsb = ADC_LSB / (U3_GAIN * R105) = 0.0006103515625 / 0.05 = 0.01220703125
    // but also = (2.5/4k) * (1/(50 * 0.001)) = (2.5/4k) * (1000/50) = (2.5/4k) * 20 = 50 / 4k = 50 / 4096 = 0.01220703125
    // so, if we have
    // int x = ...   
    // float curr_volt = x * AinMot1Current;
    // we could use integer multiplications only to achieve the killivolt
    // int milli = x * 50000; x >>= 12;
    //     
    // / 0.05
    float AinMot1Current[3] = {0};

    constexpr int16_t currOFFSET {0};
    void logvalues()
    {
    #if defined(EMBOT_HW_MOTOR_BLDC_ADC_LOG_values)    
        
        curr1low = currOFFSET+AinAdc1BufferF[0];
        curr2low = currOFFSET+AinAdc1BufferF[1];
        curr3low = currOFFSET+AinAdc1BufferF[2];
        curr0high = currOFFSET+AinAdc1BufferS[0];
        curr1high = currOFFSET+AinAdc1BufferS[1];;
        curr2high = currOFFSET+AinAdc1BufferS[2];;

    #endif    
    }
    
    void resetlogvalues()
    {
    #if defined(EMBOT_HW_MOTOR_BLDC_ADC_LOG_values)    
        curr1low = 0;
        curr2low = 0;
        curr3low = 0;
        curr0high = 0;
        curr1high = 0;
        curr2high = 0;
    #endif          
    }    
    
    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 1st half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc2HT_cb(ADC_HandleTypeDef *hadc)
    {
        for(int i=0; i<3; i++)
        {
            AinAdc1BufferF[i] = AinDma2Buffer[2*i+0];
            AinAdc1BufferS[i] = AinDma2Buffer[2*i+1];  
            AinMot1Current[i] = AinAdc2Lsb * (float)AinAdc1BufferF[i];            
        } 

        logvalues();  
        
        embot::hw::motor::bldc::Currents cc {AinMot1Current[0], AinMot1Current[1], AinMot1Current[2]};
        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::one};
        embot::hw::motor::bldc::adc::bsp::impl::oncurrents[embot::core::tointegral(m)].execute(&cc);        
        
//        /* Motor 2: first sample is in pulse LOW */
//        AinAdc1Buffer[0] = (__HAL_TIM_GetCompare(&hMot1, TIM_CHANNEL_1) >= 512)? AinDma2Buffer[0] : AinDma2Buffer[1];
//        AinAdc1Buffer[1] = (__HAL_TIM_GetCompare(&hMot1, TIM_CHANNEL_2) >= 512)? AinDma2Buffer[2] : AinDma2Buffer[3];
//        AinAdc1Buffer[2] = (__HAL_TIM_GetCompare(&hMot1, TIM_CHANNEL_3) >= 512)? AinDma2Buffer[4] : AinDma2Buffer[5];
//        /* Convert in float */
//        AinMot1Current[0] = AinAdc2Lsb * (float)AinAdc1Buffer[0];
//        AinMot1Current[1] = AinAdc2Lsb * (float)AinAdc1Buffer[1];
//        AinMot1Current[2] = AinAdc2Lsb * (float)AinAdc1Buffer[2];
//        
//        Pwm1_pulse_finished_cb(&hMot1);
//        pwmMot1Foc(AinMot1Current[0], AinMot1Current[1], AinMot1Current[2]);
    }

    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 2nd half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc2TC_cb(ADC_HandleTypeDef *hadc)
    {
        for(int i=0; i<3; i++)
        {
            AinAdc1BufferF[i] = AinDma2Buffer[6+2*i+0];
            AinAdc1BufferS[i] = AinDma2Buffer[6+2*i+1]; 
            AinMot1Current[i] = AinAdc2Lsb * (float)AinAdc1BufferF[i];            
        }  
        
        logvalues(); 

        embot::hw::motor::bldc::Currents cc {AinMot1Current[0], AinMot1Current[1], AinMot1Current[2]};
        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::one};
        embot::hw::motor::bldc::adc::bsp::impl::oncurrents[embot::core::tointegral(m)].execute(&cc); 
        
//        /* Motor 2: first sample is in pulse LOW */
//        AinAdc1Buffer[0] = (__HAL_TIM_GetCompare(&hMot1, TIM_CHANNEL_1) >= 512)? AinDma2Buffer[6]  : AinDma2Buffer[7];
//        AinAdc1Buffer[1] = (__HAL_TIM_GetCompare(&hMot1, TIM_CHANNEL_2) >= 512)? AinDma2Buffer[8]  : AinDma2Buffer[9];
//        AinAdc1Buffer[2] = (__HAL_TIM_GetCompare(&hMot1, TIM_CHANNEL_3) >= 512)? AinDma2Buffer[10] : AinDma2Buffer[11];
//        /* Convert in float */
//        AinMot1Current[0] = AinAdc2Lsb * (float)AinAdc1Buffer[0];
//        AinMot1Current[1] = AinAdc2Lsb * (float)AinAdc1Buffer[1];
//        AinMot1Current[2] = AinAdc2Lsb * (float)AinAdc1Buffer[2];
//        
//        Pwm1_pulse_finished_cb(&hMot1);
//        pwmMot1Foc(AinMot1Current[0], AinMot1Current[1], AinMot1Current[2]);
    }    
    
    HAL_StatusTypeDef AinInit(void)
    {
        
        resetlogvalues();
        
        {

            {
                /* Stop all ADCs */
                HAL_ADC_Stop_DMA(&hadc2);
        
                /* Calibrate all ADCs. Caution: blocking functions! */
                HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

                /* Register all the callback functions */
                HAL_ADC_RegisterCallback(&hadc2, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AinAdc2HT_cb);
                HAL_ADC_RegisterCallback(&hadc2, HAL_ADC_CONVERSION_HALF_CB_ID,     AinAdc2TC_cb);

                /* Start conversions */
                HAL_ADC_Start_DMA(&hadc2, (uint32_t *)AinDma2Buffer, lengthof(AinDma2Buffer));
                
                /* All done */
                return HAL_OK;
            }

        }

        return HAL_ERROR;
    }
    

    void AinDeInit(void)
    {
        
        resetlogvalues();

        /* Stop all ADCs */
        HAL_ADC_Stop_DMA(&hadc2);
        
        /* Remove all callback functions */
        HAL_ADC_UnRegisterCallback(&hadc2, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&hadc2, HAL_ADC_CONVERSION_HALF_CB_ID);

    }    

} // namespace embot::hw::motor::bldc::adc::bsp::impl {



namespace embot::hw::motor::bldc::adc::bsp::impl {
    
    void init()
    {
        // at first the dma and the adc2. but only once
        static bool startupinit_done {false};
        
        if(false == startupinit_done)
        {
            startupinit_done = true;
            
            embot::hw::motor::bldc::adc::bsp::impl::MX_DMA_Init();
            embot::hw::motor::bldc::adc::bsp::impl::MX_ADC2_Init(); 
    #if defined(EMBOT_HW_MOTOR_BLDC_ADC_USE_internaltrigger)         
            embot::hw::motor::bldc::adc::bsp::impl::MX_TIMx_Init(); 
    #endif   
        }        

        AinInit();         
    }
    
    void deinit()
    {
        AinDeInit();      
    }
       
    
} // namespace embot::hw::motor::bldc::adc::bsp::impl {


// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------


namespace embot::hw::motor::bldc::adc::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p)           { embot::hw::motor::bldc::adc::bsp::impl::HAL_ADC_MspInit(p); }
    void HAL_ADC_MspDeInit(void* p)         { embot::hw::motor::bldc::adc::bsp::impl::HAL_ADC_MspDeInit(p); }
    
    void HAL_TIM_Base_MspInit(void* p)      { embot::hw::motor::bldc::adc::bsp::impl::HAL_TIM_Base_MspInit(p); }
    void HAL_TIM_Base_MspDeInit(void* p)    { embot::hw::motor::bldc::adc::bsp::impl::HAL_TIM_Base_MspDeInit(p); }
        
} // namespace embot::hw::motor::bldc::adc::bsp::stm32 {

extern "C"
{
    void ADC_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(&embot::hw::motor::bldc::adc::bsp::impl::hadc2);
    }    

    void DMA2_Stream0_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::motor::bldc::adc::bsp::impl::hdma_adc2);
    }    
}


#endif  // #if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation)



#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_adc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

