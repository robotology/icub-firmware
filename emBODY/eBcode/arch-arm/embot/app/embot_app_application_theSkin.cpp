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

#include "embot_app_application_theSkin.h"



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

#include "AD7147.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//#define TEST_CANPRINT

struct embot::app::application::theSkin::Impl
{    
    Config config;
        
    bool ticking;
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    
    embot::app::canprotocol::Message_aspoll_SKIN_SET_BRDCFG::Info boardconfig;
    
    embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info triangleconfigcommand;
    
    std::uint16_t trianglemask;

    std::uint8_t canaddress;

    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::sys::Timer;   
        boardconfig.txperiod = 50*embot::common::time1millisec;    

        canaddress = 0;  

        trianglemask = 0xffff;        
    }
    
   
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configtriangles(embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info &trgcfg);
                  
};


bool embot::app::application::theSkin::Impl::configtriangles(embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info &trgcfg)
{    
    triangleconfigcommand = trgcfg;
    
    if((triangleconfigcommand.trgStart > 15) || (triangleconfigcommand.trgEnd > 15) || (triangleconfigcommand.trgStart > triangleconfigcommand.trgEnd))
    {
        return false;
    }
    
    for(std::uint8_t i=triangleconfigcommand.trgStart; i<=triangleconfigcommand.trgEnd; i++)
    {
        if(true == triangleconfigcommand.enabled)
        {
            embot::common::bit::set(trianglemask, i);
        }
        else
        {
            embot::common::bit::clear(trianglemask, i);
        }
    }    
        
    return true;    
}


bool embot::app::application::theSkin::Impl::start()
{
    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance();
#if defined(TEST_CANPRINT)        
    tr.start();
#endif
    
    ticktimer->start(boardconfig.txperiod, embot::sys::Timer::Type::forever, action);
    ticking = true;    
    return true;
}


bool embot::app::application::theSkin::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}


bool embot::app::application::theSkin::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
#if defined(TEST_CANPRINT)     
    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance();
    tr.stop(std::string("u"), replies);    
    tr.start();
//    tr.print(std::string("ciao uomo, come stai? io bene!xxx devo dire. anche se il protocollo tronca oltre xxx"), replies);
#endif  
    
    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance(); 
    tr.start();
    ad7147_acquire();  
    tr.stop(std::string("u"), replies);       

#if 1
    // in here there are some things to be done:
    // implement a simple state machine with states: calib / transmit
    // in any case we acquire values and we place them inside the triangle data structure.
    // if state is calib: use read values to better tune the scales ...
    // if state is transmit: we compensate the read values with the scales ... then we fill replies with many many canframes (2 per active triangle).
    
    // i emit a single ...
    
    embot::app::canprotocol::Message_skper_TRG msg;
    embot::app::canprotocol::Message_skper_TRG::Info info;
    for(std::uint8_t t=0; t<16; t++)
    {
        if(true == embot::common::bit::check(trianglemask, t))
        {
            std::uint16_t *the12values = ad7147_get12valuesoftriangle(t);
            if(nullptr != the12values)
            {
                info.canaddress = canaddress;
                info.trianglenum = t;
                for(std::uint8_t i=0; i<12; i++)
                {
                    info.the12s[i] = (the12values[i] >> 8) & 0xff;
                }
                info.outofrangemaskofthe12s = 0;    // none
                info.errorintriangle = 0;           // none
                msg.load(info);
                
                embot::hw::can::Frame frame0;
                embot::hw::can::Frame frame1;
                msg.get(frame0, frame1);
                replies.push_back(frame0);
                replies.push_back(frame1);
            }
            else
            {
                the12values = the12values;
            }
        }
    }
    
    
    #warning this is just a test. TODO: add a struct with 16 triangles with 12 values + errors each one. 
#endif

    
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theSkin::theSkin()
: pImpl(new Impl)
{       

}

   
        
bool embot::app::application::theSkin::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.set(embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask));

    // retrieve address    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    pImpl->canaddress = canbrdinfo.getCANaddress();
    
    pImpl->trianglemask = 0xffff;
    
    ad7147_init();
    
    return true;
}
  

bool embot::app::application::theSkin::config(embot::app::canprotocol::Message_aspoll_SKIN_SET_BRDCFG::Info &brdcfg)
{
    pImpl->boardconfig = brdcfg;
    
    pImpl->trianglemask = 0;
  
    return true;    
}


bool embot::app::application::theSkin::config(embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info &trgcfg)
{ 
    return pImpl->configtriangles(trgcfg);
}


bool embot::app::application::theSkin::start()
{    
    return pImpl->start();
}


bool embot::app::application::theSkin::stop()
{    
    return pImpl->stop();
}

bool embot::app::application::theSkin::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


