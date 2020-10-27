
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

#ifndef _EMBOT_HW_H_
#define _EMBOT_HW_H_

// this module initialises the hw layer and enables the use of the drivers: hw::gpio, hw::can, hw::i2c, etc.
// it is however, still a glue layer which allows boards using different HAL, such as the ems and the pmc, to run 
// the embot system.
// the dependency from a specific HAL library (stm32hal or other) is in:
// - embot::hw::board which calls initialization of the HAL, 
// - embot::bsp which tells which drivers are availale,
// - in the actual drivers embot::gpio, embot::can, embo::i2c etc.

#include "embot_core.h"

namespace embot { namespace hw {
    
    struct Config
    {         
        embot::core::fpWorker initmicrotime {nullptr};  
        embot::core::fpGetU64 get1microtime {nullptr};         
        
        constexpr Config() = default;
        constexpr Config(embot::core::fpWorker _init, embot::core::fpGetU64 _tmicro) : initmicrotime(_init), get1microtime(_tmicro) {}
        bool isvalid() const { if(nullptr != get1microtime) { return true; } else { return false; } }
    }; 
            
    bool initialised();
    
    // it calls the proper initialisations for the hw layer and links its to embot::core
    // after this call you can use the hw drivers
    bool init(const Config &config);       
}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


