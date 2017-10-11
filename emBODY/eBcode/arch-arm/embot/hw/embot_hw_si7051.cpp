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



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


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

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        Temperature temp;
        std::uint8_t rxdata[2];
        embot::common::Callback userdefCBK;  
        void clear() { done = false; ongoing = false; temp = 0; rxdata[0] = rxdata[1] = 0; userdefCBK.callback = nullptr; userdefCBK.arg = nullptr; }         
    };
    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(Sensor::maxnumberof)];        
        Acquisition acquisition[static_cast<unsigned int>(Sensor::maxnumberof)];
        PrivateData() { }
    };
    
    
    static const std::uint8_t i2caddress = 0x80;
    static const std::uint8_t registerTemperatureRead = 0xE3;
    
    static PrivateData s_privatedata;
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);        
        
        std::uint16_t value = (acq->rxdata[0]<<8) + acq->rxdata[1];
               
        std::int32_t res = ( (17572 * static_cast<std::int32_t>(value) ) >> 16) - 4685;
        res /= 10;
        
        acq->temp = static_cast<std::int16_t>(res);
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
        
        std::uint8_t index = sensor2index(s);
                
        // init i2c ..
        embot::hw::i2c::init(config.i2cdes.bus, config.i2cdes.config);
        
        if(false == embot::hw::i2c::ping(config.i2cdes.bus, i2caddress))
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
        return embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus);             
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
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        }
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
        
        // ok, now i trigger i2c.
        embot::common::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::common::Data data = embot::common::Data(&s_privatedata.acquisition[index].rxdata[0], 2);
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, registerTemperatureRead, data, cbk);
        
        
        return resOK;
    }
    
    bool isalive(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return false;
        }

        return embot::hw::i2c::ping(s_privatedata.config[index].i2cdes.bus, i2caddress);        
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
    
    
    result_t read(Sensor s, Temperature &temp)
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
        temp = s_privatedata.acquisition[index].temp;
  
        return resOK;        
    }
    
    

 
}}} // namespace embot { namespace hw { namespace SI7051 {



#endif //defined(EMBOT_SI7051_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

