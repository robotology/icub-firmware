/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:    Marco Accame
 * email:     marco.accame@iit.it
 * Coauthor:  Kevin Sangalli, Jacopo  Losi
 * email:     kevin.sangalli@iit.it, jacopo.losi@iit.it
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

#include "embot_hw_bsp_amc_config.h"


#include "embot_hw_i2c.h"
#include "embot_hw_i2c_bsp.h"

#if !defined(EMBOT_ENABLE_hw_i2c)

namespace embot::hw::i2c 
{
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::I2C h) const { ;}
    const BSP& getBSP() 
    {
        return thebsp;
    }
}

#else

namespace embot::hw::i2c {

    #if defined(STM32HAL_BOARD_AMC)
    
    DMA_HandleTypeDef hdma_i2c1_tx;
    DMA_HandleTypeDef hdma_i2c1_rx;
    DMA_HandleTypeDef hdma_i2c2_rx;
    DMA_HandleTypeDef hdma_i2c2_tx;
    DMA_HandleTypeDef hdma_i2c3_rx;
    DMA_HandleTypeDef hdma_i2c3_tx;  
    
    I2C_HandleTypeDef hi2c1;
    constexpr PROP i2c1p = { &hi2c1, embot::hw::i2c::Speed::standard100 }; 
    
    I2C_HandleTypeDef hi2c2;
    constexpr PROP i2c2p = { &hi2c2, embot::hw::i2c::Speed::standard100 }; 
     
    I2C_HandleTypeDef hi2c3;
    constexpr PROP i2c3p = { &hi2c3, embot::hw::i2c::Speed::standard100 };     
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two) | mask::pos2mask<uint32_t>(I2C::three),
        // properties
        {{
            &i2c1p, &i2c2p, &i2c3p, nullptr            
        }}        
    };
    
    
    void s_J5_I2Cpinout(embot::hw::I2C h, bool enable)
    {
        static constexpr embot::hw::gpio::Config out { embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium };    
        static constexpr embot::hw::gpio::State stateI2C[2] = {embot::hw::gpio::State::SET, embot::hw::gpio::State::RESET};
        static constexpr embot::hw::gpio::State stateNONE[2] = {embot::hw::gpio::State::RESET, embot::hw::gpio::State::RESET};
    
        static constexpr size_t i2cnum {embot::core::tointegral(embot::hw::I2C::three)+1};
        static constexpr embot::hw::GPIO X1ENi2c[i2cnum][2] = 
        {
            {   // i2c1
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::zero},   // i2c1 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eight}   // spi1 ENABLE
            },
            {   // i2c2
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::one},   // i2c2 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::nine}   // spi2 ENABLE
            },
            {   // i2c3
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::two},   // i2c3 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::ten}    // spi3 ENABLE 
            }
        };
        
        uint8_t x = embot::core::tointegral(h);
        if(x <= i2cnum)
        {
            // i2cx
            for(uint8_t i=0; i<2; i++)
            {
                embot::hw::gpio::init(X1ENi2c[x][i], out);
                embot::hw::gpio::set(X1ENi2c[x][i], enable ? stateI2C[i] : stateNONE[i]);
            }  
        }
    }  
    
    // we need this extconfig because we want to pass information to HAL_I2C_MspInit() and HAL_I2C_MspDeInit()
//    utils::ExtendedConfig extconfig {};
        
    void s_I2Cinit(embot::hw::I2C h)
    {   
        // I2C_specific handle --> this is i2c* handle 
        embot::hw::i2c::I2C_Handle * hi2c = embot::hw::i2c::getBSP().getPROP(h)->handle;
        switch(h)
        {
            case I2C::one:
            {
                hi2c->Instance = I2C1;
            } break;
            
            case I2C::two:
            {
                hi2c->Instance = I2C2;
            } break;
            case I2C::three:
            {
                hi2c->Instance = I2C3;
            } break;
            case I2C::four:
            {
                // nothing to do
            } break;
            
            default: {} break;    
        }
        
        // prepare and then call HAL_I2C_Init()
        // These specs are equal for all hi2c, thus we do not need to add them to the switch case
        hi2c->Init.Timing = 0x10C0ECFF;
        hi2c->Init.OwnAddress1 = 0;
        hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        hi2c->Init.OwnAddress2 = 0;
        hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
        hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
        if (HAL_I2C_Init(hi2c) != HAL_OK)
        {
            Error_Handler();
        }
        
        // TODO: japo. Check if we really need filters configuration or we can remove these lines
        /** Configure Analogue filter
        */
        if (HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
        {
            Error_Handler();
        }
        /** Configure Digital filter
        */
        if (HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
        {
            Error_Handler();
        }     
        
        // clear extconfig
//        extconfig.clear(); 
    }  
    

    void s_I2Cdeinit(embot::hw::I2C h)
    {
        embot::hw::i2c::I2C_Handle * hi2c = embot::hw::i2c::getBSP().getPROP(h)->handle;    
        
        HAL_I2C_DeInit(hi2c); 
        
//        // clear extconfig
//        extconfig.clear();            
    }  

    void i2c_MX_DMA_Init(void)
    {

      static bool called {false};
      
      if(true == called)
      {
          return;
      }

      called = true;
      
      /* DMA controller clock enable */
      __HAL_RCC_DMA2_CLK_ENABLE();
      __HAL_RCC_DMA1_CLK_ENABLE();

      /* DMA interrupt init */
      /* DMA1_Stream0_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
      /* DMA1_Stream1_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
      /* DMA1_Stream2_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
      /* DMA1_Stream3_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
      /* DMA1_Stream5_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
      /* DMA1_Stream6_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
      /* DMA2_Stream0_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    }    
    
    
    void BSP::init(embot::hw::I2C h) const
    {        
        switch(h)
        {
            case I2C::one:
            case I2C::two:
            case I2C::three:
            {
                // we are on J5: enable the port x1
                s_J5_I2Cpinout(h, true); 
                i2c_MX_DMA_Init();
                // and call I2C init              
                s_I2Cinit(h);                
            } break;
            
            case I2C::four:
            {
                // nothing to do
            } break;
            
            default: {} break;                            
        }
    }

    bool BSP::deinit(embot::hw::I2C h) const
    {       
        switch(h)
        {
            case I2C::one:
            case I2C::two:
            case I2C::three:
            {
                // call I2C deinit              
                s_I2Cdeinit(h);  
                // we are on J5: disable port x1
                s_J5_I2Cpinout(h, false);                
            } break;
            
            case I2C::four:
            {
                // nothing to do
            } break;
            
            default: {} break;                            
        }        
        
        return true;        
    }
    
    #else
        #error embot::hw::bsp::i2c::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}


extern "C"
{
    
// I2C INIT Define
#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */
#define I2C_STATE_NONE            ((uint32_t)(HAL_I2C_MODE_NONE)) 

// SDA-SCL I2C1 PINs
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_XEN_Pin GPIO_PIN_0
#define I2C1_XEN_GPIO_Port GPIOG

// SDA-SCL I2C2 PINs
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SDA_GPIO_Port GPIOF
#define I2C2_SCL_Pin GPIO_PIN_1
#define I2C2_SCL_GPIO_Port GPIOF
#define I2C2_XEN_Pin GPIO_PIN_1
#define I2C2_XEN_GPIO_Port GPIOG

// SDA-SCL I2C3 PINs
#define I2C3_SDA_Pin GPIO_PIN_8
#define I2C3_SDA_GPIO_Port GPIOH
#define I2C3_SCL_Pin GPIO_PIN_7
#define I2C3_SCL_GPIO_Port GPIOH
#define I2C3_XEN_Pin GPIO_PIN_2
#define I2C3_XEN_GPIO_Port GPIOG    
    

    
    void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
        if(hi2c->Instance==I2C1)
        {
            /* USER CODE BEGIN I2C1_MspInit 0 */

            /* USER CODE END I2C1_MspInit 0 */
            /** Initializes the peripherals clock
            */
            PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
            PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
            if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_RCC_GPIOB_CLK_ENABLE();
            /**I2C1 GPIO Configuration
            PB8     ------> I2C1_SCL
            PB7     ------> I2C1_SDA
            */
            GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            /* Peripheral clock enable */
            __HAL_RCC_I2C1_CLK_ENABLE();

            /* I2C1 DMA Init */
            /* I2C1_TX Init */
            embot::hw::i2c::hdma_i2c1_tx.Instance = DMA1_Stream0;
            embot::hw::i2c::hdma_i2c1_tx.Init.Request = DMA_REQUEST_I2C1_TX;
            embot::hw::i2c::hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
            embot::hw::i2c::hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
            embot::hw::i2c::hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
            embot::hw::i2c::hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
            embot::hw::i2c::hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
            embot::hw::i2c::hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c1_tx) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_LINKDMA(hi2c,hdmatx,embot::hw::i2c::hdma_i2c1_tx);

            /* I2C1_RX Init */
            embot::hw::i2c::hdma_i2c1_rx.Instance = DMA1_Stream1;
            embot::hw::i2c::hdma_i2c1_rx.Init.Request = DMA_REQUEST_I2C1_RX;
            embot::hw::i2c::hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
            embot::hw::i2c::hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
            embot::hw::i2c::hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
            embot::hw::i2c::hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
            embot::hw::i2c::hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
            embot::hw::i2c::hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c1_rx) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_LINKDMA(hi2c,hdmarx,embot::hw::i2c::hdma_i2c1_rx);
            
            /* I2C1 interrupt Init */
            HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
            /* USER CODE BEGIN I2C1_MspInit 1 */

            /* USER CODE END I2C1_MspInit 1 */
        }
        else if(hi2c->Instance==I2C2)
        {
        /* USER CODE BEGIN I2C2_MspInit 0 */

        /* USER CODE END I2C2_MspInit 0 */

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
        PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**I2C2 GPIO Configuration
        PF1     ------> I2C2_SCL
        PF0     ------> I2C2_SDA
        */
        GPIO_InitStruct.Pin = I2C2_SCL_Pin|I2C2_SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /* I2C2 DMA Init */
        /* I2C2_RX Init */
        embot::hw::i2c::hdma_i2c2_rx.Instance = DMA1_Stream2;
        embot::hw::i2c::hdma_i2c2_rx.Init.Request = DMA_REQUEST_I2C2_RX;
        embot::hw::i2c::hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        embot::hw::i2c::hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c2_rx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmarx,embot::hw::i2c::hdma_i2c2_rx);
        
        /* I2C2_TX Init */
        embot::hw::i2c::hdma_i2c2_tx.Instance = DMA1_Stream3;
        embot::hw::i2c::hdma_i2c2_tx.Init.Request = DMA_REQUEST_I2C2_TX;
        embot::hw::i2c::hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        embot::hw::i2c::hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c2_tx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmatx,embot::hw::i2c::hdma_i2c2_tx);

        
        
        /* I2C2 interrupt Init */
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
        HAL_NVIC_SetPriority(I2C2_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
        /* USER CODE BEGIN I2C2_MspInit 1 */

        /* USER CODE END I2C2_MspInit 1 */
        }
        else if(hi2c->Instance==I2C3)
        {
        /* USER CODE BEGIN I2C3_MspInit 0 */

        /* USER CODE END I2C3_MspInit 0 */

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
        PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_GPIOH_CLK_ENABLE();
        /**I2C3 GPIO Configuration
        PH7     ------> I2C3_SCL
        PH8     ------> I2C3_SDA
        */
        GPIO_InitStruct.Pin = I2C3_SCL_Pin|I2C3_SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C3_CLK_ENABLE();
        

        /* I2C3 DMA Init */
        /* I2C3_RX Init */
        embot::hw::i2c::hdma_i2c3_rx.Instance = DMA1_Stream5;
        embot::hw::i2c::hdma_i2c3_rx.Init.Request = DMA_REQUEST_I2C3_RX;
        embot::hw::i2c::hdma_i2c3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        embot::hw::i2c::hdma_i2c3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c3_rx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_rx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c3_rx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c3_rx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmarx,embot::hw::i2c::hdma_i2c3_rx);
        
        /* I2C3_TX Init */
        embot::hw::i2c::hdma_i2c3_tx.Instance = DMA1_Stream6;
        embot::hw::i2c::hdma_i2c3_tx.Init.Request = DMA_REQUEST_I2C3_TX;
        embot::hw::i2c::hdma_i2c3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        embot::hw::i2c::hdma_i2c3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c3_tx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_tx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c3_tx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c3_tx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmatx,embot::hw::i2c::hdma_i2c3_tx);
        
        
        /* I2C3 interrupt Init */
        HAL_NVIC_SetPriority(I2C3_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_SetPriority(I2C3_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
        /* USER CODE BEGIN I2C3_MspInit 1 */

        /* USER CODE END I2C3_MspInit 1 */
        }

    }
    


    void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
    {

        if(hi2c->Instance==I2C1)
        {
            /* USER CODE BEGIN I2C1_MspDeInit 0 */

            /* USER CODE END I2C1_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_I2C1_CLK_DISABLE();

            /**I2C1 GPIO Configuration
            PB8     ------> I2C1_SCL
            PB7     ------> I2C1_SDA
            */
            HAL_GPIO_DeInit(I2C1_SCL_GPIO_Port, I2C1_SCL_Pin);

            HAL_GPIO_DeInit(I2C1_SDA_GPIO_Port, I2C1_SDA_Pin);

            /* I2C1 DMA DeInit */
            HAL_DMA_DeInit(hi2c->hdmatx);
            HAL_DMA_DeInit(hi2c->hdmarx);
            
            /* I2C1 interrupt DeInit */
            HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
            /* USER CODE BEGIN I2C1_MspDeInit 1 */

            /* USER CODE END I2C1_MspDeInit 1 */
        }
        else if(hi2c->Instance==I2C2)
        {
            /* USER CODE BEGIN I2C2_MspDeInit 0 */

            /* USER CODE END I2C2_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_I2C2_CLK_DISABLE();

            /**I2C2 GPIO Configuration
            PF1     ------> I2C2_SCL
            PF0     ------> I2C2_SDA
            */
            HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);

            HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);

            /* I2C2 DMA DeInit */
            HAL_DMA_DeInit(hi2c->hdmarx);
            HAL_DMA_DeInit(hi2c->hdmatx);
            
            /* I2C2 interrupt DeInit */
            HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
            /* USER CODE BEGIN I2C2_MspDeInit 1 */

            /* USER CODE END I2C2_MspDeInit 1 */
        }
        else if(hi2c->Instance==I2C3)
        {
            /* USER CODE BEGIN I2C3_MspDeInit 0 */

            /* USER CODE END I2C3_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_I2C3_CLK_DISABLE();

            /**I2C3 GPIO Configuration
            PH7     ------> I2C3_SCL
            PH8     ------> I2C3_SDA
            */
            HAL_GPIO_DeInit(I2C3_SCL_GPIO_Port, I2C3_SCL_Pin);

            HAL_GPIO_DeInit(I2C3_SDA_GPIO_Port, I2C3_SDA_Pin);

            /* I2C3 DMA DeInit */
            HAL_DMA_DeInit(hi2c->hdmarx);
            HAL_DMA_DeInit(hi2c->hdmatx);
            
            /* I2C3 interrupt DeInit */
            HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
            /* USER CODE BEGIN I2C3_MspDeInit 1 */

            /* USER CODE END I2C3_MspDeInit 1 */
        }
    }
    
}

extern "C"
{
    // Event interrupt
    void I2C1_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&embot::hw::i2c::hi2c1);
    }

    void I2C2_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&embot::hw::i2c::hi2c2);
    }
 
    void I2C3_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&embot::hw::i2c::hi2c3);
    }
    
    // Error interrupt
    void I2C1_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&embot::hw::i2c::hi2c1);
    }

    void I2C2_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&embot::hw::i2c::hi2c2);
    }
 
    void I2C3_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&embot::hw::i2c::hi2c3);
    }
    
    
    void DMA1_Stream0_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c1_tx);
    }


    void DMA1_Stream1_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c1_rx);
    }    
    

    void DMA1_Stream2_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c2_rx);
    }

    void DMA1_Stream3_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c2_tx);
    }

    void DMA1_Stream5_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c3_rx);
    }

    void DMA1_Stream6_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c3_tx);
    }    
}

#endif // i2c


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


