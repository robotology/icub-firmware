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

#include "embot_app_theBootloader.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_theJumper.h"
#include "embot_sys_Timer.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_bsp.h"
#include "embot.h"
#include "osal.h"


#include <new>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::theBootloader::Impl
{    
    Config config;
    embot::sys::Timer *resetTimer;
    embot::sys::Action action;
    
    Impl() 
    {              
        config.userdeflauncher.clear();
        config.countdown = 5*embot::common::time1second;   
        resetTimer = nullptr;   
    }
    
    
    void *operator new(std::size_t size) noexcept(false)
    {
        static std::uint64_t s_data_impl[(sizeof(embot::app::theBootloader::Impl)+7)/8] = {0};
        return s_data_impl;
    }
    
    static void restart2application(void *p);
    
    static void onidle(embot::sys::Task *t, void* param);
    static void init(embot::sys::Task *t, void* param);
    static void userdefonOSerror(void *param);
    
    //static void osalstarter(void *p);

};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


void * embot::app::theBootloader::operator new(std::size_t size) noexcept(false)
{
    static std::uint64_t s_data_jumper[(sizeof(embot::app::theBootloader)+7)/8] = {0};
    return s_data_jumper;
}


embot::app::theBootloader::theBootloader()
: pImpl(new Impl)
{       
    embot::app::theJumper& thejumper = embot::app::theJumper::getInstance();
}


embot::app::theJumper::Command embot::app::theBootloader::getcommand(std::uint32_t &parameter)
{
    embot::app::theJumper& thejumper = embot::app::theJumper::getInstance();  
    return thejumper.get(parameter);
}


bool embot::app::theBootloader::jump(std::uint32_t address)
{
    embot::app::theJumper& thejumper = embot::app::theJumper::getInstance();    
    thejumper.jump(address);
    
    return false;
}


bool embot::app::theBootloader::restart2application()
{
    embot::app::theJumper &thejumper = embot::app::theJumper::getInstance();
    std::uint32_t addressOfApplication = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
    thejumper.set(embot::app::theJumper::Command::jump, addressOfApplication);
    thejumper.restart();   
    
    return false;
}
   
embot::app::theBootloader::evalRes embot::app::theBootloader::eval()
{
    evalRes res = evalRes::jumpaftercountdown;    
    std::uint32_t param = 0;    
    embot::app::theJumper::Command cmd = getcommand(param);  

    switch(cmd)
    {        
        case embot::app::theJumper::Command::stay:
        {   // we have received a command from the previous running process to stay in here forever
            res = evalRes::stayforever;
        } break; 
        
        case embot::app::theJumper::Command::jump:
        {   // we have received command to jump to an address
            jump(param); 
            // if we cannot jump to the specified address ... we just tell
            res = evalRes::jumpfailed;
        } break;     
        
        case embot::app::theJumper::Command::none:
        {   // we dont change what we want to do: we shall jump after countdown
            res = evalRes::jumpaftercountdown;
        } break;  

        default:
        {
            res = evalRes::unexected;           
        } break;
    }

    return res;        
}
        
[[noreturn]] void embot::app::theBootloader::execute(Config &config)
{
    pImpl->config = config;
    
    // now we init the hw, we start the scheduler at 1 ms, we start a countdown with sys restart at the end ... we exec the activity ...
    embot::hw::bsp::Config cc(embot::sys::now);
    embot::hw::bsp::init(cc);
             
    static const embot::sys::InitTask::Config initconfig { 4*2048, embot::app::theBootloader::Impl::init, nullptr };
    static const embot::sys::IdleTask::Config idleconfig { 512, nullptr, nullptr, embot::app::theBootloader::Impl::onidle };
    static const embot::common::Callback onOSerror = { embot::app::theBootloader::Impl::userdefonOSerror, nullptr };
    
    embot::sys::theScheduler::Config cfg { 
            embot::sys::theScheduler::Timing(embot::hw::sys::clock(embot::hw::CLOCK::syscore),  embot::common::time1millisec),  
            embot::sys::theScheduler::Behaviour { initconfig, idleconfig, onOSerror} 
    };    
    embot::sys::theScheduler &thescheduler = embot::sys::theScheduler::getInstance();
    thescheduler.start(cfg);

    for(;;);
}
  
  
bool embot::app::theBootloader::stopcountdown()
{    
    pImpl->resetTimer->stop();
//    eo_ledpulser_Stop(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
//    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 2*EOK_reltime1sec, 0);
    
    return true;
}
  

bool embot::app::theBootloader::startcountdown(embot::common::relTime countdown)
{    
    pImpl->resetTimer->stop();
    
    if(0 == countdown)
    {
        restart2application();
        return false;
    }
    
//    embot::sys::Action action(embot::sys::Action::ExecuteCallback(restart2application, nullptr, nullptr));   
    embot::sys::Timer::Config cfg;
    cfg.countdown = countdown;
    cfg.mode = embot::sys::Timer::Mode::oneshot;
    cfg.onexpiry = pImpl->action;
    pImpl->resetTimer->start(cfg);
    
    return true;
}    


void embot::app::theBootloader::Impl::restart2application(void *p)
{
    embot::app::theJumper &thejumper = embot::app::theJumper::getInstance();
    std::uint32_t addressOfApplication = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
    thejumper.set(embot::app::theJumper::Command::jump, addressOfApplication);
    thejumper.restart();    
}


void embot::app::theBootloader::Impl::onidle(embot::sys::Task *t, void* param)
{
    
}

void embot::app::theBootloader::Impl::userdefonOSerror(void *param)
{
    
}

void embot::app::theBootloader::Impl::init(embot::sys::Task *t, void* param)
{
    embot::app::theBootloader &handle2bootloader  = embot::app::theBootloader::getInstance();
    
    // start sys services: timer manager & callback manager
    // start them both w/ default config;
    embot::sys::theTimerManager::getInstance().start({});     
    embot::sys::theCallbackManager::getInstance().start({});   
    
    // start reset timer
    handle2bootloader.pImpl->resetTimer = new embot::sys::Timer;
    handle2bootloader.pImpl->action.load(embot::sys::CallbackToTask(embot::common::Callback(restart2application, nullptr), nullptr));
        
    if(handle2bootloader.pImpl->config.countdown > 0)
    {
        embot::sys::Timer::Config cfg(handle2bootloader.pImpl->config.countdown, handle2bootloader.pImpl->action, embot::sys::Timer::Mode::oneshot);
        handle2bootloader.pImpl->resetTimer->start(cfg);
    }

    // execute the user-def callback
    handle2bootloader.pImpl->config.userdeflauncher.execute();     
}

//void embot::app::theBootloader::Impl::osalstarter(void *p)
//{
//    embot::app::theBootloader &handle2bootloader  = embot::app::theBootloader::getInstance();
//    
//    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();
//    embot::sys::theTimerManager::Config tmrmanconfig;
//    tmrman.start(tmrmanconfig);
//    
//    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
//    embot::sys::theCallbackManager::Config cbkmanconfig;
//    cbkman.start(cbkmanconfig);    
//    
//    handle2bootloader.pImpl->resetTimer = new embot::sys::Timer;
//    handle2bootloader.pImpl->action.load(embot::sys::CallbackToTask(embot::common::Callback(restart2application, nullptr), nullptr));
//        
//    if(handle2bootloader.pImpl->config.countdown > 0)
//    {
//        embot::sys::Timer::Config cfg(handle2bootloader.pImpl->config.countdown, handle2bootloader.pImpl->action, embot::sys::Timer::Mode::oneshot);
//        handle2bootloader.pImpl->resetTimer->start(cfg);
//    }

//    // the execution of a callback is always protected
//    handle2bootloader.pImpl->config.userdeflauncher.execute();     
//}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


