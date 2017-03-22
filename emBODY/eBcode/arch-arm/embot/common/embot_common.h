
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

#ifndef _EB_COMMON_H_
#define _EB_COMMON_H_

#include <cstdint>

namespace embot { namespace common {


    using fpWorker      = void (*)(void);
    using fpCallback    = void (*)(void *);
    using fpGetU32      = std::uint32_t (*)(void);
    using fpParU32      = void (*)(std::uint32_t);
    using fpBoolParU32  = bool (*)(std::uint32_t);
    using fpI08ParU32   = std::int8_t (*)(std::uint32_t);
    
//    using fp_int8_par_uint32 = std::int8_t (*)(std::uint32_t);
//    using fp_bool_par_uint32 = bool (*)(std::uint32_t);
    
    struct Callback
    {
        fpCallback  callback;
        void*       arg;
        Callback() : callback(nullptr), arg(nullptr) {}
        Callback(fpCallback _cbk, void *_arg) : callback(_cbk), arg(_arg) {}
    };
    
    using Time          = std::uint64_t;    // expressed in usec.  expresses absolute time    
    using relTime       = std::uint32_t;    // expressed in usec. it is used to express relative time. 0 means: 0 usec from ...
    
    const relTime timeWaitNone      = 0;
    const relTime time1microsec     = 1;    
    const relTime time1millisec     = 1000;
    const relTime time1second       = 1000000;
    const relTime timeWaitForever   = 0xffffffff;
    
    using Event = std::uint32_t;
    using Message = std::uint32_t;
            
} } // namespace embot { namespace common


namespace embot { namespace common { namespace bit {
        
    template<typename T>
    void set(T &value, std::uint8_t pos)
    {
        value |= (static_cast<T>(1)<<pos);
    }
    
    template<typename T>
    void clear(T &value, std::uint8_t pos)
    {
        value &= (~(static_cast<T>(1)<<pos));
    }
    
    template<typename T>
    void toggle(T &value, std::uint8_t pos)
    {
        value ^= (static_cast<T>(1)<<pos);
    }
    
    template<typename T>
    bool check(const T value, std::uint8_t pos)
    {
        if(value & (static_cast<T>(1)<<pos))
        {
            return true;
        }
        return false;
    }    
    
   
} } } // namespace embot { namespace common { namespace bit


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
