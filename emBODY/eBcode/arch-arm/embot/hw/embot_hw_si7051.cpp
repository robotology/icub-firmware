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


#include "embot_hw_bsp_config.h"
#include "embot_hw_si7051_bsp.h"

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


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_si7051)

namespace embot { namespace hw { namespace si7051 {

    bool supported(SI7051 s)                                                                        { return false; }
    bool initialised(SI7051 s)                                                                      { return false; }
    result_t init(SI7051 s, const Config &config)                                                   { return resNOK; }    
    bool isalive(embot::hw::SI7051 s, embot::core::relTime timeout)                               { return false; }    
    bool isacquiring(embot::hw::SI7051 s)                                                           { return false; }    
    bool canacquire(embot::hw::SI7051 s)                                                            { return false; }        
    result_t acquisition(embot::hw::SI7051 s, const embot::core::Callback &oncompletion)          { return resNOK; }
    bool operationdone(embot::hw::SI7051 s)                                                         { return false; }
    result_t read(embot::hw::SI7051 s, Temperature &temp)                                           { return resNOK; }   

}}} // namespace embot { namespace hw { namespace SI7051 {


#else

namespace embot { namespace hw { namespace si7051 {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(SI7051 a)
    {
        return embot::hw::si7051::getBSP().supported(a);
    }
    
    bool initialised(SI7051 a)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(a));
    }    
      

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        Temperature temp;
        std::uint8_t rxdata[2];
        embot::core::Callback userdefCBK;  
        void clear() { done = false; ongoing = false; temp = 0; rxdata[0] = rxdata[1] = 0; userdefCBK.clear(); }         
    };
    
    struct PrivateData
    {
        embot::hw::i2c::Descriptor i2cdes[embot::core::tointegral(SI7051::maxnumberof)];   
        Config config[embot::core::tointegral(SI7051::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(SI7051::maxnumberof)];
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
        embot::hw::si7051::getBSP().init(s);
        
        std::uint8_t index = embot::core::tointegral(s);
                
        // init i2c with the relevant bus specified by the bsp for this chip
        embot::hw::i2c::init(embot::hw::si7051::getBSP().getPROP(s)->i2cdes.bus, {});
        if(false == embot::hw::i2c::ping(embot::hw::si7051::getBSP().getPROP(s)->i2cdes.bus, embot::hw::si7051::getBSP().getPROP(s)->i2cdes.adr, 3*embot::core::time1millisec))
        {
            return resNOK;
        }
        
        s_privatedata.i2cdes[index] = embot::hw::si7051::getBSP().getPROP(s)->i2cdes;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
                
        return resOK;
    }

    
    bool isacquiring(SI7051 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(SI7051 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);  
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return false;
        }
        
        return !embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus);             
    }    
    
    result_t acquisition(SI7051 s, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
        
        // ok, now i trigger i2c.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].rxdata[0], 2);
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, registerTemperatureRead, data, cbk);
                
        return resOK;
    }
    
    bool isalive(SI7051 s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
        std::uint8_t index = embot::core::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, timeout);  
    }

    
    bool operationdone(SI7051 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(s)].done;        
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
        
        std::uint8_t index = embot::core::tointegral(s);
        temp = s_privatedata.acquisition[index].temp;
  
        return resOK;        
    }
    
 
}}} // namespace embot { namespace hw { namespace si7051 {



#endif //defined(EMBOT_ENABLE_hw_si7051)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

