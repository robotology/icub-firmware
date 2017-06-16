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

namespace embot { namespace sys {
    
    
    
    class theTimerManager
    {
    public:
        static theTimerManager& getInstance()
        {
            static theTimerManager* p = new theTimerManager();
            return *p;
        }
        
    public:
        struct Config
        {
            std::uint8_t        priority;
            std::uint16_t       stacksize;
            std::uint16_t       capacityofhandler;          
            Config() : priority(240), stacksize(1024), capacityofhandler(8) {}
        }; 
        
        bool init(Config &config);
        
        bool start();    

    private:
        theTimerManager();  

    public:
        // remove copy constructors and copy assignment operators
        theTimerManager(const theTimerManager&) = delete;
        theTimerManager(theTimerManager&) = delete;
        void operator=(const theTimerManager&) = delete;
        void operator=(theTimerManager&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
