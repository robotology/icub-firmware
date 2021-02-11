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



namespace embot { namespace os { namespace timertools {
        
    struct TMR 
    {
        Timer *owner {nullptr};
        embot::os::rtos::timer_t *rtostimer {nullptr};
        Timer::Config tconfig {};         
        Timer::Status sta {Timer::Status::idle};             
        std::uint32_t maxshots {0};
        std::uint32_t count {0};        
        bool canexecuteaction {false}; 
        bool onemoretime {false};                 
        
        TMR(Timer *own)   
        {
            owner = own;
            reset(Timer::Status::idle, true);
            rtostimer = embot::os::rtos::timer_new();
        }
        
        ~TMR()   
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
            canexecuteaction = false;
            onemoretime = false;
            if(alsoconfig)
            {
                tconfig.clear();
                tconfig.onexpiry.clear();
            }
        }
        
        Timer::Status getstatus()
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
            if((true == forcerestart) || (Timer::Status::counting != getstatus()))
            {
                tconfig = config;
                             
                canexecuteaction = false;                              

                embot::os::rtos::timerMode mode = (Timer::Mode::oneshot == tconfig.mode) ? (embot::os::rtos::timerMode::oneshot) : (embot::os::rtos::timerMode::forever);
                 
                if(true == embot::os::rtos::timer_start(rtostimer, mode, tconfig.countdown, OnExpiryCbk, this))               
                {
                    sta = Timer::Status::counting;
                    count = 0;
                    maxshots = ( (Timer::Mode::someshots == tconfig.mode) ? (tconfig.numofshots) : (0) );
                    onemoretime = false;
                    ret = true;
                }
                else
                {
                    sta  = Timer::Status::idle;
                    onemoretime =  false;
                    ret = false;
                }
            }
                    
            return ret;
        }  
        
        bool start()
        {
            constexpr bool forcerestart = true;
            return start(tconfig, forcerestart);
        }

        // keep it simple. it is executed by osal inside the systick  and by cmsisos2 inside a small thread
#if defined(EMBOT_USE_rtos_cmsisos2)        
        static void OnExpiryCbk(void *par) 
#else
        static void OnExpiryCbk(embot::os::rtos::timer_t *rtostmr, void *par)
#endif        
        {
            TMR *otm = reinterpret_cast<TMR*>(par);
            otm->canexecuteaction = true;
            embot::os::theTimerManager &tm = embot::os::theTimerManager::getInstance();
            if(true == tm.started())
            {
                tm.onexpiry(*otm->owner);
            }
            else
            {
                otm->owner->execute();
            }
            if(otm->tconfig.mode == Timer::Mode::someshots)
            {
                otm->count ++;
                if(otm->count >= otm->maxshots)
                {
                    otm->onemoretime = true;
                    otm->stop();
                }
            }            
        }          


        bool stop()
        {
            if(Timer::Status::counting == getstatus())
            {                                
                // stop the rtos timer. operation is null safe.
                embot::os::rtos::timer_stop(rtostimer);
            }
          
            if(false == onemoretime)
            {
                reset(Timer::Status::idle, false);   
                canexecuteaction = false;   
            }
            
            return true;            
        }
        
    }; 
        
    
}}} // namespace embot { namespace os {

     
struct embot::os::Timer::Impl
{          
    embot::os::timertools::TMR *tmr;
    
    Impl(Timer *own) 
    {
        tmr = new embot::os::timertools::TMR(own);
    }
    ~Impl()
    {
        delete tmr;
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

bool embot::os::Timer::start(const Config &config, bool forcerestart)
{
    return pImpl->tmr->start(config, forcerestart);
}

bool embot::os::Timer::load(const Config &config)
{
    return pImpl->tmr->load(config);
}

bool embot::os::Timer::start()
{
    return pImpl->tmr->start();
}

bool embot::os::Timer::stop()
{
    return pImpl->tmr->stop();
}

embot::os::Timer::Status embot::os::Timer::getStatus() const
{
    return pImpl->tmr->getstatus();   
}

//embot::os::Timer::Mode embot::os::Timer::getMode() const
//{
//    return pImpl->tmr->getmode();  
//}

//embot::core::relTime embot::os::Timer::getCountdown() const
//{
//    return pImpl->tmr->period;
//}

//const embot::os::Action& embot::os::Timer::getAction() const
//{
//    return pImpl->tmr->tconfig.onexpiry;
//}

const embot::os::Timer::Config& embot::os::Timer::getConfig() const
{
    return pImpl->tmr->tconfig;
}

bool embot::os::Timer::execute()
{
    bool canexecute = pImpl->tmr->canexecuteaction;
    if(true == canexecute)
    {
        pImpl->tmr->tconfig.onexpiry.execute();  
        pImpl->tmr->canexecuteaction = false;   
        if(true == pImpl->tmr->onemoretime)
        {
            pImpl->tmr->onemoretime = false;
            pImpl->tmr->reset(Timer::Status::idle, false);   
        }
    }
    return canexecute;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

