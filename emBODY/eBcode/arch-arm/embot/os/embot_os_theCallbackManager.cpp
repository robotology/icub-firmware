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

#include "embot_os_theCallbackManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_Thread.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::theCallbackManager::Impl
{    
    Config config {};    
    embot::os::CallbackThread *thread {nullptr};
    
    Impl() = default;
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::os::theCallbackManager& embot::os::theCallbackManager::getInstance()
{
    static theCallbackManager* p = new theCallbackManager();
    return *p;
}

embot::os::theCallbackManager::theCallbackManager()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::os::theCallbackManager::~theCallbackManager() { }

void tCBKman(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

bool embot::os::theCallbackManager::start(const Config &config)
{       
    if(true == started())
    {
        return false;
    }
    
    pImpl->config = config;
        
    pImpl->thread = new embot::os::CallbackThread;
    
    embot::os::CallbackThread::Config cfg {};
    cfg.priority = pImpl->config.priority;
    cfg.stacksize = pImpl->config.stacksize;
    cfg.queuesize = pImpl->config.capacityofhandler;
    cfg.timeout = embot::core::reltimeWaitForever;
    cfg.startup = nullptr;
    cfg.name = "tCBKman";
    
    pImpl->thread->start(cfg, tCBKman);
    
    return true;    
}

bool embot::os::theCallbackManager::started() const
{
    return (nullptr == pImpl->thread) ? false : true;
}

embot::os::Thread * embot::os::theCallbackManager::thread() const
{
    return pImpl->thread;
}
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

