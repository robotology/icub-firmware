
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

    using Time = std::uint64_t;
    //typedef std::uint64_t Time;     // expressed in usec. it keeps the time of the system since embot::sys::theScheduler::start().
    
    using fpCallback = void (*)(void *);
    using fpWorker = void (*)(void);
    
    struct Callback
    {
        fpCallback  callback;
        void*       arg;
        Callback() : callback(nullptr), arg(nullptr) {}
        Callback(fpCallback _cbk, void *_arg) : callback(_cbk), arg(_arg) {}
    };
    
    using fp_int8_par_uint32 = std::int8_t (*)(std::uint32_t);
    using fp_bool_par_uint32 = bool (*)(std::uint32_t);

    using relTime = std::uint32_t;
    //typedef std::uint32_t relTime;  // expressed in usec. it is used to express relative time. 0 means: 0 usec from ...
    
    const relTime timeWaitNone      = 0;
    const relTime time1microsec     = 1;    
    const relTime time1millisec     = 1000;
    const relTime time1second       = 1000000;
    const relTime timeWaitForever   = 0xffffffff;
    
    //typedef std::uint32_t Event;
    //typedef std::uint32_t Message;
    using Event = std::uint32_t;
    using Message = std::uint32_t;
    
} } // namespace embot { namespace common


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
