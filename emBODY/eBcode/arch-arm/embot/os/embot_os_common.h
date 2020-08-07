
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_OS_COMMON_H_
#define _EMBOT_OS_COMMON_H_

#include "embot_core.h"


namespace embot { namespace os {
        
    using Event         = std::uint32_t;
    using EventMask     = std::uint32_t;
    using Message       = void *;
    class Thread;
  
    enum class Priority : std::uint8_t {
        schedIdle = 1, schedInit = 48+7,                         // reserved to the scheduler
        minimum = 8,  maximum = 48+6,                            // allowed ranges         
        low010 = 8, low011 = 8+1, low012 = 8+2,                                                     // low              
        medium100 = 24, medium101 = 24+1, medium102 = 24+2, medium103 = 24+3, medium104 = 24+4,     // normal          
        high200 = 40, high201 = 41, high202 = 42, high203 = 43, high204 = 44,                       // high      
        system220 = 48, system230 = 48+1, system240 = 48+2, system250 = 48+3,                       // realtime
        systemMIN = system220, systemMAX = system250        
    };
        
    constexpr bool isSystem(Priority prio)
    {
        std::uint8_t v = embot::core::tointegral(prio);
        return ((v >= embot::core::tointegral(Priority::systemMIN)) && (v <= embot::core::tointegral(Priority::systemMAX))) ? true : false;       
    }

    constexpr Priority topriority(uint8_t prio)
    {   // we should check vs the converted value ....
        return static_cast<Priority>(prio);
    }
    
}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
