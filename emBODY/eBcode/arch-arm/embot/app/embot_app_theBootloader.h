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

#ifndef _EMBOT_APP_THEBOOTLOADER_H_
#define _EMBOT_APP_THEBOOTLOADER_H_

#include "embot_common.h"

#include "embot_sys.h"

#include "embot_sys_theJumper.h"

#include <new>

namespace embot { namespace app {
    
    
    
    class theBootloader
    {
    public:
        static theBootloader& getInstance()
        {
            static theBootloader* p = new theBootloader();
            return *p;
        }
        
        
    public:
        struct Config
        {
            embot::common::Callback             userdeflauncher;
            embot::common::relTime              countdown;    
            Config() : userdeflauncher(nullptr, nullptr), countdown(5*embot::common::time1second) {}
            Config(embot::common::fpCallback _userdeflauncher, void* _param, embot::common::relTime _countdown = 5*embot::common::time1second) :  userdeflauncher(_userdeflauncher, _param), countdown(_countdown) {}
        }; 
        
        //enum class Command { none = 0, jump = 1, stay = 2};
        
        embot::sys::theJumper::Command getcommand(std::uint32_t &parameter);
        bool jump(std::uint32_t address);
        
        void execute(Config &config); // the activity and after countdown we jump ... it never returns ..
        
        bool stopcountdown(); // unless someone decide to stop the countdown

    private:
        theBootloader(); 
        void *operator new(std::size_t size) throw(std::bad_alloc);    

    public:
        // remove copy constructors and copy assignment operators
        theBootloader(const theBootloader&) = delete;
        theBootloader(theBootloader&) = delete;
        void operator=(const theBootloader&) = delete;
        void operator=(theBootloader&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
