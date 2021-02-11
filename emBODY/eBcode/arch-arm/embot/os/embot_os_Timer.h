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

#ifndef _EMBOT_OS_TIMER_H_
#define _EMBOT_OS_TIMER_H_

#include "embot_core.h"

#include "embot_os.h"

#include "embot_os_Action.h"

namespace embot { namespace os {
    
    class Timer
    {
    public:
        
        enum class Status { idle = 0, counting = 1 }; 
        
        enum class Mode { oneshot = 0, someshots = 1, forever = 2 }; 
        
        struct Config
        {
            core::relTime   countdown {0};          // time to count before execute the action
            os::Action      onexpiry {};            // the action executed at expiry of the countdown
            Mode            mode {Mode::oneshot};   // the mode. we allow one shot, infinite shots or a limited number
            std::uint32_t   numofshots {0};         // in case of limited number this number tells how many. not zero....

            Config() = default;
            Config(core::relTime c, const os::Action &a, Mode m = Mode::oneshot, std::uint32_t n = 0) : countdown(c), onexpiry(a), mode(m), numofshots(n) {} 
            void clear() { countdown = 0;  onexpiry.clear(); mode = Mode::oneshot; numofshots = 0;}
            bool isvalid() const 
            { 
                if((0 == countdown) || (false == onexpiry.isvalid()) || ((Mode::someshots == mode) && (0 == numofshots))) 
                { 
                    return false; 
                }
                else 
                { 
                    return true; 
                }  
            }
        };
      
        Timer();
        ~Timer();
        
    
        bool start(const Config &config, bool forcerestart = false); // standard mode: it loads a config and starts the timer      
        
        bool load(const Config &config); // it just loads a config w/out starting anything. if Status::counting it stops the timer before loading config 
        bool start(); // it start/restart the loaded config 

        bool stop(); // it stops the timer. it does not unload the config 
                          
        Status getStatus() const;              
        const Config& getConfig() const;
        
    public: 
        bool execute(); // to be called by the TimerManager only. if a brave user calls it ... it does nothing because ... black magic woman   
                

    private:        
        struct Impl;
        Impl *pImpl;    
    };

    
}} // namespace embot { namespace os {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
