/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_i2c.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;

#include "embot_binary.h"
#include "embot_hw_sys.h"


extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c2_tx;
extern DMA_HandleTypeDef hdma_i2c2_rx;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if     !defined(HAL_I2C_MODULE_ENABLED)


namespace embot { namespace hw { namespace i2c {

    bool supported(Bus b)                                                                           { return false; }
    bool initialised(Bus b)                                                                         { return false; }
    result_t init(Bus b, const Config &config)                                                      { return resNOK; }
    
    bool ping(Bus b, std::uint8_t adr, std::uint8_t retries, embot::common::relTime timeout)        { return false; }       
    result_t read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, embot::common::Callback oncompletion) { return resNOK; }
    bool isbusy(Bus b)                                                                              { return false; }

}}} // namespace embot { namespace hw { namespace i2c {


#elif   defined(HAL_I2C_MODULE_ENABLED)


namespace embot { namespace hw { namespace i2c {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)        
    static const bspmap_t bspmap = 
    {
        0x00000003  // means... 2 buses
    };   
    #else
    static const bspmap_t bspmap = 
    {
        0x00000000
    };
    #endif
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t bus2index(Bus b)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(b);
    }
        
    bool supported(Bus b)
    {
        if((Bus::none == b) || (Bus::maxnumberof == b))
        {
            return false;
        }
        return embot::binary::bit::check(bspmap.mask, bus2index(b));
    }
    
    bool initialised(Bus b)
    {
        if(Bus::none == b)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, bus2index(b));
    }   


    struct stm32_i2c_mapping
    {
        I2C_HandleTypeDef* phandlei2cx;
    };  

#if     defined(STM32HAL_BOARD_STRAIN2)       
    static const stm32_i2c_mapping s_stm32_i2c_mapping[static_cast<unsigned int>(Bus::maxnumberof)] =
    {
        { &hi2c1 },
        { &hi2c2 }         
    };
#else
    static const stm32_i2c_mapping s_stm32_i2c_mapping[static_cast<unsigned int>(Bus::maxnumberof)] =
    {
        { nullptr },
        { nullptr }         
    };
#endif    
    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(Bus::maxnumberof)];          
        volatile bool busy[static_cast<unsigned int>(Bus::maxnumberof)];
        // transaction properties:
        embot::common::Callback oncompletion[static_cast<unsigned int>(Bus::maxnumberof)];  
        std::uint8_t adr[static_cast<unsigned int>(Bus::maxnumberof)];
        void * rxdata[static_cast<unsigned int>(Bus::maxnumberof)];
        std::uint8_t rxsize[static_cast<unsigned int>(Bus::maxnumberof)];
        PrivateData() { }
    };
    
    static PrivateData s_privatedata;
    
    

    
   
    result_t init(Bus b, const Config &config)
    {
        if(false == supported(b))
        {
            return resNOK;
        }
        
        if(true == initialised(b))
        {
            return resOK;
        }
        
        s_privatedata.config[bus2index(b)] = config;

#if     defined(STM32HAL_BOARD_STRAIN2)   
        if(b == Bus::one)
        {            
            MX_I2C1_Init();
        }
        else if(b == Bus::two)
        {
            MX_I2C2_Init();
        }                        
#endif
        
        embot::binary::bit::set(initialisedmask, bus2index(b));
                
        return resOK;
    }
    
    
    bool ping(Bus b, std::uint8_t adr, std::uint8_t retries, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return false;
        } 
                
        if(true == isbusy(b))
        {
            return false;
        }
        
        std::uint8_t index = bus2index(b);
        
        s_privatedata.busy[index] = true;
                
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(s_stm32_i2c_mapping[index].phandlei2cx, adr, retries, timeout/1000);
        
        s_privatedata.busy[index] = false;
        
        return (HAL_OK == status) ? true : false;       
    }

    
    result_t read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, embot::common::Callback oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
                
        if(true == isbusy(b))
        {
            return resNOK;
        }
        
        if(false == destination.isvalid())
        {
            return resNOK;
        }
        
        std::uint8_t index = bus2index(b);
        
        s_privatedata.adr[index] = adr;
        s_privatedata.oncompletion[index] = oncompletion;
        s_privatedata.busy[index] = true;
        s_privatedata.rxdata[index] = destination.pointer;
        s_privatedata.rxsize[index] = destination.size;
        
        HAL_I2C_Master_Transmit_DMA(s_stm32_i2c_mapping[index].phandlei2cx, adr, static_cast<std::uint8_t*>(&reg), 1);
                
        return resOK;
    }
 
    
    bool isbusy(Bus b)
    {
        if(false == initialised(b))
        {
            return false;
        } 

        return s_privatedata.busy[bus2index(b)];        
    }
    
     
}}} // namespace embot { namespace hw { namespace i2c {


    
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    if(I2cHandle->Instance==I2C1)
    {   
        std::uint8_t index = 0;
        HAL_I2C_Master_Receive_DMA(&hi2c1, embot::hw::i2c::s_privatedata.adr[index], static_cast<std::uint8_t*>(embot::hw::i2c::s_privatedata.rxdata[index]), embot::hw::i2c::s_privatedata.rxsize[index]);
    }
    if(I2cHandle->Instance==I2C2)
    {   
        std::uint8_t index = 1;
        HAL_I2C_Master_Receive_DMA(&hi2c2, embot::hw::i2c::s_privatedata.adr[index], static_cast<std::uint8_t*>(embot::hw::i2c::s_privatedata.rxdata[index]), embot::hw::i2c::s_privatedata.rxsize[index]);
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    std::uint8_t index = 0;
    
    if(I2cHandle->Instance == I2C1)
    {
       index = 0;
    }
    else if(I2cHandle->Instance == I2C2)
    {
       index = 1;
    }
        
    if(nullptr != embot::hw::i2c::s_privatedata.oncompletion[index].callback)
    {
        embot::hw::i2c::s_privatedata.oncompletion[index].callback(embot::hw::i2c::s_privatedata.oncompletion[index].arg);
    }
    embot::hw::i2c::s_privatedata.busy[index] = false;

}    
    
    
    
    /**
* @brief This function handles I2C1 event interrupt.
*/
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
* @brief This function handles I2C1 error interrupt.
*/
void I2C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}


/**
* @brief This function handles DMA1 channel6 global interrupt.
*/
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel7 global interrupt.
*/
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}



/**
* @brief This function handles DMA1 channel4 global interrupt.
*/
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c2_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel5 global interrupt.
*/
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c2_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
* @brief This function handles I2C2 event interrupt.
*/
void I2C2_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_EV_IRQn 0 */

  /* USER CODE END I2C2_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_EV_IRQn 1 */

  /* USER CODE END I2C2_EV_IRQn 1 */
}

/**
* @brief This function handles I2C2 error interrupt.
*/
void I2C2_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_ER_IRQn 0 */

  /* USER CODE END I2C2_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_ER_IRQn 1 */

  /* USER CODE END I2C2_ER_IRQn 1 */
}


#endif //defined(HAL_I2C_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

