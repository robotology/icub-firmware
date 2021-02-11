
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_OS_COMMON_H_
#define _EMBOT_OS_COMMON_H_

#include "embot_core.h"
#include "embot_core_binary.h"


namespace embot { namespace os {
                               
    using Event = std::uint32_t;        
    using EventMask = std::uint32_t;    
    using Message = void *;             
    using Value = uint32_t;             
    class Thread;    
  
    // this enum class uses the same values as osPriority_t: {0, 1, 8 -> 55}
    enum class Priority : std::uint8_t {
        none = 0,
        schedIdle = 1,                              // reserved for idle tread and managed by the scheduler
        schedInit = 55,                             // reserved for the init thread and managed by the scheduler
        minimum = 8,  maximum = 54,                 // allowed ranges     
        // low        
        low08 = 8, low09 = 9, low10 = 10, low11 = 11, 
        low12 = 12, low13 = 13, low14 = 14, low15 = 15,   
        // below normal
        belownorm16 = 16, belownorm17 = 17, belownorm18 = 18, belownorm19 = 19, 
        belownorm20 = 20, belownorm21 = 21, belownorm22 = 22, belownorm23 = 23,  
        // normal        
        normal24 = 24, normal25 = 25, normal26 = 26, normal27 = 27, 
        normal28 = 28, normal29 = 29, normal30 = 30, normal31 = 31,   
        // above normal
        abovenorm32 = 32, abovenorm33 = 33, abovenorm34 = 34, abovenorm35 = 35, 
        abovenorm36 = 36, abovenorm37 = 37, abovenorm38 = 38, abovenorm39 = 39,
        // high
        high40 = 40, high41 = 41, high42 = 42, high43 = 43, 
        high44 = 44, high45 = 45, high46 = 46, high47 = 47,        
        // system (realtime in osPriority_t)
        system48 = 48, system49 = 49, system50 = 50, system51 = 51, 
        system52 = 52, system53 = 53, system54 = 54, 
        
        systemMIN = system48, systemMAX = system54
        
        //medium100 = 24, medium101 = 24+1, medium102 = 24+2, medium103 = 24+3, medium104 = 24+4,   // normal          
        //high200 = 40, high201 = 41, high202 = 42, high203i = 43, high204i = 44,                   // high      
        //system220 = 48, system230i = 48+1, system240i = 48+2, system250 = 48+3,                   // realtime
                
    };
    
    constexpr Event bitpos2event(uint8_t bitpos)
    {
        if(bitpos > 32)
            return 0;
        
        return embot::core::binary::mask::pos2mask<embot::os::Event>(bitpos);
    }
        
    
}}

namespace embot { namespace os { namespace priority {
        
    constexpr bool isvalid(uint8_t v)
    {
        if((embot::core::tointegral(Priority::schedIdle) == v) || (embot::core::tointegral(Priority::schedIdle) == v))
        {
            return true;
        }
        if((v >= embot::core::tointegral(Priority::minimum)) && (v <= embot::core::tointegral(Priority::maximum)))
        {
            return true;
        }
        
        return false;              
    }
    
    constexpr Priority convert(uint8_t v)
    {   
        if(true == embot::os::priority::isvalid(v))
        {
            return static_cast<Priority>(v);
        }
        return Priority::none;
    }
    
    constexpr bool isvalid(Priority prio)
    {
        return Priority::none != prio;              
    }   
    
    constexpr bool isScheduler(Priority prio)
    {
        return ((Priority::schedIdle == prio) || (Priority::schedIdle == prio)) ? true : false;       
    }   

    constexpr bool isSystem(Priority prio)
    {
        std::uint8_t v = embot::core::tointegral(prio);
        return ((v >= embot::core::tointegral(Priority::systemMIN)) && (v <= embot::core::tointegral(Priority::systemMAX))) ? true : false;       
    }    
    
}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
