
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_enc_bsp.h"


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


#if !defined(EMBOT_ENABLE_hw_motor_bldc_enc)

namespace embot::hw::motor::bldc::enc::bsp {

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

namespace embot::hw::motor::bldc::enc::bsp::stm32 {

    void HAL_TIM_IC_MspInit(void *p) {}  
    void HAL_TIM_IC_MspDeInit(void *p) {} 
        
    void HAL_TIM_Encoder_MspInit(void *p)   {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}

} // namespace embot::hw::motor::bldc::enc::bsp::stm32 {

#elif defined(EMBOT_ENABLE_hw_motor_bldc_enc)


#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_enc)

namespace embot::hw::motor::bldc::enc::bsp {
 

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

namespace embot::hw::motor::bldc::enc::bsp::stm32 {

    void HAL_TIM_IC_MspInit(void *p) {}  
    void HAL_TIM_IC_MspDeInit(void *p) {} 
        
    void HAL_TIM_Encoder_MspInit(void *p)   {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}        

} // namespace embot::hw::motor::bldc::enc::bsp::stm32 {

#else


namespace embot::hw::motor::bldc::enc::bsp::impl {
    
    void init(embot::hw::MOTOR m);
    void deinit(embot::hw::MOTOR m);
    void start(embot::hw::MOTOR m, const Mode &mo);
    void stop(embot::hw::MOTOR m);
    uint8_t ticksperpulse(embot::hw::MOTOR m);
    uint32_t pulsesperrevolution(embot::hw::MOTOR m);
    int64_t ticks(embot::hw::MOTOR m, AngleQE a);
    int64_t pulses(embot::hw::MOTOR m);
    int64_t indexcrossings(embot::hw::MOTOR m);
    float angle(embot::hw::MOTOR m, AngleQE a);
    
} // namespace embot::hw::motor::bldc::enc::bsp::impl {


namespace embot::hw::motor::bldc::enc::bsp {

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
        embot::hw::motor::bldc::enc::bsp::impl::init(m);
        return true;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::enc::bsp::impl::deinit(m);
        return true;       
    } 

    bool BSP::start(embot::hw::MOTOR m, const Mode &mo) const
    {
        embot::hw::motor::bldc::enc::bsp::impl::start(m, mo);
        return true;
    }   

    bool BSP::stop(embot::hw::MOTOR m) const
    {
        embot::hw::motor::bldc::enc::bsp::impl::stop(m);
        return true;
    }  

    uint8_t BSP::ticksperpulse(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::enc::bsp::impl::ticksperpulse(m);
    }  

    uint32_t BSP::pulsesperrevolution(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::enc::bsp::impl::pulsesperrevolution(m);
    }
    
    int64_t BSP::ticks(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::enc::bsp::impl::ticks(m, AngleQE::current);      
    }

    int64_t BSP::pulses(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::enc::bsp::impl::pulses(m);
    }
    
    int64_t BSP::indexcrossings(embot::hw::MOTOR m) const
    {
        return embot::hw::motor::bldc::enc::bsp::impl::indexcrossings(m);   
    }
 
    float BSP::angle(embot::hw::MOTOR m, AngleQE a) const
    {
        return embot::hw::motor::bldc::enc::bsp::impl::angle(m, a);;
    }
    
} // namespace embot::hw::motor::bldc::enc::bsp {


namespace embot::hw::motor::bldc::enc::bsp::impl {

    // we use timer TIM1 that is a 16-bit timer
    // on amcfoc we had TIM2 and TIM5 that are 32-bit timers

    TIM_HandleTypeDef htim1 {};
        
    constexpr size_t timARR {65535+1};
        
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
      }
    }  


    void HAL_TIM_Encoder_MspInit(void *p) {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}         

}


namespace embot::hw::motor::bldc::enc::bsp::impl {
            
    struct encData
    {
        int32_t numberofrevolutions {0};
        int64_t cumulativeticksatindex {0};     // cumulative ticks updated at index since start()
        uint32_t pulsesperrevolution {1024};    // encoder pulses per revolution of datasheet 
        uint8_t ticksperpulse {4};              // x2 or x4 decoding 
        uint32_t ticksperrevolution {1};         
        float ticks2degreesfactor {1.0f};       // scaling factor to avoid runtime divisions

        embot::core::Callback onindexcrossing {};
        
        void clear()
        {
            numberofrevolutions = 0;
            cumulativeticksatindex = 0;
            pulsesperrevolution = 1024;            
            ticksperpulse = 4;
            ticksperrevolution = 0;
            ticks2degreesfactor = 1.0f;
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
        

#define TEST_CODE_fastmovingrotor
        
        
#if defined(TEST_CODE_fastmovingrotor)
        
    static void EncCapture_cb(TIM_HandleTypeDef *htim)
    {
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
                // ---------------------------------------------------------------
                // CALLBACK LATENCY COMPENSATION
                //
                // The HW capture registers (te, le) are frozen at the exact moment
                // of each edge — they are not affected by how late this CB fires.
                //
                // However, the timer keeps running after the index crossing.
                // If we blindly call SET_COUNTER(0) here, the ticks accumulated
                // between the index (te) and the moment we actually execute are
                // silently discarded. Those ticks would never be counted in any
                // future cumulativeticksatindex update, corrupting the position.
                //
                // Fix: read the counter NOW (before touching it), compute how many
                // ticks have elapsed since te, and restore that value after the
                // reset.  The next period then starts from latency_ticks rather
                // than 0, so no ticks are lost.
                //
                // Modular arithmetic handles counter wrap in both directions:
                //   dir > 0:  latency = (now - te)        mod (ARR+1)
                //   dir < 0:  latency = (te - now)        mod (ARR+1)
                // The (a + M - b) % M form avoids signed underflow in both cases.
                // ---------------------------------------------------------------                
                uint32_t ARR = __HAL_TIM_GET_AUTORELOAD(&htim1);
                uint32_t modulus = ARR + 1;

                // read counter NOW before resetting, to account for callback latency
                uint32_t counter_now = __HAL_TIM_GetCounter(&htim1);
                
                // compute ticks elapsed between index capture (te) and now, handling wrap
                uint32_t latency_ticks {0};
                if (dir > 0)
                {
                    // counter was counting up: counter_now >= te (modulo ARR+1)
                    latency_ticks = (counter_now + modulus - te_u) % modulus;
                }
                else
                {
                    // counter was counting down: counter_now <= te (modulo ARR+1)
                    latency_ticks = (te_u + modulus - counter_now) % modulus;
                }

                // convert te to signed: ticks elapsed since last __HAL_TIM_SET_COUNTER(&htim1, xxx)
                // i.e. since last index crossing or since start()
                int32_t signed_te = (dir > 0) ? (te) : (te - static_cast<int32_t>(ARR));

                // update the cumulative number of ticks
                _encdata.cumulativeticksatindex += static_cast<int64_t>(signed_te);
                _encdata.numberofrevolutions += dir;

                // reset counter but compensate for latency: the next period starts
                // from latency_ticks, not from 0, so no ticks are lost
                __HAL_TIM_SET_COUNTER(&htim1, latency_ticks);

                // call any configured callback
                _encdata.onindexcrossing.execute();
            }

            // always clear the leading-edge flag to avoid false positives
            __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC3);
        }
    }        
        
#else        
        
    // called at index crossing of the trailing edge
    static void EncCapture_cb(TIM_HandleTypeDef *htim)
    {
        // there must be a leading edge before 
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
                // convert te to signed: ticks elapsed since last call of __HAL_TIM_SET_COUNTER(&htim1, 0): in this callback or at start()
                int32_t signed_te = (dir > 0) ? (te) : (te - static_cast<int32_t>(timARR));

                // update the cumulative number of ticks 
                _encdata.cumulativeticksatindex += static_cast<int64_t>(signed_te);
                _encdata.numberofrevolutions += dir;

                __HAL_TIM_SET_COUNTER(&htim1, 0);

                // call any configured callback
                _encdata.onindexcrossing.execute();
                
            }
            
            // always clear the leading-edge flag to avoid false positives
            __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC3);   
            
        }
    }

#endif

    
    HAL_StatusTypeDef start__former_EncInit(embot::hw::MOTOR m, const Mode &mo)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        
        _encdata.clear();
        _encdata.load(mo.resolution, mo.onindex, ticksperpulse(m)); 

        /* Register the callback function used to signal the activation of the Index pulse */
        if (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
        {
            __HAL_TIM_SET_COUNTER(&htim1, 0);
            /* Start timers in encoder mode */
            if (HAL_OK == HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL))
            {
                /* Enable leading edge capture, without interrupts */
                HAL_TIM_IC_Start(&htim1, ENC_INDEX_LEADING_EDGE);
                /* Enable trailing edge capture, with interrupts */
                // it is this one that triggers the EncCapture_cb callback
                HAL_TIM_IC_Start_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
                return HAL_OK;
            }
            /* Failed start of the timer */
            HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        }
        /* Errors detected */
        return HAL_ERROR;
    }    
  
    
    void stop__former_Enc1DeInit(void)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        
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
 
    
    void start(embot::hw::MOTOR m, const Mode &mo)
    {       
        start__former_EncInit(m, mo);        
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

    static inline int32_t cnt_to_signed(uint32_t cnt, bool counting_down)
    {
        // se sto contando down, cnt rappresenta un numero modulo timARR (or 65536)
        // il signed corretto: cnt - 65536 (es: 65526 -> -10).
        return counting_down ? (static_cast<int32_t>(cnt) - timARR) : static_cast<int32_t>(cnt);
    }
    

    static inline int32_t counter_signed_value()
    {
        uint32_t cnt = __HAL_TIM_GetCounter(&htim1);
        bool down = (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1) != 0);
        return cnt_to_signed(cnt, down);
    }
    
    int64_t ticks(embot::hw::MOTOR m, AngleQE a)
    {
        int64_t cumulated {0};  // ticks cumulated so far at index detection
        int32_t current {0};    // current signed value of the TIM counter register

        // i must protect vs execution of the EncCapture_cb() callback.       
        // i disable only the IRQ that drives EncCapture_cb leaving all other interrupts unaffected.
        HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
        // no: __disable_irq();
        
        cumulated = _encdata.cumulativeticksatindex;
        if(AngleQE::current == a)
        {
            current  = counter_signed_value();
        }
        
        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        // no: __enable_irq();

        return cumulated + current;
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
    
    
} // namespace embot::hw::motor::bldc::enc::bsp::impl {

// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------


namespace embot::hw::motor::bldc::enc::bsp::stm32 {
        
    void HAL_TIM_IC_MspInit(void* p)        { embot::hw::motor::bldc::enc::bsp::impl::HAL_TIM_IC_MspInit(p); }
    void HAL_TIM_IC_MspDeInit(void* p)      { embot::hw::motor::bldc::enc::bsp::impl::HAL_TIM_IC_MspDeInit(p); }
    
    void HAL_TIM_Encoder_MspInit(void *p)   { embot::hw::motor::bldc::enc::bsp::impl::HAL_TIM_Encoder_MspInit(p); }
    void HAL_TIM_Encoder_MspDeInit(void *p) { embot::hw::motor::bldc::enc::bsp::impl::HAL_TIM_Encoder_MspDeInit(p); }
            
} // namespace embot::hw::motor::bldc::hall::bsp::stm32 {

extern "C"
{

    void TIM1_CC_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::enc::bsp::impl::htim1);
    }

}

#if 0 // other versions
// --------------------------------------------------------------------------------------------------------------------
// other versions
// -


namespace embot::hw::motor::bldc::enc::bsp::impl::originalrevised {

    // we use timer TIM1 that is a 16-bit timer
    // on amcfoc we had TIM2 and TIM5 that are 32-bit timers

    TIM_HandleTypeDef htim1 {};
        
    constexpr size_t timARR {65535+1};
        
    constexpr uint8_t QEncMode {TIM_ENCODERMODE_TI12};

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
      if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
      sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
      sConfigIC.ICFilter = 0;
      if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
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
        /* TIM1 clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**TIM1 GPIO Configuration
        PE13     ------> TIM1_CH3
        PE9     ------> TIM1_CH1
        PE11     ------> TIM1_CH2
        */
        GPIO_InitStruct.Pin = MOT_ENCZ_Pin|MOT_ENCA_Pin|MOT_ENCB_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        #warning ....... TIM1_BRK_IRQn+TIM1_UP_IRQn per encoder -> sicuro??? forse TIM1_IRQn oppure CC
        /* TIM1 interrupt Init */
        HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);
        HAL_NVIC_SetPriority(TIM1_UP_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

      }
    }


    void HAL_TIM_IC_MspDeInit(void *p)
    {
         TIM_HandleTypeDef* tim_icHandle = reinterpret_cast<TIM_HandleTypeDef*>(p);

      if(tim_icHandle->Instance==TIM1)
      {
      /* USER CODE BEGIN TIM1_MspDeInit 0 */

      /* USER CODE END TIM1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();

        /**TIM1 GPIO Configuration
        PE13     ------> TIM1_CH3
        PE9     ------> TIM1_CH1
        PE11     ------> TIM1_CH2
        */
        HAL_GPIO_DeInit(GPIOE, MOT_ENCZ_Pin|MOT_ENCA_Pin|MOT_ENCB_Pin);

        /* TIM1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM1_BRK_IRQn);
        HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
      /* USER CODE BEGIN TIM1_MspDeInit 1 */

      /* USER CODE END TIM1_MspDeInit 1 */
      }
    }  


    void HAL_TIM_Encoder_MspInit(void *p) {}
    void HAL_TIM_Encoder_MspDeInit(void *p) {}         

}

namespace embot::hw::motor::bldc::enc::bsp::impl::originalrevised {
        

    struct encData
    {
        /* Status register values */
        static constexpr auto STATUS_IDLE  = 0;
        static constexpr auto STATUS_WAIT  = 1;
        static constexpr auto STATUS_READY = 2;
        
        
        uint8_t  status {STATUS_IDLE};          // state of index acquisition
        int32_t numberofrevolutions {0};
        uint32_t encFirstIndexCross {0};
        uint32_t absolutezero {0};              // first detected index -> reference
        uint32_t rotorzero {0};                 // offset between timer count and real mechanical zero
        bool firstacquisition {true};   
        uint32_t pulsesperrevolution {1024};    // encoder pulses per revolution of datasheet 
        uint8_t ticksperpulse {4};                    // x2 or x4 decoding   
        float counter2degreesfactor {1.0};      // scaling factor to avoid runtime divisions
        float degrees {0.0};                    // current angle     
        
        void clear()
        {
            status = STATUS_IDLE;
            numberofrevolutions = 0;
            encFirstIndexCross = 0;
            absolutezero = 0;
            rotorzero = 0;
            pulsesperrevolution = 1024;
            counter2degreesfactor = 1;
            ticksperpulse = 4;
            degrees = 0.0;
            firstacquisition = true;
        }
        
        void loadpulsesperrevolution(uint32_t ppr, uint8_t div)
        {
            pulsesperrevolution = ppr;
            ticksperpulse = div;
            counter2degreesfactor = 360.0f / static_cast<float>(pulsesperrevolution*ticksperpulse);
        }
          
        encData() = default;
    };
    
    
    encData _encdata {};
        
    
    // both edges are needed to determine rotation direction at the index crossing  
    static constexpr auto ENC_INDEX_LEADING_EDGE = TIM_CHANNEL_3;
    static constexpr auto ENC_INDEX_IT_LEADING_EDGE = TIM_IT_CC3;
    static constexpr auto ENC_INDEX_TRAILING_EDGE = TIM_CHANNEL_4;
    static constexpr auto ENC_INDEX_IT_TRAILING_EDGE = TIM_IT_CC4;

    
    /* Offset between the counting edge and the index edge */
    static constexpr auto ENC_UP_COUNTING_OFFSET = 1;
    static constexpr auto ENC_DOWN_COUNTING_OFFSET = 1; 

    
    // called at index crossing of the trailing edge
    static void EncCapture_cb(TIM_HandleTypeDef *htim)
    {
        /* There must be a leading edge before */
        if (0 != __HAL_TIM_GET_FLAG(&htim1, ENC_INDEX_IT_LEADING_EDGE))
        {
            // ok, we have both edges: leanding and then trailing
            /* Read the index trailing edge position */
            int32_t trailingedge = (int32_t)__HAL_TIM_GetCompare(&htim1, ENC_INDEX_TRAILING_EDGE);
            /* Read the index leading edge position */
            int32_t leadingedge = (int32_t)__HAL_TIM_GetCompare(&htim1, ENC_INDEX_LEADING_EDGE);
            /* Take the difference between readings */
            // it tells if the counter increased or decreases. if delta > 0 it counts up
            int32_t delta = trailingedge - leadingedge;
            /* Avoid inversion of direction over the index */
            if (0 != delta)
            {
                /* Update the index position */
                _encdata.rotorzero = (uint32_t)((delta>=0)? (leadingedge + ENC_UP_COUNTING_OFFSET) : (leadingedge - ENC_DOWN_COUNTING_OFFSET));

                /* Mark the absolute zero position */
                // we do that only once when the status is in WAIT
                if (encData::STATUS_WAIT == _encdata.status)
                {
                    /* Store the absolute zero */
                    _encdata.absolutezero = _encdata.rotorzero;
                    /* Do not repeat again */
                    _encdata.status = encData::STATUS_READY;
                }
            }
        }
    }
    
    
    HAL_StatusTypeDef start__former_EncInit(embot::hw::MOTOR m, const Mode &mo)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        
        _encdata.clear();
        _encdata.loadpulsesperrevolution(mo.resolution, ticksperpulse(m)); 

        /* Register the callback function used to signal the activation of the Index pulse */
        if (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
        {
            __HAL_TIM_SET_COUNTER(&htim1, 0);
            /* Start timers in encoder mode */
            if (HAL_OK == HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL))
            {
                /* Enable leading edge capture, without interrupts */
                HAL_TIM_IC_Start(&htim1, ENC_INDEX_LEADING_EDGE);
                /* Enable trailing edge capture, with interrupts */
                // it is this one that triggers the EncCapture_cb callback
                HAL_TIM_IC_Start_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
                return HAL_OK;
            }
            /* Failed start of the timer */
            HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        }
        /* Errors detected */
        return HAL_ERROR;
    }    
  
    
    void stop__former_Enc1DeInit(void)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        
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
 
    
    void start(embot::hw::MOTOR m, const Mode &mo)
    {       
        start__former_EncInit(m, mo);        
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
    
    int64_t ticks(embot::hw::MOTOR m)
    { 
        return __HAL_TIM_GetCounter(&htim1) - _encdata.rotorzero;              
    } 
 
    
    int64_t indexcrossings(embot::hw::MOTOR m)
    { 
        return _encdata.numberofrevolutions;              
    }  

    
    float angle(embot::hw::MOTOR m, AngleQE a)
    { 
        return _encdata.counter2degreesfactor * static_cast<float>(ticks(m));              
    } 
    
    
} // namespace embot::hw::motor::bldc::enc::bsp::impl {


namespace embot::hw::motor::bldc::enc::bsp::impl::multiturn_buggy {
        

    struct encData
    {
        int32_t numberofrevolutions {0};
        int64_t totalcounts {0};
        uint32_t pulsesperrevolution {1024};    // encoder pulses per revolution of datasheet 
        uint8_t ticksperpulse {4};                    // x2 or x4 decoding 
        uint32_t countsperrevolution {1};         
        float counter2degreesfactor {1.0f};      // scaling factor to avoid runtime divisions

        
        void clear()
        {
            numberofrevolutions = 0;
            totalcounts = 0;
            pulsesperrevolution = 1024;            
            ticksperpulse = 4;
            countsperrevolution = 0;
            counter2degreesfactor = 1.0f;
        }
        
        void loadpulsesperrevolution(uint32_t ppr, uint8_t div)
        {
            pulsesperrevolution = ppr;
            ticksperpulse = div;
            countsperrevolution = pulsesperrevolution*ticksperpulse;
            counter2degreesfactor = 360.0f / static_cast<float>(countsperrevolution);
        }        
          
        encData() = default;
    };
    
    
    encData _encdata {};
        
    
    // both edges are needed to determine rotation direction at the index crossing  
    static constexpr auto ENC_INDEX_LEADING_EDGE = TIM_CHANNEL_3;
    static constexpr auto ENC_INDEX_IT_LEADING_EDGE = TIM_IT_CC3;
    static constexpr auto ENC_INDEX_TRAILING_EDGE = TIM_CHANNEL_4;
    static constexpr auto ENC_INDEX_IT_TRAILING_EDGE = TIM_IT_CC4;

       
    // called at index crossing of the trailing edge
    static void EncCapture_cb(TIM_HandleTypeDef *htim)
    {
        /* There must be a leading edge before */
        if (0 != __HAL_TIM_GET_FLAG(&htim1, ENC_INDEX_IT_LEADING_EDGE))
        {
            // ok, we have both edges: leading and then trailing
            // read the index trailing edge position
            int32_t trailingedge = (int32_t)__HAL_TIM_GetCompare(&htim1, ENC_INDEX_TRAILING_EDGE);
            // read the index leading edge position 
            int32_t leadingedge = (int32_t)__HAL_TIM_GetCompare(&htim1, ENC_INDEX_LEADING_EDGE);
            // get the difference between readings
            // it tells if the counter increased or decreases. if delta > 0 it counts up
            int32_t delta = trailingedge - leadingedge;
            // important: avoid inversion of direction over the index 
            if (0 != delta)
            {
                int dir = (delta >= 0) ? +1 : -1;
                _encdata.numberofrevolutions += dir;                
                
                // reset counter at index
                __HAL_TIM_SET_COUNTER(&htim1, 0);
            }
        }
    }
    
    
    HAL_StatusTypeDef start__former_EncInit(embot::hw::MOTOR m, const Mode &mo)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        
        _encdata.clear();
        _encdata.loadpulsesperrevolution(mo.resolution, ticksperpulse(m)); 

        /* Register the callback function used to signal the activation of the Index pulse */
        if (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
        {
            __HAL_TIM_SET_COUNTER(&htim1, 0);
            /* Start timers in encoder mode */
            if (HAL_OK == HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL))
            {
                /* Enable leading edge capture, without interrupts */
                HAL_TIM_IC_Start(&htim1, ENC_INDEX_LEADING_EDGE);
                /* Enable trailing edge capture, with interrupts */
                // it is this one that triggers the EncCapture_cb callback
                HAL_TIM_IC_Start_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
                return HAL_OK;
            }
            /* Failed start of the timer */
            HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        }
        /* Errors detected */
        return HAL_ERROR;
    }    
  
    
    void stop__former_Enc1DeInit(void)
    {
        /* Stop any pending operation */
        HAL_TIM_IC_Stop_IT(&htim1, ENC_INDEX_TRAILING_EDGE);
        HAL_TIM_IC_Stop(&htim1, ENC_INDEX_LEADING_EDGE);
        HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
        HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID);
        
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
 
    
    void start(embot::hw::MOTOR m, const Mode &mo)
    {       
        start__former_EncInit(m, mo);        
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
    
    // has values in range [0, countsperrevolution)
    int32_t modcounter(embot::hw::MOTOR m)
    { 
        return __HAL_TIM_GetCounter(&htim1);              
    } 
 
    
    int64_t ticks(embot::hw::MOTOR m)
    { 
#define counterPROTECTisr 

#if defined(counterPROTECTisr)
        
        int32_t revs {0};
        int32_t cnt {0};

        // _encdata.numberofrevolutions and counter of TIM1 are chenged in an ISR, so i had better protect it
        __disable_irq();
        revs = _encdata.numberofrevolutions;
        cnt  = __HAL_TIM_GetCounter(&htim1);
        __enable_irq();

        return cnt + revs * _encdata.countsperrevolution;
#else        
        return modcounter(m) + _encdata.numberofrevolutions * _encdata.countsperrevolution;    
#endif        
    } 
    
    
    int64_t indexcrossings(embot::hw::MOTOR m)
    { 
        return _encdata.numberofrevolutions;              
    }  

    
    float angle(embot::hw::MOTOR m, AngleQE a)
    { 
        return ticks(m) * _encdata.counter2degreesfactor; 
    } 
    
    
} // namespace embot::hw::motor::bldc::enc::bsp::impl::multiturn::multiturn_buggy {

    // multiturn_buggy does not manage the initial count when we start un-aligned to index and
    // also doe not manage properly when we go backwards

#endif // #if 0 // other versions

#endif // #if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_enc)


#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_enc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

