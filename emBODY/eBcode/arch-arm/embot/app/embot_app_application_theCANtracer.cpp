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

#include "embot_app_application_theCANtracer.h"



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



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANtracer::Impl
{    
    Config config;
        
    embot::common::Time starttime;
    
    
    Impl() 
    {   
        starttime = 0;  
    }
    
                  
};





// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANtracer::theCANtracer()
: pImpl(new Impl)
{       

}

   
        
bool embot::app::application::theCANtracer::initialise(Config &config)
{
    pImpl->config = config;    
    return true;
}
  


embot::common::Time embot::app::application::theCANtracer::start()
{  
    pImpl->starttime = embot::sys::timeNow();     
    return pImpl->starttime;    
}


embot::common::relTime embot::app::application::theCANtracer::stop(const std::string &prefix, std::vector<embot::hw::can::Frame> &frames, embot::common::Time started)
{ 
    embot::common::Time tt = embot::sys::timeNow();
    
    if(0 == started)
    {
        started = pImpl->starttime;
    }
    pImpl->starttime = 0;
    
    tt -= started;    
    embot::common::relTime delta = static_cast<embot::common::relTime>(tt);
    
    char strTT[12] = {0};
    std::snprintf(strTT, sizeof(strTT), "%d", delta);    
    std::string text = prefix + std::string(strTT);
    
    theCANtracer::print(text, frames);

    return delta;    
}


bool embot::app::application::theCANtracer::print(const std::string &text, std::vector<embot::hw::can::Frame> &frames)
{ 
    embot::app::canprotocol::Message_mcper_PRINT msg;
    embot::app::canprotocol::Message_mcper_PRINT::Info info;

    info.canaddress = pImpl->config.canaddress;
    std::snprintf(info.text, sizeof(info.text), "%s", text.c_str());
       
    msg.load(info);
    std::uint8_t nframes = msg.numberofframes();
    
    for(int i=0; i<nframes; i++)
    {    
        embot::hw::can::Frame frame0;
        msg.get(frame0);
        frames.push_back(frame0);
    }
    
    return true;    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


