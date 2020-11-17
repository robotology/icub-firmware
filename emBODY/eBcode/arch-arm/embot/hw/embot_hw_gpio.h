
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

#ifndef _EMBOT_HW_GPIO_H_
#define _EMBOT_HW_GPIO_H_

#include "embot_core.h"
#include "embot_hw_types.h"

namespace embot { namespace hw { namespace gpio {
    
    
    enum class State : std::uint8_t { RESET = 0, SET = 1 };
    
    // look at stm32's GPIO_mode_define
    enum class Mode : std::uint8_t { 
        INPUT = 0,
        OUTPUTpushpull = 1, 
        OUTPUTopendrain = 2, 
        //AFpushpull = 3,
        //AFopendrain = 4,
        //ANALOG = 5,
        EXTIrising = 6,
        EXTIfalling = 7,
        EXTIrisingfalling = 8,
        //EVTrising = 9,
        //EVTfalling = 10,
        //EVTrisingfalling = 11
    };
     
    // look at GPIO_pull_define
    enum class Pull : std::uint8_t { 
        nopull = 0, 
        pullup = 1, 
        pulldown = 2 
    };
    
    enum class Speed : uint8_t {
        low = 0,
        medium = 1,
        high = 2,
        veryhigh = 3
    };   

    struct Config
    {
        Mode mode {Mode::OUTPUTopendrain};
        Pull pull {Pull::nopull};
        Speed speed {Speed::medium};
        constexpr Config(Mode m, Pull p, Speed s) : mode(m), pull(p), speed(s) {}
    };
    
    bool supported(const embot::hw::GPIO &g);    
    bool initialised(const embot::hw::GPIO g);    
    result_t init(const embot::hw::GPIO &g, const Config &config);
    
    result_t configure(const embot::hw::GPIO &g, Mode m, Pull p, Speed s);
    
    result_t set(const embot::hw::GPIO &g, State s);
    
    result_t toggle(const embot::hw::GPIO &g);
    
    State get(const embot::hw::GPIO &g);
        
       
}}} // namespace embot { namespace hw { namespace gpio 





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


