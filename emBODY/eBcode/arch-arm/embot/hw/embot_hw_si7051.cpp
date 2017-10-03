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

#include "embot_hw_si7051.h"
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

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#define TEST_HAL_DMA

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_SI7051_ENABLED

#if     !defined(EMBOT_SI7051_ENABLED)


namespace embot { namespace hw { namespace SI7051 {

    bool supported(Sensor s)                                                                        { return false; }
    bool initialised(Sensor s)                                                                      { return false; }
    result_t init(Sensor s, const Config &config)                                                   { return resNOK; }
    
    result_t get(Sensor s, Temperature &temp)                                                       { return resNOK; }

}}} // namespace embot { namespace hw { namespace SI7051 {


#elif   defined(EMBOT_SI7051_ENABLED)


namespace embot { namespace hw { namespace SI7051 {
        
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
        embot::hw::i2c::init(config.i2cbus, config.i2cconfig);
        
        bool present = embot::hw::i2c::ping(config.i2cbus, 0x80);
        
        present = present;
            

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
        std::uint8_t rxdata[2] = {0};
        // must read via i2c in blocking way
        embot::common::Callback cbk;
        embot::hw::i2c::read(s_privatedata.config[index].i2cbus, 0x80, 0xE3, rxdata, 2, cbk);
        
        for(;;)
        {
            if(false == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cbus))
            {
                break;
            }            
        }
        
// 
//        
//        // in polling
//#if !defined(TEST_HAL_DMA)               
//        std::uint32_t tout = 5;
//        HAL_I2C_Master_Transmit(&hi2c1, 0x80, &txdata, 1, tout);        
//        HAL_I2C_Master_Receive(&hi2c1, 0x80, &rxdata[0], 2, tout);
//#else
//        
//        embot::hw::SI7051::done = false;
//        HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)0x80, &txdata, 1);
//        
//        for(;;)
//        {
//            if(true == embot::hw::SI7051::done)
//            {
//                break;
//            }            
//        }
//        
//        embot::hw::SI7051::tempcode = (embot::hw::SI7051::rxdata[0]<<8) + embot::hw::SI7051::rxdata[1];
//#endif        
        
        value = (rxdata[0]<<8) + rxdata[1];
               
        std::int32_t res = ( (17572 * static_cast<std::int32_t>(value) ) >> 16) - 4685;
        res /= 10;
        
        temp = static_cast<std::int16_t>(res);
        
        return r;        
    }

 
}}} // namespace embot { namespace hw { namespace SI7051 {



#endif //defined(EMBOT_SI7051_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

