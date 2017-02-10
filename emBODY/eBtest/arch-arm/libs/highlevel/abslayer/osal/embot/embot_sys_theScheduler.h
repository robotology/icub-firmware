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

namespace embot { namespace sys {

    class theScheduler
    {
    public:
        static theScheduler& getInstance()
        {
            static theScheduler* p = new theScheduler();
            return *p;
        }
        
    public:
        struct Config
        {
            std::uint32_t       clockfrequency;
            common::relTime     ticktime;
            common::fpWorker    launcher;
            std::uint16_t       launcherstacksize;
            common::fpWorker    onidle;
            std::uint16_t       onidlestacksize;
            common::fpWorker    onfatalerror;     
            Config() 
            {
                clockfrequency = 168000000; ticktime = 1000; 
                launcher = nullptr; launcherstacksize = 1024; 
                onidle = nullptr; onidlestacksize = 512;
                onfatalerror = nullptr;
            }
            void setTiming(std::uint32_t _clockfrequency, common::relTime _ticktime = 1000) { clockfrequency = _clockfrequency; ticktime = _ticktime; }
            void setLauncher(common::fpWorker _launcher, std::uint16_t _launcherstacksize = 1024) { launcher = _launcher; launcherstacksize = _launcherstacksize; }
            void setOnIdle(common::fpWorker _onidle, std::uint16_t _onidlestacksize = 512) { onidle = _onidle; onidlestacksize = _onidlestacksize; }
            void setOnFatal(common::fpWorker _onfatalerror) { onfatalerror = _onfatalerror; }
        }; 
        
        bool init(Config &config);  // can be called many times but not after start(). if so, it does nothing
        
        void start();    // it does not return ... unless the scheduler is already started
        
        bool isStarted();
        
        common::relTime getTick();

    private:
        theScheduler();  

    public:
        // remove copy constructors and copy assignment operators 
        theScheduler(const theScheduler&) = delete;
        theScheduler(theScheduler&) = delete;
        void operator=(const theScheduler&) = delete;
        void operator=(theScheduler&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };   

}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
