
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

#include "embot_core.h"
#include "embot_hw_types.h"


namespace embot { namespace hw { namespace timer {
 

    enum class Mode : uint8_t { oneshot = 0, periodic = 1};   

    enum class Status : uint8_t { none = 0, idle = 1, running = 2, expired = 3 };  
    
    struct Config
    {
        embot::core::relTime time {embot::core::time1millisec};               
        Mode mode {Mode::periodic};             
        embot::core::Callback onexpiry {};
        constexpr Config() = default;
        constexpr Config(embot::core::relTime t, Mode m, const embot::core::Callback &e ) : time(t), mode(m), onexpiry(e) {}         
    };
    
    
    bool supported(embot::hw::TIMER t);    
    bool initialised(embot::hw::TIMER t);    
    result_t init(embot::hw::TIMER t, const Config &config);
    result_t deinit(embot::hw::TIMER t);
    
    // configure() allows to change the configuration after the init()
    result_t configure(embot::hw::TIMER t, const Config &config);
    
    bool isrunning(embot::hw::TIMER t);
    
    result_t start(embot::hw::TIMER t);
    
    result_t stop(embot::hw::TIMER t);
    
    Status status(embot::hw::TIMER t);
    
    // execute() must be placed inside the IRQHandler of the timer
    void execute(embot::hw::TIMER t);
 
}}} // namespace embot { namespace hw { namespace timer { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


