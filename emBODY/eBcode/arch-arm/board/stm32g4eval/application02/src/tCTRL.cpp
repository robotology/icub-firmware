
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "tCTRL.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_Thread.h"
#include "embot_os_Timer.h"
#include "embot_os_rtos.h"

#include "embot_app_PZdriver.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


     
struct embot::app::ctrl::tCTRL::Impl
{  
    Config _config {};  

    static constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);        

    embot::os::EventThread * evt {nullptr};
    embot::os::Timer *tmr {nullptr};
    
    bool _active {true};
    
    embot::app::PZdriver *pzd {nullptr};
    
    embot::app::PZdriver::setpoint_t _setpoint {0};
    
    Impl() 
    {
        // ....
    }
    
    ~Impl()
    {
        // ...
    }
    
    bool start(const Config &config)
    {
        _config = config;
        // ...
        
         embot::os::EventThread::Config configEV { 
            6*1024, 
            embot::os::Priority::high44, 
            ctrl_startup,
            this,
            50*embot::core::time1millisec,
            ctrl_onevent,
            "tCTRL"
        };
        
            
        // create the main thread 
        evt = new embot::os::EventThread;          
        // and start it
        evt->start(configEV);        
        
        // and activate the timer.
        tmr = new embot::os::Timer;   
        embot::os::Action act(embot::os::EventToThread(evtTick, evt));
        embot::os::Timer::Config cfg{_config.period, act, embot::os::Timer::Mode::forever, 0};
        tmr->start(cfg);       
        
        return true;
    } 
    
    static void ctrl_startup(embot::os::Thread *t, void *param)
    {
        embot::app::ctrl::tCTRL::Impl *impl = reinterpret_cast<embot::app::ctrl::tCTRL::Impl*>(param);
        
        impl->pzd = new embot::app::PZdriver;
        impl->pzd->init({});
        impl->pzd->start();
    }
    
    static void ctrl_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {   
        if(0 == eventmask)
        {   // timeout ...         
            return;
        }
        
        embot::app::ctrl::tCTRL::Impl *impl = reinterpret_cast<embot::app::ctrl::tCTRL::Impl*>(param);

        if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
        {      
            embot::core::TimeFormatter tf(embot::core::now());        
            // embot::hw::sys::puts("tCTRL::onevent() -> evtTick received @ time = " + tf.to_string());  

            if(true == impl->_active)
            {
                impl->get_data();
                
                impl->run_matlab();
                
                impl->get_data();
            }
        }
        
        // other possible events ...
        
    }
    
    
    void get_data() 
    {
        
        // read shared memory pool
    
    }
            
    void run_matlab()
    {
        // ...
        
        
        _setpoint++;    
    }
            
    void apply_results()
    {
        pzd->set(_setpoint);
    }
        
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

    

embot::app::ctrl::tCTRL::tCTRL()
: pImpl(new Impl())
{ 

}


embot::app::ctrl::tCTRL::~tCTRL()
{   
    delete pImpl;
}

bool embot::app::ctrl::tCTRL::start(const Config &config)
{
    return pImpl->start(config);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





