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
#include "embot_hw_sys.h"
#include "embot_hw_bsp.h"

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
    if(pImpl->config.addressofapplication > embot::hw::sys::startOfFLASH)
    {
        std::uint32_t vectorlocation = pImpl->config.addressofapplication - embot::hw::sys::startOfFLASH;
        if(0 != vectorlocation)
        {
            embot::hw::sys::relocatevectortable(vectorlocation);
        }
    }
    
    const embot::hw::bsp::stm32halConfig stm32c(nullptr, embot::sys::millisecondsNow);
    embot::hw::bsp::Config cc(stm32c, embot::sys::timeNow);
    embot::hw::bsp::init(cc);
    
    
    embot::sys::theScheduler &thescheduler = embot::sys::theScheduler::getInstance();
    embot::sys::theScheduler::Config cfg;
    cfg.launcher = embot::app::theApplication::Impl::osalstarter;
    cfg.launcherstacksize = pImpl->config.stacksizes.tasksysteminit;
    cfg.onidle = embot::app::theApplication::Impl::onidle;
    cfg.onidlestacksize = pImpl->config.stacksizes.taskonidle;
    cfg.onfatalerror = embot::app::theApplication::Impl::onfatal;
    cfg.clockfrequency = embot::hw::sys::clock(embot::hw::sys::CLOCK::syscore);
    cfg.ticktime = config.osaltickperiod;
    
    thescheduler.init(cfg);
    thescheduler.start();    

    for(;;);
}
  


void embot::app::theApplication::Impl::onidle(void)
{
    embot::app::theApplication &handle = embot::app::theApplication::getInstance();
    if(nullptr != handle.pImpl->config.operations.onidle.callback)
    {
        handle.pImpl->config.operations.onidle.callback(handle.pImpl->config.operations.onidle.arg);
    }    
}

void embot::app::theApplication::Impl::onfatal(void)
{
    embot::app::theApplication &handle = embot::app::theApplication::getInstance();
    if(nullptr != handle.pImpl->config.operations.onfatalerror.callback)
    {
        handle.pImpl->config.operations.onfatalerror.callback(handle.pImpl->config.operations.onfatalerror.arg);
    }     
}


void embot::app::theApplication::Impl::osalstarter(void)
{
    embot::app::theApplication &handle = embot::app::theApplication::getInstance();
    
    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();

    embot::sys::theTimerManager::Config tmrmanconfig;

    tmrman.init(tmrmanconfig);
    tmrman.start();
    
    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
    cbkman.start();    
    

    if(nullptr != handle.pImpl->config.operations.atsysteminit.callback)
    {
        handle.pImpl->config.operations.atsysteminit.callback(handle.pImpl->config.operations.atsysteminit.arg);
    }
      
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


