
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_hall_bsp.h"


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


#if !defined(EMBOT_ENABLE_hw_motor_bldc_hall)

namespace embot::hw::motor::bldc::hall::bsp {

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

    bool BSP::start(embot::hw::MOTOR m) const 
    {
        return false;       
    } 

    bool BSP::stop(embot::hw::MOTOR m) const 
    {
        return false;       
    } 
    
    uint8_t BSP::triple(embot::hw::MOTOR m) const
    {
        return 0;
    }
    
    void BSP::capture(embot::hw::MOTOR m, uint8_t triple) const
    {    
    }


} // namespace embot::hw::motor::bldc::hall::bsp {

namespace embot::hw::motor::bldc::hall::bsp::stm32 {

    void HAL_TIMEx_HallSensor_MspInit(void *p) {}  
    void HAL_TIMEx_HallSensor_MspDeInit(void *p) {} 

} // namespace embot::hw::motor::bldc::hall::bsp::stm32 {

#elif defined(EMBOT_ENABLE_hw_motor_bldc_hall)


#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_hall)

namespace embot::hw::motor::bldc::hall::bsp {

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

    bool BSP::start(embot::hw::MOTOR m) const 
    {
        return false;       
    }     

    bool BSP::stop(embot::hw::MOTOR m) const 
    {
        return false;       
    } 
    
    uint8_t BSP::triple(embot::hw::MOTOR m) const
    {
        return 0;
    }

// implemented inside file embot_hw_motor_bldc_hall.cpp so that it can use its internal data
//    void BSP::capture(embot::hw::MOTOR m, uint8_t triple) const
//    {    
//    }
       

} // namespace embot::hw::motor::bldc::hall::bsp {


namespace embot::hw::motor::bldc::hall::bsp::stm32 {

    void HAL_TIMEx_HallSensor_MspInit(void *p) {}  
    void HAL_TIMEx_HallSensor_MspDeInit(void *p) {} 

} // namespace embot::hw::motor::bldc::hall::bsp::stm32 {

#else

namespace embot::hw::motor::bldc::hall::bsp::impl {
    
    void init(embot::hw::MOTOR m);
    void deinit(embot::hw::MOTOR m);
    void start(embot::hw::MOTOR m);
    void stop(embot::hw::MOTOR m);
    
} // namespace embot::hw::motor::bldc::hall::bsp::impl {


namespace embot::hw::motor::bldc::hall::bsp {

    constexpr uint32_t supportedmask
    {
        embot::core::binary::mask::pos2mask<uint32_t>(embot::hw::MOTOR::one)
    };
    
    // for amcmj1 are: h1 = B4, h2 = B5, h3 = B0
    // for amcfoc motor 2 they were: h1 = B4, h2 = B5, h3 = B0
    // verify the order of the gpios. in here we have teh same order in an array as in amcfoc 
    // the rule is that in phallmotor1.hhh[0] we have h1, in hhh[1] h2 and in hhh[2] h3
    constexpr PROP phallmotor1 {         
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::four},     // h1
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::five},     // h2  
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::zero}      // h3
    };
        
    constexpr BSP thebsp {
        supportedmask,
        {{
            &phallmotor1
        }}
    };
    
    const BSP& getBSP() 
    {
        return thebsp;
    }     

    bool BSP::init(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::hall::bsp::impl::init(m);
        return true;       
    } 

    bool BSP::deinit(embot::hw::MOTOR m) const 
    {
        embot::hw::motor::bldc::hall::bsp::impl::deinit(m);
        return true;       
    } 

    bool BSP::start(embot::hw::MOTOR m) const
    {
        embot::hw::motor::bldc::hall::bsp::impl::start(m);
        return true;
    }   

    bool BSP::stop(embot::hw::MOTOR m) const
    {
        embot::hw::motor::bldc::hall::bsp::impl::stop(m);
        return true;
    }  
    
    uint8_t BSP::triple(embot::hw::MOTOR m) const
    {
        uint8_t v = 0;

        // i use directly phallmotor1 rather than using getBSP(). etc to retrieve the PROP *           
        // const PROP *pPROPmotONE = getBSP().getPROP(m);
        // if(nullptr == pPROPmotONE)
        // {
        //    return v;
        // }
        
        const PROP *pPROPmotONE {&phallmotor1};
               
        for(size_t i=0; i<numofhallsensors; i++)
        {
            if(embot::hw::gpio::State::SET == embot::hw::gpio::get(pPROPmotONE->hhh[i]))
            {
                embot::core::binary::bit::set(v, i);
            }
        }
        
        // embot::core::print("hall value for " + embot::hw::motor::bldc::to_string(m) + " = " + std::to_string(v));
        
        return v;       
    }
    
// implemented inside file embot_hw_motor_bldc_hall.cpp so that it can use its internal data
//    void BSP::capture(embot::hw::MOTOR m, uint8_t triple) const
//    {    
//    }    
    
} // namespace embot::hw::motor::bldc::hall::bsp {


namespace embot::hw::motor::bldc::hall::bsp::impl {
    
    // we use timer TIM3 and some GPIOs

    TIM_HandleTypeDef htim3 {};
        
    #define MOT_HALL1_Pin GPIO_PIN_4
    #define MOT_HALL1_GPIO_Port GPIOB        
    #define MOT_HALL2_Pin GPIO_PIN_5
    #define MOT_HALL2_GPIO_Port GPIOB        
    #define MOT_HALL3_Pin GPIO_PIN_0
    #define MOT_HALL3_GPIO_Port GPIOB
        
    void Error_Handler() { for(;;); }

    void MX_TIM3_Init(void)
    {

      /* USER CODE BEGIN TIM3_Init 0 */

      /* USER CODE END TIM3_Init 0 */

      TIM_HallSensor_InitTypeDef sConfig = {0};
      TIM_MasterConfigTypeDef sMasterConfig = {0};

      /* USER CODE BEGIN TIM3_Init 1 */

      /* USER CODE END TIM3_Init 1 */
      htim3.Instance = TIM3;
      htim3.Init.Prescaler = 0;
      htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim3.Init.Period = 65535;
      htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
      sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
      sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
      sConfig.IC1Filter = 0;
      sConfig.Commutation_Delay = 0;
      if (HAL_TIMEx_HallSensor_Init(&htim3, &sConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
      if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
      {
        Error_Handler();
      }
      /* USER CODE BEGIN TIM3_Init 2 */

      /* USER CODE END TIM3_Init 2 */

    }
    

    void MX_TIM3_DeInit(void)
    {
      /* USER CODE BEGIN TIM3_DeInit 0 */

      /* USER CODE END TIM3_DeInit 0 */

      if (HAL_TIMEx_HallSensor_DeInit(&htim3) != HAL_OK)
      {
        Error_Handler();
      }

      /* USER CODE BEGIN TIM3_DeInit 1 */

      /* USER CODE END TIM3_DeInit 1 */
    }

    void HAL_TIMEx_HallSensor_MspInit(void *p)
    {
        
        TIM_HandleTypeDef* timex_hallsensorHandle = reinterpret_cast<TIM_HandleTypeDef*>(p);

      GPIO_InitTypeDef GPIO_InitStruct = {0};
      if(timex_hallsensorHandle->Instance==TIM3)
      {
      /* USER CODE BEGIN TIM3_MspInit 0 */

      /* USER CODE END TIM3_MspInit 0 */
        /* TIM3 clock enable */
        __HAL_RCC_TIM3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**TIM3 GPIO Configuration
        PB5     ------> TIM3_CH2
        PB4 (NJTRST)     ------> TIM3_CH1
        PB0     ------> TIM3_CH3
        */
        GPIO_InitStruct.Pin = MOT_HALL2_Pin|MOT_HALL1_Pin|MOT_HALL3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* TIM3 interrupt Init */
        HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
      /* USER CODE BEGIN TIM3_MspInit 1 */

      /* USER CODE END TIM3_MspInit 1 */
      }
    }    


    void HAL_TIMEx_HallSensor_MspDeInit(void *p)
    {
        TIM_HandleTypeDef* timex_hallsensorHandle = reinterpret_cast<TIM_HandleTypeDef*>(p);

      if(timex_hallsensorHandle->Instance==TIM3)
      {
      /* USER CODE BEGIN TIM3_MspDeInit 0 */

      /* USER CODE END TIM3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM3_CLK_DISABLE();

        /**TIM3 GPIO Configuration
        PB5     ------> TIM3_CH2
        PB4 (NJTRST)     ------> TIM3_CH1
        PB0     ------> TIM3_CH3
        */
        HAL_GPIO_DeInit(GPIOB, MOT_HALL2_Pin|MOT_HALL1_Pin|MOT_HALL3_Pin);

        /* TIM3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM3_IRQn);
      /* USER CODE BEGIN TIM3_MspDeInit 1 */

      /* USER CODE END TIM3_MspDeInit 1 */
      }
    }    


} // namespace embot::hw::motor::bldc::hall::bsp::impl {


namespace embot::hw::motor::bldc::hall::bsp::impl {
        
    static void OnCaptureM1(TIM_HandleTypeDef *htim)
    {
        uint8_t v = embot::hw::motor::bldc::hall::bsp::thebsp.triple(embot::hw::MOTOR::one);
        embot::hw::motor::bldc::hall::bsp::thebsp.capture(embot::hw::MOTOR::one, v);
    }
    
    void init(embot::hw::MOTOR m)
    {
        // init the timer plus its linked gpios inside the called HAL_TIMEx_HallSensor_MspInit()
        MX_TIM3_Init();

        // may re-init the pins already initted by the timer msp call BUT ... must use correct settings, so BETTER not      


        // what else ...    

        stop(m);        
        
    }
    
    void deinit(embot::hw::MOTOR m)
    {
        // for sure call stop(), then deinit the timer
        
        stop(m); 
        MX_TIM3_DeInit();
        
    }
    
    void start(embot::hw::MOTOR m)
    {
        bool ret {false};
        
        // stop timer, register callback and start base and start IC in interrupt mode
        
        // marco.accame on 19 may 2026: i get this code from the amcfoc driver because the amcmj1 does not configure any capture

        // marco.accame: i stop things
        
        /* Stop counter */
        HAL_TIM_Base_Stop(&htim3);
        /* Stop interrupt */
        HAL_TIM_IC_Stop(&htim3, TIM_CHANNEL_1);
        
        // marco.accame: i register the callback and start counter.
        /* Register the callback function for CH1 capture event */
        if (HAL_OK == HAL_TIM_RegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID, OnCaptureM1))
        {
            /* Start counter */
            if (HAL_OK == HAL_TIM_Base_Start(&htim3))
            {
                /* Start channel 1 in interrupt mode */
                if (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1))
                {
                    ret = true;
                }
                else
                {
                    ret = false;
                    /* Stop counter */
                    HAL_TIM_Base_Stop(&htim3);
                }
            }
            else
            {
                ret = false;
                /* Remove the callback function */
                HAL_TIM_UnRegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID);
            }
        }    
        
    }
    
    void stop(embot::hw::MOTOR m)
    {        
        // stop timer and unregister the callback   

        // marco.accame: stop interrupt first (mirror of Start_IT)
        HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);

        // marco.accame: stop base counter
        HAL_TIM_Base_Stop(&htim3);

        // marco.accame: unregister callback
        HAL_TIM_UnRegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID);        
    }    
    
} // namespace embot::hw::motor::bldc::hall::bsp::impl {


// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------


namespace embot::hw::motor::bldc::hall::bsp::stm32 {
        
    void HAL_TIMEx_HallSensor_MspInit(void* p)      { embot::hw::motor::bldc::hall::bsp::impl::HAL_TIMEx_HallSensor_MspInit(p); }
    void HAL_TIMEx_HallSensor_MspDeInit(void* p)    { embot::hw::motor::bldc::hall::bsp::impl::HAL_TIMEx_HallSensor_MspDeInit(p); }
            
} // namespace embot::hw::motor::bldc::hall::bsp::stm32 {


extern "C"
{

    void TIM3_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::hall::bsp::impl::htim3);
    } 
    
}

#endif // #if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_hall)


#endif // #elif defined(EMBOT_ENABLE_hw_motor_hall)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

