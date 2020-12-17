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

#include "embot_hw_bno055.h"
//#include "embot_hw_bsp_strain2.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theIMU::Impl
{ 
    
    struct canLegacyConfig
    {
        embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::Info  accgyroinfo;
        bool accelEnabled;
        bool gyrosEnabled;  
        canLegacyConfig() { reset(); } 
        void reset()
        {
            accgyroinfo.maskoftypes = 0;
            accgyroinfo.txperiod = 50*embot::core::time1millisec;
            accelEnabled = false;       
            gyrosEnabled = false;
        }            
    };
    
    struct canRevisitedConfig
    {
        embot::prot::can::analog::polling::Message_IMU_CONFIG_SET::Info  imuinfo;
        embot::core::relTime txperiod;
        canRevisitedConfig() { reset(); } 
        void reset()
        {
            counter = 0;
            imuinfo.sensormask = 0;
            txperiod = 50*embot::core::time1millisec;
        }
        std::uint8_t counter;
    };
    
    
    struct imuAcquisition
    {
        embot::core::relTime duration;
        embot::core::Time timeofstart;
        bool dataisready;
        embot::hw::bno055::Data data;
        imuAcquisition() { reset(); } 
        void reset()
        {
            duration = 0;
            timeofstart = 0;
            dataisready = false;
            data.clear();
        } 
        void onstart()
        {
            timeofstart = embot::core::now(); 
            duration = 0;            
            dataisready = false;
            data.clear();
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

    imuAcquisition imuacquisition;

    bool legacymode;    // if true we work with the old protocol mode.
    canLegacyConfig canlegacyconfig;
    canRevisitedConfig canrevisitedconfig;
    
   
    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::os::Timer;           

        legacymode = true;
        canlegacyconfig.reset();
        

        imuacquisition.reset();
    }
    
   
    bool start();
    bool stop();    
    bool tick(std::vector<embot::prot::can::Frame> &replies);
    bool processdata(std::vector<embot::prot::can::Frame> &replies);
    
    
    bool fill(embot::prot::can::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info &info);
    bool fill(embot::prot::can::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info &info);
    
    bool fill(embot::prot::can::inertial::periodic::Message_IMU_TRIPLE::Info &info);
    
    
    
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
        embot::os::Timer::Config cfg(canlegacyconfig.accgyroinfo.txperiod, action, embot::os::Timer::Mode::forever);
        ticktimer->start(cfg);
        ticking = true;    
        return true;
    }
    else
    {
        canrevisitedconfig.counter = 0;
        embot::os::Timer::Config cfg(canrevisitedconfig.txperiod, action, embot::os::Timer::Mode::forever);
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


bool embot::app::application::theIMU::Impl::fill(embot::prot::can::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info &info)
{
    bool ret = true;
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = imuacquisition.data.acc.x;
    info.y = imuacquisition.data.acc.y;
    info.z = imuacquisition.data.acc.z;
         
    return ret;    
}


bool embot::app::application::theIMU::Impl::fill(embot::prot::can::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = imuacquisition.data.gyr.x;
    info.y = imuacquisition.data.gyr.y;
    info.z = imuacquisition.data.gyr.z;
    
    return ret;    
}

bool embot::app::application::theIMU::Impl::fill(embot::prot::can::inertial::periodic::Message_IMU_TRIPLE::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    
    switch(info.sensor)
    {
        case embot::prot::can::analog::imuSensor::acc:
        {
            info.value = imuacquisition.data.acc;            
        } break;
        
        case embot::prot::can::analog::imuSensor::gyr:
        {
            info.value = imuacquisition.data.gyr;            
        } break;
        
        default:
        {
            info.value.clear();            
        } break;        
        
    }
        
    return ret;    
}


bool embot::app::application::theIMU::Impl::tick(std::vector<embot::prot::can::Frame> &replies)
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


bool embot::app::application::theIMU::Impl::processdata(std::vector<embot::prot::can::Frame> &replies)
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
     
    if(true == legacymode) 
    {        
        if(true == canlegacyconfig.accelEnabled)
        {
            embot::prot::can::inertial::periodic::Message_DIGITAL_ACCELEROMETER msg;
            embot::prot::can::inertial::periodic::Message_DIGITAL_ACCELEROMETER::Info accelinfo;
            if(true == fill(accelinfo))
            {
                msg.load(accelinfo);
                msg.get(frame);
                replies.push_back(frame);
            }            
        }
        
        if(true == canlegacyconfig.gyrosEnabled)
        {
            embot::prot::can::inertial::periodic::Message_DIGITAL_GYROSCOPE msg;
            embot::prot::can::inertial::periodic::Message_DIGITAL_GYROSCOPE::Info gyrosinfo;
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
        
        embot::prot::can::inertial::periodic::Message_IMU_TRIPLE msg;
        embot::prot::can::inertial::periodic::Message_IMU_TRIPLE::Info info;  

        
        info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
        info.seqnumber = canrevisitedconfig.counter;        
        
        // evaluate what to tx
        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::acc))
        {
            // generate a acc message with canrevisitedconfig.imuinfo.counter
            info.sensor = embot::prot::can::analog::imuSensor::acc;
            info.value = imuacquisition.data.acc;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);                      
        }
        
        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::mag))
        {
            // generate a mag message with canrevisitedconfig.counter
            info.sensor = embot::prot::can::analog::imuSensor::mag;
            info.value = imuacquisition.data.mag;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);           
        }        

        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::gyr))
        {
            // generate a gyr message with canrevisitedconfig.counter
            info.sensor = embot::prot::can::analog::imuSensor::gyr;
            info.value = imuacquisition.data.gyr;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        } 
        
        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::eul))
        {
            // generate a eul message with canrevisitedconfig.counter
            info.sensor = embot::prot::can::analog::imuSensor::eul;
            // The Bosch euler representation can be either the windows(default) or android, but in YARP
            // we have a different representation(see https://github.com/robotology/yarp/blob/0481f994c6e03897d038c5f1d1078145646a1772/src/libYARP_dev/src/yarp/dev/MultipleAnalogSensorsInterfaces.h#L302-L348)
            info.value.x = imuacquisition.data.eul.z;
            info.value.y = -imuacquisition.data.eul.y;
            info.value.z = imuacquisition.data.eul.x;

            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }
        
        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::lia))
        {
            // generate a lia message with canrevisitedconfig.counter
            info.sensor = embot::prot::can::analog::imuSensor::lia;
            info.value = imuacquisition.data.lia;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }

        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::grv))
        {

            info.sensor = embot::prot::can::analog::imuSensor::grv;
            info.value = imuacquisition.data.grv;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }  

        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::qua))
        {
            
            embot::prot::can::inertial::periodic::Message_IMU_QUATERNION msg;
            embot::prot::can::inertial::periodic::Message_IMU_QUATERNION::Info info;  

            info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();    
            info.value = imuacquisition.data.qua;
            
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame);               
        }          

        if(true == canrevisitedconfig.imuinfo.enabled(embot::prot::can::analog::imuSensor::status))
        {
            
            embot::prot::can::inertial::periodic::Message_IMU_STATUS msg;
            embot::prot::can::inertial::periodic::Message_IMU_STATUS::Info info;  

            info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();    
            info.seqnumber = canrevisitedconfig.counter;
            info.acquisitiontime = imuacquisition.duration;
            info.acccalib = static_cast<embot::prot::can::inertial::periodic::Message_IMU_STATUS::Calibration>(imuacquisition.data.calibrationOfACC());
            info.magcalib = static_cast<embot::prot::can::inertial::periodic::Message_IMU_STATUS::Calibration>(imuacquisition.data.calibrationOfMAG());
            info.gyrcalib = static_cast<embot::prot::can::inertial::periodic::Message_IMU_STATUS::Calibration>(imuacquisition.data.calibrationOfGYR());
            
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
    embot::core::Callback cbk(alertdataisready, this);
    embot::hw::bno055::acquisition(config.sensor, embot::hw::bno055::Set::FULL, imuacquisition.data, cbk); 
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
    
    pImpl->action.load(embot::os::EventToThread(pImpl->config.tickevent, pImpl->config.totask));
   
    embot::hw::bno055::init(pImpl->config.sensor, pImpl->config.sensorconfig); 
    embot::hw::bno055::set(pImpl->config.sensor, embot::hw::bno055::Mode::NDOF, 5*embot::core::time1millisec);
     
    return true;
}



bool embot::app::application::theIMU::start()
{    
    return pImpl->start();
}


bool embot::app::application::theIMU::set(const embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    // we are in legacy mode:
    pImpl->legacymode = true;
    
    // copy new configuration
    pImpl->canlegacyconfig.accgyroinfo = info;
    
    // get some settings from it.    
//    static const std::uint8_t accelmask =   static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialType::analogaccelerometer) |
//                                            static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialType::internaldigitalaccelerometer) |
//                                            static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialType::externaldigitalaccelerometer) ;                                        
//    static const std::uint8_t gyrosmask =   static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialType::externaldigitalgyroscope);  
                                                   
    pImpl->canlegacyconfig.accelEnabled = 
                            embot::core::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::analogaccelerometer))             ||
                            embot::core::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::internaldigitalaccelerometer))    ||
                            embot::core::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::externaldigitalaccelerometer));                                               
    pImpl->canlegacyconfig.gyrosEnabled =   
                            embot::core::binary::bit::check(pImpl->canlegacyconfig.accgyroinfo.maskoftypes, static_cast<std::uint8_t>(embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::InertialTypeBit::externaldigitalgyroscope));   
    
    // if there is something to acquire and the rate is not zero: start acquisition
            
    if((0 != pImpl->canlegacyconfig.accgyroinfo.maskoftypes) && (0 != pImpl->canlegacyconfig.accgyroinfo.txperiod))
    {
        start();
    }
  
    return true;    
}

bool embot::app::application::theIMU::set(const embot::prot::can::analog::polling::Message_IMU_CONFIG_SET::Info &info)
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


bool embot::app::application::theIMU::set(const embot::prot::can::analog::polling::Message_IMU_TRANSMIT::Info &info)
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


bool embot::app::application::theIMU::get(const embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::ReplyInfo &replyinfo)
{    
    // copy configuration
    replyinfo.sensormask = pImpl->canrevisitedconfig.imuinfo.sensormask;
    replyinfo.fusion = pImpl->canrevisitedconfig.imuinfo.fusion;
    replyinfo.ffu_ranges_measureunits = pImpl->canrevisitedconfig.imuinfo.ffu_ranges_measureunits;    

    return true;    
}

bool embot::app::application::theIMU::start(embot::core::relTime period)
{      
    pImpl->canrevisitedconfig.txperiod = period;
    return pImpl->start();
}


bool embot::app::application::theIMU::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::theIMU::tick(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}


bool embot::app::application::theIMU::processdata(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->processdata(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


