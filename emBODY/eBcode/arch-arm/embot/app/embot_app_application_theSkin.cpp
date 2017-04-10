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



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theSkin::Impl
{    
    Config config;
        
    bool ticking;
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    
    embot::app::canprotocol::Message_aspoll_SKIN_SET_BRDCFG::Info boardconfig;
    
    embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info triangleconfigcommand;

    

    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::sys::Timer;   
        boardconfig.txperiod = 50*embot::common::time1millisec;        
    }
    
   
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configtriangles(embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info &trgcfg);
                  
};


bool embot::app::application::theSkin::Impl::configtriangles(embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG::Info &trgcfg)
{    
    triangleconfigcommand = trgcfg;
    
    #warning TODO: apply the values to the triangles
        
    return true;    
}


bool embot::app::application::theSkin::Impl::start()
{    
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
    
    // in here there are some things to be done:
    // implement a simple state machine with states: calib / transmit
    // in any case we acquire values and we place them inside the triangle data structure.
    // if state is calib: use read values to better tune the scales ...
    // if state is transmit: we compensate the read values with the scales ... then we fill replies with many many canframes (2 per active triangle).
    
    
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
        
    return true;
}
  

bool embot::app::application::theSkin::config(embot::app::canprotocol::Message_aspoll_SKIN_SET_BRDCFG::Info &brdcfg)
{
    pImpl->boardconfig = brdcfg;
  
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


