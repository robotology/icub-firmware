
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_AD7147_H_
#define _EMBOT_HW_AD7147_H_

#include "embot_core.h"
#include "embot_hw_types.h"


#include "embot_hw_i2c.h"

    
namespace embot { namespace hw { namespace ad7147 {
         
        
    struct Config
    {   // each sensor may use different i2c addresses, hence they can share teh same i2c bus
        embot::hw::i2c::Descriptor i2cdes {};        
        constexpr Config(embot::hw::I2C b, std::uint32_t s) : i2cdes(b, s) {}        
        constexpr Config() : i2cdes(embot::hw::I2C::one, 400000) {}
        constexpr Config(const embot::hw::i2c::Descriptor &des) : i2cdes(des) {}
    };
    
    
    using Values = std::array<std::uint16_t, 12>; 
    
    
    bool supported(embot::hw::AD7147 s);    
    bool initialised(embot::hw::AD7147 s);    
    result_t init(embot::hw::AD7147 s, const Config &config);
        
    
    // after that init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(embot::hw::AD7147 s, embot::core::relTime timeout = 3*embot::core::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(embot::hw::AD7147 s);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(embot::hw::AD7147 s);    
    
    
    // we start acquisition
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(embot::hw::AD7147 s, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));

    // it tells if a previous operation of acquisition is over
    bool operationdone(embot::hw::AD7147 s);
    
    // ok, now we can read the values previously acquired
    result_t read(embot::hw::AD7147 s, Values &values);   

 
}}} // namespace embot { namespace hw { namespace ad7147 {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


