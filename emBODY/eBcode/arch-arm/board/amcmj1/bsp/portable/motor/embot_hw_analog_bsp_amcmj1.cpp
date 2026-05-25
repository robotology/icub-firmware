
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it, kevin.sanngalli@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_analog_bsp.h"

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



#if defined(EMBOT_ENABLE_hw_analog)


#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_analog)

namespace embot::hw::analog::bsp {
    
    constexpr BSP thebsp { };   

    bool BSP::supported(const Value v, embot::hw::MOTOR m) const 
    {
        return false;
    }    
    
    bool BSP::init(const Configuration &config) const 
    {
        return false;       
    } 

    bool BSP::deinit() const 
    {
        return false;       
    } 
    
    float BSP::get(const Value v, embot::hw::MOTOR m) const
    {
        float r {0.0};        
        return r;
    }     
    
    const BSP& getBSP() 
    {
        return thebsp;
    }

} // namespace embot::hw::analog::bsp 


namespace embot::hw::analog::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p) {};          
    void HAL_ADC_MspDeInit(void* p) {};        
    
    void HAL_TIM_Base_MspInit(void* p) {};     
    void HAL_TIM_Base_MspDeInit(void* p) {};    
    
} // namespace embot::hw::analog::bsp::stm32 {

#elif !defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_analog)
    
    
#define EMBOT_HW_ANALOG_LOG_values    

#if defined(EMBOT_HW_ANALOG_LOG_values)
    uint32_t TEMPdecidegrees = 0;
    uint32_t VINmillivolt = 0;
    uint32_t CINmilliampere = 0;
#endif

#define EMBOT_HW_ANALOG_USE_internaltrigger

namespace embot::hw::analog::bsp::impl {
     
    void init();
    void deinit();
    
    struct Measurements
    {
        float cin {0.0};        // Input current (A) 
        float vin {0.0};        // Input voltage (V)           
        float vaux {0.0};       // Auxiliary +5V voltage (V)    
        float vcc {0.0};        // Main 3.3V voltage (V)        
        float vcore {0.0};      // Core 1.2V voltage (V)        
        float temp {0.0};       // Core temperature (C)         
        float tdrv[2] {0.0};    // Motor driver temperature (C) 
        float tmot[2] {0.0};    // Motor temperature (C)       
        uint32_t faults {0};    // Emergency status flags  
        void clear()
        {
            cin = vin = vaux = vcc = vcore = temp = tdrv[0] = tdrv[1] = tmot[0] = tmot[1] = faults = 0;
        }            
    };  

    const Measurements & getmeasurements();
    void clearmeasurements();     
    
}

namespace embot::hw::analog::bsp {
        
    constexpr uint32_t supportedmask
    {
        embot::core::binary::mask::pos2mask<uint32_t>(Value::Vin) | embot::core::binary::mask::pos2mask<uint32_t>(Value::Cin) | 
        embot::core::binary::mask::pos2mask<uint32_t>(Value::Tcore) | embot::core::binary::mask::pos2mask<uint32_t>(Value::Vaux) |
        embot::core::binary::mask::pos2mask<uint32_t>(Value::Vcc) | embot::core::binary::mask::pos2mask<uint32_t>(Value::Vcore) | 
        embot::core::binary::mask::pos2mask<uint32_t>(Value::Tmotordriver) | embot::core::binary::mask::pos2mask<uint32_t>(Value::Tmotor)
    };

    constexpr BSP thebsp { supportedmask };   

    bool BSP::supported(const Value v, embot::hw::MOTOR m) const
    {
        if(embot::hw::MOTOR::two == m)
        {
            return false;
        }
        
        return embot::core::binary::bit::check(supportedmask, embot::core::tointegral(v)); 
    }    
    
    bool BSP::init(const Configuration &config) const 
    {
        embot::hw::analog::bsp::impl::init();
        return true;       
    } 

    bool BSP::deinit() const 
    {
        embot::hw::analog::bsp::impl::deinit();
        return true;       
    } 
    
    float BSP::get(const Value v, embot::hw::MOTOR m) const
    {
        float r {0.0};        
        
        const embot::hw::analog::bsp::impl::Measurements & mea = embot::hw::analog::bsp::impl::getmeasurements();
        
        switch(v)
        {
            case Value::Cin:            { r = mea.cin; } break;
            case Value::Vin:            { r = mea.vin; } break;
            case Value::Tcore:          { r = mea.temp; } break;
            case Value::Vaux:           { r = mea.vaux; } break;
            case Value::Vcc:            { r = mea.vcc; } break;
            case Value::Vcore:          { r = mea.vcore; } break;
            case Value::Tmotordriver:   { r = (embot::hw::MOTOR::one == m) ? mea.tdrv[0] : 0.0; } break;
            case Value::Tmotor:         { r = (embot::hw::MOTOR::one == m) ? mea.tmot[0] : 0.0; } break;           
            
            default:                    { r = 0.0; } break;
        }
        
        return r;
    }     
    
    const BSP& getBSP() 
    {
        return thebsp;
    }    
     
    
} // namespace embot::hw::analog::bsp {    


namespace embot::hw::analog::bsp::impl {
    
    // as in cm4 gzini code. 
    // only dma2 is used and for analog we use only DMA2_Stream2 for ADC3, rank 1 to 10
    // but which tim is used to trigger the ADC3?
    // TIM1 is encoder
    // TIM3 is hall
    // TIM8 is pwm of motor 
    // ADC3 is triggered by TIM8, specifically by the TIM8 TRGO signal on a rising edge.
    //    hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO;
    //    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    // so, i should configure the pwm and the adc as well
    // BUT i can trigger the ADC3 by a timer of my own .... the TIM6
    // i do it configurable w/ EMBOT_HW_ANALOG_USE_internaltrigger    
    
    ADC_HandleTypeDef hadc3 {};
    DMA_HandleTypeDef hdma_adc3 {};    

#if defined(EMBOT_HW_ANALOG_USE_internaltrigger)        
    TIM_HandleTypeDef htim6 {};        
#endif
        
    #define PWR_VCC_Pin GPIO_PIN_3
    #define PWR_VCC_GPIO_Port GPIOF
    #define PWR_VAUX_Pin GPIO_PIN_5
    #define PWR_VAUX_GPIO_Port GPIOF
    #define PWR_TEMP_DRV_Pin GPIO_PIN_6
    #define PWR_TEMP_DRV_GPIO_Port GPIOF
    #define PWR_VIN_Pin GPIO_PIN_7
    #define PWR_VIN_GPIO_Port GPIOF
    #define PWR_TEMP_MOT_Pin GPIO_PIN_8
    #define PWR_TEMP_MOT_GPIO_Port GPIOF        
    #define PWR_CIN_Pin GPIO_PIN_9
    #define PWR_CIN_GPIO_Port GPIOF
    #define PWR_VCORE_Pin GPIO_PIN_10
    #define PWR_VCORE_GPIO_Port GPIOF        
    
    void Error_Handler() { for(;;); }        
    
    void HAL_TIM_Base_MspInit(void* t)
    {
#if defined(EMBOT_HW_ANALOG_USE_internaltrigger) 
        
        TIM_HandleTypeDef* tim_baseHandle = reinterpret_cast<TIM_HandleTypeDef*>(t);
        
        if(tim_baseHandle->Instance == TIM6)
        {
            /* TIM6 clock enable */
            __HAL_RCC_TIM6_CLK_ENABLE();

            //          /* Optional: enable interrupt (only if you use it elsewhere) */
            //          HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 5, 0);
            //          HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
        }
        
#endif        
    }
    
    void HAL_TIM_Base_MspDeInit(void *t)
    {
#if defined(EMBOT_HW_ANALOG_USE_internaltrigger) 
        
        TIM_HandleTypeDef* tim_baseHandle = reinterpret_cast<TIM_HandleTypeDef*>(t);
        
        if(tim_baseHandle->Instance == TIM6)
        {
            /* Peripheral clock disable */
            __HAL_RCC_TIM6_CLK_DISABLE();

        //        /* Optional: disable interrupt */
        //        HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
        }
        
#endif        
    }

    void HAL_ADC_MspInit(void* p)
    {
        ADC_HandleTypeDef *adcHandle = reinterpret_cast<ADC_HandleTypeDef*>(p);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
//        if(adcHandle->Instance==ADC1)
//        {
//        /* USER CODE BEGIN ADC1_MspInit 0 */

//        /* USER CODE END ADC1_MspInit 0 */
//        /* ADC1 clock enable */
//        HAL_RCC_ADC12_CLK_ENABLED++;
//        if(HAL_RCC_ADC12_CLK_ENABLED==1){
//          __HAL_RCC_ADC12_CLK_ENABLE();
//        }

//        __HAL_RCC_GPIOF_CLK_ENABLE();
//        __HAL_RCC_GPIOA_CLK_ENABLE();
//        /**ADC1 GPIO Configuration
//        PF12     ------> ADC1_INP6
//        PF11     ------> ADC1_INP2
//        PA6     ------> ADC1_INP3
//        */
//        GPIO_InitStruct.Pin = MOT_VPHASE3_Pin|MOT_VPHASE1_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

//        GPIO_InitStruct.Pin = MOT_VPHASE2_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        HAL_GPIO_Init(MOT_VPHASE2_GPIO_Port, &GPIO_InitStruct);

//        /* ADC1 DMA Init */
//        /* ADC1 Init */
//        hdma_adc1.Instance = DMA2_Stream7;
//        hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
//        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
//        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
//        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
//        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//        hdma_adc1.Init.Mode = DMA_CIRCULAR;
//        hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
//        hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
//        {
//          Error_Handler();
//        }

//        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

//        /* ADC1 interrupt Init */
//        HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(ADC_IRQn);
//        /* USER CODE BEGIN ADC1_MspInit 1 */

//        /* USER CODE END ADC1_MspInit 1 */
//        }
//        else if(adcHandle->Instance==ADC2)
//        {
//        /* USER CODE BEGIN ADC2_MspInit 0 */

//        /* USER CODE END ADC2_MspInit 0 */
//        /* ADC2 clock enable */
//        HAL_RCC_ADC12_CLK_ENABLED++;
//        if(HAL_RCC_ADC12_CLK_ENABLED==1){
//          __HAL_RCC_ADC12_CLK_ENABLE();
//        }

//        __HAL_RCC_GPIOF_CLK_ENABLE();
//        __HAL_RCC_GPIOA_CLK_ENABLE();
//        /**ADC2 GPIO Configuration
//        PF13     ------> ADC2_INP2
//        PA3     ------> ADC2_INP15
//        PF14     ------> ADC2_INP6
//        */
//        GPIO_InitStruct.Pin = MOT_CPHASE1_Pin|MOT_CPHASE2_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

//        GPIO_InitStruct.Pin = MOT_CPHASE3_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        HAL_GPIO_Init(MOT_CPHASE3_GPIO_Port, &GPIO_InitStruct);

//        /* ADC2 DMA Init */
//        /* ADC2 Init */
//        hdma_adc2.Instance = DMA2_Stream0;
//        hdma_adc2.Init.Request = DMA_REQUEST_ADC2;
//        hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
//        hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
//        hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
//        hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//        hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//        hdma_adc2.Init.Mode = DMA_CIRCULAR;
//        hdma_adc2.Init.Priority = DMA_PRIORITY_VERY_HIGH;
//        hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//        if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
//        {
//          Error_Handler();
//        }

//        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc2);

//        /* ADC2 interrupt Init */
//        HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(ADC_IRQn);
//        /* USER CODE BEGIN ADC2_MspInit 1 */

//        /* USER CODE END ADC2_MspInit 1 */
//        }
//        else if(adcHandle->Instance==ADC3)
        if(ADC3 == adcHandle->Instance)
        {
            /* USER CODE BEGIN ADC3_MspInit 0 */

            /* USER CODE END ADC3_MspInit 0 */
            /* ADC3 clock enable */
            __HAL_RCC_ADC3_CLK_ENABLE();

            __HAL_RCC_GPIOF_CLK_ENABLE();
            /**ADC3 GPIO Configuration
            PF3     ------> ADC3_INP5
            PF5     ------> ADC3_INP4
            PF6     ------> ADC3_INP8
            PF7     ------> ADC3_INP3
            PF8     ------> ADC3_INP7
            PF9     ------> ADC3_INP2
            PF10     ------> ADC3_INP6
            */
            GPIO_InitStruct.Pin = PWR_VCC_Pin|PWR_VAUX_Pin|PWR_TEMP_DRV_Pin|PWR_VIN_Pin
                                  |PWR_TEMP_MOT_Pin|PWR_CIN_Pin|PWR_VCORE_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

            /* ADC3 DMA Init */
            /* ADC3 Init */
            hdma_adc3.Instance = DMA2_Stream2;
            hdma_adc3.Init.Request = DMA_REQUEST_ADC3;
            hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
            hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
            hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
            hdma_adc3.Init.Mode = DMA_CIRCULAR;
            hdma_adc3.Init.Priority = DMA_PRIORITY_HIGH;
            hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&hdma_adc3) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc3);

            /* ADC3 interrupt Init */
            HAL_NVIC_SetPriority(ADC3_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(ADC3_IRQn);
            /* USER CODE BEGIN ADC3_MspInit 1 */

            /* USER CODE END ADC3_MspInit 1 */
        }
    }

    void HAL_ADC_MspDeInit(void* p)
    {        
        ADC_HandleTypeDef *adcHandle = reinterpret_cast<ADC_HandleTypeDef*>(p);

//        if(adcHandle->Instance==ADC1)
//        {
//        /* USER CODE BEGIN ADC1_MspDeInit 0 */

//        /* USER CODE END ADC1_MspDeInit 0 */
//        /* Peripheral clock disable */
//        HAL_RCC_ADC12_CLK_ENABLED--;
//        if(HAL_RCC_ADC12_CLK_ENABLED==0){
//          __HAL_RCC_ADC12_CLK_DISABLE();
//        }

//        /**ADC1 GPIO Configuration
//        PF12     ------> ADC1_INP6
//        PF11     ------> ADC1_INP2
//        PA6     ------> ADC1_INP3
//        */
//        HAL_GPIO_DeInit(GPIOF, MOT_VPHASE3_Pin|MOT_VPHASE1_Pin);

//        HAL_GPIO_DeInit(MOT_VPHASE2_GPIO_Port, MOT_VPHASE2_Pin);

//        /* ADC1 DMA DeInit */
//        HAL_DMA_DeInit(adcHandle->DMA_Handle);

//        /* ADC1 interrupt Deinit */
//        /* USER CODE BEGIN ADC1:ADC_IRQn disable */
//        /**
//        * Uncomment the line below to disable the "ADC_IRQn" interrupt
//        * Be aware, disabling shared interrupt may affect other IPs
//        */
//        /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
//        /* USER CODE END ADC1:ADC_IRQn disable */

//        /* USER CODE BEGIN ADC1_MspDeInit 1 */

//        /* USER CODE END ADC1_MspDeInit 1 */
//        }
//        else if(adcHandle->Instance==ADC2)
//        {
//        /* USER CODE BEGIN ADC2_MspDeInit 0 */

//        /* USER CODE END ADC2_MspDeInit 0 */
//        /* Peripheral clock disable */
//        HAL_RCC_ADC12_CLK_ENABLED--;
//        if(HAL_RCC_ADC12_CLK_ENABLED==0){
//          __HAL_RCC_ADC12_CLK_DISABLE();
//        }

//        /**ADC2 GPIO Configuration
//        PF13     ------> ADC2_INP2
//        PA3     ------> ADC2_INP15
//        PF14     ------> ADC2_INP6
//        */
//        HAL_GPIO_DeInit(GPIOF, MOT_CPHASE1_Pin|MOT_CPHASE2_Pin);

//        HAL_GPIO_DeInit(MOT_CPHASE3_GPIO_Port, MOT_CPHASE3_Pin);

//        /* ADC2 DMA DeInit */
//        HAL_DMA_DeInit(adcHandle->DMA_Handle);

//        /* ADC2 interrupt Deinit */
//        /* USER CODE BEGIN ADC2:ADC_IRQn disable */
//        /**
//        * Uncomment the line below to disable the "ADC_IRQn" interrupt
//        * Be aware, disabling shared interrupt may affect other IPs
//        */
//        /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
//        /* USER CODE END ADC2:ADC_IRQn disable */

//        /* USER CODE BEGIN ADC2_MspDeInit 1 */

//        /* USER CODE END ADC2_MspDeInit 1 */
//        }
//        else if(adcHandle->Instance==ADC3)
        if(ADC3 == adcHandle->Instance)
        {
            /* USER CODE BEGIN ADC3_MspDeInit 0 */

            /* USER CODE END ADC3_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_ADC3_CLK_DISABLE();

            /**ADC3 GPIO Configuration
            PF3     ------> ADC3_INP5
            PF5     ------> ADC3_INP4
            PF6     ------> ADC3_INP8
            PF7     ------> ADC3_INP3
            PF8     ------> ADC3_INP7
            PF9     ------> ADC3_INP2
            PF10     ------> ADC3_INP6
            */
            HAL_GPIO_DeInit(GPIOF, PWR_VCC_Pin|PWR_VAUX_Pin|PWR_TEMP_DRV_Pin|PWR_VIN_Pin
                                  |PWR_TEMP_MOT_Pin|PWR_CIN_Pin|PWR_VCORE_Pin);

            /* ADC3 DMA DeInit */
            HAL_DMA_DeInit(adcHandle->DMA_Handle);

            /* ADC3 interrupt Deinit */
            HAL_NVIC_DisableIRQ(ADC3_IRQn);
            /* USER CODE BEGIN ADC3_MspDeInit 1 */

            /* USER CODE END ADC3_MspDeInit 1 */
        }
    }        
}


namespace embot::hw::analog::bsp::impl {
    
    void MX_DMA_Init(void);
    void MX_ADC3_Init(void);
    
#if defined(EMBOT_HW_ANALOG_USE_internaltrigger)    
    void MX_TIM6_Init();
#endif
       
    
    void MX_DMA_Init(void)
    {

        /* DMA controller clock enable */
        __HAL_RCC_DMA2_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* DMA interrupt init */
        /* DMA2_Stream0_IRQn interrupt configuration */
//        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        /* DMA2_Stream2_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
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

    /* ADC3 init function */
    void MX_ADC3_Init(void)
    {

        /* USER CODE BEGIN ADC3_Init 0 */

        /* USER CODE END ADC3_Init 0 */

        ADC_ChannelConfTypeDef sConfig = {0};

        /* USER CODE BEGIN ADC3_Init 1 */

        /* USER CODE END ADC3_Init 1 */

        /** Common config
        */
        hadc3.Instance = ADC3;
        hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        hadc3.Init.Resolution = ADC_RESOLUTION_12B;
        hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
        hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
        hadc3.Init.LowPowerAutoWait = DISABLE;
        hadc3.Init.ContinuousConvMode = DISABLE;
        hadc3.Init.NbrOfConversion = 10;
        hadc3.Init.DiscontinuousConvMode = ENABLE;
        hadc3.Init.NbrOfDiscConversion = 1;
#if defined(EMBOT_HW_ANALOG_USE_internaltrigger) 
        hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;
#else        
        hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO;
#endif                
        hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
        hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
        hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
        hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        hadc3.Init.OversamplingMode = DISABLE;
        hadc3.Init.Oversampling.Ratio = 1;
        if (HAL_ADC_Init(&hadc3) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_2;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_16CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_1;
        sConfig.Offset = 2048;
        sConfig.OffsetRightShift = DISABLE;
        sConfig.OffsetSignedSaturation = DISABLE;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = ADC_REGULAR_RANK_2;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_4;
        sConfig.Rank = ADC_REGULAR_RANK_3;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_5;
        sConfig.Rank = ADC_REGULAR_RANK_4;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_6;
        sConfig.Rank = ADC_REGULAR_RANK_5;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_7;
        sConfig.Rank = ADC_REGULAR_RANK_6;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_8;
        sConfig.Rank = ADC_REGULAR_RANK_7;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_VBAT;
        sConfig.Rank = ADC_REGULAR_RANK_8;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
        sConfig.Rank = ADC_REGULAR_RANK_9;
        sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_VREFINT;
        sConfig.Rank = ADC_REGULAR_RANK_10;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
        Error_Handler();
        }
        /* USER CODE BEGIN ADC3_Init 2 */

        /* USER CODE END ADC3_Init 2 */

    }    
    
    
    

    /* ADC related constants */
    #define ADC_VREF                (2.5/*V*/)
    #define ADC_NUMBER_OF_BITS      (12)
    #define ADC_RESOLUTION          (1U<<ADC_NUMBER_OF_BITS)
    #define ADC_LSB                 (ADC_VREF/(double)ADC_RESOLUTION)
    #define ADC1_NUMBER_OF_CHANNELS (3u)
    #define ADC3_NUMBER_OF_CHANNELS (10u)
    #define ADC3_TIMEOUT            (20u)

    /* Currents, voltages and temperature limits */
    #define AIN_MAX_INPUT_CURRENT   (20.0)
    #define AIN_MIN_INPUT_CURRENT   (-20.0)
    #define AIN_MAX_INPUT_VOLTAGE   (58.0)
    #define AIN_MIN_INPUT_VOLTAGE   (9.5)
    #define AIN_MAX_VCORE           (1.21*1.05)
    #define AIN_MIN_VCORE           (1.21*0.95)
    #define AIN_MAX_VCC             (3.33*1.05)
    #define AIN_MIN_VCC             (3.33*0.95)
    #define AIN_MAX_VAUX            (5.00*1.05)
    #define AIN_MIN_VAUX            (5.00*0.95)
    #define AIN_MAX_CORE_TEMP       (125.0)
    #define AIN_MAX_PTC_DRV_TEMP    (90.0)
    #define AIN_MAX_PTC_MOT_TEMP    (90.0)


    /* Utilities */
    #ifndef lengthof
    #define lengthof(array) (sizeof(array)/sizeof((array)[0]))
    #endif



    /* AMC-MJ1 schematics related constants */
    #define R160                (33000.0)
    #define R157                (22000.0)
    #define R158                (22000.0)
    #define R161                (33000.0)
    #define R181                (560000.0)
    #define R182                (24000.0)
    #define R104                (0.001)
    #define U2_GAIN             (50)
    #define VAUX_ATTEN          ((R160+R157)/R157)
    #define VCC_ATTEN           ((R158+R161)/R161)
    #define VCORE_ATTEN         (1.0)
    #define VIN_ATTEN           ((R181+R182)/R182)
    #define CIN_ATTEN           (1.0/(R104*U2_GAIN))
    #define PTC_GAIN            (132.0/2.5)
    #define PTC_OFFS            (-12.0)            

    #define U3_GAIN             (50.0)
    #define R105                (0.001)




        

    /* All possible generic faults */
    #define AIN_FAULTS  (PWM_FAULT_MAX_INPUT_CURRENT\
                        |PWM_FAULT_MAX_INPUT_VOLTAGE\
                        |PWM_FAULT_MIN_INPUT_VOLTAGE\
                        |PWM_FAULT_MAX_VCORE\
                        |PWM_FAULT_MIN_VCORE\
                        |PWM_FAULT_MAX_VCC\
                        |PWM_FAULT_MIN_VCC\
                        |PWM_FAULT_MAX_VAUX\
                        |PWM_FAULT_MIN_VAUX\
                        |PWM_FAULT_MAX_CORE_TEMP\
                        |PWM_FAULT_MAX_PTC_DRV_TEMP)


    /* Private typedefs ***************************************************************************************************/

    typedef enum
    {
        CIN,
        VIN,
        VAUX,
        VCC,
        VCORE,
        PTC_MOT,
        PTC_DRV,
        VBAT,
        TEMP_CORE,
        VREFINT
    } Ain3Channels_t;
        
     
    /* DMA circular buffers for ADC3: Double buffer */
    volatile uint16_t AinDma3Buffer[2 * ADC3_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));    
    volatile int16_t AinAdc3Buffer[ADC3_NUMBER_OF_CHANNELS];  /* Miscellaneous data */    


    void fillvalues();
    
    void resetlogvalues();
        
        
    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 1st half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc3HT_cb(ADC_HandleTypeDef *hadc)
    {
        /* Copy the 1st half of the buffer filled by DMA */
        memcpy((void *)AinAdc3Buffer, (void *)&(AinDma3Buffer[0]), sizeof(AinAdc3Buffer));
        fillvalues();
    }

    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 2nd half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc3TC_cb(ADC_HandleTypeDef *hadc)
    {
        /* Copy the 2nd half of the buffer filled by DMA */
        memcpy((void *)AinAdc3Buffer, (void *)&(AinDma3Buffer[ADC3_NUMBER_OF_CHANNELS]), sizeof(AinAdc3Buffer));
        fillvalues();
    }    
        

    /* Core temperature and VREFINT calibration */
    struct Calibration
    {
        float TempGain {1.0};
        float TempOffs {0.0};
        float VrefintCal {1.216}; // volts
    };

    Calibration _calib {};

    //float AinTempGain;
    //float AinTempOffs;
    //float AinVrefintCal = 1.216 /*V*/;


    HAL_StatusTypeDef AinInit(void)
    {
        
        resetlogvalues();
        
        {

            {
                /* Stop all ADCs */
    //            HAL_ADC_Stop_DMA(&hadc2);
                HAL_ADC_Stop_DMA(&hadc3);
        
                /* Calibrate all ADCs. Caution: blocking functions! */
    //            HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
                HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

                /* Register all the callback functions */
    //            HAL_ADC_RegisterCallback(&hadc2, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AinAdc2HT_cb);
    //            HAL_ADC_RegisterCallback(&hadc2, HAL_ADC_CONVERSION_HALF_CB_ID,     AinAdc2TC_cb);
                HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AinAdc3HT_cb);
                HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID,     AinAdc3TC_cb);

                /* Start conversions */
    //            HAL_ADC_Start_DMA(&hadc2, (uint32_t *)AinDma2Buffer, lengthof(AinDma2Buffer));
                HAL_ADC_Start_DMA(&hadc3, (uint32_t *)AinDma3Buffer, lengthof(AinDma3Buffer));
                
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
    //    HAL_ADC_Stop_DMA(&hadc2);
        HAL_ADC_Stop_DMA(&hadc3);
        
        /* Remove all callback functions */
    //    HAL_ADC_UnRegisterCallback(&hadc2, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    //    HAL_ADC_UnRegisterCallback(&hadc2, HAL_ADC_CONVERSION_HALF_CB_ID);

        HAL_ADC_UnRegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID);
    }

    void init()
    {
        // at first the dma and the adc3. but only once
        static bool startupinit_done {false};
        
        if(false == startupinit_done)
        {
            startupinit_done = true;
            
            embot::hw::analog::bsp::impl::MX_DMA_Init();
            embot::hw::analog::bsp::impl::MX_ADC3_Init(); 
    #if defined(EMBOT_HW_ANALOG_USE_internaltrigger)         
            embot::hw::analog::bsp::impl::MX_TIM6_Init(); 
    #endif   
            
            const embot::hw::adc::bsp::FactoryCalibration &fc = embot::hw::adc::bsp::getfactorycalibration();
        
    //        /* Calculate the VREFINT value given by the manufacturer */
    //        AinVrefintCal = ((double)fc.vrefint_cal)*(double)fc.vrefint_cal_vref/65536000.0;
    //        AinTempGain = (float)(fc.ts_cal2_temp - fc.ts_cal1_temp)/(float)(fc.ts_cal2 - fc.ts_cal1);
    //        AinTempOffs = (float)fc.ts_cal1_temp - AinTempGain*(float)fc.ts_cal1;
    //        /* Compensate the resolution and VREF */
    //        AinTempGain *= 65536000.0/(float)fc.ts_cal_vref;    

            /* Calculate the VREFINT value given by the manufacturer */
            _calib.VrefintCal = ((double)fc.vrefint_cal)*(double)fc.vrefint_cal_vref/65536000.0;
            _calib.TempGain = (float)(fc.ts_cal2_temp - fc.ts_cal1_temp)/(float)(fc.ts_cal2 - fc.ts_cal1);
            _calib.TempOffs = (float)fc.ts_cal1_temp - _calib.TempGain*(float)fc.ts_cal1;
            /* Compensate the resolution and VREF */
            _calib.TempGain *= 65536000.0/(float)fc.ts_cal_vref; 
        }        
           
        clearmeasurements();
        AinInit();    
    }


    void deinit()
    {
        AinDeInit();
        clearmeasurements();
    }


    ///* LSB voltage of all ADC */
    //float AinLsb = ADC_LSB;

    ///* LSB current of ADC1 */
    //float AinAdc2Lsb = ADC_LSB/(U3_GAIN * R105);


    Measurements mea {};
        
    const Measurements & getmeasurements()
    {
        return mea;
    }
    
    void clearmeasurements()
    {
        mea.clear();
    }

     
    void logvalues()
    {
    #if defined(EMBOT_HW_ANALOG_LOG_values)    
        TEMPdecidegrees = 10.0*embot::hw::analog::bsp::impl::mea.temp;
        VINmillivolt = 1000.0*embot::hw::analog::bsp::impl::mea.vin;   
        CINmilliampere = 1000.0*embot::hw::analog::bsp::impl::mea.cin;
    #endif    
    }
    
    void resetlogvalues()
    {
    #if defined(EMBOT_HW_ANALOG_LOG_values)    
        TEMPdecidegrees = 0;
        VINmillivolt = 0;   
        CINmilliampere = 0;
    #endif          
    }

    void fillvalues()
    {
        /* LSB voltage of all ADC */
        float AinLsb = ADC_LSB;  
        
        AinLsb = (0 == AinAdc3Buffer[VREFINT])? ADC_LSB : _calib.VrefintCal/(float)AinAdc3Buffer[VREFINT];
    //    AinAdc2Lsb = AinLsb / (U3_GAIN * R105);
        mea.cin = CIN_ATTEN * (float)AinAdc3Buffer[CIN] * AinLsb;
        mea.vin = VIN_ATTEN * (float)AinAdc3Buffer[VIN] * AinLsb;
        mea.vaux = VAUX_ATTEN * (float)AinAdc3Buffer[VAUX] * AinLsb;
        mea.vcc = VCC_ATTEN * (float)AinAdc3Buffer[VCC] * AinLsb;
        mea.vcore = VCORE_ATTEN * (float)AinAdc3Buffer[VCORE] * AinLsb;
        mea.temp = _calib.TempGain * (float)AinAdc3Buffer[TEMP_CORE] * AinLsb + _calib.TempOffs;
        mea.tdrv[0] = PTC_GAIN * (float)AinAdc3Buffer[PTC_DRV] * AinLsb + PTC_OFFS;
        mea.tmot[0] = PTC_GAIN * (float)AinAdc3Buffer[PTC_MOT] * AinLsb + PTC_OFFS;    
        
        logvalues();

    }

    
#if defined(EMBOT_HW_ANALOG_USE_internaltrigger) 

    // configured to emit the TIM_TRGO_UPDATE every 1 ms, so all measures every 10 ms
    void MX_TIM6_Init(void)
    {
      TIM_MasterConfigTypeDef sMasterConfig = {0};

      htim6.Instance = TIM6;
      htim6.Init.Prescaler = 200 - 1;      // 200 MHz / 200 = 1 MHz
      htim6.Init.Period = 1000 - 1;        // 1 MHz / 1000 = 1 kHz (1 ms)
      htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

      if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
      {
        Error_Handler();
      }

      /* TIM6 TRGO selection: update event */
      sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

      if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
      {
        Error_Handler();
      }

      /* Start timer */
      HAL_TIM_Base_Start(&htim6);
    } 
    
#endif    
    
} // namespace embot::hw::analog::bsp::impl {


namespace embot::hw::adc::bsp {
    
    #warning MUST SOLVE how to get FactoryCalibration values
    // if we are CM7 ok, but if we are CM4 ... maybe store it in some eeprom place or ... retrieve from ram or from flash
    FactoryCalibration fc {};
        
    const FactoryCalibration & getfactorycalibration()
    {
#if defined(STM32HAL_CORE_CM7)
   
    fc.vrefint_cal = *VREFINT_CAL_ADDR;
    fc.vrefint_cal_vref = VREFINT_CAL_VREF;
    fc.ts_cal1 = *TEMPSENSOR_CAL1_ADDR;
    fc.ts_cal2 = *TEMPSENSOR_CAL2_ADDR;
    fc.ts_cal1_temp = TEMPSENSOR_CAL1_TEMP;
    fc.ts_cal2_temp = TEMPSENSOR_CAL2_TEMP;
    fc.ts_cal_vref  = TEMPSENSOR_CAL_VREFANALOG;   

#else

    #warning HEI JOE: where you going with that gun in your hand? I'm going to solve how to get FactoryCalibration from a CM4
    fc.vrefint_cal = 24150;
    fc.vrefint_cal_vref = VREFINT_CAL_VREF;
    fc.ts_cal1 = 12241;
    fc.ts_cal2 = 16310;
    fc.ts_cal1_temp = TEMPSENSOR_CAL1_TEMP;
    fc.ts_cal2_temp = TEMPSENSOR_CAL2_TEMP;
    fc.ts_cal_vref  = TEMPSENSOR_CAL_VREFANALOG;  

#endif         
        return fc;
    }
    
} // namespace embot::hw::adc::bsp {


// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------

namespace embot::hw::analog::bsp::stm32 {

    void HAL_ADC_MspInit(void* p)           { embot::hw::analog::bsp::impl::HAL_ADC_MspInit(p); }
    void HAL_ADC_MspDeInit(void* p)         { embot::hw::analog::bsp::impl::HAL_ADC_MspDeInit(p); }
    
    void HAL_TIM_Base_MspInit(void* p)      { embot::hw::analog::bsp::impl::HAL_TIM_Base_MspInit(p); }
    void HAL_TIM_Base_MspDeInit(void* p)    { embot::hw::analog::bsp::impl::HAL_TIM_Base_MspDeInit(p); }
    
}

extern "C"
{
    void ADC3_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(&embot::hw::analog::bsp::impl::hadc3);
    }    

    void DMA2_Stream2_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::analog::bsp::impl::hdma_adc3);
    }    
}


#endif // #elif !defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_analog)

 






#endif // #if defined(EMBOT_ENABLE_hw_analog)





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

