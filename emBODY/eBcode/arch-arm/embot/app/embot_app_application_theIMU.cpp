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
#include "embot_common.h"
#include "embot_binary.h"

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

#include "embot_hw_bno055.h"
#include "embot_hw_bsp_strain2.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theIMU::Impl
{ 
    
    struct canLegacyConfig
    {
        embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info  accgyroinfo;
        bool accelEnabled;
        bool gyrosEnabled;  
        canLegacyConfig() { reset(); } 
        void reset()
        {
            accgyroinfo.maskoftypes = 0;
            accgyroinfo.txperiod = 50*embot::common::time1millisec;
            accelEnabled = false;       
            gyrosEnabled = false;
        }            
    };
    
    struct canRevisitedConfig
    {
        embot::app::canprotocol::analog::polling::Message_IMU_CONFIG_SET::Info  imuinfo;
        embot::common::relTime txperiod;
        canRevisitedConfig() { reset(); } 
        void reset()
        {
            counter = 0;
            imuinfo.sensormask = 0;
            txperiod = 50*embot::common::time1millisec;
        }
        std::uint8_t counter;
    };
    
    
    struct imuAcquisition
    {
        embot::common::relTime duration;
        embot::common::Time timeofstart;
        bool dataisready;
        embot::hw::BNO055::Data data;
        imuAcquisition() { reset(); } 
        void reset()
        {
            duration = 0;
            timeofstart = 0;
            dataisready = false;
            data.reset();
        } 
        void onstart()
        {
            timeofstart = embot::sys::timeNow(); 
            duration = 0;            
            dataisready = false;
            data.reset();
        }
        void onstop()
        {
            dataisready = true;  
            duration = embot::sys::timeNow() - timeofstart;    
        }
    };
    
   

    Config config;
           
    bool ticking;
        
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;

    imuAcquisition imuacquisition;

    bool legacymode;    // if true we work with the old protocol mode.
    canLegacyConfig canlegacyconfig;
    canRevisitedConfig canrevisitedconfig;
    
   
    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::sys::Timer;           

        legacymode = true;
        canlegacyconfig.reset();
        

        imuacquisition.reset();
    }
    
   
    bool start();
    bool stop();    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    bool processdata(std::vector<embot::hw::can::Frame> &replies);
    
//    bool configure(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info &ag);
    
    
    bool fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info &info);
    bool fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info &info);
    
    bool fill(embot::app::canprotocol::inertial::periodic::Message_IMU_TRIPLE::Info &info);
    
    
    
    // imu support
    bool acquisition_start();
    bool acquisition_retrieve();
    bool acquisition_processing();
    
    
    static void alertdataisready(void *p)
    {
        embot::app::application::theIMU::Impl *mypImpl = reinterpret_cast<embot::app::application::theIMU::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.totask->setEvent(mypImpl->config.datareadyevent);
        }
        
        mypImpl->imuacquisition.onstop();    
    }
                      
};



bool embot::app::application::theIMU::Impl::start()
{ 
    if(true == legacymode)
    {
        embot::sys::Timer::Config cfg(canlegacyconfig.accgyroinfo.txperiod, action, embot::sys::Timer::Mode::forever);
        ticktimer->start(cfg);
        ticking = true;    
        return true;
    }
    else
    {
        canrevisitedconfig.counter = 0;
        embot::sys::Timer::Config cfg(canrevisitedconfig.txperiod, action, embot::sys::Timer::Mode::forever);
        ticktimer->start(cfg);
        ticking = true;    
        return true;        
    }
    
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
    info.x = imuacquisition.data.acc.x;
    info.y = imuacquisition.data.acc.y;
    info.z = imuacquisition.data.acc.z;
         
    return ret;    
}


bool embot::app::application::theIMU::Impl::fill(embot::app::canprotocol::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = imuacquisition.data.gyr.x;
    info.y = imuacquisition.data.gyr.y;
    info.z = imuacquisition.data.gyr.z;
    
    return ret;    
}

bool embot::app::application::theIMU::Impl::fill(embot::app::canprotocol::inertial::periodic::Message_IMU_TRIPLE::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    
    switch(info.sensor)
    {
        case embot::app::canprotocol::analog::imuSensor::acc:
        {
            info.value = imuacquisition.data.acc;            
        } break;
        
        case embot::app::canprotocol::analog::imuSensor::gyr:
        {
            info.value = imuacquisition.data.gyr;            
        } break;
        
        default:
        {
            info.value.reset();            
        } break;        
        
    }
        
    return ret;    
}


bool embot::app::application::theIMU::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
    if(true == legacymode)
    {
        if(0 == canlegacyconfig.accgyroinfo.maskoftypes)
        {
            return false;  
        }
    }
    else //  new mode
    {
        if(0 == canrevisitedconfig.imuinfo.sensormask)
        {
            return false;  
        }        
    }
    
    
    // start acquisition
    acquisition_start();
       
    return true;    
}


bool embot::app::application::theIMU::Impl::processdata(std::vector<embot::hw::can::Frame> &replies)
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
    embot::hw::can::Frame frame;   
     
    if(true == legacymode) 
    {        
        if(true == canlegacyconfig.accelEnabled)
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
        
        if(true == canlegacyconfig.gyrosEnabled)
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
    }
    else
    {
        canrevisitedconfig.counter++;
        
        embot::app::canprotocol::inertial::periodic::Message_IMU_TRIPLE msg;
        embot::app::canprotocol::inertial::periodic::Message_IMU_TRIPLE::Info info;  

        
        info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
        info.seqnumber = canrevisitedconfig.counter;        
        
        // evaluate what to tx
        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::acc))
        {
            // generate a acc message with canrevisitedconfig.imuinfo.counter
            info.sensor = embot::app::canprotocol::analog::imuSensor::acc;
            info.value = imuacquisition.data.acc;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);                      
        }
        
        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::mag))
        {
            // generate a mag message with canrevisitedconfig.counter
            info.sensor = embot::app::canprotocol::analog::imuSensor::mag;
            info.value = imuacquisition.data.mag;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);           
        }        

        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::gyr))
        {
            // generate a gyr message with canrevisitedconfig.counter
            info.sensor = embot::app::canprotocol::analog::imuSensor::gyr;
            info.value = imuacquisition.data.gyr;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        } 
        
        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::eul))
        {
            // generate a eul message with canrevisitedconfig.counter
            info.sensor = embot::app::canprotocol::analog::imuSensor::eul;
            info.value = imuacquisition.data.eul;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }
        
        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::lia))
        {
            // generate a lia message with canrevisitedconfig.counter
            info.sensor = embot::app::canprotocol::analog::imuSensor::lia;
            info.value = imuacquisition.data.lia;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }

        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::grv))
        {

            info.sensor = embot::app::canprotocol::analog::imuSensor::grv;
            info.value = imuacquisition.data.grv;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }  

        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::qua))
        {
            
            embot::app::canprotocol::inertial::periodic::Message_IMU_QUATERNION msg;
            embot::app::canprotocol::inertial::periodic::Message_IMU_QUATERNION::Info info;  

            info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();    
            info.value = imuacquisition.data.qua;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }          

        if(true == canrevisitedconfig.imuinfo.enabled(embot::app::canprotocol::analog::imuSensor::status))
        {
            
            embot::app::canprotocol::inertial::periodic::Message_IMU_STATUS msg;
            embot::app::canprotocol::inertial::periodic::Message_IMU_STATUS::Info info;  

            info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();    
            info.seqnumber = canrevisitedconfig.counter;
            info.acquisitiontime = imuacquisition.duration;
            info.acccalib = static_cast<embot::app::canprotocol::inertial::periodic::Message_IMU_STATUS::Calibration>(imuacquisition.data.calibrationOfACC());
            info.magcalib = static_cast<embot::app::canprotocol::inertial::periodic::Message_IMU_STATUS::Calibration>(imuacquisition.data.calibrationOfMAG());
            info.gyrcalib = static_cast<embot::app::canprotocol::inertial::periodic::Message_IMU_STATUS::Calibration>(imuacquisition.data.calibrationOfGYR());
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }      
    }
         
    return true;           
}


bool embot::app::application::theIMU::Impl::acquisition_start()
{
    imuacquisition.onstart();
    embot::common::Callback cbk(alertdataisready, this);
    embot::hw::BNO055::acquisition(config.sensor, embot::hw::BNO055::Set::FULL, imuacquisition.data, cbk); 
    return true;
}


bool embot::app::application::theIMU::Impl::acquisition_retrieve()
{

    // 1. in imuacquisition.data we have the values.    
    //std::memmove(runtimedata.data.adcvalue, runtimedata.data.dmabuffer, sizeof(runtimedata.data.adcvalue));
           
    return true;
}

bool embot::app::application::theIMU::Impl::acquisition_processing()
{
    // we dont process IMU data
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theIMU& embot::app::application::theIMU::getInstance()
{
    static theIMU* p = new theIMU();
    return *p;
}

embot::app::application::theIMU::theIMU()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theIMU::~theIMU() { }

         
bool embot::app::application::theIMU::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.load(embot::sys::EventToTask(pImpl->config.tickevent, pImpl->config.totask));
   
    embot::hw::BNO055::init(pImpl->config.sensor, pImpl->config.sensorconfig); 
    embot::hw::BNO055::set(pImpl->config.sensor, embot::hw::BNO055::Mode::NDOF, 5*embot::common::time1millisec);
     
    return true;
}


bool embot::app::application::theIMU::configure(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info &ag)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    // we are in legacy mode:
    pImpl->legacymode = true;
    
    // copy new configuration
    pImpl->canlegacyconfig.accgyroinfo = ag;
    
    // get some settings from it.    
//    static const std::uint8_t accelmask =   static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::analogaccelerometer) |
//                                            static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::internaldigitalaccelerometer) |
//                                            static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::externaldigitalaccelerometer) ;                                        
//    static const std::uint8_t gyrosmask =   static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialType::externaldigitalgyroscope);  
                                                   
    pImpl->canlegacyconfig.accelEnabled = 
                            embot::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::analogaccelerometer))             ||
                            embot::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::internaldigitalaccelerometer))    ||
                            embot::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::externaldigitalaccelerometer));                                               
    pImpl->canlegacyconfig.gyrosEnabled =   
                            embot::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::externaldigitalgyroscope));   
    
    // if there is something to acquire and the rate is not zero: start acquisition
            
    if((0 != pImpl->canlegacyconfig.accgyroinfo.maskoftypes) && (0 != pImpl->canlegacyconfig.accgyroinfo.txperiod))
    {
        start();
    }
  
    return true;    
}


bool embot::app::application::theIMU::start()
{    
    return pImpl->start();
}

bool embot::app::application::theIMU::configure(embot::app::canprotocol::analog::polling::Message_IMU_CONFIG_SET::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    // we are not in legacy mode:
    pImpl->legacymode = false;
    
    // copy new configuration
    pImpl->canrevisitedconfig.imuinfo = info;
    
    return true;    
}

bool embot::app::application::theIMU::get(embot::app::canprotocol::analog::polling::Message_IMU_CONFIG_GET::ReplyInfo &info)
{    
    // copy configuration
    info.sensormask = pImpl->canrevisitedconfig.imuinfo.sensormask;
    info.fusion = pImpl->canrevisitedconfig.imuinfo.fusion;
    info.ffu_ranges_measureunits = pImpl->canrevisitedconfig.imuinfo.ffu_ranges_measureunits;    

    return true;    
}

bool embot::app::application::theIMU::start(embot::common::relTime period)
{      
    pImpl->canrevisitedconfig.txperiod = period;
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


bool embot::app::application::theIMU::processdata(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->processdata(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


