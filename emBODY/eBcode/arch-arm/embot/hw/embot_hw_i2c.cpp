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
    
    struct Transaction
    {
        volatile bool ongoing;
        std::uint8_t adr;
        embot::common::Data recdata;
        std::uint8_t txbuffer[4];
        std::uint8_t txbuffersize;
        embot::common::Callback oncompletion;
        void clear() { ongoing = false; adr = 0; recdata.clear(); oncompletion.clear(); std::memset(txbuffer, 0, sizeof(txbuffer)); txbuffersize = 0;}         
    };
    
    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(Bus::maxnumberof)];  
        Transaction  transaction[static_cast<unsigned int>(Bus::maxnumberof)];      
        PrivateData() { }
    };
    
    static PrivateData s_privatedata;
    
    static result_t s_read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    static result_t s_write(Bus b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    static result_t s_wait(Bus b, embot::common::relTime timeout);
    

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
    
    
    bool ping(Bus b, std::uint8_t adr, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return false;
        } 
          
        embot::common::relTime remaining = timeout;
        if(true == isbusy(b, timeout, remaining))
        {
            return false;
        }
        
        std::uint8_t index = bus2index(b);
        
        s_privatedata.transaction[index].ongoing = true;
                
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(s_stm32_i2c_mapping[index].phandlei2cx, adr, 1, remaining/1000);
        
        s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == status) ? true : false;       
    }

    
    result_t read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
        
        if(false == destination.isvalid())
        {
            return resNOK;
        }     
                
        if(true == isbusy(b))
        {
            return resNOK;
        }
                           
        return s_read(b, adr, reg, destination, oncompletion);
    }
    
    result_t read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
             
        if(false == destination.isvalid())
        {
            return resNOK;
        }
        
        embot::common::relTime remaining = timeout;       
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOK;
        }
                        
        result_t r = s_read(b, adr, reg, destination);
        
        if(resOK == r)
        {
            r = s_wait(b, remaining);
        }        
        return r;                      
    }
    

    result_t write(Bus b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
        
        if(false == content.isvalid())
        {
            return resNOK;
        } 
        
        if(true == isbusy(b))
        {
            return resNOK;
        }
                        
        return s_write(b, adr, reg, content, oncompletion);
    }    


    result_t write(Bus b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if(false == content.isvalid())
        {
            return resNOK;
        }
        
        embot::common::relTime remaining = timeout;
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOK;
        }        
         
        result_t r = s_write(b, adr, reg, content);
        
        if(resOK == r)
        {
            r = s_wait(b, remaining);
        }
        
        return r;
    }

    
    bool isbusy(Bus b)
    {
        if(false == initialised(b))
        {
            return false;
        } 

        return s_privatedata.transaction[bus2index(b)].ongoing;     
    }

    
    bool isbusy(Bus b, embot::common::relTime timeout, embot::common::relTime &remaining)
    {
        if(false == initialised(b))
        {
            return false;
        } 

        if(0 == timeout)
        {
            remaining = timeout;
            return s_privatedata.transaction[bus2index(b)].ongoing;   
        }
       
        
        embot::common::Time deadline = embot::hw::sys::now() + timeout;
        
        bool res = true;
        for(;;)
        {
            std::int64_t rem = deadline - embot::hw::sys::now();
            
            if(rem <= 0)
            {   
                remaining = 0;
                res = true;
                break;
            }
            else if(false == s_privatedata.transaction[bus2index(b)].ongoing)
            {
                remaining = static_cast<embot::common::relTime>(rem);
                res = false;   
                break;                
            }
            
        }
        
        return res;        
    }    
    
    void ontransactionterminated(std::uint8_t index)
    {
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = false;        
        embot::hw::i2c::s_privatedata.transaction[index].oncompletion.execute();                                
    }
    
    
    static result_t s_wait(Bus b, embot::common::relTime timeout)
    {
        embot::common::Time start = embot::hw::sys::now();
        
        result_t res = resOK;
        for(;;)
        {
            if(false == isbusy(b))
            {
                break;
            }
            
            if(embot::hw::sys::now() > (start+timeout))
            {
                res = resNOK;
                break;
            }
        } 

        return res;
    }   
    
       
    
    static result_t s_write(Bus b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, const embot::common::Callback &oncompletion)
    {   
        std::uint8_t index = bus2index(b);        
        s_privatedata.transaction[index].adr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata.clear();
        s_privatedata.transaction[index].txbuffer[0] = reg;
        std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.size);
        s_privatedata.transaction[index].txbuffersize = content.size + 1;
        
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_stm32_i2c_mapping[index].phandlei2cx, adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
                
        return (HAL_OK == r) ? resOK : resNOK;
    }    
    
    static result_t s_read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, const embot::common::Callback &oncompletion)
    {
        std::uint8_t index = bus2index(b);        
        s_privatedata.transaction[index].adr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata = destination;  
        // register address is 1 byte long ...
        s_privatedata.transaction[index].txbuffer[0] = reg;
        s_privatedata.transaction[index].txbuffersize = 1;
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_stm32_i2c_mapping[index].phandlei2cx, adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
        
        return (HAL_OK == r) ? resOK : resNOK;
    }
     
}}} // namespace embot { namespace hw { namespace i2c {


    
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
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
    else
    {
        return;
    }
        
        
    if(false == embot::hw::i2c::s_privatedata.transaction[index].recdata.isvalid())
    {
        // it is a write operation: the transaction is terminated
        embot::hw::i2c::ontransactionterminated(index);        
    }
    else
    {
        // it is a read operation i still need to receive data 
        I2C_HandleTypeDef *hi2cx = (0 == index) ? (&hi2c1) : (&hi2c2);
        HAL_I2C_Master_Receive_DMA( hi2cx, 
                                    embot::hw::i2c::s_privatedata.transaction[index].adr, 
                                    static_cast<std::uint8_t*>(embot::hw::i2c::s_privatedata.transaction[index].recdata.pointer),  
                                    static_cast<std::uint16_t>(embot::hw::i2c::s_privatedata.transaction[index].recdata.size)
                                    );
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
    else
    {
        return;
    }
    
    embot::hw::i2c::ontransactionterminated(index);        
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

