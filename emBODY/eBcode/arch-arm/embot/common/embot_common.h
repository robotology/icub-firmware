
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

namespace embot { namespace common {


    using fpWorker      = void (*)(void);
    using fpCallback    = void (*)(void *);
    using fpGetU32      = std::uint32_t (*)(void);
    using fpGetU64      = std::uint64_t (*)(void);
    using fpParU32      = void (*)(std::uint32_t);
    using fpBoolParU32  = bool (*)(std::uint32_t);
    using fpI08ParU32   = std::int8_t (*)(std::uint32_t);
    
    struct Callback
    {
        fpCallback  callback;
        void*       arg;
        Callback() : callback(nullptr), arg(nullptr) {}
        Callback(fpCallback _cbk, void *_arg) : callback(_cbk), arg(_arg) {}
        void load(fpCallback _cbk, void *_arg) { callback = _cbk; arg = _arg; }
        void clear() { callback = nullptr; arg = nullptr; }
        bool isvalid() const { if(nullptr != callback){ return true; } else { return false; } } 
        void execute() { if(true == isvalid()) { callback(arg); } }
    };
    
    struct Data
    {
        void * pointer;
        std::uint32_t size;
        
        Data() : pointer(nullptr), size(0) {}
        Data(void *p, std::uint32_t s) : pointer(p), size(s) {}
            
        void load(void *p, std::uint32_t s) { pointer = p; size = s; }
        void clear() { pointer = nullptr; size = 0; }
        bool isvalid() const { if((nullptr != pointer) && (0 != size)){ return true; } else { return false; } } 
        
        std::uint8_t  * getU08() { return reinterpret_cast<std::uint8_t*>(pointer); }  
        std::uint16_t * getU16() { return reinterpret_cast<std::uint16_t*>(pointer); } 
        std::uint32_t * getU32() { return reinterpret_cast<std::uint32_t*>(pointer); } 
        std::uint64_t * getU64() { return reinterpret_cast<std::uint64_t*>(pointer); } 
        
        std::uint8_t  U08(std::uint32_t i) { return getU08()[i]; }      
        std::uint16_t U16(std::uint32_t i) { return getU16()[i]; }
        std::uint32_t U32(std::uint32_t i) { return getU32()[i]; }        
        std::uint64_t U64(std::uint32_t i) { return getU64()[i]; }
        
        void * get(std::uint32_t offset = 0) { std::uint8_t *d = getU08(); return &d[offset]; }        
    };
    

    template<typename T>
    struct Triple
    {
        T   x;
        T   y;
        T   z;
        Triple() { reset(); }
        Triple(T _x, T _y, T _z) { load(_x, _y, _z); }
        void load(T _x, T _y, T _z) { x = _x; y = _y; z = _z; }  
        void reset() { x = 0; y = 0; z = 0; }
        void load(void *littleendianmemory) { T *p = reinterpret_cast<T*>(littleendianmemory); x = p[0]; y = p[1]; z = p[2]; }
    }; 

    template<typename T>
    struct Quadruple
    {
        T   w;
        T   x;
        T   y;
        T   z;
        Quadruple() { reset(); }
        Quadruple(T _w, T _x, T _y, T _z) { load(_w, _x, _y, _z); }
        void load(T _w, T _x, T _y, T _z) { w = _w; x = _x; y = _y; z = _z; }  
        void reset() { w = 0; x = 0; y = 0; z = 0; }
        void load(void *littleendianmemory) { T *p = reinterpret_cast<T*>(littleendianmemory); w = p[0]; x = p[1]; y = p[2]; z = p[3]; }
    };     
    
    using Time          = std::uint64_t;    // expressed in usec.  expresses absolute time    
    using relTime       = std::uint32_t;    // expressed in usec. it is used to express relative time. 0 means: 0 usec from ...
    
    const relTime timeWaitNone      = 0;
    const relTime time1microsec     = 1;    
    const relTime time1millisec     = 1000;
    const relTime time1second       = 1000000;
    const relTime timeWaitForever   = 0xffffffff;
    
    using Event = std::uint32_t;
    using EventMask = std::uint32_t;
    using Message = std::uint32_t;
            
} } // namespace embot { namespace common {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
