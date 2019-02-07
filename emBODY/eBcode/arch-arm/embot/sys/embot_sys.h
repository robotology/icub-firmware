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

#ifndef _EMBOT_SYS_H_
#define _EMBOT_SYS_H_

#include "embot_common.h"


namespace embot { namespace sys {

    common::Time timeNow();
    common::relTime tickPeriod();    
    std::uint32_t millisecondsNow();
                 
    class Task;      
    Task* taskRunning();
    
    struct Operation 
    {
        common::Callback    activity;       // the performed activity
        std::uint16_t       stacksize;      // the amount of used stack
        Operation() : activity(nullptr, nullptr), stacksize(512) {}
        Operation(const common::Callback &a, const std::uint16_t s) : activity(a), stacksize(s) {}    
        void clear() { activity.clear(); stacksize = 512; }
        bool isvalid() const { if((false == activity.isvalid()) || (0 == stacksize)) { return false; } else { return true; } }
    };    
    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

