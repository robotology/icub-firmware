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

#include "embot_sys_theCallbackManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_sys_Task.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::theCallbackManager::Impl
{    
    Config config;    
    embot::sys::CallbackTask *task;
    
    Impl() 
    {              
        task = nullptr;
        config.capacityofhandler = 8;
        config.priority = 230;
        config.stacksize = 1024;
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::theCallbackManager& embot::sys::theCallbackManager::getInstance()
{
    static theCallbackManager* p = new theCallbackManager();
    return *p;
}

embot::sys::theCallbackManager::theCallbackManager()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::sys::theCallbackManager::~theCallbackManager() { }


bool embot::sys::theCallbackManager::start(const Config &config)
{       
    if(true == started())
    {
        return false;
    }
    
    pImpl->config = config;
        
    pImpl->task = new embot::sys::CallbackTask;
    
    embot::sys::CallbackTask::Config cfg;
    cfg.priority = pImpl->config.priority;
    cfg.stacksize = pImpl->config.stacksize;
    cfg.queuesize = pImpl->config.capacityofhandler;
    cfg.timeout = embot::common::timeWaitForever;
    cfg.startup = nullptr;
    
    pImpl->task->start(cfg);
    
    return true;    
}
bool embot::sys::theCallbackManager::started() const
{
    return (nullptr == pImpl->task) ? false : true;
}

embot::sys::Task * embot::sys::theCallbackManager::task() const
{
    return pImpl->task;
}
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

