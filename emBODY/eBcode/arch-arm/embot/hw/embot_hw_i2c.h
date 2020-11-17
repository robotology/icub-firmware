
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
#include "embot_hw_types.h"



// see also https://www.i2c-bus.org/

namespace embot { namespace hw { namespace i2c {
    
    // speeds so far are those 
    enum class Speed : uint32_t { standard100 = 100000, fast400 = 400000, fastplus1000 = 1000000, high3400 = 3400000, none = 0 };
    
    // address is always expressed by <= 8 bits
    using ADR = std::uint8_t;
   
    // device registers can be addressed in multiple address modes. in this implementation <= 16 bits
    struct Reg
    {   // 8 bits + 16 bits (big endian)
        enum class Size : uint8_t { none = 0, eightbits = 8, sixteenbits = 16 };
        static constexpr uint16_t addressNONE {0xffff};
        uint16_t address {addressNONE};
        Size size {Size::eightbits};
        // this ctor transform a single number into a 8-bit register and it MAINTAINS BACKWARDS COMPATIBILITY w/ old API which used REG as a uint8_t
        constexpr Reg(uint16_t a, Size s = Size::eightbits) { address = a; size = s; }
        constexpr Reg() = default;
    };
   
          
    struct Config
    {   // so far we cannot initialize I2C with a given speed at runtime   
        uint8_t speedIStobedone {0};       
        constexpr Config() = default;
        constexpr bool isvalid() const { return true; }
    };
    
    struct Descriptor
    {
        embot::hw::I2C bus {embot::hw::I2C::none};
        ADR adr {0};
        constexpr Descriptor() = default;
        constexpr Descriptor(embot::hw::I2C b, ADR a = 0) : bus(b), adr(a) {}
        constexpr bool isvalid() const { return (embot::hw::I2C::none != bus) && (0 != adr); }              
    };

    
    bool supported(embot::hw::I2C b);    
    bool initialised(embot::hw::I2C b);    
    result_t init(embot::hw::I2C b, const Config &config);
    
    // discovers all chips on the bus. if any, it returns true and fills the vector
    bool discover(embot::hw::I2C b, std::vector<ADR> &adrs);
    
    // read() and write() operate on a register of the chip.
    // transmit() and receive() operate on the chip without specifying a given register
    
    
    // BLOCKING mode.
    // in such a mode, the functions start operations and wait until completion or until a timeout has expired.
    // the timeout must always be specified. for example 3*embot::core::time1millisec, 500*embot::core::time1microsec, etc...
    
    // BLOCKING BUS ACTIVITY CHECK
    // it tells if the specified i2c bus is busy with some operation. if busy the bus cannot be used.
    // returns true only if the bus gets free by the specified timeout. it gives back in remaining the time = timeout - time spent inside.  
    // this function is internally used by functions ping(), read(), write(), transmit() and receive(). they wait until the bus is free before 
    // they get ownership of the bus for their tx/rx operations.
    bool isbusy(embot::hw::I2C b, embot::core::relTime timeout, embot::core::relTime &remaining);
    
    // PING of a device
    // check is a device is present on a given bus b and at a given address adr. this function waits up to a timeout 
    // whose typical value is 3*embot::core::time1millisec
    // before any activity this functions waits until isbusy(b, timeout, remaining) returns false.
    bool ping(embot::hw::I2C b, ADR adr, embot::core::relTime timeout);
    
    
    // BLOCKING CORE BYTE TX.
    // it is the most general transmission function.
    // it ensure to call directly the direct low level functions (not DMA). it sets inside the bus busy and then free again 
    // it accepts zero address and empty content (in such a case sends just the address)
    result_t tx(embot::hw::I2C b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout);    
    
    // BLOCKING READ from a register. 
    // it reads starting from register reg a total of destination.capacity bytes and waits until a timeout. 
    // operation is OK only if read() returns resOK. In this case destination.pointer contains the data; 
    // operation fails if read() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started)
    // before any activity this functions waits until isbusy() returns false. any such wait consumes the specified timeout. 
    result_t read(embot::hw::I2C b, ADR adr, Reg reg, embot::core::Data &destination, embot::core::relTime timeout);
    
    // BLOCKING WRITE to a register. 
    // it writes starting from register reg a total of content.size bytes pointed by content.pointer and waits until a timeout.
    // operation is OK only if write() returns resOK. In this case the bytes pointed by content.pointer are guaranteed to be written to register.
    // operation fails if write() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started).
    // before any activity this functions waits until isbusy(b, timeout, remaining) returns false.
    result_t write(embot::hw::I2C b, ADR adr, Reg reg, const embot::core::Data &content, embot::core::relTime timeout);
            
    // BLOCKING BYTE RECEIVE. 
    // it receives the bytes pointed by destination.pointer for a total of destination.capacity from a given i2c::ADR adr and waits until a timeout.
    // operation is OK only if receive() returns resOK. In this case the bytes pointed by destination.pointer are those transmitted by the device.
    // operation fails if receive() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started).
    // before any activity this functions waits until isbusy(b, timeout, remaining) returns false.
    result_t receive(embot::hw::I2C b, ADR adr, embot::core::Data &destination, embot::core::relTime timeout);    
    
    // BLOCKING BYTE TRANSMIT. 
    // its send the bytes pointed by content.pointer for a total of content.capacity to a given i2c::ADR adr and waits until a timeout.
    // operation is OK only if transmit() returns resOK. In this case the bytes pointed by content.pointer are guaranteed to be transmitted.
    // operation fails if transmit() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started).
    // before any activity this functions waits until isbusy(b, timeout, remaining) returns false.
    result_t transmit(embot::hw::I2C b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout);    
    
                   
    // NON-BLOCKING mode.
    // in such a mode, the functions returns control to the calling thread in the shortest possible time.
    // if the bus is busy they return without doing any activity. if not busy they set the bus busy, start hardware operations,  
    // dont wait for completion and return control to the calling thread. 
    // the calling thread can now operate in two modes:
    // 1. wait until isbusy() returns false (because the hardware sets the bus free at completion)
    // 2. can continue its execution and be alerted by the specified callback that the operation is completed.
    
    // NON-BLOCKING BUS ACTIVITY CHECK.
    // it immediatly returns true if the bus is busy or false if the bus is available.
    // this function is internally used by functions read() and write() which immediately returns w/out doing anything if the bus is busy.  
    bool isbusy(embot::hw::I2C b);
    
    // NON-BLOCKING READ from a register. 
    // if the bus is not busy it starts the reading operation of a total of destination.capacity bytes starting from register reg and returns. 
    // at completion of the reading operation the hardware copies the received data inside destination.pointer and ensure the execution of 
    // the specified callback which can be used to alert a thread. 
    // this funtions returns resOK if the hardware can succesfully start its operations and resNOK in any failure case.
    result_t read(embot::hw::I2C b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion);
    
    // NON-BLOCKING WRITE to register. 
    // if the bus is not busy it starts the writing operation of a total of content.capacity bytes pointed by content.pointer starting from
    // register reg and returns.
    // at completion of the writing operation the hardware ensures the execution of the specified callback which can be used to alert a thread. 
    // this funtions returns resOK if the hardware can succesfully start its operations and resNOK in any failure case.
    result_t write(embot::hw::I2C b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));
    

    // NON-BLOCKING BYTE RECEIVE. 
    // if the bus is not busy it starts the receiving operation of a total of destination.capacity bytes and returns. 
    // at completion of the reading operation the hardware copies the received data inside destination.pointer and ensure the execution of 
    // the specified callback which can be used to alert a thread. 
    // this funtions returns resOK if the hardware can succesfully start its operations and resNOK in any failure case.
    result_t receive(embot::hw::I2C b, ADR adr, embot::core::Data &destination, const embot::core::Callback &oncompletion);  
    
    // NON-BLOCKING BYTE TRANSMIT.
    result_t transmit(I2C b, ADR adr, const embot::core::Data &content, const embot::core::Callback &oncompletion);
    
}}} // namespace embot { namespace hw { namespace i2c {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


