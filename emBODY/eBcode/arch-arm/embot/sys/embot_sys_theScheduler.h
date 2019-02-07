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

#ifndef _EMBOT_SYS_THESCHEDULER_H_
#define _EMBOT_SYS_THESCHEDULER_H_

#include "embot_common.h"
#include "embot_sys.h"
#include <memory>

namespace embot { namespace sys {

    class theScheduler
    {
    public:
        static theScheduler& getInstance();
        
    public:
                
        struct Timing
        {
            std::uint32_t       clockfrequency;     // it must be equal to the cpu speed expressed in hz: use embot::hw::sys::clock(embot::hw::sys::CLOCK::syscore)
            common::relTime     ticktime;           // the resolution of the scheduler
            Timing() : clockfrequency(168000000), ticktime(embot::common::time1millisec) {}
            Timing(std::uint32_t c, common::relTime t = embot::common::time1millisec) : clockfrequency(c), ticktime(t) {}
            void clear() { clockfrequency = 168000000; ticktime = embot::common::time1millisec; }
            bool isvalid() const { if((0 == clockfrequency) || (0 == ticktime)) { return false; } else { return true; } }
        };
        
        struct Config
        {
            Timing timing;
            embot::sys::Operation oninit;
            embot::sys::Operation onidle;
            embot::sys::Operation onfatal;   // you may not specify it ...  
            Config() {}
            Config(const Timing &tim, const Operation &ini, const Operation &idl, const Operation &fat) : timing(tim), oninit(ini), onidle(idl), onfatal(fat) {}
            bool isvalid() const { if((false == timing.isvalid()) || (false == oninit.isvalid()) || (false == onidle.isvalid())) { return false; } else { return true; } }
            void clear() { timing.clear(); oninit.clear(); onidle.clear(); onfatal.clear(); }
        }; 
        
        
        bool start(const Config &config);    // it does not return ... unless the config is not valid
        bool started() const;
        
    private:
        theScheduler();  
        ~theScheduler(); // i dont want a fool can delete it

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;   
    };   

}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
