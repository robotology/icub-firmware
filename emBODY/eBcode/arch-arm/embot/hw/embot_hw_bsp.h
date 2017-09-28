
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

#ifndef _EMBOT_HW_BSP_H_
#define _EMBOT_HW_BSP_H_

#include "embot_common.h"
#include "embot_hw.h"


namespace embot { namespace hw { namespace bsp {
    
    struct Config
    {
        embot::common::fpWorker     init1mstick;
        embot::common::fpGetU32     get1mstick; 
        Config() : init1mstick(nullptr), get1mstick(nullptr) {}
        Config(embot::common::fpWorker _init1mstick, embot::common::fpGetU32 _get1mstick) : init1mstick(_init1mstick), get1mstick(_get1mstick) {}
    }; 
    
        
    bool initialised();
    
    result_t init(const Config &config);
        
      
}}} // namespace embot { namespace hw { namespace bsp {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


