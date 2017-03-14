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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_theApplication.h"

#include "embot.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


//void onIdle(void)
//{
//}

struct embot::app::theApplication::Impl
{    
    Config config;
    
    Impl() 
    {              
        config.userdeflauncher.callback = nullptr;
        config.userdeflauncher.arg = nullptr;
    }
            
    static void onidle(void);    
    static void onfatal(void);    
    static void osalstarter(void);
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::app::theApplication::theApplication()
: pImpl(new Impl)
{
       
}

        
void embot::app::theApplication::execute(Config &config)
{
    pImpl->config = config;
    
    // now we init the hw, we start the scheduler, we start a countdown with sys restart at the end ... we exec the activity ...
    //embot::hw::sys::relocatevectortable(embot::hw::sys::addressOfApplication - embot::hw::sys::startOfFLASH);
    
    embot::hw::bsp::init();
    
    
    embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();
    embot::sys::theScheduler::Config cfg;
    cfg.launcher = embot::app::theApplication::Impl::osalstarter;
    cfg.launcherstacksize = 2048;
    cfg.onidle = embot::app::theApplication::Impl::onidle;
//    cfg.onidle = onIdle;
    cfg.onidlestacksize = 512;
    cfg.onfatalerror = embot::app::theApplication::Impl::onfatal;
    cfg.clockfrequency = embot::hw::sys::clock();
    cfg.ticktime = config.osaltickperiod;
    
    thesystem.init(cfg);
    thesystem.start();    

    for(;;);
}
  


void embot::app::theApplication::Impl::onidle(void)
{
    
}

void embot::app::theApplication::Impl::onfatal(void)
{
    
}

void embot::app::theApplication::Impl::osalstarter(void)
{
    embot::app::theApplication &handle  = embot::app::theApplication::getInstance();
    
    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();

    embot::sys::theTimerManager::Config tmrmanconfig;

    tmrman.init(tmrmanconfig);
    tmrman.start();
    
    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
    cbkman.start();    
    

    if (nullptr != handle.pImpl->config.userdeflauncher.callback)
    {
        handle.pImpl->config.userdeflauncher.callback(handle.pImpl->config.userdeflauncher.arg);
    }
      
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


