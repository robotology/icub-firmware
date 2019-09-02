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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include "stm32hal.h"
#include "embot_hw_bsp.h"

using namespace std;

#include "embot_binary.h"
#include "embot_hw_sys.h"


using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_SI7051_ENABLED

#if     !defined(EMBOT_SI7051_ENABLED)


namespace embot { namespace hw { namespace si7051 {

    bool supported(SI7051 s)                                                                        { return false; }
    bool initialised(SI7051 s)                                                                      { return false; }
    result_t init(SI7051 s, const Config &config)                                                   { return resNOK; }
    
    result_t get(SI7051 s, Temperature &temp)                                                       { return resNOK; }

}}} // namespace embot { namespace hw { namespace SI7051 {


#elif   defined(EMBOT_SI7051_ENABLED)


namespace embot { namespace hw { namespace si7051 {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(SI7051 a)
    {
        return embot::hw::bsp::si7051::getBSP().supported(a);
    }
    
    bool initialised(SI7051 a)
    {
        return embot::binary::bit::check(initialisedmask, embot::common::tointegral(a));
    }    
      

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        Temperature temp;
        std::uint8_t rxdata[2];
        embot::common::Callback userdefCBK;  
        void clear() { done = false; ongoing = false; temp = 0; rxdata[0] = rxdata[1] = 0; userdefCBK.clear(); }         
    };
    
    struct PrivateData
    {
        std::uint8_t i2caddress[embot::common::tointegral(SI7051::maxnumberof)];   
        Config config[embot::common::tointegral(SI7051::maxnumberof)];        
        Acquisition acquisition[embot::common::tointegral(SI7051::maxnumberof)];
        PrivateData() { }
    };
    
    
    //static const std::uint8_t i2caddress = 0x80;
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
        
        acq->userdefCBK.execute();
    }
              
    result_t init(SI7051 s, const Config &config)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        // init peripheral
        embot::hw::bsp::si7051::getBSP().init(s);
        
        std::uint8_t index = embot::common::tointegral(s);
                
        // init i2c ..
        embot::hw::i2c::init(config.i2cdes.bus, config.i2cdes.config);
        if(false == embot::hw::i2c::ping(config.i2cdes.bus, embot::hw::bsp::si7051::getBSP().getPROP(s)->i2caddress))
        {
            return resNOK;
        }
        
        s_privatedata.i2caddress[index] = embot::hw::bsp::si7051::getBSP().getPROP(s)->i2caddress;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::binary::bit::set(initialisedmask, embot::common::tointegral(s));
                
        return resOK;
    }

    
    bool isacquiring(SI7051 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::common::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(SI7051 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::common::tointegral(s);  
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return false;
        }
        
        return !embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus);             
    }    
    
    result_t acquisition(SI7051 s, const embot::common::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::common::tointegral(s);
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
        
        // ok, now i trigger i2c.
        embot::common::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::utils::Data data = embot::utils::Data(&s_privatedata.acquisition[index].rxdata[0], 2);
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], registerTemperatureRead, data, cbk);
                
        return resOK;
    }
    
    bool isalive(SI7051 s, embot::common::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
        std::uint8_t index = embot::common::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], timeout);  
    }

    
    bool operationdone(SI7051 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::common::tointegral(s)].done;        
    } 
    
    
    result_t read(SI7051 s, Temperature &temp)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == operationdone(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::common::tointegral(s);
        temp = s_privatedata.acquisition[index].temp;
  
        return resOK;        
    }
    
 
}}} // namespace embot { namespace hw { namespace si7051 {



#endif //defined(EMBOT_SI7051_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

