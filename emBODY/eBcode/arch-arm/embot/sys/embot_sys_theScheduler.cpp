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

    Config config;    
    osal_cfg_t osalConfig; 
    bool started;

    void set_osal_default_config(osal_cfg_t &cfg)
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
    
    
    
    static void osalIdleTask(void)
    {
        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();        
        
        for(;;)
        {
            thesystem.pImpl->config.onidle.activity.execute();
        }
    } 
    
    static void osalOnFatalError(void* task, osal_fatalerror_t errorcode, const char * errormsg)
    {
        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();        

        thesystem.pImpl->config.onfatal.activity.execute();
            
        // we stop in here
        for(;;);  
    }

                     
    static void osalLauncher(void) 
    {    
        embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();
        thesystem.pImpl->config.oninit.activity.execute();        
    }

    bool start(const Config &cfg)
    {
        if(false == cfg.isvalid())
        {
            return false;
        }  
        // init part
        
        config = cfg;
        
        osalConfig.cpufreq = config.timing.clockfrequency;
        osalConfig.tick = config.timing.ticktime;            
        osalConfig.launcherstacksize = config.oninit.stacksize;        
        osalConfig.idlestacksize = config.onidle.stacksize;
                        
        //osalFatalErrorCallback = config.onfatalerror;

        // start part
        
        osalInit();
            
        // 1. init rtos in standard way:
    
        started = true;
        osal_system_start(osalLauncher);  

        return false;        
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
              
    }
    
    Impl() 
    {        
        set_osal_default_config(osalConfig);  
        config.clear();
        started = false;  
    }
};

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::sys::theScheduler& embot::sys::theScheduler::getInstance()
{
    static theScheduler* p = new theScheduler();
    return *p;
}

embot::sys::theScheduler::theScheduler()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::sys::theScheduler::~theScheduler() { }


bool embot::sys::theScheduler::start(const Config &config)
{  
    if(true == started())
    {
        return false;
    }
    return pImpl->start(config);
}

bool embot::sys::theScheduler::started() const
{   
    return pImpl->started;
}


//bool embot::sys::theScheduler::isStarted()
//{
//    return pImpl->started;
//}

//embot::common::relTime embot::sys::theScheduler::getTick()
//{
//    return pImpl->osalConfig.tick;
//}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

