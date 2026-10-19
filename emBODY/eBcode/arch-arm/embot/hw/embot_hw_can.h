
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

#include "embot_core.h"
#include "embot_hw_types.h"
#include<algorithm>

namespace embot { namespace hw { namespace can {
    
    struct Frame            
    {
        std::uint32_t id {0};           // 11 bits if not extended, 29 bits if extended
        std::uint8_t size {0};
        std::uint8_t filler[3] {0};     // filler[0] is the extended flag: see isextended() / setextended(). sizeof(Frame) is unchanged
        std::uint8_t data[8] {0};  
        constexpr Frame() = default;
        // the old 3-argument form still builds a standard frame, so the existing code does not change
        constexpr Frame(std::uint32_t i, std::uint8_t s, std::uint8_t *d, bool extended = false) 
            : id(i), size(std::min(s, static_cast<std::uint8_t>(8))), filler{static_cast<std::uint8_t>(extended ? 1 : 0), 0, 0}
        {
            if(nullptr != d) { std::memmove(data, d, size); }
        }
        constexpr bool isextended() const { return 0 != filler[0]; }
        constexpr void setextended(bool extended) { filler[0] = extended ? 1 : 0; }
    };
    static_assert(sizeof(Frame) == 16, "embot::hw::can::Frame must stay 16 bytes");
        
    struct Config
    {
        std::uint8_t txcapacity {8};
        std::uint8_t rxcapacity {8};
        embot::core::Callback ontxframe {}; 
        embot::core::Callback txqueueempty {}; 
        embot::core::Callback onrxframe {};
        bool acceptextended {false};    // true: the driver also receives frames w/ extended id (29 bits). on FDCAN the bsp must have Init.ExtFiltersNbr >= 1
        constexpr Config() = default;
        constexpr Config(uint8_t txc, uint8_t rxc, const embot::core::Callback &otx, 
                         const embot::core::Callback &etx, const embot::core::Callback &orx, bool ext = false)
                            : txcapacity(txc), rxcapacity(rxc), ontxframe(otx), txqueueempty(etx), onrxframe(orx), acceptextended(ext) {}
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
    
    // about filters: after init() every CAN ID is enabled for reception
    // then, setfilters() allows receptions only of the following CAN IDs sent by node @ 0
    // bootloader class towards address or towards  0xf (broadcast) 
    // analog sensor polling class towards address or towards 0xf (broadcast)
    // motion control polling class towards ID = address or towards ID = 0xf (broadcast) 
    // for motion control boards only, such as pmc and amcbldc ...
    // motion control streaming class sent by node @ 0 (but not by other boards)
    result_t setfilters(embot::hw::CAN p, std::uint8_t address);   
   
    enum class Direction : uint8_t { TX = 0, RX = 1 }; 
    bool lock(embot::hw::CAN p, embot::hw::can::Direction dir);
    void unlock(embot::hw::CAN p, embot::hw::can::Direction dir, bool lockstatus);
           
}}} // namespace embot { namespace hw { namespace can {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


