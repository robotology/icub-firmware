
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

#include "embot_core.h"
#include "embot_core_utils.h"
#include "embot_hw.h"



// see also https://www.i2c-bus.org/

namespace embot { namespace hw { namespace i2c {
    
    using ADR = std::uint8_t;
    using REG = std::uint8_t;
    
    static const std::uint8_t regNONE = 0xff;
    
    enum class Speed : uint32_t { standard100 = 100000, fast400 = 400000, fastplus1000 = 1000000, high3400 = 3400000 };
        
    struct Config
    {   
        std::uint32_t speed {400000}; 
        constexpr Config(std::uint32_t s) : speed(s) {}        
        constexpr Config() : speed(400000) {}
    };
    
    struct Descriptor
    {
        embot::hw::I2C     bus;
        Config  config;
        constexpr Descriptor() : bus(embot::hw::I2C::none), config(400000) {}
        constexpr Descriptor(embot::hw::I2C b, std::uint32_t s) : bus(b), config(s) {}
    };

    
    bool supported(embot::hw::I2C b);
    
    bool initialised(embot::hw::I2C b);
    
    result_t init(embot::hw::I2C b, const Config &config);
    
    // if a transaction is ongoing the bus cannot be used.
    bool isbusy(embot::hw::I2C b);
    // we also have a version w/ timeout. use it carefully!
    bool isbusy(embot::hw::I2C b, embot::core::relTime timeout, embot::core::relTime &remaining);
    
    // check is the device is present.
    // it internally calls isbusy(timeout, remaining)
    bool ping(embot::hw::I2C b, ADR adr, embot::core::relTime timeout = 3*embot::core::time1millisec);
    
    // blocking byte-transmission to ADR: it just sends the bytes inside content to a given i2c address and waits for completion for a maximum timeout
    result_t transmit(embot::hw::I2C b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout = 3*embot::core::time1millisec);
        
    // not blocking read from REG. we read from register reg a total of destination.capacity bytes
    // at the end of transaction, data is copied into destination.pointer and oncompletion.callback() is called (if non nullptr). 
    result_t read(embot::hw::I2C b, ADR adr, REG reg, embot::core::Data &destination, const embot::core::Callback &oncompletion);
    
    // blocking read from REG. we read from register reg a total of destination.size bytes and we wait until a timeout. 
    // if result is resOK, destination.pointer contains the data; if resNOKtimeout, the timeout expired. if resNOK the operation was not even started
    // the functions internally waits until not busy for the timeout ... however, please check isbusy() outside. 
    result_t read(embot::hw::I2C b, ADR adr, REG reg, embot::core::Data &destination, embot::core::relTime timeout);
        
    // not blocking write to REG. we write in register reg the content.size byte pointed by content.pointer.
    // when the write is done, the function oncompletion.callback() is called to alert the user.
    result_t write(embot::hw::I2C b, ADR adr, REG reg, const embot::core::Data &content, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));
    
    // blocking write to REG. we write in register reg the content.size byte pointed by content.pointer and we wait until a timeout.
    // if result is resOK, the operation is successful. if resNOKtimeout, the timeout expired. if resNOK the operation was not even started
    // the functions internally waits until not busy for the timeout ... however, please check isbusy() outside.
    result_t write(embot::hw::I2C b, ADR adr, REG reg, const embot::core::Data &content, embot::core::relTime timeout);    

}}} // namespace embot { namespace hw { namespace i2c {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


