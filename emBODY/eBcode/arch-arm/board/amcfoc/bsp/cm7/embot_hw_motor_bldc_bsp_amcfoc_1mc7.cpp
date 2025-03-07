
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"


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

#define WIP_REMOVE_AMC_1CM7_CODE


#if defined(WIP_REMOVE_AMC_1CM7_CODE)
    #warning WIP_REMOVE_AMC_1CM7_CODE is defined
#endif


#include "embot_hw_motor_bldc_bsp.h"
#include "embot_hw_analog.h"


#if !defined(EMBOT_ENABLE_hw_motor_bldc)

namespace embot::hw::motor::bldc::bsp {
    
    constexpr BSP thebsp { }; 
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
    bool BSP::init(embot::hw::MOTOR h) const { return false; }   

    bool configure(embot::hw::MOTOR m, const Config &config) const { return false; }
    bool enable(MOTOR m, bool on) const { return false; }
//    bool enabled(MOTOR m) const { return false; }
    
    bool fault(MOTOR m, bool on) const { return false; }
//    bool faulted(MOTOR m) const { return false; }

    bool set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents) const { return false; }  
    
    HallStatus hall(MOTOR m) const { return 0; }
    Angle angle(MOTOR m, AngleType type) const { return 0.0f; } 
    
    bool set(MOTOR m, const PWM3 &pwm) const { return false; }
    Voltage powersupply(MOTOR m) const { return 0.0f; }        
}

namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {
    
    void TIM_base_MspInit(TIM_HandleTypeDef* htim_base) {}
    void TIM_base_MspDeInit(TIM_HandleTypeDef* htim_base) {}    
}

#elif defined(EMBOT_ENABLE_hw_motor_bldc)


// used by the amcfoc.mot running on cm7
#include "embot_hw_motor_adc.h"  
#include "embot_hw_motor_enc.h"  
#include "embot_hw_motor_hall.h"  
#include "embot_hw_motor_pwm.h" 

namespace embot::hw::motor::bldc::bsp {
                  
    constexpr PROP propM1 { 0 };
    constexpr PROP propM2 { 0 };
    
    constexpr BSP thebsp {     

        // maskofsupported
        mask::pos2mask<uint32_t>(MOTOR::one) | mask::pos2mask<uint32_t>(MOTOR::two),        
        // properties
        {{
            &propM1, &propM2
        }}
  
    };
    
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
    
    bool BSP::deinit(embot::hw::MOTOR m) const
    {
        embot::hw::motor::adc::deinit(m);
        embot::hw::motor::enc::deinit(); 
        embot::hw::motor::hall::deinit(m); 
        embot::hw::motor::pwm::deinit(m); 

        return true;
    }
    
    
    bool BSP::init(embot::hw::MOTOR m) const 
    {  
        // we init the two motors together. we cannot do one and not the other, so ...        
        static bool onceonly_initted {false};
                
        if(false == onceonly_initted)
        {
            amcfoc::cm7::Init_MOTOR(m);
            onceonly_initted = true;
        } 
        

        // and now the init that can be done multiple times 
        {
            // i want to be sure that the pwm is not active       
            embot::hw::motor::pwm::deinit(m); 
            // adc acquisition of the currents starts straigth away with ::init()
            embot::hw::motor::adc::init(m, {});         
            // then we init the encoder. we actually dont start acquisition because we do that in enc::start()            
            embot::hw::motor::enc::init({}); 
            // same applies for hall 
            embot::hw::motor::hall::init(m, {});               
            // ok, we start pwm
            embot::hw::motor::pwm::init(m, {});          
            // now we calibrate adc acquisition
            embot::hw::motor::adc::calibrate(m, {});
            // now we start analog acquisition
            embot::hw::analog::init({});
                
            // we may calibrate also the encoder so that it is aligned w/ hall values
            // but maybe better do it later    
        }            
    
        
        return true;
    }
    
    std::array<Config, 2> _configs {};
        
    bool BSP::configure(embot::hw::MOTOR m, const Config &cfg) const
    {
        bool r {false};
        
        if((true == cfg.has_quad_enc) && (0 != cfg.enc_resolution) && (cfg.pwm_num_polar_couples > 0))
        {
            // start the encoder
            #warning the amcfoc must init encoder using MOTOR m
            embot::hw::motor::enc::Mode mode {cfg.enc_resolution, cfg.pwm_num_polar_couples, false, false};
            embot::hw::motor::enc::start(mode);
        }
        
        if(true == cfg.pwm_has_hall_sens)
        {
            // start the hall acquisition
             embot::hw::motor::hall::Mode mode { cfg.pwm_swapBC ?  embot::hw::motor::hall::Mode::SWAP::BC :  embot::hw::motor::hall::Mode::SWAP::none, cfg.pwm_hall_offset, cfg.pwm_num_polar_couples };
             embot::hw::motor::hall::start(m, mode);
        }
        
        _configs[embot::core::tointegral(m)] = cfg;
        
        r = true;
        
        return r;
    }
    
    bool BSP::enable(MOTOR m, bool on) const
    {
        bool r {false};
        embot::hw::motor::pwm::enable(m, on);
        return true;
    }     
    
//    bool BSP::enabled(MOTOR m) const
//    {
//        bool r {false};
//        
//        return r;
//    }     
   
    bool BSP::fault(MOTOR m, bool on) const
    {
        bool r {false};        
        // in this case it does NOTHING
        r = true;
        return r;
    }     


//    bool BSP::faulted(MOTOR m) const
//    {
//        bool r {false};
//        
//        return r;
//    }     



    bool BSP::set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents) const
    {
        bool r {false};
        r = embot::hw::motor::adc::set(m, oncurrents);
        return r;
    }     


    
    HallStatus BSP::hall(MOTOR m) const
    {
        return embot::hw::motor::hall::getstatus(m);        
    }     


    Angle BSP::angle(MOTOR m, AngleType type) const
    {
        Angle r {0.0f};
        #warning fill BSP::angle(MOTOR m, AngleType type) for amcfoc
        if((type == AngleType::hall_electrical) || (type == AngleType::hall_mechanical))
        {
            r = embot::hw::motor::hall::angle(m, type);
        }            
        //Angle r = (enc == Encoder::hall) ? 0.0 : 1.0;        
        //r = embot::hw::motor::hall::angle(m) OR .....;         
        return r;
    }     

    
    bool BSP::set(MOTOR m, const PWM3 &pwm) const
    {
        embot::hw::motor::pwm::setperc(m, pwm.u, pwm.v, pwm.w);
        return true;
    }     


    Voltage BSP::powersupply(MOTOR m) const
    {
        Voltage r {0.0f};
        r = embot::hw::analog::getVin();
        return r;
    }     

     
} // namespace embot::hw::motor::bldc::bsp {

 

// the motorhal requires:
// ADC1, ADC2
// TIM1, TIM4, TIM5
// ADC3 and TIM15 for measuring power supply and currents


namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {


    TIM_HandleTypeDef htim1;        // motor::2
    TIM_HandleTypeDef htim2;        // qenc
    TIM_HandleTypeDef htim3;        // hall::2
    TIM_HandleTypeDef htim4;        // hall::1 // motor 1
    TIM_HandleTypeDef htim5;        // qenc
    TIM_HandleTypeDef htim8;        // motor::1
    TIM_HandleTypeDef htim15;       // periodically triggers ADC3
    
    

    TIM_HandleTypeDef &htimHALL1 = htim4;        // hall
    TIM_HandleTypeDef &htimHALL2 = htim3;        // hall
    
    TIM_HandleTypeDef &htimMOT1 = htim8;
    TIM_HandleTypeDef &htimMOT2 = htim1;
    
    TIM_HandleTypeDef &htimTriggerOfadcOTHERS = htim15;

    
    
    // adc1 and adc2 sample motor currents in synch with pwm (tim1 and tim 8)
    // for each pwm cycle the adc samples the same current twice, when pwm line is low and then high.
    // hence 3 pwm cycles are required to collect all the three phases.
    
    ADC_HandleTypeDef hadc1;        // motor::2 -> currents c1, c2, c3
    ADC_HandleTypeDef hadc2;        // motor::1 -> currents c1, c2, c3
    ADC_HandleTypeDef hadc3;        // keeps values such as: ptc1, ptc2, cin, vin, vaux, vcc, vcore, vbat, vrefint, temp
    DMA_HandleTypeDef hdma_adc1;
    DMA_HandleTypeDef hdma_adc2;
    DMA_HandleTypeDef hdma_adc3;    
    
    DAC_HandleTypeDef hdac1;
    
    
    ADC_HandleTypeDef &hadcMOT1 = hadc2;     
    ADC_HandleTypeDef &hadcMOT2 = hadc1;  
    ADC_HandleTypeDef &hadcOTHERS = hadc3;          
    

    void Error_Handler() { }
    
    void MX_DMA_Init(void);    
    
    void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);        

    void MX_TIM1_Init(void);
    void MX_TIM2_Init(void);
    void MX_TIM3_Init(void);
    void MX_TIM4_Init(void);
    void MX_TIM5_Init(void);
    void MX_TIM8_Init(void);
    void MX_TIM15_Init(void);
        
    void MX_ADC1_Init(void);
    void MX_ADC2_Init(void);
    void MX_ADC3_Init(void);    
    
//    void MX_DAC1_Init(void);

}


namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {

       
    void Init_MOTOR(embot::hw::MOTOR m)
    {
        
        // add in here the initialisation of the GPIO that are relevant to the motor.
        // keept out the break which is managed inside embot::hw::button::bsp     

        
        // gpio
        // nothing inside gpio for now.
        
        // dma
        MX_DMA_Init();
        
        // timers
        
        // tim2 is qenc
        MX_TIM2_Init();
        // tim3 is hall sensor
        MX_TIM3_Init();
        // tim4 is hall sensor
        MX_TIM4_Init();
        // tim5 is qenc
        MX_TIM5_Init();
        // tim8 is motor::1
        MX_TIM8_Init();        
        // adc1 is 
        MX_ADC1_Init();
        MX_ADC3_Init();

        MX_TIM1_Init();
        MX_ADC2_Init();
        //MX_DAC1_Init();
        MX_TIM15_Init(); 

    }   
    
     
    
} // namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {




#if defined(WIP_REMOVE_AMC_1CM7_CODE)

#include "cm7_cubemx_defines.h"

namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {
    
    
// DMA section
// - only one _Init()



void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}      
      
    
// TIM section
// - as many _Init() you need, one for each TIMx
//   TIM1, TIM2, TIM3, TIM4, TIM5, TIM8
// TIM1 TIM8 TIM5
// - one HAL_TIM_Base_MspInit() and one HAL_TIM_Base_MspDeInit() for TIM1 TIM8 TIM15 
// - one HAL_TIM_MspPostInit() for TIM1 TIM8
// TIM2 TIM5
// - one HAL_TIM_IC_MspInit() and one HAL_TIM_IC_MspDeInit() for TIM2 TIM5
// TIM3 TIM4
// - one HAL_TIMEx_HallSensor_MspInit() and one HAL_TIMEx_HallSensor_MspDeInit()     
    
    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspPostInit 0 */

  /* USER CODE END TIM1_MspPostInit 0 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PB14     ------> TIM1_CH2N
    PB13     ------> TIM1_CH1N
    PE14     ------> TIM1_CH4
    PE12     ------> TIM1_CH3N
    PE13     ------> TIM1_CH3
    PE9      ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    */
    GPIO_InitStruct.Pin = MOT2_PWM2L_Pin|MOT2_PWM1L_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT2_PWM4_Pin|MOT2_PWM3L_Pin|MOT2_PWM3H_Pin|MOT2_PWM1H_Pin
                          |MOT2_PWM2H_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM1_MspPostInit 1 */
    /* TOP transistor OFF */
    HAL_GPIO_WritePin(MOT2_PWM1H_GPIO_Port, MOT2_PWM1H_Pin|MOT2_PWM2H_Pin|MOT2_PWM3H_Pin, GPIO_PIN_RESET);
    /* BOTTOM transistor OFF */
    HAL_GPIO_WritePin(MOT2_PWM1L_GPIO_Port, MOT2_PWM1L_Pin|MOT2_PWM2L_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOT2_PWM3L_GPIO_Port, MOT2_PWM3L_Pin, GPIO_PIN_RESET);
  /* USER CODE END TIM1_MspPostInit 1 */
  }
  else if(timHandle->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspPostInit 0 */

  /* USER CODE END TIM8_MspPostInit 0 */

    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM8 GPIO Configuration
    PH14     ------> TIM8_CH2N
    PH13     ------> TIM8_CH1N
    PH15     ------> TIM8_CH3N
    PC8      ------> TIM8_CH3
    PC7      ------> TIM8_CH2
    PC9      ------> TIM8_CH4
    PC6      ------> TIM8_CH1
    */
    GPIO_InitStruct.Pin = MOT1_PWM2L_Pin|MOT1_PWM1L_Pin|MOT1_PWM3L_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT1_PWM3H_Pin|MOT1_PWM2H_Pin|MOT1_PWM4_Pin|MOT1_PWM1H_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM8_MspPostInit 1 */
    /* TOP transistor OFF */
    HAL_GPIO_WritePin(MOT1_PWM1H_GPIO_Port, MOT1_PWM1H_Pin|MOT1_PWM2H_Pin|MOT1_PWM3H_Pin, GPIO_PIN_RESET);
    /* BOTTOM transistor OFF */
    HAL_GPIO_WritePin(MOT1_PWM1L_GPIO_Port, MOT1_PWM1L_Pin|MOT1_PWM2L_Pin|MOT1_PWM3L_Pin, GPIO_PIN_RESET);
  /* USER CODE END TIM8_MspPostInit 1 */
  }

} 


void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_ClearInputConfigTypeDef sClearInputConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIMEx_BreakInputConfigTypeDef sBreakInputConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
  htim1.Init.Period = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofTIMperiod(); // 1023
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClearInputConfig.ClearInputState = ENABLE;
  sClearInputConfig.ClearInputSource = TIM_CLEARINPUTSOURCE_ETR;
  sClearInputConfig.ClearInputPolarity = TIM_CLEARINPUTPOLARITY_NONINVERTED;
  sClearInputConfig.ClearInputPrescaler = TIM_CLEARINPUTPRESCALER_DIV1;
  sClearInputConfig.ClearInputFilter = 15;
  if (HAL_TIM_ConfigOCrefClear(&htim1, &sClearInputConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_ConfigOCrefClear(&htim1, &sClearInputConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_ConfigOCrefClear(&htim1, &sClearInputConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_ConfigOCrefClear(&htim1, &sClearInputConfig, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_OC5REF_RISING_OC6REF_RISING;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakInputConfig.Source = TIM_BREAKINPUTSOURCE_BKIN;
  sBreakInputConfig.Enable = TIM_BREAKINPUTSOURCE_ENABLE;
  sBreakInputConfig.Polarity = TIM_BREAKINPUTSOURCE_POLARITY_HIGH;
  if (HAL_TIMEx_ConfigBreakInput(&htim1, TIM_BREAKINPUT_BRK, &sBreakInputConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIMEx_ConfigBreakInput(&htim1, TIM_BREAKINPUT_BRK2, &sBreakInputConfig) != HAL_OK)
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
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofHIGHsampleoffset(); //1000;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_5) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofLOWsampleoffset(); // 24;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_6) != HAL_OK)
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
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_LOW;
  sBreakDeadTimeConfig.Break2Filter = 15;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 4;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 4;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 4;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

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

void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_HallSensor_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.Commutation_Delay = 0;
  if (HAL_TIMEx_HallSensor_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 4;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 4;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 4;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_ClearInputConfigTypeDef sClearInputConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIMEx_BreakInputConfigTypeDef sBreakInputConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
  htim8.Init.Period = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofTIMperiod(); // 1023;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
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
  sClearInputConfig.ClearInputState = ENABLE;
  sClearInputConfig.ClearInputSource = TIM_CLEARINPUTSOURCE_ETR;
  sClearInputConfig.ClearInputPolarity = TIM_CLEARINPUTPOLARITY_NONINVERTED;
  sClearInputConfig.ClearInputPrescaler = TIM_CLEARINPUTPRESCALER_DIV1;
  sClearInputConfig.ClearInputFilter = 15;
  if (HAL_TIM_ConfigOCrefClear(&htim8, &sClearInputConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_ConfigOCrefClear(&htim8, &sClearInputConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_ConfigOCrefClear(&htim8, &sClearInputConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_ConfigOCrefClear(&htim8, &sClearInputConfig, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim8, &sSlaveConfig) != HAL_OK)
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
  sConfigOC.Pulse = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofHIGHsampleoffset(); // 1000;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_5) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofLOWsampleoffset(); // 24;
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


void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 200;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 63;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}

// ADC section
// - one _Init() for ADC1 and one for ADC2 and one for ADC3
// - only one HAL_ADC_MspInit() HAL_ADC_MspDeInit() for all

void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 6;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO2;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_1;
  sConfig.Offset = 2048;
  sConfig.OffsetRightShift = DISABLE;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  sConfig.OffsetNumber = ADC_OFFSET_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  sConfig.OffsetNumber = ADC_OFFSET_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}
/* ADC2 init function */
void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

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
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
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
  sConfig.Channel = ADC_CHANNEL_3;
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
  sConfig.Channel = ADC_CHANNEL_5;
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
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

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
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T15_TRGO;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_16CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_16;
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
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_10;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}


// DAC section
// - as many _Init() you need, one for each DACx
// - one HAL_DAC_MspInit() for all
// - one HAL_DAC_MspDeInit() for all


///* DAC1 init function */
//void MX_DAC1_Init(void)
//{

//  /* USER CODE BEGIN DAC1_Init 0 */

//  /* USER CODE END DAC1_Init 0 */

//  DAC_ChannelConfTypeDef sConfig = {0};

//  /* USER CODE BEGIN DAC1_Init 1 */

//  /* USER CODE END DAC1_Init 1 */

//  /** DAC Initialization
//  */
//  hdac1.Instance = DAC1;
//  if (HAL_DAC_Init(&hdac1) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  /** DAC channel OUT1 config
//  */
//  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
//  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
//  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
//  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
//  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
//  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  /** DAC channel OUT2 config
//  */
//  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN DAC1_Init 2 */

//  /* USER CODE END DAC1_Init 2 */

//}

//void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
//{

//  if(dacHandle->Instance==DAC1)
//  {
//  /* USER CODE BEGIN DAC1_MspInit 0 */

//  /* USER CODE END DAC1_MspInit 0 */
//    /* DAC1 clock enable */
//    __HAL_RCC_DAC12_CLK_ENABLE();

//    /* DAC1 interrupt Init */
//    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
//    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
//  /* USER CODE BEGIN DAC1_MspInit 1 */

//  /* USER CODE END DAC1_MspInit 1 */
//  }
//}

//void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
//{

//  if(dacHandle->Instance==DAC1)
//  {
//  /* USER CODE BEGIN DAC1_MspDeInit 0 */

//  /* USER CODE END DAC1_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_DAC12_CLK_DISABLE();

//    /* DAC1 interrupt Deinit */
//    HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
//  /* USER CODE BEGIN DAC1_MspDeInit 1 */

//  /* USER CODE END DAC1_MspDeInit 1 */
//  }
//}

// acemorissimo
    
}


#elif !defined(WIP_REMOVE_AMC_1CM7_CODE)

namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {
   


    vedi questo perche' .... ho inizializzazione con coppie polari

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
    


  

} // namespace embot::hw::motor::bsp {

#endif // #elif !defined(WIP_REMOVE_AMC_1CM7_CODE)  



#if defined(WIP_REMOVE_AMC_1CM7_CODE)


// MSP functions
extern "C" {
    

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PE6     ------> TIM1_BKIN2
    PG5     ------> TIM1_ETR
    PE15     ------> TIM1_BKIN
    */
    GPIO_InitStruct.Pin = MOT2_BREAK2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(MOT2_BREAK2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT2_CLIMIT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(MOT2_CLIMIT_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT2_BREAK1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(MOT2_BREAK1_GPIO_Port, &GPIO_InitStruct);

    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspInit 0 */

  /* USER CODE END TIM8_MspInit 0 */
    /* TIM8 clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**TIM8 GPIO Configuration
    PG8     ------> TIM8_ETR
    PG2     ------> TIM8_BKIN
    PG3     ------> TIM8_BKIN2
    */
    GPIO_InitStruct.Pin = MOT1_CLIMIT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(MOT1_CLIMIT_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT1_BREAK1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(MOT1_BREAK1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT1_BREAK2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(MOT1_BREAK2_GPIO_Port, &GPIO_InitStruct);

    /* TIM8 interrupt Init */
    HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
    HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  /* USER CODE BEGIN TIM8_MspInit 1 */

  /* USER CODE END TIM8_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM15)
  {
  /* USER CODE BEGIN TIM15_MspInit 0 */

  /* USER CODE END TIM15_MspInit 0 */
    /* TIM15 clock enable */
    __HAL_RCC_TIM15_CLK_ENABLE();

    /* TIM15 interrupt Init */
    HAL_NVIC_SetPriority(TIM15_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM15_IRQn);
  /* USER CODE BEGIN TIM15_MspInit 1 */

  /* USER CODE END TIM15_MspInit 1 */
  }

}   


void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /**TIM1 GPIO Configuration
    PE6     ------> TIM1_BKIN2
    PG5     ------> TIM1_ETR
    PB14     ------> TIM1_CH2N
    PB13     ------> TIM1_CH1N
    PE14     ------> TIM1_CH4
    PE12     ------> TIM1_CH3N
    PE13     ------> TIM1_CH3
    PE15     ------> TIM1_BKIN
    PE9     ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    */
    HAL_GPIO_DeInit(GPIOE, MOT2_BREAK2_Pin|MOT2_PWM4_Pin|MOT2_PWM3L_Pin|MOT2_PWM3H_Pin
                          |MOT2_BREAK1_Pin|MOT2_PWM1H_Pin|MOT2_PWM2H_Pin);

    HAL_GPIO_DeInit(MOT2_CLIMIT_GPIO_Port, MOT2_CLIMIT_Pin);

    HAL_GPIO_DeInit(GPIOB, MOT2_PWM2L_Pin|MOT2_PWM1L_Pin);

    /* TIM1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM1_BRK_IRQn);
    HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspDeInit 0 */

  /* USER CODE END TIM8_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM8_CLK_DISABLE();

    /**TIM8 GPIO Configuration
    PH14     ------> TIM8_CH2N
    PH13     ------> TIM8_CH1N
    PH15     ------> TIM8_CH3N
    PC8     ------> TIM8_CH3
    PC7     ------> TIM8_CH2
    PC9     ------> TIM8_CH4
    PC6     ------> TIM8_CH1
    PG8     ------> TIM8_ETR
    PG2     ------> TIM8_BKIN
    PG3     ------> TIM8_BKIN2
    */
    HAL_GPIO_DeInit(GPIOH, MOT1_PWM2L_Pin|MOT1_PWM1L_Pin|MOT1_PWM3L_Pin);

    HAL_GPIO_DeInit(GPIOC, MOT1_PWM3H_Pin|MOT1_PWM2H_Pin|MOT1_PWM4_Pin|MOT1_PWM1H_Pin);

    HAL_GPIO_DeInit(GPIOG, MOT1_CLIMIT_Pin|MOT1_BREAK1_Pin|MOT1_BREAK2_Pin);

    /* TIM8 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
    HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
  /* USER CODE BEGIN TIM8_MspDeInit 1 */

  /* USER CODE END TIM8_MspDeInit 1 */
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
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* tim_icHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_icHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> TIM2_CH2
    PA15 (JTDI)     ------> TIM2_CH1
    PB10     ------> TIM2_CH3
    */
    GPIO_InitStruct.Pin = MOT2_ENCB_Pin|MOT2_ENCZ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT2_ENCA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(MOT2_ENCA_GPIO_Port, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(tim_icHandle->Instance==TIM5)
  {
  /* USER CODE BEGIN TIM5_MspInit 0 */

  /* USER CODE END TIM5_MspInit 0 */
    /* TIM5 clock enable */
    __HAL_RCC_TIM5_CLK_ENABLE();

    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**TIM5 GPIO Configuration
    PH11     ------> TIM5_CH2
    PH10     ------> TIM5_CH1
    PH12     ------> TIM5_CH3
    */
    GPIO_InitStruct.Pin = MOT1_ENCB_Pin|MOT1_ENCA_Pin|MOT1_ENCZ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* TIM5 interrupt Init */
    HAL_NVIC_SetPriority(TIM5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
  /* USER CODE BEGIN TIM5_MspInit 1 */

  /* USER CODE END TIM5_MspInit 1 */
  }
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* tim_icHandle)
{

  if(tim_icHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> TIM2_CH2
    PA15 (JTDI)     ------> TIM2_CH1
    PB10     ------> TIM2_CH3
    */
    HAL_GPIO_DeInit(GPIOB, MOT2_ENCB_Pin|MOT2_ENCZ_Pin);

    HAL_GPIO_DeInit(MOT2_ENCA_GPIO_Port, MOT2_ENCA_Pin);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
  else if(tim_icHandle->Instance==TIM5)
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
    HAL_GPIO_DeInit(GPIOH, MOT1_ENCB_Pin|MOT1_ENCA_Pin|MOT1_ENCZ_Pin);

    /* TIM5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM5_IRQn);
  /* USER CODE BEGIN TIM5_MspDeInit 1 */

  /* USER CODE END TIM5_MspDeInit 1 */
  }
}


void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef* timex_hallsensorHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timex_hallsensorHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB4 (NJTRST)     ------> TIM3_CH1
    PB5     ------> TIM3_CH2
    PB0     ------> TIM3_CH3
    */
    GPIO_InitStruct.Pin = MOT2_HALL1_Pin|MOT2_HALL2_Pin|MOT2_HALL3_Pin;
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
  else if(timex_hallsensorHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PB8     ------> TIM4_CH3
    PB7     ------> TIM4_CH2
    PD12     ------> TIM4_CH1
    */
    GPIO_InitStruct.Pin = MOT1_HALL3_Pin|MOT1_HALL2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT1_HALL1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(MOT1_HALL1_GPIO_Port, &GPIO_InitStruct);

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }
}

void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef* timex_hallsensorHandle)
{

  if(timex_hallsensorHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /**TIM3 GPIO Configuration
    PB4 (NJTRST)     ------> TIM3_CH1
    PB5     ------> TIM3_CH2
    PB0     ------> TIM3_CH3
    */
    HAL_GPIO_DeInit(GPIOB, MOT2_HALL1_Pin|MOT2_HALL2_Pin|MOT2_HALL3_Pin);

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(timex_hallsensorHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /**TIM4 GPIO Configuration
    PB8     ------> TIM4_CH3
    PB7     ------> TIM4_CH2
    PD12     ------> TIM4_CH1
    */
    HAL_GPIO_DeInit(GPIOB, MOT1_HALL3_Pin|MOT1_HALL2_Pin);

    HAL_GPIO_DeInit(MOT1_HALL1_GPIO_Port, MOT1_HALL1_Pin);

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }
}



static uint32_t HAL_RCC_ADC12_CLK_ENABLED=0;

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    HAL_RCC_ADC12_CLK_ENABLED++;
    if(HAL_RCC_ADC12_CLK_ENABLED==1){
      __HAL_RCC_ADC12_CLK_ENABLE();
    }

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PF11     ------> ADC1_INP2
    PA1_C     ------> ADC1_INP1
    PA0_C     ------> ADC1_INP0
    */
    GPIO_InitStruct.Pin = MOT2_CPHASE3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MOT2_CPHASE3_GPIO_Port, &GPIO_InitStruct);

    HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_OPEN);

    HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_OPEN);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Instance = DMA2_Stream0;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.Mode = DMA_CIRCULAR;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1) != HAL_OK)
    {
      embot::hw::motor::bldc::bsp::amcfoc::cm7::Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
  else if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */
    /* ADC2 clock enable */
    HAL_RCC_ADC12_CLK_ENABLED++;
    if(HAL_RCC_ADC12_CLK_ENABLED==1){
      __HAL_RCC_ADC12_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PB1     ------> ADC2_INP5
    PF13     ------> ADC2_INP2
    PA6     ------> ADC2_INP3
    */
    GPIO_InitStruct.Pin = MOT1_CPHASE3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MOT1_CPHASE3_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT1_CPHASE1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MOT1_CPHASE1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOT1_CPHASE2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MOT1_CPHASE2_GPIO_Port, &GPIO_InitStruct);

    /* ADC2 DMA Init */
    /* ADC2 Init */
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Instance = DMA2_Stream1;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.Request = DMA_REQUEST_ADC2;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.Mode = DMA_CIRCULAR;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2) != HAL_OK)
    {
      embot::hw::motor::bldc::bsp::amcfoc::cm7::Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2);

    /* ADC2 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
  else if(adcHandle->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspInit 0 */

  /* USER CODE END ADC3_MspInit 0 */
    /* ADC3 clock enable */
    __HAL_RCC_ADC3_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**ADC3 GPIO Configuration
    PF5     ------> ADC3_INP4
    PF3     ------> ADC3_INP5
    PF8     ------> ADC3_INP7
    PF7     ------> ADC3_INP3
    PF9     ------> ADC3_INP2
    PF10     ------> ADC3_INP6
    PC0     ------> ADC3_INP10
    PC2_C     ------> ADC3_INP0
    PC3_C     ------> ADC3_INP1
    PH2     ------> ADC3_INP13
    PH4     ------> ADC3_INP15
    PH3     ------> ADC3_INP14
    PH5     ------> ADC3_INP16
    */
    GPIO_InitStruct.Pin = MOT2_VPHASE2_Pin|MOT2_VPHASE3_Pin|MOT1_TEMP_Pin|MOT2_VPHASE1_Pin
                          |MOT1_VPHASE3_Pin|MOT2_TEMP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PWR_CIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PWR_CIN_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PWR_VIN_Pin|PWR_VCC_Pin|PWR_VAUX_Pin|PWR_VCORE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC2, SYSCFG_SWITCH_PC2_OPEN);

    HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3, SYSCFG_SWITCH_PC3_OPEN);

    /* ADC3 DMA Init */
    /* ADC3 Init */
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Instance = DMA2_Stream2;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.Request = DMA_REQUEST_ADC3;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.Mode = DMA_CIRCULAR;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.Priority = DMA_PRIORITY_HIGH;
    embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3) != HAL_OK)
    {
      embot::hw::motor::bldc::bsp::amcfoc::cm7::Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3);

    /* ADC3 interrupt Init */
    HAL_NVIC_SetPriority(ADC3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC3_IRQn);
  /* USER CODE BEGIN ADC3_MspInit 1 */

  /* USER CODE END ADC3_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_ADC12_CLK_ENABLED--;
    if(HAL_RCC_ADC12_CLK_ENABLED==0){
      __HAL_RCC_ADC12_CLK_DISABLE();
    }

    /**ADC1 GPIO Configuration
    PF11     ------> ADC1_INP2
    PA1_C     ------> ADC1_INP1
    PA0_C     ------> ADC1_INP0
    */
    HAL_GPIO_DeInit(MOT2_CPHASE3_GPIO_Port, MOT2_CPHASE3_Pin);

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

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
  else if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_ADC12_CLK_ENABLED--;
    if(HAL_RCC_ADC12_CLK_ENABLED==0){
      __HAL_RCC_ADC12_CLK_DISABLE();
    }

    /**ADC2 GPIO Configuration
    PB1     ------> ADC2_INP5
    PF13     ------> ADC2_INP2
    PA6     ------> ADC2_INP3
    */
    HAL_GPIO_DeInit(MOT1_CPHASE3_GPIO_Port, MOT1_CPHASE3_Pin);

    HAL_GPIO_DeInit(MOT1_CPHASE1_GPIO_Port, MOT1_CPHASE1_Pin);

    HAL_GPIO_DeInit(MOT1_CPHASE2_GPIO_Port, MOT1_CPHASE2_Pin);

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

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
  else if(adcHandle->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspDeInit 0 */

  /* USER CODE END ADC3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC3_CLK_DISABLE();

    /**ADC3 GPIO Configuration
    PF5     ------> ADC3_INP4
    PF3     ------> ADC3_INP5
    PF8     ------> ADC3_INP7
    PF7     ------> ADC3_INP3
    PF9     ------> ADC3_INP2
    PF10     ------> ADC3_INP6
    PC0     ------> ADC3_INP10
    PC2_C     ------> ADC3_INP0
    PC3_C     ------> ADC3_INP1
    PH2     ------> ADC3_INP13
    PH4     ------> ADC3_INP15
    PH3     ------> ADC3_INP14
    PH5     ------> ADC3_INP16
    */
    HAL_GPIO_DeInit(GPIOF, MOT2_VPHASE2_Pin|MOT2_VPHASE3_Pin|MOT1_TEMP_Pin|MOT2_VPHASE1_Pin
                          |MOT1_VPHASE3_Pin|MOT2_TEMP_Pin);

    HAL_GPIO_DeInit(PWR_CIN_GPIO_Port, PWR_CIN_Pin);

    HAL_GPIO_DeInit(GPIOH, PWR_VIN_Pin|PWR_VCC_Pin|PWR_VAUX_Pin|PWR_VCORE_Pin);

    /* ADC3 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC3_IRQn);
  /* USER CODE BEGIN ADC3_MspDeInit 1 */

  /* USER CODE END ADC3_MspDeInit 1 */
  }
}


// asfida
    
}

// IRQ handlers
extern "C" {
    
    void TIM1_BRK_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim1);
    }

    void TIM1_UP_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim1);
    }

    void TIM2_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim2);
    }

    void TIM3_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim3);
    }

    void TIM4_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim4);
    }  

    void TIM8_BRK_TIM12_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim8);
    }

    void TIM8_UP_TIM13_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim8);
    }

    void TIM5_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim5);
    }

    void TIM6_DAC_IRQHandler(void)
    {
        if(embot::hw::motor::bldc::bsp::amcfoc::cm7::hdac1.State != HAL_DAC_STATE_RESET) 
        {
            HAL_DAC_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdac1);
        }
        // HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim6);
    }

    void TIM15_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htim15);
    }

    void DMA2_Stream0_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc1);
    }

    void DMA2_Stream1_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc2);
    }

    void DMA2_Stream2_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hdma_adc3);
    }
    
}

#elif !defined(WIP_REMOVE_AMC_1CM7_CODE)



#endif // #elif !defined(WIP_REMOVE_AMC_1CM7_CODE)  

#if defined(WIP_REMOVE_AMC_1CM7_CODE)

#elif !defined(WIP_REMOVE_AMC_1CM7_CODE)


#endif // #elif !defined(WIP_REMOVE_AMC_1CM7_CODE)  



#if defined(WIP_REMOVE_AMC_1CM7_CODE)

#warning ... attenzione alla rimizione dl break che manca MOTORHALCONFIG_MOT_BREAK_IRQ_remove
    
#elif !defined(WIP_REMOVE_AMC_1CM7_CODE)


namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {
    
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
#if defined(EMBOT_ENABLE_hw_analog_ish)        
        else if(htim_base->Instance==TIM6)
        {
        /* USER CODE BEGIN TIM6_MspInit 0 */

        /* USER CODE END TIM6_MspInit 0 */
          /* Peripheral clock enable */
          __HAL_RCC_TIM6_CLK_ENABLE();
        /* USER CODE BEGIN TIM6_MspInit 1 */

        /* USER CODE END TIM6_MspInit 1 */
        }
#endif // #if defined(EMBOT_ENABLE_hw_analog_ish)        
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
#if defined(EMBOT_ENABLE_hw_analog_ish)        
        else if(htim_base->Instance==TIM6)
        {
        /* USER CODE BEGIN TIM6_MspDeInit 0 */
        
        /* USER CODE END TIM6_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_TIM6_CLK_DISABLE();
        /* USER CODE BEGIN TIM6_MspDeInit 1 */
        
        /* USER CODE END TIM6_MspDeInit 1 */
        }
#endif // #if defined(EMBOT_ENABLE_hw_analog_ish)        
    }  




} // namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {

#endif // #elif !defined(WIP_REMOVE_AMC_1CM7_CODE)  



#endif // #elif defined(EMBOT_ENABLE_hw_motor)





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

