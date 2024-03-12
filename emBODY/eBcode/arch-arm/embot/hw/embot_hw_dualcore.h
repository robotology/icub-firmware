
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_DUALCORE_H_
#define __EMBOT_HW_DUALCORE_H_

#include "embot_core.h"
#include "embot_hw_types.h"



namespace embot::hw::dualcore {
        
    enum class CORE : uint8_t { cm4 = 0, cm7 = 1, none = 31, maxnumberof = 2 };
    enum class BOOT : uint8_t { cm4master = 0, cm7master = 1, none = 31 };
    
    // what is actually executed .. just before embot:hw::bsp::specialize()
    struct Config
    {   
        enum class HW : uint8_t { dontinit = 0, forceinit = 1 };
        enum class CMD : uint8_t { donothing = 0, activate = 1, activateandhold = 2, release = 3 };
        HW hw {HW::dontinit};
        CMD othercore {CMD::donothing};
        constexpr Config() = default;
        constexpr Config(HW h, CMD c) : hw(h), othercore(c) {} 
    };

    
    
    // standard api
    bool supported();    
    
   
    // others
    CORE core();
    BOOT boot();
    
    // it must be called in main() before start()
    bool config(const Config &on);
    
    // it must be called just after embot::hw::bsp::DRIVER::init() or even better instead that
    bool start();
    
    bool start2();
    
   
} // namespace embot::hw::dualcore {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


