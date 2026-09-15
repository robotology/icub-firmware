
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
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


// - support map: begin of embot::hw::i2c

#include "embot_hw_i2c.h"
#include "embot_hw_i2c_bsp.h"
#include "embot_hw_gpio_bsp_amcmj1.h"

#if !defined(EMBOT_ENABLE_hw_i2c)

namespace embot::hw::i2c {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::I2C h) const { ; }
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#elif defined(EMBOT_ENABLE_hw_i2c)

namespace embot::hw::i2c {
    
#if 0
    i2c1 is connected to the IMU mounted on the board
    i2c2 is wired to connector J6
        
#endif


#define EMBOT_ENABLE_hw_i2c_one
//#define EMBOT_ENABLE_hw_i2c_two
    

#if defined(EMBOT_ENABLE_hw_i2c_one)
    I2C_HandleTypeDef hi2c1;
    constexpr PROP i2c1p = { &hi2c1, embot::hw::i2c::Speed::standard100 }; 

#endif    
 
#if defined(EMBOT_ENABLE_hw_i2c_two)
    I2C_HandleTypeDef hi2c2;
    constexpr PROP i2c2p = { &hi2c2, embot::hw::i2c::Speed::standard100 }; 

#endif
 
    
    constexpr BSP thebsp {        
        // maskofsupported
        0 |
#if defined(EMBOT_ENABLE_hw_i2c_one)        
        mask::pos2mask<uint32_t>(I2C::one) |
#endif
#if defined(EMBOT_ENABLE_hw_i2c_two)
        mask::pos2mask<uint32_t>(I2C::two) | 
#endif
     
        0,        
        // properties
        {{
#if defined(EMBOT_ENABLE_hw_i2c_one)        
        &i2c1p,
#else
        nullptr,
#endif
#if defined(EMBOT_ENABLE_hw_i2c_two)
        &i2c2p,
#else
        nullptr,
#endif

        nullptr, nullptr
        }}        
    };
 

    void Error_Handler(const char *msg) {};
        
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
            Error_Handler("");
        }
        
        // TODO: japo. Check if we really need filters configuration or we can remove these lines
        /** Configure Analogue filter
        */
        if (HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
        {
            Error_Handler("");
        }
        /** Configure Digital filter
        */
        if (HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
        {
            Error_Handler("");
        }     
         
    }  
    
    void s_I2Cdeinit(embot::hw::I2C h)
    {
        embot::hw::i2c::I2C_Handle * hi2c = embot::hw::i2c::getBSP().getPROP(h)->handle;    
        
        HAL_I2C_DeInit(hi2c); 
                 
    }      
    
    
    void BSP::init(embot::hw::I2C h) const
    {        
        switch(h)
        {
            case I2C::one:
            case I2C::two:
            {

////                i2c_MX_DMA_Init();
                // and call I2C init              
                s_I2Cinit(h);                
            } break;
            case I2C::three:
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
            {
                s_I2Cdeinit(h);
            } break;
            
            case I2C::three:
            case I2C::four:
                
            default: {} break;                            
        }        
        
        return true;        
    }
    
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
} // namespace embot::hw::i2c::bsp {

extern "C"
{
    void Error_Handler(const char *msg)
    {
        const char *more {"generic"};
        const char *c = (nullptr != msg) ? msg : more;       
        for(;;)
        {            
            embot::core::print("Error_Handler() called for I2C:" + std::string(c));
            embot::core::wait(1000000);            
        }
    }  
    
    void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
    {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(i2cHandle->Instance==I2C1)
    {

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
        PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
        Error_Handler("");
        }

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**I2C1 GPIO Configuration
        PB7     ------> I2C1_SDA
        PB8     ------> I2C1_SCL
        */
        GPIO_InitStruct.Pin = IMU_SDA_Pin|IMU_SCL_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* I2C1 clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();

        /* I2C1 interrupt Init */
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

    }
    
    if(i2cHandle->Instance==I2C2)
    {

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
        PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
          Error_Handler("");
        }

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**I2C2 GPIO Configuration
        PF0     ------> I2C2_SDA
        PF1     ------> I2C2_SCL
        */
        GPIO_InitStruct.Pin = MAG1_SDA_Pin|MAG1_SCL_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* I2C2 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /* I2C2 interrupt Init */
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
        HAL_NVIC_SetPriority(I2C2_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);

    }
}    


    void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
    {
              
        if(i2cHandle->Instance==I2C1)
        {

            /* Peripheral clock disable */
            __HAL_RCC_I2C1_CLK_DISABLE();

            /**I2C1 GPIO Configuration
            PB7     ------> I2C1_SDA
            PB8     ------> I2C1_SCL
            */
            HAL_GPIO_DeInit(IMU_SDA_GPIO_Port, IMU_SDA_Pin);

            HAL_GPIO_DeInit(IMU_SCL_GPIO_Port, IMU_SCL_Pin);

            /* I2C1 interrupt Deinit */
            HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);

        }
        
        if(i2cHandle->Instance==I2C2)
        {
            /* Peripheral clock disable */
            __HAL_RCC_I2C2_CLK_DISABLE();

            /**I2C2 GPIO Configuration
            PF0     ------> I2C2_SDA
            PF1     ------> I2C2_SCL
            */
            HAL_GPIO_DeInit(MAG1_SDA_GPIO_Port, MAG1_SDA_Pin);

            HAL_GPIO_DeInit(MAG1_SCL_GPIO_Port, MAG1_SCL_Pin);

            /* I2C2 interrupt Deinit */
            HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);

        }

    }
    
}


#endif // #elif defined(EMBOT_ENABLE_hw_i2c)

// - support map: end of embot::hw::i2c




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

