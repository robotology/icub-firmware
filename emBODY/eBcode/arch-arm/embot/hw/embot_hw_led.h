
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

#ifndef _EMBOT_HW_LED_H_
#define _EMBOT_HW_LED_H_


#include "embot_core.h"
#include "embot_hw_types.h"


namespace embot { namespace hw { namespace led {
        
//    enum class LED : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };

    bool supported(embot::hw::LED led);    
    bool initialised(embot::hw::LED led);
        
    result_t init(embot::hw::LED led);
    result_t on(embot::hw::LED led);
    result_t off(embot::hw::LED led);
    result_t toggle(embot::hw::LED led);        
       
}}} // namespace embot { namespace hw { namespace led 






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


