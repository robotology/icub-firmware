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

#include "embot_os_theTimerManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::theTimerManager::Impl
{    
    Config config {};    
    embot::os::MessageThread *thread {nullptr};
    
    Impl() 
    {              
    }    
    
    static void processtimer(Thread *t, os::Message m, void *o)
    {
        if(0 != m)
        {
            embot::os::Timer *tmr = reinterpret_cast<embot::os::Timer*>(m);
            tmr->execute();            
        }            
    }

};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::os::theTimerManager& embot::os::theTimerManager::getInstance()
{
    static theTimerManager* p = new theTimerManager();
    return *p;
}

embot::os::theTimerManager::theTimerManager()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::os::theTimerManager::~theTimerManager() { }


void tTMRman(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

bool embot::os::theTimerManager::start(const Config &config)
{       
    if(true == started())
    {
        return false;
    }
    
    pImpl->thread = new embot::os::MessageThread;
        
    embot::os::MessageThread::Config cfg {};
    cfg.priority = pImpl->config.priority;
    cfg.stacksize = pImpl->config.stacksize;
    cfg.messagequeuesize = pImpl->config.capacityofhandler;
    cfg.timeout = embot::core::reltimeWaitForever;
    cfg.startup = nullptr;
    cfg.onmessage = pImpl->processtimer;
    cfg.param = this;
    cfg.name = "tTMRman";
    
    pImpl->thread->start(cfg, tTMRman);
    
    return true;
    
}

bool embot::os::theTimerManager::started() const
{
    return (nullptr == pImpl->thread) ? false : true;
}

bool embot::os::theTimerManager::execute(Timer &timer) const
{
    if(nullptr == pImpl->thread)
    {
        return timer.execute();
    }
    
    pImpl->thread->setMessage(reinterpret_cast<embot::os::Message>(const_cast<Timer*>(&timer)), embot::core::reltimeWaitNone);
    return true;
}
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

