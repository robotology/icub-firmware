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

#ifndef _EMBOT_SYS_H_
#define _EMBOT_SYS_H_

#include "embot_common.h"


namespace embot { namespace sys {
        
    // tells is the sys already started: aka object embot::sys::theScheduler is started, aka the rtos is active.
    bool started();

    // tells time in us since bootstrap. gives a valid number only if sys already started.
    common::Time now();
        
    enum class Priority : std::uint8_t {
        schedIdle = 0, schedInit = 255,                         // reserved to the scheduler
        minimum = 2,  maximum = 251,                            // allowed ranges         
        low010 = 10, low011 = 11, low012 = 12,               
        medium100 = 100, medium101 = 101, medium102 = 102, medium103 = 103, medium104 = 104,                
        high200 = 200, high201 = 201, high203 = 203, high204 = 204,        
        system220 = 220, system230 = 230, system240 = 240, system250 = 250,
        systemMIN = system220, systemMAX = system250        
    };
    
    
    constexpr bool isSystem(Priority prio)
    {
        std::uint8_t v = embot::common::tointegral(prio);
        return ((v >= embot::common::tointegral(Priority::systemMIN)) && (v <= embot::common::tointegral(Priority::systemMAX))) ? true : false;       
    }        
    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

