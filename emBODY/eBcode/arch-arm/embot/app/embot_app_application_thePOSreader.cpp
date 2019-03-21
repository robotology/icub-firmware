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


#include "embot_hw_tlv493d.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::thePOSreader::Impl
{ 
    
    // we have numberofpositions = 2 sensors, hence we have 2 config
    
//    struct canConfig
//    {
//        embot::common::relTime txperiod;
//        std::array<embot::app::canprotocol::analog::polling::Message_POS_CONFIG_SET::Info, numberofpositions> info;
//        canConfig() { reset(); }
//        void reset()
//        {
//            txperiod = 100*embot::common::time1millisec;
//            for(auto &a : info)
//            {
//                a.reset();
//            }
//        }
//    };
    
    struct canConfig
    {
        std::array<embot::app::canprotocol::analog::polling::deciDegPOSdescriptor, numberofpositions> descriptor;
        canConfig() { reset(); }
        void reset()
        {
            for(auto &a : descriptor)
            {
                a.reset();
            }
        }
    };
    

 
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
    std::uint8_t sensorstoacquiremask;
        
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    common::relTime txperiod;

    canConfig canconfig;
    
    std::array<embot::hw::tlv493d::Position, numberofpositions> positions;
    
   
    Impl() 
    {   
        ticking = false;  
        acquisitionmask = 0;
        sensorstoacquiremask = 0;
        txperiod = 10*embot::common::time1millisec;

        ticktimer = new embot::sys::Timer;      

        canconfig.reset(); 
        positions[0] = positions[1] = 0;    
        
        canconfig.descriptor[1].invertdirection = true;
        canconfig.descriptor[1].rotation = embot::app::canprotocol::analog::polling::deciDegPOSdescriptor::ROT::plus180;
        canconfig.descriptor[1].label = embot::app::canprotocol::analog::polling::deciDegPOSdescriptor::LABEL::one;
        canconfig.descriptor[1].zero = embot::app::canprotocol::analog::polling::deciDeg_import(348.0);

//        canconfig.info[1].applyphaseshift180 = true;
//        canconfig.info[1].revertdirection = true;
//        canconfig.info[1].loadzero(348.0);
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
    if(true == embot::binary::mask::check(acquisitionmask, sensorstoacquiremask))
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
     
        
    embot::app::canprotocol::analog::periodic::Message_POS msg;
    embot::app::canprotocol::analog::periodic::Message_POS::Info info;  

    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
//    info.descriptor = canconfig.info[0].descriptor;
    
#if 0   
    #warning TBD: we now write dummy bytes, but we must write positions[0] and positions[1] properly scaled
    static volatile int32_t pp = 0;
    static volatile int32_t pn = 0;
    static volatile int32_t pp180p = 0;
    static volatile int32_t pp180n = 0;
    static volatile int32_t pn180p = 0;
    static volatile int32_t pn180n = 0;
    
    static volatile int32_t pp180pC = 0;
    static volatile int32_t pn180nC = 0;

    pp = 0 + positions[1];
    pp = pp % 36000;
    pp /= 100;
    
    pn = 0 - positions[1];
    pn = pn % 36000;
    pn /= 100;
    
    pp180p = 18000 + positions[1];
    pp180p = pp180p % 36000;
    pp180p /= 100;
    
    pp180n = -18000 + positions[1];
    pp180n = pp180n % 36000;
    pp180n /= 100;
    
       
    pn180p = 18000 - positions[1];
    pn180p = pn180p % 36000;
    pn180p /= 100;    
        
    pn180n = - 18000 - positions[1];
    pn180n = pn180n % 36000;
    pn180n /= 100;  
    
    constexpr int pn180nZERO = -168;
    pn180nC = pn180n - pn180nZERO;
    
    constexpr int pp180pZERO = +168;
    pp180pC = pp180p - pp180pZERO;
    
    constexpr int zero = 193; // 167;    
    static volatile int32_t value0 = 0;
    value0 = ((positions[1]/100) + 180) % 360;
    value0 = value0 - zero;
    value0 = -value0;

    constexpr int zeroraw = 348;    
    static volatile int32_t value1 = 0;
    value1 = ((positions[1]/100) + 180) % 360;
    value1 = value1 - ((zeroraw+180)%360);
    value1 = -value1;
#endif


    int16_t p0 = canconfig.descriptor[0].transform(positions[0]/10);
    int16_t p1 = canconfig.descriptor[1].transform(positions[1]/10);
    
    static volatile int16_t v1 = 0;
    v1 = p1;
    
//    int16_t zero0 = canconfig.info[0].getzeroI16Ddec();
//    int16_t zero1 = canconfig.info[1].getzeroI16Ddec();
//    
//    if(canconfig.info[0].applyphaseshift180)
//    {
//        p0 = (p0+1800)%3600;
//        p0 = p0 - ((zero0+1800)%3600);
//    }
//    else
//    {
//        p0 = p0 - zero0;
//    }
//    
//    if(canconfig.info[0].revertdirection)
//    {
//        p0 = -p0;
//    }
//    
//    if(canconfig.info[1].applyphaseshift180)
//    {
//        p1 = (p1+1800)%3600;
//        p1 = p1 - ((zero1+1800)%3600);
//    }
//    else
//    {
//        p1 = p1 - zero0;
//    }
//    
//    if(canconfig.info[1].revertdirection)
//    {
//        p1 = -p1;
//    }
    info.descriptor.type = embot::app::canprotocol::analog::posType::angleDeciDeg;
    info.descriptor.indexoffirst = static_cast<uint8_t>(canconfig.descriptor[0].label);
    info.descriptor.numberof = 2;
    
    info.data[0] = p0 & 0xff;         
    info.data[1] = (p0 >> 8) & 0xff;
    info.data[2] = p1 & 0xff;
    info.data[3] = (p1 >> 8) & 0xff;
    info.data[4] = 0;
    info.data[5] = 0;
    
    msg.load(info);
    msg.get(frame);
    replies.push_back(frame);        

    acquisitionmask = 0;
    sensorstoacquiremask = 0;
             
    return true;           
}


bool embot::app::application::thePOSreader::Impl::acquisition_start()
{

    acquisitionmask =  0;
        
    embot::common::Callback cbk00(alertdataisready00, this);
    embot::hw::tlv493d::acquisition(config.sensors[0].id, cbk00);
    embot::binary::bit::set(sensorstoacquiremask, 0);

    embot::common::Callback cbk01(alertdataisready01, this);
    embot::hw::tlv493d::acquisition(config.sensors[1].id, cbk01);
    embot::binary::bit::set(sensorstoacquiremask, 1);
    

    return true;
}


bool embot::app::application::thePOSreader::Impl::acquisition_retrieve(std::uint8_t n)
{
    embot::hw::tlv493d::read(config.sensors[n].id, positions[n]);
    
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
  
    embot::hw::tlv493d::init(pImpl->config.sensors[0].id, pImpl->config.sensors[0].config); 
    embot::hw::tlv493d::init(pImpl->config.sensors[1].id, pImpl->config.sensors[1].config);
     
    return true;
}




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


// interface to CANagentPOS

bool embot::app::application::thePOSreader::set(const embot::app::canprotocol::analog::polling::Message_POS_CONFIG_SET::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    pImpl->canconfig.descriptor[0] = info.descriptor[0];
    pImpl->canconfig.descriptor[1] = info.descriptor[1];
    
    return true;    
}

bool embot::app::application::thePOSreader::get(const embot::app::canprotocol::analog::polling::Message_POS_CONFIG_GET::Info &info, embot::app::canprotocol::analog::polling::Message_POS_CONFIG_GET::ReplyInfo &replyinfo)
{    

    replyinfo.tbd = info.tbd;
    replyinfo.descriptor[0] = pImpl->canconfig.descriptor[0];
    replyinfo.descriptor[1] = pImpl->canconfig.descriptor[1];
    
    return true;    
}


bool embot::app::application::thePOSreader::set(const embot::app::canprotocol::analog::polling::Message_POS_TRANSMIT::Info &info)
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



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


