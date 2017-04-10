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

#include "embot_app_application_theCANparserMTB.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <new>

#include "embot_hw.h"
#include "embot_app_canprotocol.h"

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theSkin.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserMTB::Impl
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
    
    
    bool process_set_brdcfg(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);   
    bool process_set_trgcfg(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);      
    bool process_set_txmode(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
          
};


bool embot::app::application::theCANparserMTB::Impl::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
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
            // only embot::app::canprotocol::aspollCMD::SKIN_SET_BRD_CFG, SKIN_SET_TRIANG_CFG, SET_TXMODE            
            if(static_cast<std::uint8_t>(embot::app::canprotocol::aspollCMD::SKIN_SET_BRD_CFG) == cmd)
            {
                txframe = process_set_brdcfg(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::aspollCMD::SKIN_SET_TRIANG_CFG) == cmd)
            {
                txframe = process_set_trgcfg(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::aspollCMD::SET_TXMODE) == cmd)
            {
                txframe = process_set_txmode(frame, replies);
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




bool embot::app::application::theCANparserMTB::Impl::process_set_brdcfg(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::Message_aspoll_SKIN_SET_BRDCFG msg;
    msg.load(frame);
      
    embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();    
    theskin.config(msg.info);
            
    return msg.reply();        
}


bool embot::app::application::theCANparserMTB::Impl::process_set_trgcfg(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::Message_aspoll_SKIN_SET_TRIANG_CFG msg;
    msg.load(frame);
      
    embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();    
    theskin.config(msg.info);
    
    return msg.reply();        
}


bool embot::app::application::theCANparserMTB::Impl::process_set_txmode(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::Message_aspoll_SET_TXMODE msg(embot::app::canprotocol::Board::mtb4);
    msg.load(frame);
    
    if(true == msg.info.transmit)
    {
        
    }
      
    #warning TODO: retrieve the mode and call theSkin object. 
    // if it must start, the object will start a timer which sends an event to mai task wich will calle theSkin::tick() which puts data on can frames.
    // if it must stop, the object will stop teh timer
            
    return msg.reply();        
}





// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserMTB::theCANparserMTB()
: pImpl(new Impl)
{       

}

   
        
bool embot::app::application::theCANparserMTB::initialise(Config &config)
{
    pImpl->config = config;
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve address
    pImpl->canaddress = canbrdinfo.getCANaddress();
    
    return true;
}
  


bool embot::app::application::theCANparserMTB::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


