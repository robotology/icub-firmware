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



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_sys_theScheduler.h"
#include "embot_sys_theTimerManager.h"
#include "embot_sys_theCallbackManager.h"

#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_bsp.h"
#include "embot_hw_flash.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::theApplication::Impl
{    
    Config config;
    
    Impl() 
    {

    }
            
    static void onidle(void *p);    
    static void onfatal(void *p);    
    static void osalstarter(void *p);
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------




embot::app::theApplication& embot::app::theApplication::getInstance()
{
    static theApplication* p = new theApplication();
    return *p;
}

embot::app::theApplication::theApplication()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::theApplication::~theApplication() { }

        
bool embot::app::theApplication::execute(const Config &config)
{
    if(false == config.isvalid())
    {
        return false;
    }
    
    pImpl->config = config;
    
    // now we init the hw, we start the scheduler, we start a countdown with sys restart at the end ... we exec the activity ...
    if(pImpl->config.addressofapplication > embot::hw::flash::getpartition(embot::hw::FLASH::whole).address)
    {
        std::uint32_t vectorlocation = pImpl->config.addressofapplication - embot::hw::flash::getpartition(embot::hw::FLASH::whole).address;
        if(0 != vectorlocation)
        {
            embot::hw::sys::relocatevectortable(vectorlocation);
        }
    }
    
    const embot::hw::bsp::stm32halConfig stm32c(nullptr, embot::sys::millisecondsNow);
    embot::hw::bsp::Config cc(stm32c, embot::sys::timeNow);
    embot::hw::bsp::init(cc);
    
    embot::sys::theScheduler::Config cfg;
    cfg.timing = embot::sys::theScheduler::Timing(embot::hw::sys::clock(embot::hw::CLOCK::syscore),  pImpl->config.ticktime);
    cfg.oninit = embot::sys::Operation(embot::common::Callback(embot::app::theApplication::Impl::osalstarter, nullptr), pImpl->config.init.stacksize);
    cfg.onidle = embot::sys::Operation(embot::common::Callback(embot::app::theApplication::Impl::onidle, nullptr), pImpl->config.idle.stacksize);
    cfg.onfatal = embot::sys::Operation(embot::common::Callback(embot::app::theApplication::Impl::onfatal, nullptr), pImpl->config.fatal.stacksize);
    
    embot::sys::theScheduler &thescheduler = embot::sys::theScheduler::getInstance();
    thescheduler.start(cfg);    
    
    for(;;);
    
    return true;
}
  


void embot::app::theApplication::Impl::onidle(void *p)
{
    embot::app::theApplication &handle = embot::app::theApplication::getInstance();
    handle.pImpl->config.idle.activity.execute(); 
}

void embot::app::theApplication::Impl::onfatal(void *p)
{
    embot::app::theApplication &handle = embot::app::theApplication::getInstance();
    handle.pImpl->config.fatal.activity.execute();  
}


void embot::app::theApplication::Impl::osalstarter(void *p)
{
    embot::app::theApplication &handle = embot::app::theApplication::getInstance();
    
    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();
    embot::sys::theTimerManager::Config tmrmanconfig;
    tmrman.start(tmrmanconfig);
    
    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
    embot::sys::theCallbackManager::Config cbkmanconfig;
    cbkman.start(cbkmanconfig);  
    
    handle.pImpl->config.init.activity.execute();      
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


