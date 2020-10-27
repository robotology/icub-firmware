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

#include "embot_hw_si705x.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_bsp_config.h"
#include "embot_hw_pga308_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif

using namespace embot::hw;


extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_i2c1_rx;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#define TEST_HAL_DMA

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_SI705X_ENABLED

#if     !defined(EMBOT_SI705X_ENABLED)


namespace embot { namespace hw { namespace SI705X {

    bool supported(Sensor s)                                                                        { return false; }
    bool initialised(Sensor s)                                                                      { return false; }
    result_t init(Sensor s, const Config &config)                                                   { return resNOK; }
    
    result_t get(Sensor s, Temperature &temp)                                                       { return resNOK; }

}}} // namespace embot { namespace hw { namespace SI705X {


#elif   defined(EMBOT_SI705X_ENABLED)


namespace embot { namespace hw { namespace SI705X {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)        
    static const bspmap_t bspmap = 
    {
        0x00000003  // means... 2 sensors
    };   
    #else
    static const bspmap_t bspmap = 
    {
        0x00000000
    };
    #endif
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t sensor2index(Sensor s)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(s);
    }
        
    bool supported(Sensor s)
    {
        if((Sensor::none == s) || (Sensor::maxnumberof == s))
        {
            return false;
        }
        return embot::binary::bit::check(bspmap.mask, sensor2index(s));
    }
    
    bool initialised(Sensor s)
    {
        if(Sensor::none == s)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, sensor2index(s));
    }    

    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(Sensor::maxnumberof)];        

        PrivateData() { }
    };
    
    static PrivateData s_privatedata;
    
    
#if defined(TEST_HAL_DMA)
    
        std::uint8_t txdata = 0xE3;
        std::uint8_t rxdata[2] = {0};
        std::uint16_t tempcode = 0;
        
        static volatile bool done =  false;
        
#endif
    
   
    result_t init(Sensor s, const Config &config)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        s_privatedata.config[sensor2index(s)] = config;
                        
        
        // init i2c ..
        //embot::hw::i2c::init(config.i2cbus, config.i2cconfig);
        
        //bool present = embot::hw::i2c::polling(config.i2cbus, 0x80, 3, embot::common::1millisecond);
        
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, 0x80, 3, 2);
        // must be HAL_OK (0x00)
        status = status;
        
        

        embot::binary::bit::set(initialisedmask, sensor2index(s));
                
        return resOK;
    }
    
    
    
    result_t get(Sensor s, Temperature &temp)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);     
        result_t r = resOK;
         
        std::uint16_t value = 0;
        // must read via i2c in blocking way
        //embot::hw::i2c::read(s_privatedata.config[index].i2cbus, 0x80, &value, 2);
        
 
        
        // in polling
#if !defined(TEST_HAL_DMA)               
        std::uint32_t tout = 5;
        HAL_I2C_Master_Transmit(&hi2c1, 0x80, &txdata, 1, tout);        
        HAL_I2C_Master_Receive(&hi2c1, 0x80, &rxdata[0], 2, tout);
#else
        
        embot::hw::SI705X::done = false;
        HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)0x80, &txdata, 1);
        
        for(;;)
        {
            if(true == embot::hw::SI705X::done)
            {
                break;
            }            
        }
        
        embot::hw::SI705X::tempcode = (embot::hw::SI705X::rxdata[0]<<8) + embot::hw::SI705X::rxdata[1];
#endif        
        
               
        std::int32_t res = ( (17572 * static_cast<std::int32_t>(embot::hw::SI705X::tempcode) ) >> 16) - 4685;
        res /= 10;
        
        temp = static_cast<std::int16_t>(res);
        
        return r;        
    }

 
}}} // namespace embot { namespace hw { namespace SI705X {




#if defined(TEST_SI_ORIG)
    
    void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
    {
      if(I2cHandle->Instance==I2C1)
      {
        HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)0x80, &embot::hw::SI705X::rxdata[0], 2);
      }
    }

    void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
    {
      if(I2cHandle->Instance==I2C1){
          embot::hw::SI705X::done = true;
//          embot::hw::SI705X::tempcode = (embot::hw::SI705X::rxdata[0]<<8) + embot::hw::SI705X::rxdata[1];
//            Temperature_BOTboard = ((17572*Temperature_BOTcode)/65536) - 4685;  // HEX temperature in degrees Celsius (x100)
      }
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


#endif

#endif //defined(EMBOT_SI705X_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

