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

#ifndef _EMBOT_SYS_TIMER_H_
#define _EMBOT_SYS_TIMER_H_

#include "embot_common.h"

#include "embot_sys.h"

#include "embot_sys_Action.h"

namespace embot { namespace sys {
    
    class Timer
    {
    public:
        
        enum class Type { oneshot = 0, forever = 1 };
        enum class Status { idle = 0, counting = 1, oneshotcompleted = 2 };        
        
        Timer();
        ~Timer();
        bool start(common::relTime countdown, Type type, sys::Action &onexpiry); 
        bool stop();         
        Status getStatus();        
        Type getType();

    private:        
        struct Impl;
        Impl *pImpl;    
    };

    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
