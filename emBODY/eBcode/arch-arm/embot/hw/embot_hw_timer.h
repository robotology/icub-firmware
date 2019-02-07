
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
 

    enum class Mode { oneshot = 0, periodic = 1};    
    
    struct Config
    {
        embot::common::relTime      time;               
        Mode                        mode;             
        embot::common::Callback     onexpiry;
        Config() : time(embot::common::time1millisec), mode(Mode::periodic), onexpiry(nullptr, 0) {}         
    };
    
    
    bool supported(embot::hw::TIMER t);
    
    bool initialised(embot::hw::TIMER t);
    
    result_t init(embot::hw::TIMER t, const Config &config);
    
    result_t configure(embot::hw::TIMER t, const Config &config);
    
    bool isrunning(embot::hw::TIMER t);
    
    result_t start(embot::hw::TIMER t);
    
    result_t stop(embot::hw::TIMER t);
    
    // for use of the IRQhandler only 
    void execute(embot::hw::TIMER t);
 
}}} // namespace embot { namespace hw { namespace timer { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


