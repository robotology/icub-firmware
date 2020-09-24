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

#include "embot_os_Thread.h"

#include "embot_os_rtos.h"

#include <cstring>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

constexpr bool releaseINITatend {false};


struct embot::os::theScheduler::Impl
{ 
    Config config {};     
    bool started {false};
    int latesterrorcode {0};
    const char * latesterrorstring {nullptr};
    
    embot::os::rtos::scheduler_props_t props {};
    
    static void osIdleThread(void)
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
        
        bool initISreleased {false};
        
        // start the forever loop
        for(;;)
        {
            if(true == releaseINITatend)
            {
                if(false == initISreleased)
                {
                    initISreleased = thesystem.pImpl->releaseInitResources();
                }
            }
            
            if(nullptr != onidle)
            {
                onidle(&theidlethread, param);
            }
        }        
    }
            
    
    static void osOnError(embot::os::rtos::thread_t* thread, int errorcode, const char * errormsg)
    {
        embot::os::theScheduler &thesystem = embot::os::theScheduler::getInstance();   
        thesystem.pImpl->latesterrorcode = static_cast<int>(errorcode);
        thesystem.pImpl->latesterrorstring = errormsg;
        if(nullptr != thread)
        {
            embot::os::Thread *t = embot::os::rtos::scheduler_getassociated(thread);  
            const char *tname = ((nullptr == t) || (nullptr == t->getName())) ? "none" : t->getName();             
            embot::core::print(std::string("from os::Thread ") + tname + " -> " + errormsg);  
        }            
        thesystem.pImpl->config.behaviour.onOSerror.execute();  
        
        // we dont stop in here
        // for(;;);  
    }    

                     
    static void launcher(void) 
    {
        embot::os::InitThread &initthread = embot::os::InitThread::getInstance();
        initthread.synch();
        
        embot::os::theScheduler &thesystem = embot::os::theScheduler::getInstance();                
        if(nullptr != thesystem.pImpl->config.behaviour.initconfig.startup)
        {
            thesystem.pImpl->config.behaviour.initconfig.startup(&initthread, thesystem.pImpl->config.behaviour.initconfig.param);
        } 

        initthread.terminate();
    }
    

    [[noreturn]] void start(const Config &cfg)
    {
        if(false == cfg.isvalid())
        {
            for(;;);
        }
        
        // init part
        
        config = cfg;
        
        props.prepare(config.timing.ticktime, 
                      config.behaviour.initconfig.stacksize, config.behaviour.idleconfig.stacksize, 
                      osIdleThread, launcher, osOnError);
                
        embot::os::rtos::scheduler_init(props);
              
        started = true;

        //embot::os::rtos::scheduler_start();
        
        embot::os::rtos::scheduler_start2();
        
        for(;;);
    }
    
    Impl() 
    {        
        started = false;  
    }
    
    embot::os::Thread* runningtask()
    {           
        return embot::os::rtos::scheduler_getassociated(embot::os::rtos::scheduler_thread_running());
    }  

    bool releaseInitResources()
    { 
        embot::os::InitThread &initthread = embot::os::InitThread::getInstance();   
        if(false == initthread.isterminated())
        {
            return false;
        }
        return props.release();
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

embot::os::Thread * embot::os::theScheduler::scheduled() const
{
    if(false == pImpl->started)
    {
        return nullptr;
    }
    
    return pImpl->runningtask();      
}

const char * embot::os::theScheduler::getOSerror(int &errorcode) const
{ 
    errorcode = pImpl->latesterrorcode;
    return pImpl->latesterrorstring; 
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

