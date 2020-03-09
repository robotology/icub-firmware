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

#include "embot_app_theLoop.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include "embot_hw.h"

#include "embot_sys_Timer.h"


#include "embot_sys_Task.h"
#include "EOMtask.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

void theLOOP(void *p)
{
    embot::sys::registerNameOfTask(p);
}

struct embot::app::theLoop::Impl
{    
    Config config;
    embot::sys::Timer *tmr;
    embot::sys::EventTask *tsk;
    static const embot::common::Event triggeringEvent = 0x00000001;
    
    embot::common::Time Tstart;
    embot::common::Time Tstop;
    embot::common::relTime duration;
    std::uint64_t iteration;
    
    Impl() 
    {         
        config.priority = 200;
        config.stacksize = 4096;
        config.period = embot::common::time1millisec;
        config.startup.callback = nullptr;
        config.startup.arg = nullptr;
        config.activity.callback = nullptr;
        config.activity.arg = nullptr;        
        iteration = 0;
        Tstart = Tstop = embot::sys::timeNow();
        duration = 0;
    }
            

    static void strt(embot::sys::Task *tsk, void *par);
    static void exec(embot::sys::Task *tsk, embot::common::Event evt, void *par);
};



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::app::theLoop::theLoop()
: pImpl(new Impl)
{
       
}

        
bool embot::app::theLoop::start(Config &config)
{
    pImpl->config = config;
    
    
    // start it only if it is not nullptr etc. i dont want to keke any check in runtime innside exec()
    
    if(nullptr == pImpl->config.activity.callback)
    {
        return false;
    }
    
    if(0 == pImpl->config.priority)
    {
        pImpl->config.priority = 200;
    }
    
//    if(pImpl->config.period < getTick())
//    {    
//        pImpl->config.period = getTick();            
//    }


    // create an event based task which waits forever on an event. create a timer which sends an event.
    


    pImpl->tsk = new embot::sys::EventTask;    
    //pImpl->tsk->init(nullptr, pImpl->exec, pImpl->config.stacksize, pImpl->config.priority, embot::common::timeWaitForever, pImpl, nullptr);
    pImpl->tsk->init(pImpl->strt, pImpl->exec, pImpl->config.stacksize, pImpl->config.priority, embot::common::timeWaitForever, pImpl, theLOOP);
    
    
    pImpl->tmr = new embot::sys::Timer;         
    embot::sys::Action action(embot::sys::Action::EventToTask(embot::app::theLoop::Impl::triggeringEvent, pImpl->tsk));    
    pImpl->tmr->start(pImpl->config.period, embot::sys::Timer::Type::forever, action);


    return true;
}
 

void embot::app::theLoop::Impl::strt(embot::sys::Task *tsk, void *par)
{
    embot::app::theLoop::Impl *p = reinterpret_cast<embot::app::theLoop::Impl *>(par);

    p->config.startup.callback(p->config.startup.arg);
}


void embot::app::theLoop::Impl::exec(embot::sys::Task *tsk, embot::common::Event evt, void *par)
{
    embot::app::theLoop::Impl *p = reinterpret_cast<embot::app::theLoop::Impl *>(par);
    
//    p->Tstart = embot::sys::timeNow();

    p->config.activity.callback(p->config.activity.arg);
    
//    p->Tstop = embot::sys::timeNow();
//    p->duration = p->Tstop - p->Tstart;
    p->iteration ++;
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


