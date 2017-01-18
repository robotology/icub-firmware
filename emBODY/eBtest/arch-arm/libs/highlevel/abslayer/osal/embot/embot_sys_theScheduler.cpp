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

#include "embot_sys_theScheduler.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"

#include <cstring>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::sys::theScheduler::Impl
{   
    osal_cfg_t osalConfig; 
    osal_cfg_t osalDefaultConfig;

    void set_default(osal_cfg_t &cfg)
    {
        std::memset(&cfg, 0, sizeof(osal_cfg_t));
        
        cfg.rtostype = osal_rtostype_oosiit;
        cfg.memorymodel = osal_memmode_dynamic;
        cfg.prio = 15;
        cfg.cpufam = osal_cpufam_armcm4;  
        cfg.cpufreq = 168000000;
        cfg.tick = 1000;
        cfg.launcherstacksize = 2048;
        cfg.idlestacksize = 512;
        cfg.roundrobin = osal_false;
        cfg.roundrobintick = 0;
        cfg.tasknum = 0;
        // etc.
        cfg.extfn.usr_on_fatal_error = osalOnFatalError;
        cfg.extfn.usr_on_idle = osalIdleTask;
    }
    
    bool started;
    
    static void osalIdleTask(void)
    {
        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();        
        common::fpWorker onidle = thesystem.pImpl->osalIdleActivity;
        
        for(;;)
        {
            if(nullptr != onidle)
            {
                onidle();
            }
        }
    } 
    
    static void osalOnFatalError(void* task, osal_fatalerror_t errorcode, const char * errormsg)
    {
        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();        
        common::fpWorker onfatalerror = thesystem.pImpl->osalFatalErrorActivity;
        
        if(nullptr != onfatalerror)
        {
            onfatalerror();
        }
            
        // we stop in here
        for(;;);  
    }

    common::fpWorker osalLauncher;
    common::fpWorker osalIdleActivity;   
    common::fpWorker osalFatalErrorActivity;
    
             
    static void osalDefaultLauncher(void) 
    {    
        // does nothing ...
    }

    void osalInit(void)
    {
        std::uint32_t ram08size = 0;
        uint64_t *ram08data = NULL;
        
        osal_base_memory_getsize(&osalConfig, &ram08size);
        
        if(0 != ram08size)
        {
            ram08data = (uint64_t*)(uint64_t*)osal_base_memory_new(ram08size);
        }
        osal_base_initialise(&osalConfig, ram08data);    
        

        if(nullptr == osalLauncher)
        {
            osalLauncher = osalDefaultLauncher;
        }        
    }
    
    Impl() 
    {        
        set_default(osalConfig);        
        started = false;        
        osalIdleActivity = nullptr;
        osalFatalErrorActivity = nullptr;
        osalLauncher = nullptr;
    }
};

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::sys::theScheduler::theScheduler()
: pImpl(new Impl)
{   

}


bool embot::sys::theScheduler::init(Config &config)
{   
    if(isStarted())
    {
        return false;
    }
    
    if((0 == config.clockfrequency) || (0 == config.ticktime))
    {
        return false;
    }
    
    if((nullptr == config.launcher) || (0 == config.launcherstacksize))
    {
        return false;
    }
    
    if(0 == config.onidlestacksize)
    {
        return false;
    }
    
    pImpl->osalConfig.cpufreq = config.clockfrequency;
    pImpl->osalConfig.tick = config.ticktime;    
    
    pImpl->osalLauncher = config.launcher;
    pImpl->osalConfig.launcherstacksize = config.launcherstacksize;
    
    pImpl->osalIdleActivity = config.onidle;
    pImpl->osalConfig.idlestacksize = config.onidlestacksize;
    
    pImpl->osalFatalErrorActivity = config.onfatalerror;
    
    
    return true;    
}


void embot::sys::theScheduler::start()
{   
    if(isStarted())
    {
        return;
    }
    
    pImpl->osalInit();
        
    // 1. init rtos in standard way:
    
    pImpl->started = true;
    osal_system_start(pImpl->osalLauncher);
}


bool embot::sys::theScheduler::isStarted()
{
    return pImpl->started;
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

