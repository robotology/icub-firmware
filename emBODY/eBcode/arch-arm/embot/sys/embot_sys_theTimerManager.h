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

#ifndef _EMBOT_SYS_THETIMERMANAGER_H_
#define _EMBOT_SYS_THETIMERMANAGER_H_

#include "embot_common.h"
#include "embot_sys.h"
#include "embot_sys_Timer.h"
#include <memory>

namespace embot { namespace sys {
            
    class theTimerManager
    {
    public:
        static theTimerManager& getInstance();
        
    public:
        struct Config
        {
            embot::sys::Priority    priority {embot::sys::Priority::system240};
            std::uint16_t           stacksize {1024};
            std::uint16_t           capacityofhandler {8};          
            Config() = default;
            constexpr Config(std::uint16_t ss, std::uint16_t cap, embot::sys::Priority prio = embot::sys::Priority::system240)
                : stacksize(ss), capacityofhandler(cap), priority(prio) {}
            bool isvalid() const
            {
                return ((0 == stacksize) || (0 == capacityofhandler) || (false == embot::sys::isSystem(priority))) ? false : true;
            }
        }; 
        
        bool start(const Config &config);   

        // this funtion is called by the timer. if false the expiration of the timer is processed internally and not w/ onexpiry()
        bool started() const;        
        
        // this funtion is called only by the timer. if called by someone else it returns false
        // because the action of the timer is enabled only by internals of the timer itself.
        bool onexpiry(const Timer &timer);

    private:
        theTimerManager();  
        ~theTimerManager(); // i dont want a fool can delete it

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;       
    };       


}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
