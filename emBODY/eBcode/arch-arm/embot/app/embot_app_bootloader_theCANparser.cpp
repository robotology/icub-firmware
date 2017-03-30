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

#include "embot_app_bootloader_theCANparser.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <new>

#include "embot_sys_theJumper.h"
#include "embot_sys_Timer.h"

#include "embot_hw.h"
#include "embot_app_canprotocol.h"

#include "embot_app_theBootloader.h"
#include "embot_hw_FlashBurner.h"
#include "embot_app_theCANboardInfo.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::bootloader::theCANparser::Impl
{    
    Config config;
    // add a state machine which keeps ::Idle, ::Connected, ::Updating
    // add the jumper, so that we can jump at expiry of ...
    // add an object which gets and sends packets.
    // add an object canboardinfo to get/set info (can address, etc.)
    // think of something which stores info about the running process (version, board type, etc.). maybe an object or a rom mapped variable.

    enum class State { Idle = 0, Connected = 1, Updating = 2};
    
    State state;
    
    bool countdownIsActive;
    bool txframe;
    
    embot::app::canprotocol::Clas cls;
    std::uint8_t cmd;
    
    embot::app::canprotocol::versionOfFIRMWARE version;
    embot::app::canprotocol::Board board;
    std::uint8_t canaddress;

    Impl() 
    {              
        state = State::Idle;
        countdownIsActive = true;
        txframe = false;
        cls = embot::app::canprotocol::Clas::none;
        cmd = 0;
        
        canaddress = 0;
        version.major = 0;
        version.minor = 0;
        version.build = 255;
        board = embot::app::canprotocol::Board::unknown;
       
    }
    
    void setstate(State newstate)
    {
        state = newstate;
    }
    
    bool process(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply);
    
    bool process_bl_broadcast(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply);
    
    
        
};


bool embot::app::bootloader::theCANparser::Impl::process(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    txframe = false;
    
    if(countdownIsActive)
    {   // do it as quick as possible
        embot::app::theBootloader &bl  = embot::app::theBootloader::getInstance();
        bl.stopcountdown();
        countdownIsActive = false;
    }
    
    // now get cls and cmd
    cls = embot::app::canprotocol::frame2clas(frame);
    cmd = embot::app::canprotocol::frame2cmd(frame);
    
    
    // only some states can handle them.
    
    switch(state)
    {
        case Impl::State::Idle:
        {
            // only bldrCMD::BROADCAST
            if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::BROADCAST)))
            {
                txframe = process_bl_broadcast(frame, reply);
                                
                // then go to connected.
                setstate(Impl::State::Connected);
            }
             
        } break;
        
        case Impl::State::Connected:
        {
            // only bldrCMD::BOARD, aspollCMD::SET_BOARD_ADX, mcpollCMD::SET_BOARD_ID
            if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::BOARD)))
            {
                // process_frame_board()
                
                // then go to update.
                setstate(Impl::State::Updating);
            }
            else if((cls == embot::app::canprotocol::Clas::pollingMotorControl) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::mcpollCMD::SET_BOARD_ID)))
            {
                // process_frame_change_address()
                
                // dont change state
            }
            else if((cls == embot::app::canprotocol::Clas::pollingAnalogSensor) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::aspollCMD::SET_BOARD_ADX)))
            {
                // process_frame_change_address()
                
                // dont change state
            } 
            
        } break;

        case Impl::State::Updating:
        {
            // only bldrCMD::ADDRESS, bldrCMD::DATA, bldrCMD::START, bldrCMD::END
            if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::ADDRESS)))
            {
                // process_frame_address()
                
            }
            else if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::DATA)))
            {
                // process_frame_data()
                
            }
            else if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::START)))
            {
                // process_frame_start()

            }
            else if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::END)))
            {
                // process_frame_end()
                
                // send reply, and then ... force a jump to application after the reply has been sent. 
                // we could define a callback on tx of teh next can frame ... no.
                // simplest and maybe safest way is to plan a restart in 100 ms. 
                embot::app::theBootloader &bl  = embot::app::theBootloader::getInstance();
                bl.startcountdown(100*embot::common::time1millisec);
            }             
        } break;

        
        default:
        {
            txframe = false;
        } break;
    }    
    
    return txframe;
}

bool embot::app::bootloader::theCANparser::Impl::process_bl_broadcast(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    embot::app::canprotocol::Message_bldr_BROADCAST msg;
    msg.load(frame);
    
    embot::app::canprotocol::Message_bldr_BROADCAST::ReplyInfo replyinfo;
    replyinfo.board = embot::app::canprotocol::Board::mtb4;
    replyinfo.process = embot::app::canprotocol::Process::bootloader;
    replyinfo.firmware.major = version.major;
    replyinfo.firmware.minor = version.minor;
    replyinfo.firmware.build = 255;
        
    return msg.reply(reply, canaddress, replyinfo);        
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::bootloader::theCANparser::theCANparser()
: pImpl(new Impl)
{       
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();
}

   
        
bool embot::app::bootloader::theCANparser::initialise(Config &config)
{
    pImpl->config = config;
    pImpl->setstate(Impl::State::Idle);
    pImpl->countdownIsActive = true;
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve version of bootloader + address
    embot::app::theCANboardInfo::StoredInfo storedinfo;
    if(true == canbrdinfo.get(storedinfo))
    {
        pImpl->canaddress = storedinfo.canaddress;
        pImpl->version.major = storedinfo.bootloaderVmajor;
        pImpl->version.minor = storedinfo.bootloaderVminor;
        pImpl->board = static_cast<embot::app::canprotocol::Board>(storedinfo.boardtype);        
    }
      
    
    return true;
}
  


bool embot::app::bootloader::theCANparser::process(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{    
    return pImpl->process(frame, reply);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


