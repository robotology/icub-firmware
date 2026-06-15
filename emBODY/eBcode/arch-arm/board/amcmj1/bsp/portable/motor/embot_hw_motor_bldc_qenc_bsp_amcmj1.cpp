
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_qenc_bsp.h"

    int64_t overflow_count {0};

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


uint32_t CNT {0};

#if !defined(EMBOT_ENABLE_hw_motor_bldc_qenc)

namespace embot::hw::motor::bldc::qenc::bsp {

    constexpr BSP thebsp { };   

    const BSP& getBSP() 
    {
        return thebsp;
    } 
    
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        return false;       
    }  

    bool BSP::start(embot::hw::MOTOR m, const Mode &mo) const 
    {
        return false;       
    } 

    bool BSP::stop(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    uint8_t BSP::ticksperpulse(embot::hw::MOTOR m) const
    {
        return 1;
    }
    
    uint32_t BSP::pulsesperrevolution(embot::hw::MOTOR m) const
    {
        return 1;
    }
    
    int64_t BSP::ticks(embot::hw::MOTOR m) const
    {
        return 0;
    }

    int64_t BSP::pulses(embot::hw::MOTOR m) const
    {
        return 0;
    }
    
    int64_t BSP::indexcrossings(embot::hw::MOTOR m) const
    {
        return 0;
    }

    float BSP::angle(embot::hw::MOTOR m, AngleQE a) const
    {
        return 0;
    }
    
} // namespace embot::hw::motor::bldc::hall::bsp {

namespace embot::hw::motor::bldc::qenc::bsp::stm32 {

    void HAL_TIM_IC_MspInit(void *p) {}  
    void HAL_TIM_IC_MspDeInit(void *p) {} 
        
    void HAL_TIM_Encoder_MspInit(void *p)   {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}

} // namespace embot::hw::motor::bldc::qenc::bsp::stm32 {

#elif defined(EMBOT_ENABLE_hw_motor_bldc_qenc)


#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_qenc)

namespace embot::hw::motor::bldc::qenc::bsp {
 

    constexpr BSP thebsp { };   

    const BSP& getBSP() 
    {
        return thebsp;
    } 
    
    bool BSP::init(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        return false;       
    }  

    bool BSP::start(embot::hw::MOTOR m, const Mode &mo) const 
    {
        return false;       
    } 

    bool BSP::stop(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    uint8_t BSP::ticksperpulse(embot::hw::MOTOR m) const
    {
        return 1;
    }

    uint32_t BSP::pulsesperrevolution(embot::hw::MOTOR m) const
    {
        return 1;
    }
    
    int64_t BSP::ticks(embot::hw::MOTOR m) const
    {
        return 0;
    }

    int64_t BSP::pulses(embot::hw::MOTOR m) const
    {
        return 0;
    }
    
    int64_t BSP::indexcrossings(embot::hw::MOTOR m) const
    {
        return 0;
    }

    float BSP::angle(embot::hw::MOTOR m, AngleQE a) const
    {
        return 0;
    }

}

namespace embot::hw::motor::bldc::qenc::bsp::stm32 {

    void HAL_TIM_IC_MspInit(void *p) {}  
    void HAL_TIM_IC_MspDeInit(void *p) {} 
        
    void HAL_TIM_Encoder_MspInit(void *p)   {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}        

} // namespace embot::hw::motor::bldc::qenc::bsp::stm32 {

#else


namespace embot::hw::motor::bldc::qenc::bsp::impl {
    
    void init(embot::hw::MOTOR m);
    void deinit(embot::hw::MOTOR m);
    bool start(embot::hw::MOTOR m, const Mode &mo);
    void stop(embot::hw::MOTOR m);
    uint8_t ticksperpulse(embot::hw::MOTOR m);
    uint32_t pulsesperrevolution(embot::hw::MOTOR m);
    int64_t ticks(embot::hw::MOTOR m, AngleQE a);
    int64_t pulses(embot::hw::MOTOR m);
    int64_t indexcrossings(embot::hw::MOTOR m);
    float angle(embot::hw::MOTOR m, AngleQE a);
    
} // namespace embot::hw::motor::bldc::qenc::bsp::impl {


namespace embot::hw::motor::bldc::qenc::bsp {

    constexpr uint32_t supportedmask
    {
        embot::core::binary::mask::pos2mask<uint32_t>(embot::hw::MOTOR::one)
    };
    
    constexpr PROP penc1 {};
        
    constexpr BSP thebsp {
        supportedmask,
        {{
            &penc1
        }}
    };
    
    const BSP& getBSP() 
    {
        return thebsp;
    }     

    bool BSP::init(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::qenc::bsp::impl::init(m);
        return true;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::qenc::bsp::impl::deinit(m);
        return true;       
    } 

    bool BSP::start(embot::hw::MOTOR m, const Mode &mo) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::start(m, mo);
    }   

    bool BSP::stop(embot::hw::MOTOR m) const
    {
        embot::hw::motor::bldc::qenc::bsp::impl::stop(m);
        return true;
    }  

    uint8_t BSP::ticksperpulse(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::ticksperpulse(m);
    }  

    uint32_t BSP::pulsesperrevolution(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::pulsesperrevolution(m);
    }
    
    int64_t BSP::ticks(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::ticks(m, AngleQE::current);      
    }

    int64_t BSP::pulses(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::pulses(m);
    }
    
    int64_t BSP::indexcrossings(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::indexcrossings(m);   
    }
 
    float BSP::angle(embot::hw::MOTOR m, AngleQE a) const
    {
        return embot::hw::motor::bldc::qenc::bsp::impl::angle(m, a);;
    }
    
} // namespace embot::hw::motor::bldc::qenc::bsp {


namespace embot::hw::motor::bldc::qenc::bsp::impl {

    // we use timer TIM1 that is a 16-bit timer
    // so, we manage overflow and undeflow
    
    #define handleUPDATEinIRQhandler
    
    constexpr size_t maxPPM {32*1024};

    TIM_HandleTypeDef htim1 {};
       
    constexpr size_t timARRshift {16};  // useful to speed up multiplications
    constexpr size_t timARR {1 << timARRshift};
    static_assert((64*1024) == timARR, "");
    constexpr size_t timZERO {0};
    
    constexpr uint8_t QEncMode {TIM_ENCODERMODE_TI12};
    
    // both edges are needed to determine rotation direction at the index crossing  
    static constexpr auto ENC_INDEX_LEADING_EDGE = TIM_CHANNEL_3;
    static constexpr auto ENC_INDEX_TRAILING_EDGE = TIM_CHANNEL_4;    

    #define MOT_ENCA_Pin GPIO_PIN_9
    #define MOT_ENCA_GPIO_Port GPIOE
    #define MOT_ENCB_Pin GPIO_PIN_11
    #define MOT_ENCB_GPIO_Port GPIOE       
    #define MOT_ENCZ_Pin GPIO_PIN_13
    #define MOT_ENCZ_GPIO_Port GPIOE
        
    
    void Error_Handler() { for(;;); }
    
    void MX_TIM1_Init(void)
    {
      TIM_Encoder_InitTypeDef sConfig = {0};
      TIM_MasterConfigTypeDef sMasterConfig = {0};
      TIM_IC_InitTypeDef sConfigIC = {0};

      htim1.Instance = TIM1;
      htim1.Init.Prescaler = 0;
      htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim1.Init.Period = timARR-1;
      htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim1.Init.RepetitionCounter = 0;
      htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
      if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
      {
        Error_Handler();
      }
      sConfig.EncoderMode = QEncMode;
      sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
      sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
      sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
      sConfig.IC1Filter = 4;
      sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
      sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
      sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
      sConfig.IC2Filter = 4;
      if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
      sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
      if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
      sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
      sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
      sConfigIC.ICFilter = 4;
      if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, ENC_INDEX_LEADING_EDGE) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
      sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
      sConfigIC.ICFilter = 0;
      if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, ENC_INDEX_TRAILING_EDGE) != HAL_OK)
      {
        Error_Handler();
      }

    }   

    
    void MX_TIM1_DeInit(void)
    {
        if (HAL_TIM_Encoder_DeInit(&htim1) != HAL_OK)
        {
            Error_Handler();
        }
        
        if(HAL_TIM_IC_DeInit(&htim1) != HAL_OK)
        {
            Error_Handler();            
        }
    }

    // we use HAL_TIM_IC_MspInit() / HAL_TIM_IC_MspDeInit() 
    // and not HAL_TIM_Encoder_MspInit() / HAL_TIM_Encoder_MspDeInit()

    void HAL_TIM_IC_MspInit(void *p)
    {
        TIM_HandleTypeDef* tim_icHandle = reinterpret_cast<TIM_HandleTypeDef*>(p);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        if(tim_icHandle->Instance==TIM1)
        {
            __HAL_RCC_TIM1_CLK_ENABLE();
            __HAL_RCC_GPIOE_CLK_ENABLE();
            
            /**TIM1 GPIO Configuration
            PE13     ------> TIM1_CH3
            PE9     ------> TIM1_CH1
            PE11     ------> TIM1_CH2
            */
            GPIO_InitStruct.Pin = MOT_ENCZ_Pin | MOT_ENCA_Pin | MOT_ENCB_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
            HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

            // CC IRQ drives the index capture callback
            HAL_NVIC_SetPriority(TIM1_CC_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
            
            // update manages overflow/underflow. priority is maximum in order to avoid delays
            HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
      }
    }


    void HAL_TIM_IC_MspDeInit(void *p)
    {
        TIM_HandleTypeDef* tim_icHandle = reinterpret_cast<TIM_HandleTypeDef*>(p);

        if(tim_icHandle->Instance==TIM1)
        {
            __HAL_RCC_TIM1_CLK_DISABLE();

            /**TIM1 GPIO Configuration
            PE13     ------> TIM1_CH3
            PE9     ------> TIM1_CH1
            PE11     ------> TIM1_CH2
            */
            HAL_GPIO_DeInit(GPIOE, MOT_ENCZ_Pin | MOT_ENCA_Pin | MOT_ENCB_Pin);          
            HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
            HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
      }
    }  


    void HAL_TIM_Encoder_MspInit(void *p) {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}         

}

#undef DEBUG_AB_capture_and_log

namespace embot::hw::motor::bldc::qenc::bsp::impl {
            
    struct encData
    {
        int32_t numberofrevolutions {0};
        int64_t cumulativeticksatindex {0};     // cumulative ticks updated at index since start()
        uint32_t pulsesperrevolution {1024};    // encoder pulses per revolution of datasheet 
        uint8_t ticksperpulse {4};              // x2 or x4 decoding 
        uint32_t ticksperrevolution {1};         
        float ticks2degreesfactor {1.0f};       // scaling factor to avoid runtime divisions

        volatile int64_t overflowcount {0};
        
        embot::core::Callback onindexcrossing {};
        
        void clear()
        {
            numberofrevolutions = 0;
            cumulativeticksatindex = 0;
            pulsesperrevolution = 1024;            
            ticksperpulse = 4;
            ticksperrevolution = 0;
            ticks2degreesfactor = 1.0f;
            overflowcount = 0;
        }
        
        void load(uint32_t ppr, const embot::core::Callback &onic, uint8_t div)
        {
            pulsesperrevolution = ppr;
            onindexcrossing = onic;
            ticksperpulse = div;
            ticksperrevolution = pulsesperrevolution*ticksperpulse;
            ticks2degreesfactor = 360.0f / static_cast<float>(ticksperrevolution);
        }        
          
        encData() = default;
    };
    
    encData _encdata {};
        
     

    int64_t cumulativeticks(void);

    static void EncCapture_cb(TIM_HandleTypeDef *htim)
    {
#if defined(DEBUG_AB_capture_and_log)        
        uint32_t dir = (TIM1->CR1 & TIM_CR1_DIR) ? 1 : 0;
        CNT = TIM1->CNT;
        
        uint32_t ccr1 = TIM1->CCR1;
        uint32_t ccr2 = TIM1->CCR2;
        switch(htim->Channel)
        {
            
            case HAL_TIM_ACTIVE_CHANNEL_1:
                embot::core::print("A CNT=" + std::to_string(CNT) + " dir " + std::to_string(dir) + " ccr1 " + std::to_string(ccr1) + " ccr2 " + std::to_string(ccr2));
                break;

            case HAL_TIM_ACTIVE_CHANNEL_2:
                embot::core::print("B CNT=" + std::to_string(CNT) + " dir " + std::to_string(dir) + " ccr1 " + std::to_string(ccr1) + " ccr2 " + std::to_string(ccr2));
                break;

            case HAL_TIM_ACTIVE_CHANNEL_4:
                embot::core::print("I CNT=" + std::to_string(CNT) + " dir " + std::to_string(dir) + " ccr1 " + std::to_string(ccr1) + " ccr2 " + std::to_string(ccr2));
                break;
            
            default:
                embot::core::print("other CNT=" + std::to_string(CNT) + " dir " + std::to_string(dir) + " ccr1 " + std::to_string(ccr1) + " ccr2 " + std::to_string(ccr2));
            break;
        } 
#endif // #if defined(DEBUG_AB_capture_and_log)

        // there must be a leading edge before trailing edge
        if(0 != __HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC3))
        {            
            // ok, we have both edges: leading and then trailing
            uint32_t te_u = __HAL_TIM_GetCompare(&htim1, ENC_INDEX_TRAILING_EDGE);
            uint32_t le_u = __HAL_TIM_GetCompare(&htim1, ENC_INDEX_LEADING_EDGE);

            // direction is retrieved from hardware, so that it is immune to wrap between le and te captures
            int dir = (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1) != 0) ? -1 : +1;
                                   

            int32_t te = static_cast<int32_t>(te_u);
            int32_t le = static_cast<int32_t>(le_u);
            int32_t delta = te - le;

            // sanity check: if delta sign disagrees with dir, the counter wrapped
            // between the two captures, so we must discard this crossing
            bool wrapped = (dir > 0) ? (delta < 0) : (delta > 0);

            // important: avoid inversion of direction over the index and wraps between the two captures
            if (!wrapped && (0 != delta))
            {
               
                // update the cumulative number of ticks. 
                // i prefer using cumulativeticks() that is less precise rather than 
                // _encdata.overflowcount * timARR + te_u
                // uint32_t te_u = __HAL_TIM_GetCompare(&htim1, ENC_INDEX_TRAILING_EDGE);
                // because ... cumulativeticks() has consistent values for cnt and overflowcount 
                // and if i use te_e that is correctly captured at index it ay be that in the meantime an update has happened 
                // 
                _encdata.cumulativeticksatindex = cumulativeticks();
                _encdata.numberofrevolutions += dir;

                // call any configured callback
                _encdata.onindexcrossing.execute();

            }

            // always clear the leading-edge flag to avoid false positives
            __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC3);
        }
    }  
    


    // commento: ok ma ... se il valore balla attorno a 0xfff ed il mio isr non e' veloce
    // abbastanza posso avere problemi di mis-classification. quindi:
    // 1. ridurre la probabilita' di un late irq handler per snellire il piu' possibile con:
    //    a. mettere onupdate nel IRQ handler e non passare attraverso callback
    //    b. alzare la priorita' del irq al max con HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
    // 2. usare un approccio a polling che ha suggerito chatgpt. si polla con un altro timer a 1khz e si tiene traccia
        

    void onupdate()
    {
        uint16_t cnt = TIM1->CNT;
        // no need to verify if we really are in TIM_FLAG_UPDATE as it will be executed only inside the correct irq handler
        __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
        
        if(cnt < 0x8000)
        {   // overflow
            _encdata.overflowcount++;
        }
        else
        {   // underflow
            _encdata.overflowcount--;
        }
    }

#if !defined(handleUPDATEinIRQhandler)
    
    void MyTimerOverflowCb(TIM_HandleTypeDef *htim)
    {
        onupdate();    
    }

#endif // #if !defined(handleUPDATEinIRQhandler)

    bool start__former_EncInit(embot::hw::MOTOR m, const Mode &mo)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        
        if(mo.resolution > maxPPM)
        {
            return false;
        }
        
        _encdata.clear();
        _encdata.load(mo.resolution, mo.onindex, ticksperpulse(m)); 

        /* Register the callback function used to signal the activation of the Index pulse */
        if (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
        {
            __HAL_TIM_SET_COUNTER(&htim1, timZERO);
            
#if !defined(handleUPDATEinIRQhandler)            
            HAL_TIM_RegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID, MyTimerOverflowCb);
#endif // #if !defined(handleUPDATEinIRQhandler)
            
            /* Start timers in encoder mode */
            if (HAL_OK == HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL))
            {
#if defined(DEBUG_AB_capture_and_log)                
                __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
                __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC2);
#endif
                
                /* Enable leading edge capture, without interrupts */
                HAL_TIM_IC_Start(&htim1, ENC_INDEX_LEADING_EDGE);
                /* Enable trailing edge capture, with interrupts */
                // it is this one that triggers the EncCapture_cb callback
                HAL_TIM_IC_Start_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
                
                __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);  
                
                return true;
            }
            /* Failed start of the timer */
            HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        }
        /* Errors detected */
        return false;
    }    
  
    
    void stop__former_Enc1DeInit(void)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);

#if !defined(handleUPDATEinIRQhandler)        
        HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID);
#endif     
        __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);  
        
        _encdata.clear();
    }

    
    void init(embot::hw::MOTOR m)
    {
        // may call stop
        stop(m);
        // init the timer plus its linked gpios inside the called _MspInit()
        MX_TIM1_Init();     
    }
    
    void deinit(embot::hw::MOTOR m)
    {
        // surely stop. then deinit timer and pins
        stop(m); 
        MX_TIM1_DeInit();        
    }
 
    
    bool start(embot::hw::MOTOR m, const Mode &mo)
    {       
        return start__former_EncInit(m, mo);        
    }
 
    
    void stop(embot::hw::MOTOR m)
    { 
        stop__former_Enc1DeInit();              
    } 

    
    uint8_t ticksperpulse(embot::hw::MOTOR m)
    {
        return (TIM_ENCODERMODE_TI12 == QEncMode) ? 4 : 2;        
    }
 
    uint32_t pulsesperrevolution(embot::hw::MOTOR m)
    {
        return _encdata.pulsesperrevolution;
    }      
    
    int64_t cumulativeticks(void)
    {
        int64_t ovf {0};
        uint16_t cnt {0};
        
        __disable_irq(); // yes: i keep global irq disable in here 
        ovf = _encdata.overflowcount;
        cnt = TIM1->CNT;
        __enable_irq();
        
        return ovf * timARR + cnt;
    }    
    
    int64_t ticks(embot::hw::MOTOR m, AngleQE a)
    {
        int64_t v {0}; 
        
        if(AngleQE::current == a)
        {
            v  = cumulativeticks();
        }
        else
        {
            HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
            v = _encdata.cumulativeticksatindex;
            HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        }
        
        return v;
    }

    int64_t pulses(embot::hw::MOTOR m) 
    {
        constexpr size_t div { (TIM_ENCODERMODE_TI12 == QEncMode) ? 4 : 2 };
        return ticks(m, AngleQE::current) / div; 
    }    
    
    int64_t indexcrossings(embot::hw::MOTOR m)
    { 
        return _encdata.numberofrevolutions;              
    }  
    
    float angle(embot::hw::MOTOR m, AngleQE a)
    { 
        return static_cast<float>(ticks(m, a)) * _encdata.ticks2degreesfactor; 
    } 
        
    
} // namespace embot::hw::motor::bldc::qenc::bsp::impl {

// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------


namespace embot::hw::motor::bldc::qenc::bsp::stm32 {
        
    void HAL_TIM_IC_MspInit(void* p)        { embot::hw::motor::bldc::qenc::bsp::impl::HAL_TIM_IC_MspInit(p); }
    void HAL_TIM_IC_MspDeInit(void* p)      { embot::hw::motor::bldc::qenc::bsp::impl::HAL_TIM_IC_MspDeInit(p); }
    
    void HAL_TIM_Encoder_MspInit(void *p)   { embot::hw::motor::bldc::qenc::bsp::impl::HAL_TIM_Encoder_MspInit(p); }
    void HAL_TIM_Encoder_MspDeInit(void *p) { embot::hw::motor::bldc::qenc::bsp::impl::HAL_TIM_Encoder_MspDeInit(p); }
            
} // namespace embot::hw::motor::bldc::hall::bsp::stm32 {

extern "C"
{

    void TIM1_CC_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::qenc::bsp::impl::htim1);
    }
    
    void TIM1_UP_IRQHandler(void)
    {
#if defined(handleUPDATEinIRQhandler)          
        embot::hw::motor::bldc::qenc::bsp::impl::onupdate();
#else        
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::qenc::bsp::impl::htim1);
#endif            
    }

}

#endif // #if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_qenc)


#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_qenc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

