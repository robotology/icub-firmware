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

#include "embot_app_application_theIMU.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <new>
#include "embot_sys_Timer.h"
#include "embot_sys_Action.h"
#include "embot_hw.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_inertial_periodic.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_common.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theIMU::Impl
{ 
    // pity that armcc does not supporst std::tuple<>
    struct InertialValue
    {
        std::int16_t    x;
        std::int16_t    y;
        std::int16_t    z;
        void reset() { x = y = z = 0; }
        void set(std::int16_t xx, std::int16_t yy, std::int16_t zz) { x = xx; y = yy; z = zz; }
        InertialValue() { reset(); } 
    }; 
    
      
    Config config;
           
    bool ticking;
    
    bool forcecalibration;
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    
    embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info accgyroinfo;
        
    
    bool accelEnabled;
    bool gyrosEnabled;    
    InertialValue accelValue;
    InertialValue gyrosValue;

    Impl() 
    {   
        ticking = false;  
        forcecalibration = false;

        ticktimer = new embot::sys::Timer;   
        accgyroinfo.maskoftypes = 0;
        accgyroinfo.txperiod = 50*embot::common::time1millisec;    

        accelValue.reset();
        accelEnabled = false;
        
        gyrosValue.reset();
        gyrosEnabled = false;

        
    }
    
   
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configure(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info &ag);
    
    bool acquisition();
    
    
    bool fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info &info);
    bool fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info &info);
                      
};



bool embot::app::application::theIMU::Impl::start()
{    
    ticktimer->start(accgyroinfo.txperiod, embot::sys::Timer::Type::forever, action);
    ticking = true;    
    return true;
}


bool embot::app::application::theIMU::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}


bool embot::app::application::theIMU::Impl::fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info &info)
{
    bool ret = true;
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = accelValue.x;
    info.y = accelValue.y;
    info.z = accelValue.z;
         
    return ret;    
}


bool embot::app::application::theIMU::Impl::fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = gyrosValue.x;
    info.y = gyrosValue.y;
    info.z = gyrosValue.z;
    
    return ret;    
}


bool embot::app::application::theIMU::Impl::acquisition()
{
    #warning TODO: perform hw acquisition of all IMU values over i2c and ....  read comment.
    // i acquire all values over i2c, then fill a data structure. then depending on what we want to tx we fill the packets
    
    static std::int16_t va = 0;
    static std::int16_t vg = 3;
    
    accelValue.x = va++;
    accelValue.y = va++;
    accelValue.z = va++;
    
    gyrosValue.x = vg++;
    gyrosValue.y = vg++;
    gyrosValue.z = vg++;
    
    return true;
}


bool embot::app::application::theIMU::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
    if(0 == accgyroinfo.maskoftypes)
    {
        return false;  
    }
    
    // perform acquisition
    acquisition();
    
    embot::hw::can::Frame frame;   
                                            
    if(true == accelEnabled)
    {
        embot::app::canprotocol::inertial::periodic::Message_DIGITAL_ACCELEROMETER msg;
        embot::app::canprotocol::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info accelinfo;
        if(true == fill(accelinfo))
        {
            msg.load(accelinfo);
            msg.get(frame);
            replies.push_back(frame);
        }            
    }
    
    if(true == gyrosEnabled)
    {
        embot::app::canprotocol::inertial::periodic::Message_DIGITAL_GYROSCOPE msg;
        embot::app::canprotocol::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info gyrosinfo;
        if(true == fill(gyrosinfo))
        {
            msg.load(gyrosinfo);
            msg.get(frame);
            replies.push_back(frame);
        }            
    }    
       
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theIMU::theIMU()
: pImpl(new Impl)
{       

}

         
bool embot::app::application::theIMU::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.set(embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask));
     
    return true;
}


bool embot::app::application::theIMU::configure(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info &ag)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    // copy new configuration
    pImpl->accgyroinfo = ag;
    
    // get some settings from it.    
//    static const std::uint8_t accelmask =   static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::analogaccelerometer) |
//                                            static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::internaldigitalaccelerometer) |
//                                            static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::externaldigitalaccelerometer) ;                                        
//    static const std::uint8_t gyrosmask =   static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::externaldigitalgyroscope);  
                                                   
    pImpl->accelEnabled =   embot::common::bit::check(pImpl->accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::analogaccelerometer))             ||
                            embot::common::bit::check(pImpl->accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::internaldigitalaccelerometer))    ||
                            embot::common::bit::check(pImpl->accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::externaldigitalaccelerometer));                                               
    pImpl->gyrosEnabled =   embot::common::bit::check(pImpl->accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::externaldigitalgyroscope));   
    
    // if there is something to acquire and the rate is not zero: start acquisition
            
    if((0 != pImpl->accgyroinfo.maskoftypes) && (0 != pImpl->accgyroinfo.txperiod))
    {
        start();
    }
  
    return true;    
}


bool embot::app::application::theIMU::start()
{    
    return pImpl->start();
}


bool embot::app::application::theIMU::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::theIMU::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


