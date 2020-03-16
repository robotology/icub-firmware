
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

#ifndef _EMBOT_HW_ONEWIRE_H_
#define _EMBOT_HW_ONEWIRE_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_gpio.h"

namespace embot { namespace hw { namespace onewire {
 
    // we implement one wire communication using an interrrupt-driven bit-banging technique (no: it is not a strange leisure practice ;-) !!), 
    // where bit timing is done using a dedicated hw timer
    // we share the same hw timer across all the one wire channel, hence ... 
    // (a) we have designed the write() and read() methods to be blocked for the whole transaction time.
    // (b) the user MUST NOT use this read() write() calls concurrently in the context of different threads even for different channels.
    // we have up to six channels which in theory could be made parallel BUT  so far i dont want to use six hw timers and more RAM. 
    // A FUTURE POSSIBLE implementation could make the six channels usable in parallel (useful to speed up the autocalibration in strain2)
    // but we should use six timers (dont tell me that one hw timer is enough if every channel use the same rate!), make write() read()
    // methods not bloking but with a callback at end of transaction, etc....    
    
    
    enum class Rate { tenKbps = 10, twentyKbps = 20, fortyKbps = 40, fiftyKbps = 50, onehundredKbps = 100 };
    
    struct Config
    {
        embot::hw::GPIO             gpio;               // the one which is to be modulated 
        Rate                        rate;               // the rate of the modulation
        bool                        usepreamble;        // if true we use it
        std::uint8_t                preamble;           // the preamble to be sent before reg and value.
        Config() : gpio(embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none), rate(Rate::tenKbps), usepreamble(true), preamble(0x55) {}
        Config(const embot::hw::GPIO &_g, Rate _r) 
        {
            gpio = _g;
            rate = _r;
            usepreamble =  true;
            preamble = 0x55;
        }
    };
    
    bool supported(embot::hw::ONEWIRE c);
    
    bool initialised(embot::hw::ONEWIRE c);
    
    result_t init(embot::hw::ONEWIRE c, const Config &config);
    
    bool isrunning(embot::hw::ONEWIRE c);
    
    result_t write(embot::hw::ONEWIRE c, std::uint8_t reg, std::uint16_t value, embot::core::relTime timeout = embot::core::timeWaitForever);
 
}}} // namespace embot { namespace hw { namespace onewire { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


