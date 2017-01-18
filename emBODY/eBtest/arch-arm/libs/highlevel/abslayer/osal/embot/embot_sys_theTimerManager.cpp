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

#include "embot_sys_theTimerManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheTimerManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::theTimerManager::Impl
{    
    bool started;
    Config config;
    
    Impl() 
    {              
        started = false;
        config.capacityofhandler =8;
        config.priority = 240;
        config.stacksize = 1024;
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::theTimerManager::theTimerManager()
: pImpl(new Impl)
{   

}


bool embot::sys::theTimerManager::init(Config &config)
{   
    if(true == pImpl->started)
    {
        return false;
    }
        
    pImpl->config = config;
    
    return true;    
}


bool embot::sys::theTimerManager::start()
{       
    if(true == pImpl->started)
    {
        return false;
    }
    
    pImpl->started = true;
    
    eOmtimerman_cfg_t cfg = {0};
    cfg.messagequeuesize = pImpl->config.capacityofhandler;
    cfg.priority = pImpl->config.priority;
    cfg.stacksize = pImpl->config.stacksize;
    
    eom_timerman_Initialise(&cfg);
    
    return true;
    
}

    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

