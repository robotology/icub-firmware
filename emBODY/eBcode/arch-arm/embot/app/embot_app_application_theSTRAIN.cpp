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

#include "embot_app_application_theSTRAIN.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <new>
#include "embot_sys_Timer.h"
#include "embot_sys_Action.h"
#include "embot_hw.h"
#include "embot_app_canprotocol.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_common.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theSTRAIN::Impl
{ 
    // pity that armcc does not supporst std::tuple<>
    struct TripleValue
    {
        std::int16_t    x;
        std::int16_t    y;
        std::int16_t    z;
        void reset() { x = y = z = 0; }
        void set(std::int16_t xx, std::int16_t yy, std::int16_t zz) { x = xx; y = yy; z = zz; }
        TripleValue() { reset(); } 
    }; 
    
      
    Config config;
           
    bool ticking;
    
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
        
    
    
    std::uint8_t canaddress;
    
    embot::common::Time txperiod;
       
    TripleValue force;
    TripleValue torque;

    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::sys::Timer;   
        txperiod = 50*embot::common::time1millisec;    

        force.reset();
        torque.reset();

        canaddress = 0;  
        
    }
    
   
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configure();
    
    bool acquisition();
    
    
    bool fill(embot::app::canprotocol::Message_asper_FORCE_VECTOR::Info &info);
    bool fill(embot::app::canprotocol::Message_asper_TORQUE_VECTOR::Info &info);
                      
};



bool embot::app::application::theSTRAIN::Impl::start()
{    
    ticktimer->start(txperiod, embot::sys::Timer::Type::forever, action);
    ticking = true;    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::Message_asper_FORCE_VECTOR::Info &info)
{
    bool ret = true;
    
    info.canaddress = canaddress;
    info.x = force.x;
    info.y = force.y;
    info.z = force.z;
         
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::Message_asper_TORQUE_VECTOR::Info &info)
{
    bool ret = true;

    info.canaddress = canaddress;
    info.x = torque.x;
    info.y = torque.y;
    info.z = torque.z;
    
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::acquisition()
{
    #warning TODO: perform hw acquisition of all FT values

    
    static std::int16_t vf = 0;
    static std::int16_t vt = 3;
    
    force.x = vf++;
    force.y = vf++;
    force.z = vf++;
    
    torque.x = vt++;
    torque.y = vt++;
    torque.z = vt++;
    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
        
    // perform acquisition
    acquisition();
    
    embot::hw::can::Frame frame;   
                                            
    
    embot::app::canprotocol::Message_asper_FORCE_VECTOR::Info forceinfo;
    if(true == fill(forceinfo))
    {
        embot::app::canprotocol::Message_asper_FORCE_VECTOR msg;
        msg.load(forceinfo);
        msg.get(frame);
        replies.push_back(frame);
    }            

    
    embot::app::canprotocol::Message_asper_TORQUE_VECTOR::Info torqueinfo;
    if(true == fill(torqueinfo))
    {
        embot::app::canprotocol::Message_asper_TORQUE_VECTOR msg;
        msg.load(torqueinfo);
        msg.get(frame);
        replies.push_back(frame);
    }           
   
       
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theSTRAIN::theSTRAIN()
: pImpl(new Impl)
{       

}

         
bool embot::app::application::theSTRAIN::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.set(embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask));

    // retrieve address    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    pImpl->canaddress = canbrdinfo.getCANaddress();
    
 
    return true;
}


bool embot::app::application::theSTRAIN::configure(embot::common::Time txperiod)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    pImpl->txperiod = txperiod;

//    // if the rate is not zero: start acquisition
//            
//    if((0 != pImpl->txperiod))
//    {
//        start();
//    }
//  
    return true;    
}


bool embot::app::application::theSTRAIN::start()
{    
    return pImpl->start();
}


bool embot::app::application::theSTRAIN::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::theSTRAIN::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


