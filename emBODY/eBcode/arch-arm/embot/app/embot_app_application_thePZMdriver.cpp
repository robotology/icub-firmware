
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_thePZMdriver.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "embot_os_rtos.h"
#include "embot_hw.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_core.h"

#include "embot_hw_sys.h"

#include "embot_app_theLEDmanager.h"

#include "embot_os_theScheduler.h"

//#undef USE_PZMdriver

#if defined(USE_PZMdriver) 
#include "piezo.h"
#include "tables.h"
#endif


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------




    
struct embot::app::application::thePZMdriver::Impl
{   
    Config config {};
        
    embot::core::Time ageofsetpoint {0};  
    
    std::array<piezoMotorState_t, numberofmotors> _PZMstate {STATE_NOT_INIT, STATE_NOT_INIT, STATE_NOT_INIT};


    embot::os::Timer *timerDISABLEsetpoint {nullptr};    

       
    struct threadsafeVelocities
    {
        embot::os::rtos::mutex_t *_mtx {nullptr};
        std::array<int16_t, numberofmotors> _velocities {0};
        //embot::core::Time _arrival {0};
        threadsafeVelocities() : _mtx(embot::os::rtos::mutex_new()) {}
            
        bool set(int16_t v0, int16_t v1, int16_t v2, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            bool changed = false;            
            if(true == embot::os::rtos::mutex_take(_mtx, tout))
            {
                //_arrival = embot::core::now();
                _velocities[0] = v0;
                _velocities[1] = v1;
                _velocities[2] = v2;
                changed = true;
                embot::os::rtos::mutex_release(_mtx);
            }
            
            return changed;            
        }
        
//        bool check(embot::core::relTime maxage, embot::core::relTime tout = embot::core::reltimeWaitForever)
//        {
//            bool changed = false;
//            if(true == embot::os::rtos::mutex_take(_mtx, tout))
//            {                
//                embot::core::Time n = embot::core::now(); 
//                embot::core::Time delta = n - _arrival;
//                if(delta > maxage)
//                {
//                    _arrival = n; 
//                    _velocities[0] = _velocities[1] = _velocities[2] = 0; 
//                    changed = true;
//                }

//                embot::os::rtos::mutex_release(_mtx);
//            }
//            
//            return changed;               
//        }
        
        int16_t get(uint8_t n, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            int16_t v {0};
            if(n >= numberofmotors)
            {
                return v;
            }
            
            if(true == embot::os::rtos::mutex_take(_mtx, tout))
            {
                v = _velocities[n];
                embot::os::rtos::mutex_release(_mtx);
                return v;
            }            
            return v;            
        }
        
        bool get(std::array<int16_t, numberofmotors> &vv, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            if(true == embot::os::rtos::mutex_take(_mtx, tout))
            {
                vv = _velocities;
                embot::os::rtos::mutex_release(_mtx);
                return true;
            }            
            return false;            
        }
        
    };
    
    threadsafeVelocities *tsvelocities {nullptr};
    
   
    Impl() 
    {    
        timerDISABLEsetpoint = new embot::os::Timer;         
        tsvelocities = new threadsafeVelocities;
    }
              
    bool start()
    {
       
        return true;        
    }
    
    bool stop()
    {
        return true;
    }        

};



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::thePZMdriver& embot::app::application::thePZMdriver::getInstance()
{
    static thePZMdriver* p = new thePZMdriver();
    return *p;
}

embot::app::application::thePZMdriver::thePZMdriver()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::thePZMdriver::~thePZMdriver() { }

         
bool embot::app::application::thePZMdriver::initialise(const Config &config)
{
    pImpl->config = config;
        
    // start the driver
    
#if defined(USE_PZMdriver)        
    piezoMotorCfg_t pzm = { delta_8192, 8192 };
    piezoInit( &pzm, &pzm, &pzm);
        
    for(int i=0; i<pImpl->_PZMstate.size(); i++)
    { 
        piezoGetState(static_cast<piezoMotor_t>(i), &pImpl->_PZMstate[i]);       
    }
#endif  
   
       
    embot::os::Action a{};    
    a.load(embot::os::EventToThread(pImpl->config.evstop, pImpl->config.owner));
    embot::os::Timer::Config c(pImpl->config.timeout, a, embot::os::Timer::Mode::oneshot);    
    pImpl->timerDISABLEsetpoint->load(c);
                    
    embot::core::print("thePZMdriver::initialise() has started");


    return true;
}


bool embot::app::application::thePZMdriver::start()
{   
    return pImpl->start();
}


bool embot::app::application::thePZMdriver::stop()
{ 
    return pImpl->stop();
}

bool embot::app::application::thePZMdriver::process(embot::os::EventMask evtmask)
{  

    bool applyvels = false;
    bool starttimer = false;
    std::array<int16_t, numberofmotors> vels {0};

    
    if(true == embot::core::binary::mask::check(evtmask, pImpl->config.evmove))
    { 
        // someone put the target velocities in tsvelocities         
        applyvels = pImpl->tsvelocities->get(vels); 
        starttimer = true;        
    }
    
    if(true == embot::core::binary::mask::check(evtmask, pImpl->config.evstop))
    {
        // we just set the target velocities to zero.        
        vels = {0, 0, 0};
        applyvels = pImpl->tsvelocities->set(0, 0, 0); 
        applyvels = true; // we force application 
        starttimer = false;        
    }
    
    if(applyvels)
    {

#if defined(USE_PZMdriver) 
        for(int i=0; i< vels.size(); i++)
        {          
            piezoSetStepFrequency(static_cast<piezoMotor_t>(i), vels[i]);            
        }
        embot::core::print(std::string("@ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + ": PZ = " + std::to_string(vels[0]) + " " +  std::to_string(vels[1]) + " " + std::to_string(vels[2]));       

        for(int i=0; i<pImpl->_PZMstate.size(); i++)
        { 
            piezoGetState(static_cast<piezoMotor_t>(i), &pImpl->_PZMstate[i]);   
        } 
        embot::core::print(std::string("@ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + ": PZM status = " + std::to_string(pImpl->_PZMstate[0]) + " " +  std::to_string(pImpl->_PZMstate[1]) + " " + std::to_string(pImpl->_PZMstate[2]));                       
#else
        embot::core::print(std::string("@ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + ": PZ = " + std::to_string(vels[0]) + " " +  std::to_string(vels[1]) + " " + std::to_string(vels[2]));
#endif  

        if(starttimer)
        {
            embot::core::print("started timer");
            pImpl->timerDISABLEsetpoint->start();
        }        
    }
    
    return true;   
}


// interface to CANagentMC

bool embot::app::application::thePZMdriver::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    // this function is called inside the tCOMM thread.
    // in here we just load the values inside the setpoints and then we send an event to user thread ...
    bool applied = pImpl->tsvelocities->set(info.current[0], info.current[1], info.current[2]);   
    if(applied)
    {
        pImpl->config.owner->setEvent(pImpl->config.evmove);
    }
    return true;    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


