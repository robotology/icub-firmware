
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

#ifndef _EMBOT_BINARY_H_
#define _EMBOT_BINARY_H_

#include <cstdint>
#include <cstring>

#include "embot_common.h"



namespace embot { namespace binary { namespace bit {
       
    // it forces 1 into pos-th bit of value
    template<typename T>
    void set(T &value, std::uint8_t pos)
    {
        value |= (static_cast<T>(1)<<pos);
    }
    
    // it forces 0 into pos-th bit of value
    template<typename T>
    void clear(T &value, std::uint8_t pos)
    {
        value &= (~(static_cast<T>(1)<<pos));
    }
    
    // it toggles the pos-th bit of value
    template<typename T>
    void toggle(T &value, std::uint8_t pos)
    {
        value ^= (static_cast<T>(1)<<pos);
    }
    
    // it returns true if the pos-th bit of value is 1
    template<typename T>
    bool check(const T value, std::uint8_t pos)
    {
        if(value & (static_cast<T>(1)<<pos))
        {
            return true;
        }
        return false;
    } 


    std::uint8_t countUsize(const std::uint64_t value, const std::uint8_t size);

    // it tells how many 1 bits there are value
    template<typename T>
    std::uint8_t count(const T value)
    {
        return countUsize(static_cast<std::uint64_t>(value), sizeof(value));
    }        
       
} } } // namespace embot { namespace binary { namespace bit



namespace embot { namespace binary { namespace mask {
         
    // it forces to 1 the bits in value which are selected by msk (where msk has bits of value 1). the other bits of value stay unchanged.
    template<typename T>
    void set(T &value, const T msk)
    {
        value |= msk;
    }
    
    // it forces to 0 the bits in value which are selected by msk (where msk has bits of value 1). the other bits of value stay unchanged.
    template<typename T>
    void clear(T &value, const T msk)
    {
        value &= (~(msk));
    }
    
    // it toggles the bits in value which are selected by msk (where msk has bits of value 1). the other bits of value stay unchanged.
    template<typename T>
    void toggle(T &value, const T msk)
    {
        value ^= (msk);
    }
    
    // it returns true if the bits in value which are selected by msk are all 1.
    template<typename T>
    bool check(const T value, const T msk)
    {
        if(msk == (value & msk))
        {
            return true;
        }
        return false;
    }    
    
   
} } } // namespace embot { namespace binary { namespace mask



namespace embot { namespace binary { namespace nibble {
    
    using NIBBLE = std::uint8_t;
    
    // it convert any integer into a nibble by getting only its lowest significant 4 bits.
    template<typename T>
    NIBBLE convert(const T &value)
    {
        return static_cast<NIBBLE>(value) & 0x0f;
    }
        
    // it assign nib into the pos-th nibble of value.
    template<typename T>
    void assign(T &value, const NIBBLE nib, std::uint8_t pos)
    {
        embot::binary::mask::clear(value, static_cast<T>(0xf)<<(4*pos));
        embot::binary::mask::set(value, static_cast<T>(nib & 0xf)<<(4*pos));
    }

    // it puts 0x0 into the pos-th nibble of value
    template<typename T>
    void clear(T &value, std::uint8_t pos)
    {
        embot::binary::mask::clear(value, static_cast<T>(0xf)<<(4*pos));
    }
    
    // it returns true if nib is equal to the pos-th nibble of value
    template<typename T>
    bool check(const T value, const NIBBLE nib, std::uint8_t pos)
    {
        return embot::binary::mask::check(value, static_cast<T>(nib & 0xf)<<(4*pos));
    } 
       
} } } // namespace embot { namespace binary { namespace nibble

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
