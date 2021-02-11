
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "tSNSR.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_Thread.h"
#include "embot_os_Timer.h"
#include "embot_os_rtos.h"

#include "embot_hw_tlv493d.h"

#include "embot_app_application_theFAPreader.h"
#include "embot_app_application_thePOSreader2.h"

#if defined(USE_PZMdriver) 
#include "lr17_encoder.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

void thrSNSR(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
     
struct embot::app::ctrl::tSNSR::Impl
{ 

    static constexpr embot::os::Event evtSNSR01_acquire = embot::core::binary::mask::pos2mask<embot::os::Event>(0);  
    static constexpr embot::os::Event evtSNSR01_ready = embot::core::binary::mask::pos2mask<embot::os::Event>(1);  
    embot::os::Timer *tmrSNSR01 {nullptr};
    
    
    Config _config {};    

    embot::os::EventThread * evt {nullptr};
    
    Impl() 
    {
        // ....
    }
    
    ~Impl()
    {
        // ...
    }
    
    bool initialise(const Config &config)
    {
        _config = config;
        // ...
        
         embot::os::EventThread::Config configEV { 
            6*1024, 
            //embot::os::Priority::high44, 
             embot::os::Priority::abovenorm32,
            snsr_startup,
            this,
            200*embot::core::time1millisec,
            snsr_onevent,
            "tSNSR"
        };
        
            
        // create the main thread 
        evt = new embot::os::EventThread;          
        // and start it
        evt->start(configEV, thrSNSR);     

        tmrSNSR01 = new embot::os::Timer;         
        
        return true;
    } 
    
    bool start()
    {
        
        return true;
    }
    
    bool start(snsr s, embot::core::relTime freq)
    {
        // ...
//        switch(s)
//        {
//            case snsr::SNSR01:
//            {
//                embot::os::Action act(embot::os::EventToThread(evtSNSR01_acquire, evt));
//                embot::os::Timer::Config cfg{freq, act, embot::os::Timer::Mode::forever, 0};
//                tmrSNSR01->start(cfg);
//            } break;
//            
//            default:
//            {
//                
//            } break;
//            
//        }
        
        return true;
    } 
    
    bool stop(snsr s)
    {
//        switch(s)
//        {
//            case snsr::SNSR01:
//            {
//                tmrSNSR01->stop();
//            } break;
//            
//            default:
//            {
//                
//            } break;
//            
//        }
        
        return true;
    }  


    static void snsr_startup(embot::os::Thread *t, void *param)
    {
        embot::app::ctrl::tSNSR::Impl *impl = reinterpret_cast<embot::app::ctrl::tSNSR::Impl*>(param);

#if defined(USE_PZMdriver)         
        lr17_encoder_init();
#endif
    }
    
    static void snsr_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
    {   
        if(0 == eventmask)
        {   // timeout ...         
            return;
        }
        
        embot::app::application::thePOSreader2 &thereader = embot::app::application::thePOSreader2::getInstance(); 

        thereader.process(eventmask);
        
//        embot::app::ctrl::tSNSR::Impl *impl = reinterpret_cast<embot::app::ctrl::tSNSR::Impl*>(param);

//        if(true == embot::core::binary::mask::check(eventmask, evtSNSR01_acquire)) 
//        {   
//            // trigger acquisition of this sensor with a callback which sends an evt evtSNSR01_ready

//        }

//        if(true == embot::core::binary::mask::check(eventmask, evtSNSR01_ready)) 
//        {      
//            // get the value from hw and update the shared memory, so that the CTRL thread can use it
//        }       

        
        
    }    

};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

    

embot::app::ctrl::tSNSR::tSNSR()
: pImpl(new Impl())
{ 

}


embot::app::ctrl::tSNSR::~tSNSR()
{   
    delete pImpl;
}

bool embot::app::ctrl::tSNSR::initialise(const Config &config)
{
    return pImpl->initialise(config);
}


bool embot::app::ctrl::tSNSR::start()
{
    return pImpl->start();
}

bool embot::app::ctrl::tSNSR::start(snsr s, embot::core::relTime freq)
{
    return pImpl->start(s, freq);
}

bool embot::app::ctrl::tSNSR::stop(snsr s)
{
    return pImpl->stop(s);
}

embot::os::Thread * embot::app::ctrl::tSNSR::getThread()
{
    return pImpl->evt;
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





