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

#include "embot_os_theScheduler.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"

#include <cstring>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::os::theScheduler::Impl
{ 

    Config config {};    
    osal_cfg_t osalConfig; 
    bool started {false};
    int latesterrorcode {0};
    const char * latesterrorstring {nullptr};

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
        cfg.extfn.usr_on_fatal_error = osalOnError;
        cfg.extfn.usr_on_idle = osalIdleThread;
    }
            
    static void osalIdleThread(void)
    {
        embot::os::theScheduler &thesystem = embot::os::theScheduler::getInstance(); 
        IdleThread &theidlethread = embot::os::IdleThread::getInstance();
        const Thread::fpStartup startup = thesystem.pImpl->config.behaviour.idleconfig.startup;
        const Thread::fpOnIdle onidle = thesystem.pImpl->config.behaviour.idleconfig.onidle;
        void * param = thesystem.pImpl->config.behaviour.idleconfig.param;
       
        // make sure the idletask is linked to the rtos
        theidlethread.synch();

        // exec the startup
        if(nullptr != startup)
        {
            startup(&theidlethread, param);
        }
        
        // start the forever loop
        for(;;)
        {
            if(nullptr != onidle)
            {
                onidle(&theidlethread, param);
            }
        }        
    }
        
    
    static void osalOnError(void* task, osal_fatalerror_t errorcode, const char * errormsg)
    {
        embot::os::theScheduler &thesystem = embot::os::theScheduler::getInstance();   
        thesystem.pImpl->latesterrorcode = static_cast<int>(errorcode);
        thesystem.pImpl->latesterrorstring = errormsg;        
        thesystem.pImpl->config.behaviour.onOSerror.execute();            
        // we dont stop in here
        // for(;;);  
    }

                     
    static void osalLauncher(void) 
    {
        embot::os::InitThread &inittask = embot::os::InitThread::getInstance();
        inittask.synch();
        
        embot::os::theScheduler &thesystem = embot::os::theScheduler::getInstance();                
        if(nullptr != thesystem.pImpl->config.behaviour.initconfig.startup)
        {
            thesystem.pImpl->config.behaviour.initconfig.startup(&inittask, thesystem.pImpl->config.behaviour.initconfig.param);
        }        
    }
    

    [[noreturn]] void start(const Config &cfg)
    {
        if(false == cfg.isvalid())
        {
            for(;;);
        }
        
        // init part
        
        config = cfg;
        
        osalConfig.cpufreq = config.timing.clockfrequency;
        osalConfig.tick = config.timing.ticktime;            
        osalConfig.launcherstacksize = config.behaviour.initconfig.stacksize;        
        osalConfig.idlestacksize = config.behaviour.idleconfig.stacksize;
                        
        // start part        
        osalInit();
            
        // 1. init rtos in standard way:
    
        started = true;
        osal_system_start(osalLauncher);  
        
        for(;;);
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
        started = false;  
    }
};

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::os::theScheduler& embot::os::theScheduler::getInstance()
{
    static theScheduler* p = new theScheduler();
    return *p;
}

embot::os::theScheduler::theScheduler()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::os::theScheduler::~theScheduler() { }


[[noreturn]] void embot::os::theScheduler::start(const Config &config)
{  
    if(true == started())
    {
        for(;;);
    }
    pImpl->start(config);
}

bool embot::os::theScheduler::started() const
{   
    return pImpl->started;
}

embot::core::relTime  embot::os::theScheduler::ticktime() const
{
    return pImpl->config.timing.ticktime;
}

embot::os::Thread * embot::os::theScheduler::scheduledtask() const
{
    if(false == pImpl->started)
    {
        return nullptr;
    }
    
    osal_task_t *p = osal_task_get(osal_callerAUTOdetect);

    if(nullptr == p)
    {
        return(nullptr);
    }

    return reinterpret_cast<Thread*>(osal_task_extdata_get(p));         
}

const char * embot::os::theScheduler::getOSerror(int &errorcode) const
{ 
    errorcode = pImpl->latesterrorcode;
    return pImpl->latesterrorstring; 
}


//embot::core::relTime embot::os::theScheduler::getTick()
//{
//    return pImpl->osalConfig.tick;
//}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

