
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_SI7051_H_
#define _EMBOT_HW_SI7051_H_

#include "embot_common.h"
#include "embot_hw.h"


#include "embot_hw_i2c.h"

    
namespace embot { namespace hw { namespace SI7051 {
         
    enum class Sensor { one = 0, two = 1, none = 32, maxnumberof = 2 };
        
    struct Config
    {   // each sensor uses a separate channel of i2c communication and the same i2c address (sic)
        embot::hw::i2c::Descriptor      i2cdes;        
        Config(embot::hw::i2c::Bus b, std::uint32_t s) : i2cdes(b, s) {}        
        Config() : i2cdes(embot::hw::i2c::Bus::one, 400000) {}
        Config(const embot::hw::i2c::Descriptor &des) : i2cdes(des) {}
    };
    
    
    using Temperature = std::int16_t; // in 0.1 Celsius Degrees
            
    bool supported(Sensor s);
    
    bool initialised(Sensor s);
    
    result_t init(Sensor s, const Config &config);
        
    
    // after that init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(Sensor s, embot::common::relTime timeout = 3*embot::common::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(Sensor s);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(Sensor s);    
    
    
    // we start acquisition of temperature.
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(Sensor s, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));

    // it tells if a previous operation of acquisition is over
    bool operationdone(Sensor s);
    
    // ok, now we can read temperature previously acquired
    result_t read(Sensor s, Temperature &temp);   

 
}}} // namespace embot { namespace hw { namespace SI7051 {
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


