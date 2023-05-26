
/*
 * Copyright (C) 2023 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
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

#ifndef _EMBOT_HW_TESTPOINT_H_
#define _EMBOT_HW_TESTPOINT_H_


#include "embot_core.h"
#include "embot_hw_types.h"


namespace embot { namespace hw { namespace testpoint {
        
    bool supported(embot::hw::TESTPOINT testpoint);    
    bool initialised(embot::hw::TESTPOINT testpoint);        
    result_t init(embot::hw::TESTPOINT testpoint);
    
    result_t on(embot::hw::TESTPOINT testpoint);
    result_t off(embot::hw::TESTPOINT testpoint);
    result_t toggle(embot::hw::TESTPOINT testpoint);        
       
}}} // namespace embot { namespace hw { namespace testpoint





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


