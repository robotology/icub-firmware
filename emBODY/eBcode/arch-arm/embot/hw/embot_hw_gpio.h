
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

#include "embot_common.h"
#include "embot_hw.h"



namespace embot { namespace hw { namespace gpio {
    
    
    enum class State : std::uint8_t { RESET = 0, SET = 1 };
    
    enum class Mode : std::uint8_t { OUTPUTopendrain = 0, OUTPUTpushpull = 1 };
    
    result_t configure(const embot::hw::GPIO &g, Mode m);
    
    result_t set(const embot::hw::GPIO &g, State s);
    
    State get(const embot::hw::GPIO &g);
       
}}} // namespace embot { namespace hw { namespace gpio 





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


