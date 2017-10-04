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

#include "embot_hw_bno055.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;

#include "embot_binary.h"
#include "embot_hw_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_BNO055_ENABLED

#if     !defined(EMBOT_BNO055_ENABLED)


namespace embot { namespace hw { namespace BNO055 {

    bool supported(Sensor s)                                                                        { return false; }
    bool initialised(Sensor s)                                                                      { return false; }
    result_t init(Sensor s, const Config &config)                                                   { return resNOK; }
    
    result_t get(Sensor s, Temperature &temp)                                                       { return resNOK; }

}}} // namespace embot { namespace hw { namespace BNO055 {


#elif   defined(EMBOT_BNO055_ENABLED)


namespace embot { namespace hw { namespace BNO055 {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)        
    static const bspmap_t bspmap = 
    {
        0x00000001  // means... 1 sensors
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

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        Triple value;
        std::uint8_t rxdata[24];
        embot::common::Callback userdefCBK;  
        void clear() { done = false; ongoing = false; value.reset(); std::memset(rxdata, 0, sizeof(rxdata)); userdefCBK.callback = nullptr; userdefCBK.arg = nullptr; }         
    };
    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(Sensor::maxnumberof)];        
        Acquisition acquisition[static_cast<unsigned int>(Sensor::maxnumberof)];
        PrivateData() { }
    };
    
    
    static const std::uint8_t i2caddress = 0x52;
    static const std::uint8_t registerCHIP_ID = 0x00;
    static const std::uint8_t registerSTARTDATA = 0x08;
    
    static PrivateData s_privatedata;
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);        
        
        //std::uint16_t value = (acq->rxdata[0]<<8) + acq->rxdata[1];
               
        //std::int32_t res = ( (17572 * static_cast<std::int32_t>(value) ) >> 16) - 4685;
        //res /= 10;
        
        //acq->temp = static_cast<std::int16_t>(res);
        acq->ongoing = false;
        acq->done = true;
        
        if(nullptr != acq->userdefCBK.callback)
        {
            acq->userdefCBK.callback(acq->userdefCBK.arg);
        }
    }
              
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
        
        HAL_GPIO_WritePin(BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin, GPIO_PIN_SET);
        
        embot::hw::sys::delay(1000*1000);
        
        std::uint8_t index = sensor2index(s);
        
                
        // init i2c ..
        embot::hw::i2c::init(config.i2cbus, config.i2cconfig);
        
        if(false == embot::hw::i2c::ping(config.i2cbus, i2caddress))
        {
            return resNOK;
        }
        
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::binary::bit::set(initialisedmask, sensor2index(s));
                
        return resOK;
    }

    bool isbusbusy(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);  
        return embot::hw::i2c::isbusy(s_privatedata.config[index].i2cbus);             
    }
    
    bool isacquiring(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    result_t acquisition(Sensor s, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cbus))
        {
            return resNOK;
        }
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
        
        // ok, now i trigger i2c.
        embot::common::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::common::Data data = embot::common::Data(&s_privatedata.acquisition[index].rxdata[0], 18);
        embot::hw::i2c::read(s_privatedata.config[index].i2cbus, i2caddress, registerSTARTDATA, data, cbk);
        
        
        return resOK;
    }
    
    bool isalive(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cbus))
        {
            return false;
        }

        return embot::hw::i2c::ping(s_privatedata.config[index].i2cbus, i2caddress);        
    }

    
    bool isready(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);

        return s_privatedata.acquisition[index].done;        
    }
    
    
    result_t read(Sensor s, std::uint8_t &value)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == isready(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = sensor2index(s);
        value = s_privatedata.acquisition[index].rxdata[0];
  
        return resOK;        
    }
    

    result_t read(Sensor s, embot::common::Data &data)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == isready(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = sensor2index(s);
        data.pointer = &s_privatedata.acquisition[index].rxdata[0];
        data.size = 1;
        
        return resOK;        
    }    
    

    result_t read(Sensor s, Triple &triple)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == isready(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = sensor2index(s);

        
        return resOK;        
    } 
    
    
 
}}} // namespace embot { namespace hw { namespace BNO055 {



#endif //defined(EMBOT_BNO055_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

