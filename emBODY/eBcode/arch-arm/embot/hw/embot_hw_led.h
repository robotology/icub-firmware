
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


#include "embot_common.h"
#include "embot_hw.h"


namespace embot { namespace hw { namespace led {
    
    // the namespace ::led can be used as template for development of simple hw features.
    // follow these rules:
    //
    // 1. if more that one entity is [may be] available, define an enum with always: none and maxnumberof
    // 2. expose a bool returning supported() function
    // 3. expose a bool returning initialised() funtion
    // 4. expose a result_t returning init() function
    // 5. design the needed functions. they shall return embot::hw::result_t (0 is ok, negative is bad).
    
    enum class LED { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, none = 32, maxnumberof = 8 };

    bool supported(LED led);
    
    bool initialised(LED led);
        
    result_t init(LED led);
    result_t on(LED led);
    result_t off(LED led);
    result_t toggle(LED led);
        
    result_t init_legacy(LED led, const void *par);
       
}}} // namespace embot { namespace hw { namespace led 






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


