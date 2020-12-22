
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

#ifndef _EMBOT_HW_TLV493D_H_
#define _EMBOT_HW_TLV493D_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_types.h"

#include "embot_hw_i2c.h"

namespace embot { namespace hw { namespace tlv493d {

    
        
    struct Config
    { 
        enum class startupMODE : uint8_t { resetCHIP = 0, dontresetCHIP = 1};        
        startupMODE mode {startupMODE::resetCHIP}; 
        constexpr Config() = default;         
        constexpr Config(startupMODE m) : mode(m) {} 
        constexpr bool isvalid() const { return true; }
    };
    
    
    using Position = std::int32_t; // the measurement units is: 0.01 degrees
    
    std::string to_string(embot::hw::TLV493D id);
            
    bool supported(embot::hw::TLV493D h);    
    bool initialised(embot::hw::TLV493D h);    
    result_t init(embot::hw::TLV493D h, const Config &config);
        
    
    // after that init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(embot::hw::TLV493D h, embot::core::relTime timeout = 3*embot::core::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(embot::hw::TLV493D h);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(embot::hw::TLV493D h);    
    
    
    // we start acquisition of temperature.
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(embot::hw::TLV493D h, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));

    // it tells if a previous operation of acquisition is over
    bool operationdone(embot::hw::TLV493D h);
    
    // ok, now we can read temperature previously acquired
    result_t read(embot::hw::TLV493D h, Position &temp);   

 
}}} // namespace embot { namespace hw { namespace tlv493d {
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


