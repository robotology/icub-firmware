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

#ifndef __EMBOT_OS_TIMER_H_
#define __EMBOT_OS_TIMER_H_

#include "embot_core.h"

#include "embot_os.h"

#include "embot_os_Action.h"

namespace embot { namespace os {
    
    class theTimerManager;
    
    class Timer
    {
    public:
        
        enum class Status { idle = 0, counting = 1 }; 
        
        enum class Mode { oneshot = 0, someshots = 1, forever = 2 }; 
        
        static constexpr char noname[] {"os::Timer::JohnDoe"};
        
        struct Config
        {
            core::relTime   countdown {0};          // time to count before execute the action
            os::Action      onexpiry {};            // the action executed at expiry of the countdown
            Mode            mode {Mode::oneshot};   // we allow one shot, infinite shots or a limited number
            size_t          numofshots {0};         // in case of limited number this number tells how many. not zero ....
            
            constexpr Config() = default;
            constexpr Config(core::relTime c, const os::Action &a, Mode m = Mode::oneshot, size_t n = 0) 
                : countdown(c), onexpiry(a), mode(m), numofshots(n) {} 
            void clear() { countdown = 0;  onexpiry.clear(); mode = Mode::oneshot; numofshots = 0; }
            constexpr bool isvalid() const 
            { 
                bool notvalid {(0 == countdown) || (false == onexpiry.isvalid()) || ((Mode::someshots == mode) && (0 == numofshots))};
                return !notvalid;
            }
        };
      
        Timer();
        ~Timer();
        
        // it sets a name. if the name is not set, name() will return Timer::noname
        bool name(const char *na);
        
        // it loads a config and starts the timer
        bool start(const Config &config, bool forcerestart = false);      
        
        // it loads a config w/out starting. if Status::counting == status(), it calls stop() before loading config
        bool load(const Config &config);  
        
        // it starts using the loaded config. if Status::counting == status() it calls stop() before starting
        bool start();   
        
        // it stops the timer. it does not unload the config 
        bool stop();        

        // it gets the name
        const char * name() const;
        // it gets the loaded config         
        const Config& config() const;
        // it gets status
        Status status() const;          
        // it gets the number of executed shots. the number is nonzero only if Status::counting == status()
        size_t shots() const;
        
    private:         
        // it must be called only by the os::theTimerManager so that it can execute the timer
        bool execute();       
        friend class theTimerManager;                

    private:        
        struct Impl;
        Impl *pImpl;    
    };

    
}} // namespace embot { namespace os {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
