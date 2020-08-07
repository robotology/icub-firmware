
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Andrea Mura
 * email:   marco.accame@iit.it, andrea.mura@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_ADS122C04_H_
#define _EMBOT_HW_ADS122C04_H_

#include "embot_core.h"
#include "embot_hw_types.h"


#include "embot_hw_i2c.h"

    
namespace embot { namespace hw { namespace ads122c04 {
         
        
    struct Config
    {   // each sensor uses a separate channel of i2c communication and the same i2c address (sic)
        embot::hw::i2c::Descriptor      i2cdes;        
        constexpr Config(embot::hw::I2C b, std::uint32_t s) : i2cdes(b, s) {}        
        constexpr Config() : i2cdes(embot::hw::I2C::one, 400000) {}
        constexpr Config(const embot::hw::i2c::Descriptor &des) : i2cdes(des) {}
    };
    
    
    using Value = std::uint32_t; 
    
    struct Values
    {
        Value v1 {0};
        Value v2 {0};
        Values() = default;
        void clear() { v1 = v2 = 0; }
    };
    
    enum class Channel : uint8_t { one = 0, two = 1 };
    
            
    bool supported(embot::hw::ADS122C04 s);
    
    bool initialised(embot::hw::ADS122C04 s);
    
    result_t init(embot::hw::ADS122C04 s, const Config &config);
        
    
    // after that init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(embot::hw::ADS122C04 s, embot::core::relTime timeout = 3*embot::core::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(embot::hw::ADS122C04 s);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(embot::hw::ADS122C04 s);    
    
    
    // we start acquisition of temperature.
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(embot::hw::ADS122C04 s, Channel channel, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));

    // it tells if a previous operation of acquisition is over
    bool operationdone(embot::hw::ADS122C04 s);
    
    // ok, now we can read temperature previously acquired
    result_t read(embot::hw::ADS122C04 s, Values &values);   

 
}}} // namespace embot { namespace hw { namespace ads122c04 {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


