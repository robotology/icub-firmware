
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
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



// - support map: begin of embot::hw::can

#include "embot_hw_can_bsp.h"

#include "embot_hw_gpio.h" // for the pin init ... 

#if !defined(EMBOT_ENABLE_hw_can)

namespace embot::hw::can {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::CAN h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#elif defined(EMBOT_ENABLE_hw_can)
    

namespace embot::hw::can {
                      
    FDCAN_HandleTypeDef hfdcan1 {};
    constexpr PROP can1p = { .handle = &hfdcan1 };             
        
    constexpr BSP thebsp {        
        // maskofsupported
        0 |
        mask::pos2mask<uint32_t>(CAN::one) |       
        0,        
        // properties
        {{
         
            &can1p,
            nullptr    
        }}        
    };
    
    
    void Error_Handler(const char *msg)
    {
        const char *more {"generic"};
        const char *c = (nullptr != msg) ? msg : more;       
        for(;;)
        {            
            embot::core::print("Error_Handler() called for CAN:" + std::string(c));
            embot::core::wait(1000000);            
        }
    }      
    
   
    void s_FDCAN1_Init(void)
    {
        hfdcan1.Instance = FDCAN1;
        hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
        hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
        hfdcan1.Init.AutoRetransmission = ENABLE;
        hfdcan1.Init.TransmitPause = ENABLE;
        hfdcan1.Init.ProtocolException = DISABLE;
        hfdcan1.Init.NominalPrescaler = 1;
        hfdcan1.Init.NominalSyncJumpWidth = 20;
        hfdcan1.Init.NominalTimeSeg1 = 79;
        hfdcan1.Init.NominalTimeSeg2 = 20;
        hfdcan1.Init.DataPrescaler = 1;
        hfdcan1.Init.DataSyncJumpWidth = 8;
        hfdcan1.Init.DataTimeSeg1 = 11;
        hfdcan1.Init.DataTimeSeg2 = 8;
        hfdcan1.Init.MessageRAMOffset = 0;
        hfdcan1.Init.StdFiltersNbr = 8;
        hfdcan1.Init.ExtFiltersNbr = 0;
        hfdcan1.Init.RxFifo0ElmtsNbr = 1;
        hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
        hfdcan1.Init.RxFifo1ElmtsNbr = 0;
        hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
        hfdcan1.Init.RxBuffersNbr = 1;
        hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
        hfdcan1.Init.TxEventsNbr = 0;
        hfdcan1.Init.TxBuffersNbr = 1;
        hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
        hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
        hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
        if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
        {
            embot::hw::can::Error_Handler("init CAN1");
        }
    }

    // actions:
    // 1. Activate the external driver out of STANDBY
    // can1: HAL_GPIO_WritePin(CAN1_STBY_GPIO_Port, CAN1_STBY_Pin, GPIO_PIN_RESET); 
    // can2: HAL_GPIO_WritePin(CAN2_STBY_GPIO_Port, CAN2_STBY_Pin, GPIO_PIN_RESET);    
    // 2. Activate both driver out of SHUTDOWN mode 
    // can1-can2: HAL_GPIO_WritePin(CAN_SHDN_GPIO_Port, CAN_SHDN_Pin, GPIO_PIN_RESET);
    // HAL_Delay(10); 
    void hwdriver_init(embot::hw::CAN h)
    {
        constexpr std::array<embot::hw::GPIO, 2> candrivergpiosstandby = { {
            {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::nine},         // CAN1_STBY_GPIO_Port, CAN1_STBY_Pin
            {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none}       // CAN2_STBY_GPIO_Port, CAN2_STBY_Pin
        } }; 

        constexpr embot::hw::GPIO candrivergpioshutdown = 
            {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::eight};     // CAN_SHDN_GPIO_Port, CAN_SHDN_Pin, GPIO_PIN_RESET    
        
        constexpr embot::hw::gpio::Config cfg {
            embot::hw::gpio::Mode::OUTPUTpushpull, 
            embot::hw::gpio::Pull::nopull, 
            embot::hw::gpio::Speed::medium };
        
        //static bool initted {false};
        
        // init the pins
        embot::hw::gpio::init(candrivergpiosstandby[embot::core::tointegral(h)], cfg);
        embot::hw::gpio::init(candrivergpioshutdown, cfg);
        
        // move them to reset state
        embot::hw::gpio::set(candrivergpiosstandby[embot::core::tointegral(h)], embot::hw::gpio::State::RESET);
        embot::hw::gpio::set(candrivergpioshutdown, embot::hw::gpio::State::RESET);
        HAL_Delay(10);
    }
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            s_FDCAN1_Init();
        }
        
        hwdriver_init(h);
    }
        
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::can {
    

#if defined(HAL_FDCAN_MODULE_ENABLED)

extern "C"
{
    // the irq handlers
    
    void FDCAN1_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&embot::hw::can::hfdcan1);
    } 
            
    void FDCAN1_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&embot::hw::can::hfdcan1);
    }          


    // the msp init / deinit

    constexpr uint16_t vCAN1_TXD_Pin {GPIO_PIN_1};
    GPIO_TypeDef *vCAN1_TXD_GPIO_Port {GPIOD};  
    
//    constexpr uint16_t vCAN2_TXD_Pin {GPIO_PIN_6};
//    GPIO_TypeDef *vCAN2_TXD_GPIO_Port {GPIOB};
    
    constexpr uint16_t vCAN1_RXD_Pin {GPIO_PIN_0};
    GPIO_TypeDef *vCAN1_RXD_GPIO_Port {GPIOD};
    
//    constexpr uint16_t vCAN2_RXD_Pin {GPIO_PIN_12};
//    GPIO_TypeDef *vCAN2_RXD_GPIO_Port {GPIOB};
    
    static uint32_t HAL_RCC_FDCAN_CLK_ENABLED=0;

    void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
    {
        
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      embot::hw::can::Error_Handler("mspinit 1");
    }

    /* FDCAN1 clock enable */
    HAL_RCC_FDCAN_CLK_ENABLED++;
    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
      __HAL_RCC_FDCAN_CLK_ENABLE();
    }

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PD1     ------> FDCAN1_TX
    PD0     ------> FDCAN1_RX
    */
    GPIO_InitStruct.Pin = vCAN1_TXD_Pin|vCAN1_RXD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
    HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN_CAL_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
//  else if(fdcanHandle->Instance==FDCAN2)
//  {
//  /* USER CODE BEGIN FDCAN2_MspInit 0 */

//  /* USER CODE END FDCAN2_MspInit 0 */

//  /** Initializes the peripherals clock
//  */
//    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
//    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
//    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//    {
//      embot::hw::can::Error_Handler("mspinit 1");
//    }

//    /* FDCAN2 clock enable */
//    HAL_RCC_FDCAN_CLK_ENABLED++;
//    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
//      __HAL_RCC_FDCAN_CLK_ENABLE();
//    }

//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    /**FDCAN2 GPIO Configuration
//    PB6     ------> FDCAN2_TX
//    PB12     ------> FDCAN2_RX
//    */
//    GPIO_InitStruct.Pin = vCAN2_TXD_Pin|vCAN2_RXD_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    /* FDCAN2 interrupt Init */
//    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
//    HAL_NVIC_SetPriority(FDCAN2_IT1_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
//    HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(FDCAN_CAL_IRQn);
//  /* USER CODE BEGIN FDCAN2_MspInit 1 */

//  /* USER CODE END FDCAN2_MspInit 1 */
//  }        

    }

    void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
    {
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_FDCAN_CLK_ENABLED--;
    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
      __HAL_RCC_FDCAN_CLK_DISABLE();
    }

    /**FDCAN1 GPIO Configuration
    PD1     ------> FDCAN1_TX
    PD0     ------> FDCAN1_RX
    */
    HAL_GPIO_DeInit(GPIOD, vCAN1_TXD_Pin|vCAN1_RXD_Pin);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1:FDCAN_CAL_IRQn disable */
    /**
    * Uncomment the line below to disable the "FDCAN_CAL_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(FDCAN_CAL_IRQn); */
  /* USER CODE END FDCAN1:FDCAN_CAL_IRQn disable */

  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
//  else if(fdcanHandle->Instance==FDCAN2)
//  {
//  /* USER CODE BEGIN FDCAN2_MspDeInit 0 */

//  /* USER CODE END FDCAN2_MspDeInit 0 */
//    /* Peripheral clock disable */
//    HAL_RCC_FDCAN_CLK_ENABLED--;
//    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
//      __HAL_RCC_FDCAN_CLK_DISABLE();
//    }

//    /**FDCAN2 GPIO Configuration
//    PB6     ------> FDCAN2_TX
//    PB12     ------> FDCAN2_RX
//    */
//    HAL_GPIO_DeInit(GPIOB, vCAN2_TXD_Pin|vCAN2_RXD_Pin);

//    /* FDCAN2 interrupt Deinit */
//    HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
//    HAL_NVIC_DisableIRQ(FDCAN2_IT1_IRQn);
//  /* USER CODE BEGIN FDCAN2:FDCAN_CAL_IRQn disable */
//    /**
//    * Uncomment the line below to disable the "FDCAN_CAL_IRQn" interrupt
//    * Be aware, disabling shared interrupt may affect other IPs
//    */
//    /* HAL_NVIC_DisableIRQ(FDCAN_CAL_IRQn); */
//  /* USER CODE END FDCAN2:FDCAN_CAL_IRQn disable */

//  /* USER CODE BEGIN FDCAN2_MspDeInit 1 */

//  /* USER CODE END FDCAN2_MspDeInit 1 */
//  }
  
  
    }    
    
}


#endif // defined(HAL_FDCAN_MODULE_ENABLED)
        
#endif // defined(EMBOT_ENABLE_hw_can)


// - support map: end of embot::hw::can



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

