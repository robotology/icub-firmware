
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

#include "embot_core.h"
#include "embot_hw_types.h"


#include "embot_hw_i2c.h"

    
namespace embot { namespace hw { namespace si7051 {
         
        
    struct Config
    {   // the addressing of the sensor is given by the bps
        uint8_t tbd {0};        
        constexpr Config() = default;
    };
    
    
    using Temperature = std::int16_t; // in 0.1 Celsius Degrees
            
    bool supported(embot::hw::SI7051 s);    
    bool initialised(embot::hw::SI7051 s);    
    result_t init(embot::hw::SI7051 s, const Config &config);
        
    
    // after that init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(embot::hw::SI7051 s, embot::core::relTime timeout = 3*embot::core::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(embot::hw::SI7051 s);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(embot::hw::SI7051 s);    
    
    
    // we start acquisition of temperature.
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(embot::hw::SI7051 s, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));

    // it tells if a previous operation of acquisition is over
    bool operationdone(embot::hw::SI7051 s);
    
    // ok, now we can read temperature previously acquired
    result_t read(embot::hw::SI7051 s, Temperature &temp);   

 
}}} // namespace embot { namespace hw { namespace si7051 {
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


