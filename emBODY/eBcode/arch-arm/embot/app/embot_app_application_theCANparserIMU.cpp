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

#include "embot_app_application_theCANparserIMU.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <new>

#include "embot_hw.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_polling.h"
#include "embot_app_canprotocol_analog_periodic.h"

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theIMU.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserIMU::Impl
{    
    Config config;
        
    bool txframe;
    bool recognised;
    
    embot::app::canprotocol::Clas cls;
    std::uint8_t cmd;    
    std::uint8_t canaddress;
        
    embot::hw::can::Frame reply;
    

    Impl() 
    {   
        recognised = false;        
        txframe = false;
        cls = embot::app::canprotocol::Clas::none;
        cmd = 0;        
        canaddress = 0;        
    }
    
   
    bool process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    
    bool process_set_accgyrosetup(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);          
};


bool embot::app::application::theCANparserIMU::Impl::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    txframe = false;
    recognised = false;
    
    if(false == embot::app::canprotocol::frameis4board(frame, canaddress))
    {
        recognised = false;
        return recognised;
    }
        
    // now get cls and cmd
    cls = embot::app::canprotocol::frame2clas(frame);
    cmd = embot::app::canprotocol::frame2cmd(frame);
    
    
    // the basic can handle only some messages ...
    
    switch(cls)
    {
        
        case embot::app::canprotocol::Clas::pollingAnalogSensor:
        {
            // only ACC_GYRO_SETUP used for the inertials ...
            if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::ACC_GYRO_SETUP) == cmd)
            { 
                txframe = process_set_accgyrosetup(frame, replies);
                recognised = true;                
            }
 
        } break;

        
        default:
        {
            txframe = false;
            recognised = false;
        } break;
    }    
    
    
    return recognised;
}



bool embot::app::application::theCANparserIMU::Impl::process_set_accgyrosetup(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP msg;
    msg.load(frame);
      
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();    
    theimu.configure(msg.info);
    
    return msg.reply();        
}





// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserIMU::theCANparserIMU()
: pImpl(new Impl)
{       

}

   
        
bool embot::app::application::theCANparserIMU::initialise(Config &config)
{
    pImpl->config = config;
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve address
    pImpl->canaddress = canbrdinfo.getCANaddress();
    
    return true;
}
  


bool embot::app::application::theCANparserIMU::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


