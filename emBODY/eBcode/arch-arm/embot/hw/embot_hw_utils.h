
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if 0

## The `embot::hw::utils` library

This library contains utilities for the other drivers.


#endif 

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_UTILS_H_
#define __EMBOT_HW_UTILS_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_core_binary.h"

namespace embot::hw::utils {
        
    struct coreDRV
    {
        uint64_t initialisedmask {0};
        
        constexpr coreDRV() = default;
        constexpr coreDRV(uint64_t m) : initialisedmask(m) {}
            
        constexpr bool initialised(uint8_t s) const
        {
            return embot::core::binary::bit::check(initialisedmask, s);
        }
        
        void setinitialised(uint8_t s, bool on)
        {
            if(true == on)
            {
                embot::core::binary::bit::set(initialisedmask, s);
            }
            else
            {
                embot::core::binary::bit::clear(initialisedmask, s);
            }
        }
        
    };      
        
}   // namespace embot::hw::utils {

    
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


