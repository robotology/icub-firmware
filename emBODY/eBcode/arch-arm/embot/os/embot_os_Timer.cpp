/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#include "embot_os_Timer.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_os_Thread.h"
#include "embot_os_theTimerManager.h"

#include "embot_os_rtos.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

     
struct embot::os::Timer::Impl
{ 
    const char *name {Timer::noname};
    Timer *owner {nullptr};
    embot::os::rtos::timer_t *rtostimer {nullptr};
    Timer::Config tconfig {};         
    Timer::Status sta {Timer::Status::idle};             
    size_t maxshots {0};
    size_t count {0};        
    bool canexecuteaction {false}; 
    bool butitisthelastexecution {false};  
    
    
    Impl(Timer *own)   
    {
        owner = own;
        reset(Timer::Status::idle, true);
        rtostimer = embot::os::rtos::timer_new();
    }
    
    ~Impl()   
    {
        stop();
        // do reverse deallocation and cleaning
        embot::os::rtos::timer_delete(rtostimer);
        rtostimer = nullptr;
        reset(Timer::Status::idle, true);              
        canexecuteaction = false;  
        owner = nullptr;            
    }
    
    void reset(Timer::Status st, bool alsoconfig)
    {
        sta = st;
        count = 0;
        maxshots = 0;
        canexecuteaction = false;
        butitisthelastexecution = false;
        if(alsoconfig)
        {
            tconfig.clear();
            tconfig.onexpiry.clear();
        }
    }
    
    Timer::Status status() const
    {
        return sta;
    }

    bool load(const Timer::Config &config)
    {            
        // config must be valid 
        if(false == config.isvalid())
        {
            return false;
        }
        
        // make sure the timer is stopped
        stop();
        
        // copy the config
        tconfig = config;
                                                
        return true;
    }  
            
    bool start(const Timer::Config &config, bool forcerestart)
    {            
        // config must be valid 
        if(false == config.isvalid())
        {
            return false;
        }
                                                
        bool ret = false;
                    
        // start it only if it is not counting. if we have forcerestart we re-start it anyway
        // both osal and cmsisos2 allow to restart a running timer
        if((true == forcerestart) || (Timer::Status::counting != status()))
        {
            tconfig = config;
            count = 0;             
            canexecuteaction = false; 
            butitisthelastexecution = false;  
            sta = Timer::Status::idle;
            maxshots = 0;                
                            
            embot::os::rtos::timerMode mode = (Timer::Mode::oneshot == tconfig.mode) ? (embot::os::rtos::timerMode::oneshot) : (embot::os::rtos::timerMode::forever);
             
            if(true == embot::os::rtos::timer_start(rtostimer, mode, tconfig.countdown, OnRTOStimerExpiry, this))               
            {
                sta = Timer::Status::counting;
                maxshots = (Timer::Mode::someshots == tconfig.mode) ? tconfig.numofshots : 0;                  
                ret = true;
            }
        }
                
        return ret;
    }  
    
    bool start()
    {
        constexpr bool forcerestart = true;
        return start(tconfig, forcerestart);
    }

    // keep it simple. it is executed by the internals of the RTOS: inside SysTick_Handler() for osal or inside small thread for cmsisos2
#if defined(EMBOT_USE_rtos_cmsisos2)        
    static void OnRTOStimerExpiry(void *par) 
#else
    static void OnRTOStimerExpiry(embot::os::rtos::timer_t *rtostmr, void *par)
#endif        
    {
        // this funtion is called at the expiry of the rtos timer.
        // if we use osal, it is called inside the systick handler. if we use cmsisos2 it is called inside its timer thread.
        // so, .. keep it as lean as possible
        
        // note: in here we just prepare whatever we need so that theTimerManager can correctly call Timer::execute()
        //       we executed directly only if the theTimerManager is not started. for instance because we dont want to use it.            
        
        static embot::os::theTimerManager &tm = embot::os::theTimerManager::getInstance();
        
        Impl *impl = reinterpret_cast<Impl*>(par);
        Timer *tmr = impl->owner;
        
        // preparation of basic data.   
        // we just: tell the future call of Timer::execute() that it can execute the callback, and increment the count  
        // for Mode::forever that is all it is required             
        impl->canexecuteaction = true;
        impl->count ++;  
        impl->butitisthelastexecution = false;
        
        // for Mode::oneshot / Mode::someshots we need to do something else
        // such as: tell Timer::execute() that it must stop the Timer after the execution of the callback
        
        if(impl->tconfig.mode == Timer::Mode::oneshot)
        {
            impl->butitisthelastexecution = true;
            // no need to stop the rtos timer in one shot mode. it is already stopped.   
            // we shall however call Timer::stop() only inside Timer::execute()                 
        }
        else if(impl->tconfig.mode == Timer::Mode::someshots)
        {
            if(impl->count >= impl->maxshots)
            {
                impl->butitisthelastexecution = true;
                // we need to stop the rtos timer because it was started w/ embot::os::rtos::timerMode::forever
                embot::os::rtos::timer_stop(impl->rtostimer);
                // we shall however call Timer::stop() only inside Timer::execute() 
            }
        }  
        
        // and now ask theTimerManager to get possession of the timer so that it can call its Timer::execute()
        // if theTimerManager is started it will be its thread to execute the callback, else the callback will be executed just now
        tm.execute(*tmr);        
    }          


    bool stop()
    {
        if(Timer::Status::counting == status())
        {                                
            // stop the rtos timer. operation is null safe. also, it can stopped twice safely
            embot::os::rtos::timer_stop(rtostimer);
            // reset the internal data structure
            reset(Timer::Status::idle, false);   
        }           
        return true;   
    }    
    
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::os::Timer::Timer()
: pImpl(new Impl(this))
{ 

}

embot::os::Timer::~Timer()
{   
    delete pImpl;
}

bool embot::os::Timer::name(const char *na)
{
    if(nullptr != na)
    {
        pImpl->name = na;
    }
    
    return true;
}

const char * embot::os::Timer::name() const
{
    return pImpl->name;
}

size_t embot::os::Timer::shots() const
{
    return pImpl->count;
}

bool embot::os::Timer::start(const Config &config, bool forcerestart)
{
    return pImpl->start(config, forcerestart);
}

bool embot::os::Timer::load(const Config &config)
{
    return pImpl->load(config);
}

bool embot::os::Timer::start()
{
    return pImpl->start();
}

bool embot::os::Timer::stop()
{
    return pImpl->stop();
}

embot::os::Timer::Status embot::os::Timer::status() const
{
    return pImpl->status();   
}

const embot::os::Timer::Config& embot::os::Timer::config() const
{
    return pImpl->tconfig;
}

bool embot::os::Timer::execute()
{
    bool executed = false;
    if(true == pImpl->canexecuteaction)
    {
        executed = true;
        pImpl->tconfig.onexpiry.execute();  
        pImpl->canexecuteaction = false;   
        if(true == pImpl->butitisthelastexecution)
        {   // if in here, we have already managed the rtos timer inside OnRTOStimerExpiry() so, it is enough to use reset() 
            pImpl->reset(Timer::Status::idle, false);   
        }
    }
    return executed;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

