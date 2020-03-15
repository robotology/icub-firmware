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

#include "embot_app_application_theTHERMO.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "embot_hw.h"
#include "embot_prot_can.h"
#include "embot_prot_can_inertial_periodic.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_core.h"

#include "embot_hw_bsp_strain2.h"

#include "embot_hw_si7051.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theTHERMO::Impl
{ 
      
    struct canConfig
    {
        embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_SET::Info  info;
        embot::core::relTime txperiod;
        canConfig() { reset(); } 
        void reset()
        {
            counter = 0;
            info.sensormask = 0;
            txperiod = 1000*embot::core::time1millisec;
        }
        std::uint8_t counter;
    };
    

     
    struct tempAcquisition
    {
        embot::core::relTime duration;
        embot::core::Time timeofstart;
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
            timeofstart = embot::core::now(); 
            duration = 0;            
            dataisready = false;
            temperature = 0;
        }
        void onstop()
        {
            dataisready = true;  
            duration = embot::core::now() - timeofstart;    
        }
    };
    
 

    Config config;
           
    bool ticking;
        
    embot::os::Timer *ticktimer;
    embot::os::Action action;

    tempAcquisition tempacquisition;
    canConfig canconfig;
    
   
    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::os::Timer;                   

        tempacquisition.reset();
    }
    
   
    bool start();
    bool stop();    
    bool tick(std::vector<embot::prot::can::Frame> &replies);
    bool processdata(std::vector<embot::prot::can::Frame> &replies);
        
    
    // thermometer support
    bool acquisition_start();
    bool acquisition_retrieve();
    bool acquisition_processing();
    
    
    static void alertdataisready(void *p)
    {
        embot::app::application::theTHERMO::Impl *mypImpl = reinterpret_cast<embot::app::application::theTHERMO::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.totask->setEvent(mypImpl->config.datareadyevent);
        }
        
        mypImpl->tempacquisition.onstop();    
    }
                      
};



bool embot::app::application::theTHERMO::Impl::start()
{ 
    canconfig.counter = 0;
    embot::os::Timer::Config cfg(canconfig.txperiod, action, embot::os::Timer::Mode::forever);
    if(true == ticking)
    {
        stop();
    }
    ticktimer->start(cfg);
    ticking = true;    
    return true;            
}


bool embot::app::application::theTHERMO::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}



bool embot::app::application::theTHERMO::Impl::tick(std::vector<embot::prot::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
    if(0 == canconfig.info.sensormask)
    {
        return false;  
    }        

       
    // start acquisition
    acquisition_start();
       
    return true;    
}


bool embot::app::application::theTHERMO::Impl::processdata(std::vector<embot::prot::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
        
    // retrieve acquired imu values
    acquisition_retrieve();
    
    // processing of acquired data
    acquisition_processing();
    
    // we are ready to transmit    
    embot::prot::can::Frame frame;   
     
        
    embot::prot::can::analog::periodic::Message_THERMOMETER_MEASURE msg;
    embot::prot::can::analog::periodic::Message_THERMOMETER_MEASURE::Info info;  

    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
    info.mask = 0x01;
    info.temp0 = tempacquisition.temperature;        
    
    msg.load(info);
    msg.get(frame);
    replies.push_back(frame);                             
             
    return true;           
}


bool embot::app::application::theTHERMO::Impl::acquisition_start()
{
    tempacquisition.onstart();
    embot::core::Callback cbk(alertdataisready, this);
    embot::hw::si7051::acquisition(config.sensor, cbk);
    return true;
}


bool embot::app::application::theTHERMO::Impl::acquisition_retrieve()
{
    embot::hw::si7051::read(config.sensor, tempacquisition.temperature);           
    return true;
}

bool embot::app::application::theTHERMO::Impl::acquisition_processing()
{
    // we dont process THERMO data
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theTHERMO& embot::app::application::theTHERMO::getInstance()
{
    static theTHERMO* p = new theTHERMO();
    return *p;
}

embot::app::application::theTHERMO::theTHERMO()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theTHERMO::~theTHERMO() { }

         
bool embot::app::application::theTHERMO::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.load(embot::os::EventToThread(pImpl->config.tickevent, pImpl->config.totask));
  
    embot::hw::si7051::init(pImpl->config.sensor, pImpl->config.sensorconfig); 
     
    return true;
}



bool embot::app::application::theTHERMO::start()
{    
    return pImpl->start();
}

bool embot::app::application::theTHERMO::set(const embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_SET::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    // copy new configuration
    pImpl->canconfig.info = info;
    
    return true;    
}

bool embot::app::application::theTHERMO::get(const embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_GET::ReplyInfo &replyinfo)
{    
    // copy configuration
    replyinfo.sensormask = pImpl->canconfig.info.sensormask;
 
    return true;    
}

bool embot::app::application::theTHERMO::set(const embot::prot::can::analog::polling::Message_THERMOMETER_TRANSMIT::Info &info)
{
    if((true == info.transmit) && (info.txperiod > 0))
    {
        start(info.txperiod);
    }
    else
    {
        stop();        
    }
    
    return true;  
}


bool embot::app::application::theTHERMO::start(embot::core::relTime period)
{      
    pImpl->canconfig.txperiod = period;
    return pImpl->start();
}


bool embot::app::application::theTHERMO::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::theTHERMO::tick(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}


bool embot::app::application::theTHERMO::processdata(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->processdata(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


