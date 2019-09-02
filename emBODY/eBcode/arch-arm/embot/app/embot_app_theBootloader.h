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
#include "embot_app_theJumper.h"
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
            embot::common::Callback userdeflauncher {nullptr, nullptr};
            embot::common::relTime  countdown {5*embot::common::time1second};    
            Config() = default;
            Config(const embot::common::Callback &_launcher, embot::common::relTime _countdown = 5*embot::common::time1second) : userdeflauncher(_launcher), countdown(_countdown) {}
        }; 
        
        enum class evalRes : uint8_t { jumpaftercountdown = 0, stayforever = 1, jumpfailed = 2, unexected = 3 }; 
        
        // this function checks what to do. it may return one of evalRes values or ... just execute a jump in its internals
        evalRes eval();
                
        // this function just executes the application and never returns ....
        // it inits embot::hw, the osal, it starts countdown timer and executes a user-defined task ... if countdown timer is not stopped 
        // its expiry callback calls the same code inside restart2application() 
        [[noreturn]] void execute(Config &config);         
        
        // its get any command issued before reset by either the application or the bootloader itself.
        embot::app::theJumper::Command getcommand(std::uint32_t &parameter);
        
        // it just jumps to address returned by getcommand(). it must be used only if embot::hw was not initialised, hence before ::execute()...
        bool jump(std::uint32_t address); 
                
        // it can be used by a user-defined task started by execute() to stop the countdown, and stay in the process forever or until ... call of restart2application();
        bool stopcountdown(); 
        
        // we can call it to start (again, maybe after it was stoopped) teh countdown. we can use this with 100 ms timeout so that we can send back an ack to canlaoder and then restart
        bool startcountdown(embot::common::relTime countdown); 
        
        // it is used to force a jump to the application but passing through a restart of the mpu. 
        // IMPORTANT: you can use this after the ::execute() function is called.
        bool restart2application(); // used when the bootloader has already stopped the countdown and after fw update wants to jump to application

    private:
        theBootloader(); 
        void *operator new(std::size_t size) noexcept(false);    

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
