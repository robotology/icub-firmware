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

#include "embot_hw_sys.h"
#include "embot_hw_tlv493d.h"

#include "embot_app_theLEDmanager.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//#define DEBUG_COMPENSATE_SENSOR
    
struct embot::app::application::thePOSreader::Impl
{ 
    
    // we have numberofpositions = 2 sensors, hence we have 2 config
        
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
    



    Config config;
           
    bool ticking;
    std::uint8_t acquisitionmask;
    std::uint8_t sensorstoacquiremask;
    uint8_t sensorspresencemask;
        
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    common::relTime txperiod;

    canConfig canconfig;
    
    static constexpr embot::hw::tlv493d::Position valueOfPositionCHIPnotinitted = 2000*100;         // which will results in 2000 degrees or 20000 decidegrees
    static constexpr embot::hw::tlv493d::Position valueOfPositionACQUISITIONnotvalid = 1000*100;    // which will results in 1000 degrees or 10000 decidegrees
    std::array<embot::hw::tlv493d::Position, numberofpositions> positions;
    std::array<embot::app::canprotocol::analog::deciDeg, 3> decidegvalues;
    
   
    Impl() 
    {   
        ticking = false;  
        acquisitionmask = 0;
        sensorstoacquiremask = 0;
        sensorspresencemask = 0;
        txperiod = 10*embot::common::time1millisec;

        ticktimer = new embot::sys::Timer;      

        canconfig.reset(); 
        canconfig.descriptor[0].label = embot::app::canprotocol::analog::posLABEL::zero;
        canconfig.descriptor[1].label = embot::app::canprotocol::analog::posLABEL::one;
        
        positions[0] = positions[1] = 0;  
        decidegvalues[0] = decidegvalues[1] = decidegvalues[2] = 0;        

#if defined(DEBUG_COMPENSATE_SENSOR)
        
        canconfig.descriptor[0].invertdirection = false;
        canconfig.descriptor[0].rotation = embot::app::canprotocol::analog::polling::deciDegPOSdescriptor::ROT::none;
        canconfig.descriptor[0].label = embot::app::canprotocol::analog::posLABEL::zero;
        canconfig.descriptor[0].zero = embot::app::canprotocol::analog::deciDeg_import(0.0);
        
        canconfig.descriptor[1].invertdirection = true;
        canconfig.descriptor[1].rotation = embot::app::canprotocol::analog::polling::deciDegPOSdescriptor::ROT::plus180;
        canconfig.descriptor[1].label = embot::app::canprotocol::analog::posLABEL::one;
        canconfig.descriptor[1].zero = embot::app::canprotocol::analog::deciDeg_import(348.0);
        
#endif

    }
    
    bool isvalid(const embot::hw::tlv493d::Position &po) const
    {
        if((po == valueOfPositionCHIPnotinitted) || (po == valueOfPositionACQUISITIONnotvalid))
        {
            return false;
        }
        return true;        
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
    }
    
    
    static void alertdataisready01(void *p)
    {
        embot::app::application::thePOSreader::Impl *mypImpl = reinterpret_cast<embot::app::application::thePOSreader::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.owner->setEvent(mypImpl->config.events.dataready[1]);
        }        
    }
                      
};



bool embot::app::application::thePOSreader::Impl::start()
{ 
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
    
 
    if(true == embot::binary::mask::check(acquisitionmask, sensorstoacquiremask))
    {
        acquisition_transmit(replies);
    }
       
    return true;    
}


bool embot::app::application::thePOSreader::Impl::acquisition_transmit(std::vector<embot::hw::can::Frame> &replies)
{   
        
    if(false == embot::binary::mask::check(acquisitionmask, sensorspresencemask))
    {
        return false;
    }
        
    // we are ready to transmit    
    embot::hw::can::Frame frame;   
                 
    embot::app::canprotocol::analog::periodic::Message_POS msg;
    embot::app::canprotocol::analog::periodic::Message_POS::Info info;  
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
    if((true == canconfig.descriptor[0].enabled) && (true == canconfig.descriptor[1].enabled))
    {   // we transmit two
        if(isvalid(positions[0]))
        {
            decidegvalues[0] = canconfig.descriptor[0].transform(positions[0]/10);
        }
        else
        {
            decidegvalues[0] = positions[0]/10;
        }
        if(isvalid(positions[1]))
        {
            decidegvalues[1] = canconfig.descriptor[1].transform(positions[1]/10);
        }
        else
        {
            decidegvalues[1] = positions[1]/10;
        }
        info.loadDeciDeg(canconfig.descriptor[0].label, 2, decidegvalues);
    }
    else if((true == canconfig.descriptor[0].enabled))
    {   // we transmit only the first
        if(isvalid(positions[0]))
        {
            decidegvalues[0] = canconfig.descriptor[0].transform(positions[0]/10);
        }
        else
        {
            decidegvalues[0] = positions[0]/10;
        }
        info.loadDeciDeg(canconfig.descriptor[0].label, 1, decidegvalues);
    }
    else if((true == canconfig.descriptor[1].enabled))
    {   // we transmit only the second
        if(isvalid(positions[1]))
        {
            decidegvalues[0] = canconfig.descriptor[1].transform(positions[1]/10);
        }
        else
        {
            decidegvalues[0] = positions[1]/10;
        }
        info.loadDeciDeg(canconfig.descriptor[1].label, 1, decidegvalues);
    }
    else
    {   // we transmit none
        
    }
    
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
    sensorstoacquiremask = 0;
    
    if(0 == sensorspresencemask)
    {
        // if we dont have any sensor because the initi has failed, then we must transmit the failure anyway
        // hence, we emit a alertdataisready00
        alertdataisready00(this);        
        return true;
    }
      
    // else, we start acquisition from the sensors which are available
    if(embot::binary::bit::check(sensorspresencemask, 0))
    {        
        embot::common::Callback cbk00(alertdataisready00, this);
        embot::hw::tlv493d::acquisition(config.sensors[0].id, cbk00);
        embot::binary::bit::set(sensorstoacquiremask, 0);
    }

    if(embot::binary::bit::check(sensorspresencemask, 1))
    {
        embot::common::Callback cbk01(alertdataisready01, this);
        embot::hw::tlv493d::acquisition(config.sensors[1].id, cbk01);
        embot::binary::bit::set(sensorstoacquiremask, 1);
    }
       
    return true;
}


bool embot::app::application::thePOSreader::Impl::acquisition_retrieve(std::uint8_t n)
{
    if(embot::hw::resOK != embot::hw::tlv493d::read(config.sensors[n].id, positions[n]))
    {
        positions[n] = valueOfPositionACQUISITIONnotvalid;
    }
    
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
    
    pImpl->positions[0] = pImpl->positions[1] = pImpl->valueOfPositionCHIPnotinitted;
    
    embot::hw::sys::delay(50*embot::common::time1millisec);
      
    if(embot::hw::resOK == embot::hw::tlv493d::init(pImpl->config.sensors[0].id, pImpl->config.sensors[0].config))
    {
        embot::binary::bit::set(pImpl->sensorspresencemask, static_cast<uint8_t>(pImpl->config.sensors[0].id));
    }  

    
    embot::hw::sys::delay(50*embot::common::time1millisec);
    
    if(embot::hw::resOK == embot::hw::tlv493d::init(pImpl->config.sensors[1].id, pImpl->config.sensors[1].config))
    {
        embot::binary::bit::set(pImpl->sensorspresencemask, static_cast<uint8_t>(pImpl->config.sensors[1].id));
    }
    
    
    // pImpl->sensorspresencemask = 2;
    
    if(false == embot::binary::mask::check(pImpl->sensorspresencemask, static_cast<uint8_t>(0b11)))
    {
        
        embot::app::LEDwaveT<64> ledwave(100*embot::common::time1millisec, 50, std::bitset<64>(0b0101010101));
        
        if(true == embot::binary::bit::check(pImpl->sensorspresencemask, 0))
        {
            ledwave.load(20, std::bitset<64>(0b0001), 4); 
        }
        else if(true == embot::binary::bit::check(pImpl->sensorspresencemask, 1))
        {
            ledwave.load(20, std::bitset<64>(0b0101), 4);
        }
        
        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();
        theleds.get(embot::hw::LED::one).wave(&ledwave);  
    }

     
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



// interface to CANagentPOS

bool embot::app::application::thePOSreader::set(const embot::app::canprotocol::analog::polling::Message_POS_CONFIG_SET::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    if(info.type == embot::app::canprotocol::analog::posTYPE::angleDeciDeg)
    {
        pImpl->canconfig.descriptor[0] = info.descriptor[0];
        pImpl->canconfig.descriptor[1] = info.descriptor[1];
    }
    
    return true;    
}

bool embot::app::application::thePOSreader::get(const embot::app::canprotocol::analog::polling::Message_POS_CONFIG_GET::Info &info, embot::app::canprotocol::analog::polling::Message_POS_CONFIG_GET::ReplyInfo &replyinfo)
{    

    replyinfo.type = info.type;
    if(info.type == embot::app::canprotocol::analog::posTYPE::angleDeciDeg)
    {
        replyinfo.descriptor[0] = pImpl->canconfig.descriptor[0];
        replyinfo.descriptor[1] = pImpl->canconfig.descriptor[1];
    }
    
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


