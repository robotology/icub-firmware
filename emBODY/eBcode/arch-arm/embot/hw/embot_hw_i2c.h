
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

#ifndef _EMBOT_HW_I2C_H_
#define _EMBOT_HW_I2C_H_

#include "embot_common.h"
#include "embot_hw.h"



// see also https://www.i2c-bus.org/

namespace embot { namespace hw { namespace i2c {
    
    enum class Bus { one = 0, two = 1, none = 32, maxnumberof = 2 };
    
    struct Config
    {   
        std::uint32_t   speed; 
        Config(std::uint32_t s) : speed(s) {}        
        Config() : speed(400000) {}
    };
    
    
    bool supported(Bus b);
    
    bool initialised(Bus b);
    
    result_t init(Bus b, const Config &config);
    
    // if a transaction is ongoing, hence the bus cannot be used.
    bool isbusy(Bus b);
    
    // check is the device is present
    bool ping(Bus b, std::uint8_t adr, std::uint8_t retries = 3, embot::common::relTime timeout = embot::common::time1millisec);
    
    // it asks to device with address adr the content of the register reg (so far only 8 bit addressing, sic).
    // at the end of transaction, the result is put in data and the callback oncompletion is called. 
    result_t read(Bus b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, embot::common::Callback oncompletion);
    

}}} // namespace embot { namespace hw { namespace i2c {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


