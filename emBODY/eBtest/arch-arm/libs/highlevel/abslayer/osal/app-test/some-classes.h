
/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _SOMECLASSES_H_
#define _SOMECLASSES_H_


#include "EOaction.h"

#include <cstdint>

namespace embot 
{
    typedef std::uint32_t relTime; // expressed in usec
    enum class timeValue { microsec = 1, millisec = 1000, second = 1000000 };
}

namespace embot 
{
    namespace sys 
    {
        
    class Timer
    {
    public:
        
        enum class Mode { oneshot = 0, forever = 1 };

        enum class Status { idle = 0, running = 1, oneshotcompleted = 2 };
        
        Timer();
        ~Timer();
        bool start(relTime countdown, Mode mode, EOaction *action); 
        bool stop(); 
        
        Status getStatus();

    private:
        
        struct Impl;
        Impl *pImpl;    
    };

    }
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

