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

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"

#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_bsp.h"
#include "embot_hw_flash.h"

#include "osal.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::theApplication::Impl
{    
    Config config {};
    
    Impl() = default;
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
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::theApplication::~theApplication() { }

        
[[noreturn]] void embot::app::theApplication::execute(const Config &config)
{
    if(false == config.isvalid())
    {
        for(;;);
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
        
    embot::os::init({});
    
    embot::os::theScheduler::Config cfg { embot::os::theScheduler::Timing(embot::hw::sys::clock(embot::hw::CLOCK::syscore),  pImpl->config.ticktime), pImpl->config.behaviour };    
    embot::os::theScheduler &thescheduler = embot::os::theScheduler::getInstance();
    thescheduler.start(cfg);    
    
    for(;;);
    
    //return true;
}
  


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


