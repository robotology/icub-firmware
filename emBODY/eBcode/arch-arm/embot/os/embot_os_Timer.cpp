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
#include "osal.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



namespace embot { namespace os { namespace timertools {
        
    struct osalTMR 
    {
        Timer               *owner;
        osal_timer_t        *osaltimer;
        Timer::Config       tconfig;         
        Timer::Status       sta;             
        std::uint32_t       maxshots;
        std::uint32_t       count;        
        bool                canexecuteaction; 
        bool onemoretime;                 
        
        osalTMR(Timer *own)   
        {
            owner = own;
            reset(Timer::Status::idle);
            osaltimer = osal_timer_new();
        }
        
        ~osalTMR()   
        {
            stop();
            // do reverse deallocation and cleaning
            osal_timer_delete(osaltimer);
            osaltimer = nullptr;
            reset(Timer::Status::idle);              
            canexecuteaction = false;  
            owner = nullptr;            
        }
        
        void reset(Timer::Status st)
        {
            tconfig.clear();
            sta = st;
            canexecuteaction = false;
            onemoretime = false;
            tconfig.onexpiry.clear();
        }
        
        Timer::Status getstatus()
        {
            return sta;
        }
        
//        Timer::Mode getmode()
//        {
//            return mod;
//        }
        
        bool start(const Timer::Config &config)
        {            
            // config must be valid 
            if(false == config.isvalid())
            {
                return false;
            }
                                                    
            bool ret = false;
            
            // 1. lock the timer manager
            
            // 2. start it only if it is not counting
            if(Timer::Status::counting != getstatus())
            {
                tconfig = config;
                
                // 1. copy the onexp and disable it. it will be enabled only at expiry of the countdown so that it can be executed
                
                canexecuteaction = false;
                                
                osal_timer_timing_t timing { 
                    .startat = OSAL_abstimeNONE, 
                    .count = tconfig.countdown, 
                    .mode = ( (Timer::Mode::oneshot == tconfig.mode) ? (osal_tmrmodeONESHOT) : (osal_tmrmodeFOREVER) )
                };
                osal_timer_onexpiry_t onexpi {.cbk = OnExpiryCbk, .par = this };

                osal_result_t r = osal_timer_start(osaltimer, &timing, &onexpi, osal_callerAUTOdetect);                
                
                if(osal_res_OK == r)
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
            
            // 5. unlock the timer manager
        
            return ret;
        }  

        // it is executed by osal inside the systick, hence ... keep it lean
        static void OnExpiryCbk(osal_timer_t *osaltmr, void *par) 
        {
            osalTMR *otm = reinterpret_cast<osalTMR*>(par);
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
                // stop the osal timer. operation is null safe.
                osal_timer_stop(osaltimer, osal_callerAUTOdetect);
            }
          
            if(false == onemoretime)
            {
                reset(Timer::Status::idle);   
                canexecuteaction = false;   
            }
            
            return true;            
        }
        
    }; 
        
    
}}} // namespace embot { namespace os {

     
struct embot::os::Timer::Impl
{          
    embot::os::timertools::osalTMR *tmr;
    
    Impl(Timer *own) 
    {
        tmr = new embot::os::timertools::osalTMR(own);
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

bool embot::os::Timer::start(const Config &config)
{
    return pImpl->tmr->start(config);
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
            pImpl->tmr->reset(Timer::Status::idle);   
        }
    }
    return canexecute;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

