
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCFOCM7_H_
#define __EMBOT_HW_BSP_AMCFOCM7_H_

#include "embot_core.h"
#include "embot_hw_types.h"

namespace embot { namespace hw { namespace bsp { namespace amcfocm7 {
    
    // adds in here constants or functions specific to the board
    constexpr embot::hw::LED blinkLED {embot::hw::LED::one};
    
    struct OnSpecialize 
    {
        enum class CM4MODE { donothing = 0, activateandhold = 1, release = 2};
        
        CM4MODE cm4mode {CM4MODE::donothing};
        bool enableJ5powersupply_at_startup {false};
        bool enableJ5spi_123_at_startup {false};  
        constexpr OnSpecialize() = default;        
        constexpr OnSpecialize(CM4MODE cm4m, bool j5pow, bool j5spi) :
            cm4mode(cm4m),
            enableJ5powersupply_at_startup(j5pow),
            enableJ5spi_123_at_startup(j5spi) 
            {}
    };
    
    constexpr OnSpecialize OnSpecLoader 
    {
        OnSpecialize::CM4MODE::activateandhold, false, false
    };

    constexpr OnSpecialize OnSpecUpdater 
    {
        OnSpecialize::CM4MODE::donothing, false, false
    };

    constexpr OnSpecialize OnSpecApplication 
    {
        OnSpecialize::CM4MODE::release, true, false
    };
    
    void set(const OnSpecialize& onsp);
    
}}}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



