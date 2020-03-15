
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
#error dont include me
#if 0
#ifndef _EMBOT_COMMON_H_
#define _EMBOT_COMMON_H_

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <string>

namespace embot { namespace common {
      
    // the time is always expressed in micro-seconds    
    using Time          = std::uint64_t;    // keeps absolute time since bootstrap.   
    using relTime       = std::uint32_t;    // it is used for relative time. 0 means: 0 usec from ...
    
    // some useful Time / relTime values
    constexpr relTime timeWaitNone      = 0;
    constexpr relTime time1microsec     = 1;    
    constexpr relTime time1millisec     = 1000;
    constexpr relTime time1second       = 1000000;
    constexpr relTime timeWaitForever   = 0xffffffff;
    
    // it returns absolute time as configured by one of embot::core::init() or embot::hw::init() or embot::os::init() 
    embot::core::Time now();  

    // it formats Time (or relTime) for print purposes
    struct TimeFormatter
    {   
        uint16_t    D {0}; // day
        uint8_t     H {0}; // hour
        uint8_t     M {0}; // minute
        uint8_t     S {0}; // second
        uint16_t    m {0}; // milli
        uint16_t    u {0}; // micro
        
        TimeFormatter() = default;
        
        constexpr TimeFormatter(const core::Time &t) 
        {
            uint64_t tmp = t;
            u = tmp%1000; tmp /= 1000;
            m = tmp%1000; tmp /= 1000;
            S = tmp%60; tmp /= 60;
            M = tmp%60; tmp /= 60;
            H = tmp%24; tmp /= 24;
            D = tmp;            
        }
        
        uint64_t to_seconds() const { return S+60*M+3600*H+86400*D; }
        
        enum class Mode : std::uint8_t { compact, full };
        std::string to_string(const Mode &mode = Mode::compact) const
        {
            if(Mode::compact == mode)
            {
                return "S" + std::to_string(to_seconds()) + ":m" + std::to_string(m) + ":u" + std::to_string(u);        
            }
            else
            {
                return "D" + std::to_string(D) + ":H" + std::to_string(H) + ":M" + std::to_string(M) + ":S" + std::to_string(S) + ":m" + std::to_string(m) + ":u" + std::to_string(u); 
            }
        }
        
        core::Time to_time() const
        {
            return static_cast<std::uint64_t>(u) + 1000*static_cast<std::uint64_t>(m) + 1000*1000*to_seconds();
        }
    };    
        
    
    // some pointer to functions
    using fpGetU32      = std::uint32_t (*)(void);
    using fpGetU64      = std::uint64_t (*)(void);
    using fpParU32      = void (*)(std::uint32_t);
    using fpBoolParU32  = bool (*)(std::uint32_t);
    using fpI08ParU32   = std::int8_t (*)(std::uint32_t);
    using fpWorker      = void (*)(void);
    using fpCaller      = void (*)(void *);
             
    // some useful data structures   

    // it represents a function call with a generic argument
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

    // it holds generic data in form of a pointer to memory and its size.
    struct Data
    {    
        void * pointer {nullptr};
        size_t size {0};
        
        Data() = default;
        Data(void *p, size_t s) : pointer(p), size(s) {}
                    
        void load(void *littleendianmemory, size_t s) { pointer = littleendianmemory; size = s; }
        void clear() { pointer = nullptr; size = 0; }
        bool isvalid() const { if((nullptr != pointer) && (0 != size)){ return true; } else { return false; } } 
      
//        std::uint8_t  * getU08ptr() const { return reinterpret_cast<std::uint8_t*>(pointer); }  
//        std::uint16_t * getU16ptr() const { return reinterpret_cast<std::uint16_t*>(pointer); } 
//        std::uint32_t * getU32ptr() const { return reinterpret_cast<std::uint32_t*>(pointer); } 
//        std::uint64_t * getU64ptr() const { return reinterpret_cast<std::uint64_t*>(pointer); } 
//        
//        std::uint8_t  getU08val(size_t pos) const { if(isvalid() && (pos < size)) { return getU08ptr()[pos]; } else { return 0; } }      
//        std::uint16_t getU16val(size_t pos) const { if(isvalid() && ((2*pos) < size)) { return getU16ptr()[pos]; } else { return 0; } }
//        std::uint32_t getU32val(size_t pos) const { if(isvalid() && ((4*pos) < size)) { return getU32ptr()[pos]; } else { return 0; } }       
//        std::uint64_t getU64val(size_t pos) const { if(isvalid() && ((8*pos) < size)) { return getU64ptr()[pos]; } else { return 0; } }
//        
//        void * getVOIDptr(size_t offset = 0) const { if(isvalid() && (offset < size)) { std::uint8_t *d = getU08ptr(); return &d[offset]; } else { return nullptr;} }      
    };      

    // useful functions
    template<typename E>         // C++14 [Meyers, pag. 73]
    constexpr auto tointegral(E enumerator) noexcept 
    {
        return static_cast<std::underlying_type_t<E>>(enumerator);
    }

    
}} // namespace embot { namespace common {



#endif  // include-guard
#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
