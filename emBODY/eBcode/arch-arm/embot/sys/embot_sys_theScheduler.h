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
#include "embot_sys_task.h"
#include <memory>

namespace embot { namespace sys {
    
    // the method theScheduler::start(cfg) does the following:
    // if already started or if the cfg is not valid: it stays in a forever loop otherwise:
    // 1. starts the OS with cfg.timing.ticktime, sets the idle task as specified by cfg.behaviour.idleconfig
    // 2. executes at maximum priority the init task as specified by cfg.behaviour.initconfig
    // after the function cfg.behaviour.initconfig.startup(cfg.behaviour.initconfig.param) returns, the init task
    // is deleted and the OS schedules any task started inside startup() or the idle task, depending on their
    // priority and ready/blocked status. the OS executte the ready task w/ highest priority.    
    // the method theScheduler::start(cfg) does does not return

    class theScheduler
    {
    public:
        static theScheduler& getInstance();
        
    public:
                            
        struct Timing
        {
            std::uint32_t clockfrequency {10000000};                        // it must be equal to the cpu speed expressed in hz: use embot::hw::sys::clock(embot::hw::sys::CLOCK::syscore)
            embot::common::relTime ticktime {embot::common::time1millisec}; // the resolution of the scheduler
            
            Timing() = default;
            Timing(std::uint32_t c, common::relTime t = embot::common::time1millisec) : clockfrequency(c), ticktime(t) {}
            void clear() { clockfrequency = 10000000; ticktime = embot::common::time1millisec; }
            bool isvalid() const { if((0 == clockfrequency) || (0 == ticktime)) { return false; } else { return true; } }
        };
        
        struct Behaviour
        {
            embot::sys::InitTask::Config initconfig {};
            embot::sys::IdleTask::Config idleconfig {};
            embot::common::Callback onOSerror {};
            
            Behaviour() = default;
            Behaviour(const embot::sys::InitTask::Config &ini, const embot::sys::IdleTask::Config &idl, const embot::common::Callback &err) : initconfig(ini), idleconfig(idl), onOSerror(err) {}            
            bool isvalid() const { if((false == initconfig.isvalid()) || (false == idleconfig.isvalid())) { return false; } else { return true; } }
        };
        
        struct Config
        {
            Timing timing {};
            Behaviour behaviour {};  
            
            Config() = default;
            Config(const Timing &tim, const Behaviour &beh) : timing(tim), behaviour(beh) {}
            bool isvalid() const { if((false == timing.isvalid()) || (false == behaviour.isvalid())) { return false; } else { return true; } }
        }; 
        
        
        [[noreturn]] void start(const Config &config);  
        
        bool started() const;
        
        embot::common::relTime ticktime() const;
        
        Task * scheduledtask() const;    

        const char * getOSerror(int &errorcode) const;
        
    private:
        theScheduler();  
        ~theScheduler(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;   
    };   

}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
