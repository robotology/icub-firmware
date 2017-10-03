
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
        embot::hw::i2c::Bus             i2cbus;
        embot::hw::i2c::Config          i2cconfig; 
        Config(embot::hw::i2c::Bus b, const embot::hw::i2c::Config &c) : i2cbus(b), i2cconfig(c) {}        
        Config() : i2cbus(embot::hw::i2c::Bus::one), i2cconfig(400000) {}
    };
    
    
    using Temperature = std::int16_t; // in 0.1 Celsius Degrees
    
        
    bool supported(Sensor s);
    
    bool initialised(Sensor s);
    
    result_t init(Sensor s, const Config &config);
        
    bool isalive(Sensor s);
    
    bool isbusbusy(Sensor s);
    
    // usage 1: 
    // a. we call acquistion() with a callback. 
    // b. at expiry of the callback we verify with isready() and then we read with read().
    // usage 2:
    // a. we call acquistion() without a callback.
    // b. we loop until isready() returns true.
    // c. we read with read()
    result_t acquisition(Sensor s, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    bool isacquiring(Sensor s);
    bool isready(Sensor s);
    result_t read(Sensor s, Temperature &temp);        
 
}}} // namespace embot { namespace hw { namespace SI7051 {
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


