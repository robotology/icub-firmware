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

#ifndef _EMBOT_APP_THELOOP_H_
#define _EMBOT_APP_THELOOP_H_

#include "embot_common.h"

#include "embot_sys.h"

#include "embot_sys_Task.h"


namespace embot { namespace app {
       
    
    class theLoop
    {
    public:
        static theLoop& getInstance()
        {
            static theLoop* p = new theLoop();
            return *p;
        }
               
    public:
        struct Config
        {
            embot::sys::Task::Priority          priority;
            std::uint32_t                       stacksize;
            embot::common::relTime              period;
            embot::common::Callback             startup;
            embot::common::Callback             activity;
            Config() : priority(200), stacksize(4096), period(embot::common::time1millisec), startup(nullptr, nullptr), activity(nullptr, nullptr) {}
            Config(embot::sys::Task::Priority _priority, std::uint32_t _stacksize, embot::common::relTime _period, 
                   embot::common::fpCallback _startup, void* _params, embot::common::fpCallback _activity, void* _param) : 
                   priority(_priority), stacksize(_stacksize), period(_period), startup(_startup, _params), activity(_activity, _param) {}
        }; 
                      
        bool start(Config &config);
        
    private:
        theLoop(); 
 
    public:
        // remove copy constructors and copy assignment operators
        theLoop(const theLoop&) = delete;
        theLoop(theLoop&) = delete;
        void operator=(const theLoop&) = delete;
        void operator=(theLoop&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
