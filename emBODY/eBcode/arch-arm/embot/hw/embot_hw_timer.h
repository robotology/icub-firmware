
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

#ifndef _EMBOT_HW_TIMER_H_
#define _EMBOT_HW_TIMER_H_

#include "embot_common.h"
#include "embot_hw.h"


namespace embot { namespace hw { namespace timer {
 
    
    enum class Timer { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eigth = 7, nine = 8, 
                       ten = 9, eleven = 10, twelve = 11, thirteen = 12, fourteen = 13, fifteen = 14, sixteen = 15, 
                       none = 32, maxnumberof = 16};  

    enum class Mode { oneshot = 0, periodic = 1};    
    
    struct Config
    {
        embot::common::relTime      time;               
        Mode                        mode;             
        embot::common::Callback     onexpiry;
        Config() : time(embot::common::time1millisec), mode(Mode::periodic), onexpiry(nullptr, 0) {}         
    };
    
    
    bool supported(Timer t);
    
    bool initialised(Timer t);
    
    result_t init(Timer t, const Config &config);
    
    result_t configure(Timer t, const Config &config);
    
    bool isrunning(Timer t);
    
    result_t start(Timer t);
    
    result_t stop(Timer t);
 
}}} // namespace embot { namespace hw { namespace timer { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


