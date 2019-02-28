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

#include "embot_app_application_thePOSreader.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"

#include <new>
#include "embot_sys_Timer.h"
#include "embot_sys_Action.h"
#include "embot_hw.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_periodic.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_common.h"


#include "embot_hw_si7051.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::thePOSreader::Impl
{ 
 
#if 0 
    struct canConfig
    {
        embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_SET::Info  info;
        embot::common::relTime txperiod;
        canConfig() { reset(); } 
        void reset()
        {
            counter = 0;
            info.sensormask = 0;
            txperiod = 1000*embot::common::time1millisec;
        }
        std::uint8_t counter;
    };
    

     
    struct tempAcquisition
    {
        embot::common::relTime duration;
        embot::common::Time timeofstart;
        bool dataisready;
        embot::hw::si7051::Temperature temperature;
        tempAcquisition() { reset(); } 
        void reset()
        {
            duration = 0;
            timeofstart = 0;
            dataisready = false;
            temperature = 0;
        } 
        void onstart()
        {
            timeofstart = embot::sys::timeNow(); 
            duration = 0;            
            dataisready = false;
            temperature = 0;
        }
        void onstop()
        {
            dataisready = true;  
            duration = embot::sys::timeNow() - timeofstart;    
        }
    };
    
#endif 

    Config config;
           
    bool ticking;
    std::uint8_t acquisitionmask;
        
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    common::relTime txperiod;

//    tempAcquisition tempacquisition;
//    canConfig canconfig;
    
   
    Impl() 
    {   
        ticking = false;  
        acquisitionmask = 0;
        txperiod = 10*embot::common::time1millisec;

        ticktimer = new embot::sys::Timer;                   

//        tempacquisition.reset();
    }
    
   
    bool start();
    bool stop();    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    bool process(embot::common::Event evt, std::vector<embot::hw::can::Frame> &replies);
    
              

    bool acquisition_start();
    bool acquisition_retrieve(std::uint8_t n);
    bool acquisition_processing(std::uint8_t n);
    bool acquisition_transmit(std::vector<embot::hw::can::Frame> &replies);
    
    
    static void alertdataisready00(void *p)
    {
        embot::app::application::thePOSreader::Impl *mypImpl = reinterpret_cast<embot::app::application::thePOSreader::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.owner->setEvent(mypImpl->config.events.dataready[0]);
        }
        
//        mypImpl->tempacquisition.onstop();    
    }
    
    
    static void alertdataisready01(void *p)
    {
        embot::app::application::thePOSreader::Impl *mypImpl = reinterpret_cast<embot::app::application::thePOSreader::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.owner->setEvent(mypImpl->config.events.dataready[1]);
        }
        
//        mypImpl->tempacquisition.onstop();    
    }
                      
};



bool embot::app::application::thePOSreader::Impl::start()
{ 
//    canconfig.counter = 0;
    embot::sys::Timer::Config cfg(txperiod, action, embot::sys::Timer::Mode::forever);
    ticktimer->start(cfg);
    ticking = true;    
    return true;            
}


bool embot::app::application::thePOSreader::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}



bool embot::app::application::thePOSreader::Impl::process(embot::common::Event evt, std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
    if(evt == config.events.acquire)
    {       
        acquisition_start();
    }
    else if(evt == config.events.dataready[0])
    {
        acquisition_retrieve(0);    
        acquisition_processing(0);
    }
    else if(evt == config.events.dataready[1])
    {
        acquisition_retrieve(1);    
        acquisition_processing(1);
    }
    else
    {
        return false;
    }
    
    // devo ora recuperare 
    if(true == embot::binary::mask::check(acquisitionmask, static_cast<std::uint8_t>(0b11)))
    {
        acquisition_transmit(replies);
    }
       
    return true;    
}


bool embot::app::application::thePOSreader::Impl::acquisition_transmit(std::vector<embot::hw::can::Frame> &replies)
{   
        
    if(false == embot::binary::mask::check(acquisitionmask, static_cast<std::uint8_t>(0b11)))
    {
        return false;
    }
        
    // we are ready to transmit    
    embot::hw::can::Frame frame;   
     
        
    embot::app::canprotocol::analog::periodic::Message_THERMOMETER_MEASURE msg;
    embot::app::canprotocol::analog::periodic::Message_THERMOMETER_MEASURE::Info info;  

    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
    info.mask = 0x01;
    info.temp0 = 666; //tempacquisition.temperature;        
    
    msg.load(info);
    msg.get(frame);
    replies.push_back(frame);        

    acquisitionmask = 0;
             
    return true;           
}


bool embot::app::application::thePOSreader::Impl::acquisition_start()
{
    // entrambi
    acquisitionmask =  0;
//    tempacquisition.onstart();
//    embot::common::Callback cbk(alertdataisready00, this);
//    embot::hw::si7051::acquisition(config.sensor, cbk);
    
    
    alertdataisready00(this);
    alertdataisready01(this);
    
    return true;
}


bool embot::app::application::thePOSreader::Impl::acquisition_retrieve(std::uint8_t n)
{
//    embot::hw::si7051::read(config.sensor, tempacquisition.temperature);           
    return true;
}

bool embot::app::application::thePOSreader::Impl::acquisition_processing(std::uint8_t n)
{
    
    embot::binary::bit::set(acquisitionmask, n);
        
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::thePOSreader& embot::app::application::thePOSreader::getInstance()
{
    static thePOSreader* p = new thePOSreader();
    return *p;
}

embot::app::application::thePOSreader::thePOSreader()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::thePOSreader::~thePOSreader() { }

         
bool embot::app::application::thePOSreader::initialise(const Config &config)
{
    pImpl->config = config;
    
    pImpl->action.load(embot::sys::EventToTask(pImpl->config.events.acquire, pImpl->config.owner));
  
//    embot::hw::si7051::init(pImpl->config.sensor, pImpl->config.sensorconfig); 
     
    return true;
}




//bool embot::app::application::thePOSreader::configure(embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_SET::Info &info)
//{
//    // if ticking: stop it
//    if(true == pImpl->ticking)
//    {
//        stop();
//    }
//    
//    // copy new configuration
//    pImpl->canconfig.info = info;
//    
//    return true;    
//}

//bool embot::app::application::thePOSreader::get(embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_GET::ReplyInfo &info)
//{    
//    // copy configuration
//    info.sensormask = pImpl->canconfig.info.sensormask;
// 
//    return true;    
//}

bool embot::app::application::thePOSreader::start(embot::common::relTime period)
{   
    if(0 != period)
    {
        pImpl->txperiod = period;
    }
    return pImpl->start();
}


bool embot::app::application::thePOSreader::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::thePOSreader::process(embot::common::Event evt, std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->process(evt, replies);
}

#if 0
bool embot::app::application::thePOSreader::processdata(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->processdata(replies);
}
#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


