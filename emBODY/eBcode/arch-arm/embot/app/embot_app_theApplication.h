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

#ifndef _EMBOT_APP_THEAPPLICATION_H_
#define _EMBOT_APP_THEAPPLICATION_H_

#include "embot_core.h"
#include "embot_sys.h"
#include "embot_sys_theScheduler.h"
#include "embot_sys_task.h"
#include <memory>


namespace embot { namespace app {
       
    // the method theApplication::execute() does the following:
    // 1. relocates the application at Config::addressofapplication FLASH address
    // 2. calls embot::hw::bsp::init() if Config::initbsp is true 
    // 3. starts the scheduler with Config::ticktime and with Config::behaviour
    // the scheduler does not return, hence nor theApplication::execute()


    class theApplication
    {
    public:
        static theApplication& getInstance();
               
    public:
                        
        struct Config
        {            
            std::uint32_t                       addressofapplication {0};
            bool                                initbsp {true};
            embot::core::relTime              ticktime {0};
            embot::os::theScheduler::Behaviour behaviour {};

            Config() = default;
            Config(std::uint32_t _addr, bool _bsp, embot::core::relTime _tic, const embot::os::theScheduler::Behaviour &_beh) : addressofapplication(_addr), initbsp(_bsp), ticktime(_tic), behaviour(_beh) {}                
            bool isvalid() const { if((false == behaviour.isvalid()) || (0 == ticktime) || (0 == addressofapplication)) { return false; } else { return true; } } 
        }; 
        
        [[noreturn]] void execute(const Config &config); 
        
    private:
        theApplication(); 
        ~theApplication();

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;         
    };       


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
