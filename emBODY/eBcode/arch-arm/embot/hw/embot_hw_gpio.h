
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
    
    // State contains stm32's values such as GPIO_PIN_RESET etc.
    enum class State : std::uint8_t { RESET = 0, SET = 1 };
    
    // Mode contains stm32's defines such as GPIO_MODE_INPUT etc.
    // we dont support them all
    enum class Mode : std::uint8_t { 
        INPUT = 0,
        OUTPUTpushpull = 1, 
        OUTPUTopendrain = 2, 
        //AFpushpull = 3,
        //AFopendrain = 4,
        //ANALOG = 5,
        //ANALOGADCcontrol = 6,
        EXTIrising = 7,
        EXTIfalling = 8,
        EXTIrisingfalling = 9,
        //EVTrising = 10,
        //EVTfalling = 11,
        //EVTrisingfalling = 12
        none = 255
    };
     
    // Pull contains stm32's defines such as GPIO_NOPULL etc.
    enum class Pull : std::uint8_t { 
        nopull = 0, 
        pullup = 1, 
        pulldown = 2,
        none = 255
    };
    
    // Speed contains stm32's defines such as GPIO_SPEED_FREQ_LOW etc.
    enum class Speed : uint8_t {
        low = 0,
        medium = 1,
        high = 2,
        veryhigh = 3,
        none = 255
    };   

    struct Config
    {
        Mode mode {Mode::none};
        Pull pull {Pull::none};
        Speed speed {Speed::none};
        constexpr Config(Mode m, Pull p, Speed s) : mode(m), pull(p), speed(s) {}
        constexpr Config() = default; 
        constexpr bool isvalid() const { 
            return (mode != Mode::none) && (pull != Pull::none) && (speed != Speed::none);
        }            
    };
    
    constexpr Config cfgOUTpp {Mode::OUTPUTpushpull, Pull::nopull, Speed::veryhigh};
    
    bool supported(const embot::hw::GPIO &g);    
    bool initialised(const embot::hw::GPIO g);    
    result_t init(const embot::hw::GPIO &g, const Config &config);
    result_t deinit(const embot::hw::GPIO &g);
    
    result_t clockenable(const embot::hw::GPIO &g);
    
    result_t configure(const embot::hw::GPIO &g, Mode m, Pull p, Speed s);
    
    result_t set(const embot::hw::GPIO &g, State s);
    
    result_t toggle(const embot::hw::GPIO &g);
    
    State get(const embot::hw::GPIO &g);
        
       
}}} // namespace embot { namespace hw { namespace gpio 





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


