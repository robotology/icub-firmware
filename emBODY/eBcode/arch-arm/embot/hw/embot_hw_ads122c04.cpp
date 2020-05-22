
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Andrea Mura
 * email:   marco.accame@iit.it, andrea.mura@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_ads122c04.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include "stm32hal.h"
#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"

using namespace std;

#include "embot_core_binary.h"
#include "embot_hw_sys.h"


using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#define ads122c04_FAKEmode

#if defined(ads122c04_FAKEmode)
#warning WIP: ads122c04_FAKEmode is still defined 
#endif

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_ads122c04)

namespace embot { namespace hw { namespace ads122c04 {

    bool supported(ADS122C04 s)                                                                         { return false; }
    bool initialised(ADS122C04 s)                                                                       { return false; }
    result_t init(ADS122C04 s, const Config &config)                                                    { return resNOK; }    
    bool isalive(embot::hw::ADS122C04 s, embot::core::relTime timeout)                                  { return false; }    
    bool isacquiring(embot::hw::ADS122C04 s)                                                            { return false; }    
    bool canacquire(embot::hw::ADS122C04 s)                                                             { return false; }        
    result_t acquisition(embot::hw::ADS122C04 s, const embot::core::Callback &oncompletion)             { return resNOK; }
    bool operationdone(embot::hw::ADS122C04 s)                                                          { return false; }
    result_t read(embot::hw::ADS122C04 s, Value &temp)                                                  { return resNOK; }   

}}} // namespace embot { namespace hw { namespace ADS122C04 {


#else

namespace embot { namespace hw { namespace ads122c04 {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(ADS122C04 a)
    {
        return embot::hw::bsp::ads122c04::getBSP().supported(a);
    }
    
    bool initialised(ADS122C04 a)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(a));
    }    
      

    struct Acquisition
    {
        static constexpr uint8_t sizeofdata = 6; // 24 bits, two samples
        volatile bool done {false};
        volatile bool ongoing {false};
        Values values {};
        std::uint8_t rxdata[sizeofdata] {0};
        embot::core::Callback userdefCBK {};  
        void clear() { done = false; ongoing = false; values.clear(); memset(rxdata, 0, sizeof(rxdata)); userdefCBK.clear(); }        
        Acquisition() = default;        
    };
    
    struct PrivateData
    {
        std::uint8_t i2caddress[embot::core::tointegral(ADS122C04::maxnumberof)] = {0};   
        Config config[embot::core::tointegral(ADS122C04::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(ADS122C04::maxnumberof)];
        PrivateData() = default;
    };
    
    
    //static const std::uint8_t i2caddress = 0x80;
    static const std::uint8_t registerTemperatureRead = 0xE3;
    
    static PrivateData s_privatedata;
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);        
        

#if defined(ads122c04_FAKEmode)        
        static volatile uint32_t tmp = 0;
        tmp++;
        acq->values.v1 = tmp;
        acq->values.v2 = tmp+1;
#else
        #warning ADS122C04: to be implemented 
                //std::uint16_t value = (acq->rxdata[0]<<8) + acq->rxdata[1];
               
        //std::int32_t res = ( (17572 * static_cast<std::int32_t>(value) ) >> 16) - 4685;
        //res /= 10;
        
        //acq->values.v1 = static_cast<std::int16_t>(res);
#endif        
        acq->ongoing = false;
        acq->done = true;
        
        acq->userdefCBK.execute();
    }
              
    result_t init(ADS122C04 s, const Config &config)
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
        embot::hw::bsp::ads122c04::getBSP().init(s);
        
        std::uint8_t index = embot::core::tointegral(s);
        
#if defined(ads122c04_FAKEmode)
#else        
                
        // init i2c ..
        embot::hw::i2c::init(config.i2cdes.bus, config.i2cdes.config);
        if(false == embot::hw::i2c::ping(config.i2cdes.bus, embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress))
        {
            return resNOK;
        }
#endif        
        s_privatedata.i2caddress[index] = embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
                
        return resOK;
    }

    
    bool isacquiring(ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(ADS122C04 s)
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
#if defined(ads122c04_FAKEmode)
        return true;
#else         
        return !embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus);  
#endif        
    }    
    
    result_t acquisition(ADS122C04 s, const embot::core::Callback &oncompletion)
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

#if defined(ads122c04_FAKEmode)
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        cbk.execute();
#else         
        // ok, now i trigger i2c.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].rxdata[0], sizeof(s_privatedata.acquisition[index].rxdata));
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], registerTemperatureRead, data, cbk);
#endif                
        return resOK;
    }
    
    bool isalive(ADS122C04 s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
#if defined(ads122c04_FAKEmode)
        return true;
#else 
        std::uint8_t index = embot::core::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], timeout);  
#endif
    }

    
    bool operationdone(ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(s)].done;        
    } 
    
    
    result_t read(ADS122C04 s, Values &vals)
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
        vals.v1 = s_privatedata.acquisition[index].values.v1;
        vals.v2 = s_privatedata.acquisition[index].values.v2;
  
        return resOK;        
    }
    
 
}}} // namespace embot { namespace hw { namespace ads122c04 {



#endif //defined(EMBOT_ADS122C04_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

