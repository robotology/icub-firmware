
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

#ifndef _EMBOT_HW_CAN_H_
#define _EMBOT_HW_CAN_H_

#include "embot_common.h"
#include "embot_hw.h"



namespace embot { namespace hw { namespace can {
    
    struct Frame            
    {
        std::uint32_t       id;
        std::uint8_t        size;
        std::uint8_t        filler[3];
        std::uint8_t        data[8];  
        Frame() : id(0), size(0) { std::uint64_t *d = reinterpret_cast<std::uint64_t*>(data); *d = 0; filler[0] = filler[1] = filler[2] = 0; }
    };
    
//    enum class Port { one = 0, two = 1, none = 32, maxnumberof = 2};
    
    struct Config
    {
        std::uint8_t                txcapacity;
        std::uint8_t                rxcapacity;
        embot::common::Callback     ontxframe; 
        embot::common::Callback     txqueueempty; 
        embot::common::Callback     onrxframe;
        Config() : txcapacity(8), rxcapacity(8), ontxframe(nullptr, nullptr), txqueueempty(nullptr, nullptr), onrxframe(nullptr, nullptr) {}
    };
    
    bool supported(embot::hw::CAN p);
    
    bool initialised(embot::hw::CAN p);
    
    result_t init(embot::hw::CAN p, const Config &config);
    
    result_t enable(embot::hw::CAN p);
    
    result_t disable(embot::hw::CAN p);
    
    result_t put(embot::hw::CAN p, const Frame &frame);
    
    std::uint8_t outputqueuesize(embot::hw::CAN p);
    
    result_t transmit(embot::hw::CAN p);
    
    std::uint8_t inputqueuesize(embot::hw::CAN p);
    
    result_t get(embot::hw::CAN p, Frame &frame, std::uint8_t &remaining);
    
    result_t setfilters(embot::hw::CAN p, std::uint8_t address);
    
    
    
}}} // namespace embot { namespace hw { namespace can {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


