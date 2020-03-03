
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

#ifndef _EMBOT_COMMON_H_
#define _EMBOT_COMMON_H_

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <string>

namespace embot { namespace common {
    
    using fpGetU32      = std::uint32_t (*)(void);
    using fpGetU64      = std::uint64_t (*)(void);
    using fpParU32      = void (*)(std::uint32_t);
    using fpBoolParU32  = bool (*)(std::uint32_t);
    using fpI08ParU32   = std::int8_t (*)(std::uint32_t);
    using fpWorker      = void (*)(void);
    using fpCaller      = void (*)(void *);
           
    using Time          = std::uint64_t;    // expressed in usec. keeps absolute time since bootstrap.   
    using relTime       = std::uint32_t;    // expressed in usec. it is used for relative time. 0 means: 0 usec from ...

    using Event         = std::uint32_t;
    using EventMask     = std::uint32_t;
    using Message       = void *;
    
    
    template<typename E>         // C++14 [Meyers, pag. 73]
    constexpr auto tointegral(E enumerator) noexcept 
    {
        return static_cast<std::underlying_type_t<E>>(enumerator);
    }
         
    struct Callback
    {
        fpCaller call {nullptr};
        void * arg {nullptr};
        
        Callback() = default;
        constexpr Callback(fpCaller _c, void *_a) : call(_c), arg(_a) {}
        void load(fpCaller _c, void *_a) { call = _c; arg = _a; }
        void clear() { call = nullptr; arg = nullptr; }
        bool isvalid() const { if(nullptr != call) { return true; } else { return false; } } 
        void execute() const { if(true == isvalid()) { call(arg); } }
    };

    
    constexpr relTime timeWaitNone      = 0;
    constexpr relTime time1microsec     = 1;    
    constexpr relTime time1millisec     = 1000;
    constexpr relTime time1second       = 1000000;
    constexpr relTime timeWaitForever   = 0xffffffff;
    
    struct Date
    {   
        uint32_t    h {0};
        uint8_t     m {0};
        uint8_t     s {0};
        uint16_t    ms {0};
        uint16_t    us {0};
        
        Date() = default;
        
        constexpr Date(const common::Time &t) 
        {
            uint64_t tmp = t;
            us = tmp%1000; tmp /= 1000;
            ms = tmp%1000; tmp /= 1000;
            s =  tmp%60; tmp /= 60;
            m =  tmp%60; tmp /= 60;
            h =  tmp;            
        }
        
        std::string to_string() const
        {
            return "H" + std::to_string(h) + ":M" + std::to_string(m) + ":S" + std::to_string(s) + ":m" + std::to_string(ms) + ":u" + std::to_string(us);
        }
        
//        common::Time time() const
//        {
//            return static_cast<std::uint64_t>(us) + 1000*static_cast<std::uint64_t>(ms) + 1000*1000*static_cast<std::uint64_t>(s) + 60*1000*1000*static_cast<std::uint64_t>(m) + 60*60*1000*1000*static_cast<std::uint64_t>(h);
//        }
    };
                
} } // namespace embot { namespace common {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
