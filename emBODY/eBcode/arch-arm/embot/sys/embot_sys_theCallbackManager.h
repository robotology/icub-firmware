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

#ifndef _EMBOT_SYS_THECALLBACKMANAGER_H_
#define _EMBOT_SYS_THECALLBACKMANAGER_H_

#include "embot_core.h"
#include "embot_sys.h"
#include "embot_sys_Task.h"
#include <memory>

namespace embot { namespace os {
        
    class theCallbackManager
    {
    public:
        static theCallbackManager& getInstance();
        
    public:
        struct Config
        {
            embot::os::Priority    priority {embot::os::Priority::system230};
            std::uint16_t           stacksize {1024};
            std::uint16_t           capacityofhandler {8};          
            Config() = default;
            constexpr Config(std::uint16_t ss, std::uint16_t cap, embot::os::Priority prio = embot::os::Priority::system230)
                : stacksize(ss), capacityofhandler(cap), priority(prio) {}
            bool isvalid() const
            {
                return ((0 == stacksize) || (0 == capacityofhandler) || (false == embot::os::isSystem(priority))) ? false : true;
            }
        }; 
        
        
        bool start(const Config &config);    
        bool started() const;    
        
        embot::os::Thread * task() const;

    private:
        theCallbackManager();  
        ~theCallbackManager(); // i dont want a fool can delete it
    
    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;      
    };  

}} // namespace embot { namespace os {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
