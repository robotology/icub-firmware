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

#include "embot_sys_theJumper.h"
#include "embot_sys_Timer.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_bsp.h"
#include "embot.h"
#include "osal.h"

#include "EOtheLEDpulser.h"

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
        config.userdeflauncher.callback = nullptr;
        config.userdeflauncher.arg = nullptr;
        config.countdown = 5*embot::common::time1second;   
        resetTimer = nullptr;   
    }
    
    
    void *operator new(std::size_t size) throw(std::bad_alloc)
    {
        static std::uint64_t s_data_impl[(sizeof(embot::app::theBootloader::Impl)+7)/8] = {0};
        return s_data_impl;
    }
    
    static void restart2application(void *p);
    
    static void onidle(void);
    
    static void onfatal(void);
    
    static void osalstarter(void);

};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


void * embot::app::theBootloader::operator new(std::size_t size) throw(std::bad_alloc)
{
    static std::uint64_t s_data_jumper[(sizeof(embot::app::theBootloader)+7)/8] = {0};
    return s_data_jumper;
}


embot::app::theBootloader::theBootloader()
: pImpl(new Impl)
{       
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();
}


embot::sys::theJumper::Command embot::app::theBootloader::getcommand(std::uint32_t &parameter)
{
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();  
    return thejumper.get(parameter);
}


bool embot::app::theBootloader::jump(std::uint32_t address)
{
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();    
    thejumper.jump(address);
    
    return false;
}


bool embot::app::theBootloader::restart2application()
{
    embot::sys::theJumper &thejumper = embot::sys::theJumper::getInstance();
    thejumper.set(embot::sys::theJumper::Command::jump, embot::hw::sys::addressOfApplication);
    thejumper.restart();   
    
    return false;
}
   
        
void embot::app::theBootloader::execute(Config &config)
{
    pImpl->config = config;
    
    // now we init the hw, we start the scheduler at 1 ms, we start a countdown with sys restart at the end ... we exec the activity ...
    embot::hw::bsp::Config cc;
    cc.get1mstick = embot::sys::millisecondsNow;
    embot::hw::bsp::init(cc);
          
    embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();
    embot::sys::theScheduler::Config cfg;
    cfg.launcher = embot::app::theBootloader::Impl::osalstarter;
    cfg.launcherstacksize = 2048;
    cfg.onidle = embot::app::theBootloader::Impl::onidle;
    cfg.onidlestacksize = 512;
    cfg.onfatalerror = embot::app::theBootloader::Impl::onfatal;
    cfg.clockfrequency = embot::hw::sys::clock(embot::hw::sys::CLOCK::syscore);
    
    thesystem.init(cfg);
    thesystem.start();    

    for(;;);
}
  
  
bool embot::app::theBootloader::stopcountdown()
{    
    pImpl->resetTimer->stop();
    eo_ledpulser_Stop(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 2*EOK_reltime1sec, 0);
    
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
    pImpl->resetTimer->start(countdown, embot::sys::Timer::Type::oneshot, pImpl->action);
    
    return true;
}    


void embot::app::theBootloader::Impl::restart2application(void *p)
{
    embot::sys::theJumper &thejumper = embot::sys::theJumper::getInstance();
    thejumper.set(embot::sys::theJumper::Command::jump, embot::hw::sys::addressOfApplication);
    thejumper.restart();    
}


void embot::app::theBootloader::Impl::onidle(void)
{
    
}

void embot::app::theBootloader::Impl::onfatal(void)
{
    
}

void embot::app::theBootloader::Impl::osalstarter(void)
{
    embot::app::theBootloader &handle2bootloader  = embot::app::theBootloader::getInstance();
    
    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();

    embot::sys::theTimerManager::Config tmrmanconfig;

    tmrman.init(tmrmanconfig);
    tmrman.start();
    
    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
    cbkman.start();    
    
    handle2bootloader.pImpl->resetTimer = new embot::sys::Timer;
    
    handle2bootloader.pImpl->action.set(embot::sys::Action::ExecuteCallback(restart2application, nullptr, nullptr));
    
    
    if(handle2bootloader.pImpl->config.countdown > 0)
    {
        handle2bootloader.pImpl->resetTimer->start(handle2bootloader.pImpl->config.countdown, embot::sys::Timer::Type::oneshot, handle2bootloader.pImpl->action);
    }

    if (nullptr != handle2bootloader.pImpl->config.userdeflauncher.callback)
    {
        handle2bootloader.pImpl->config.userdeflauncher.callback(handle2bootloader.pImpl->config.userdeflauncher.arg);
    }
      
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


