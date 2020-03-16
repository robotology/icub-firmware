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

#include "embot_app_application_theCANparserBasic.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>


#include "embot_hw_sys.h" // to see embot::hw::sys::random() + embot::hw::sys::reset()
#include "embot_hw_can.h" // to see only embot::hw::can::setfilters
#include "embot_prot_can.h"
#include "embot_prot_can_bootloader.h"
#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_analog_periodic.h"
#include "embot_prot_can_analog_polling.h"

#include "embot_app_theCANboardInfo.h"
#include "embot_app_theJumper.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::application::theCANparserBasic::Impl
{    
    Config config;
    
    bool txframe;
    bool recognised;
    
    embot::prot::can::Clas cls;
    std::uint8_t cmd;
    
    embot::prot::can::versionOfCANPROTOCOL canprotocol;
    embot::prot::can::versionOfFIRMWARE version;
    embot::prot::can::Board board;
    

    
    embot::prot::can::Frame reply;
    

    Impl() 
    {   
        recognised = false;        
        txframe = false;
        cls = embot::prot::can::Clas::none;
        cmd = 0;
        
        version.major = version.minor = version.build = 0;
        canprotocol.major = canprotocol.minor = 0;
        board = embot::prot::can::Board::unknown;               
    }
    
    
    bool setcanaddress(const std::uint8_t adr, const std::uint16_t randominvalidmask)
    {
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
        // i reinforce a reading from storage. just for safety. in here we are dealing w/ can address change and i want to be sure.
        std::uint8_t canaddress = canbrdinfo.getCANaddress();
        
        std::uint8_t target = adr;
        
        if(0xff == adr)
        {
            // compute a new random address. use the randoinvalid mask to filter out the undesired values. for sure 0x8001.
            std::uint16_t mask = randominvalidmask;
            mask |= 0x8001;
            if(0xffff == mask)
            {   // hei, nothing is good for you.
                return false;
            }
            
            bool ok = false;
            for(std::uint16_t i=0; i<250; i++)
            {
                target = (embot::hw::sys::random()-embot::hw::sys::minrandom()) & 0xf;
               
                if(false == embot::core::binary::bit::check(mask, target))
                {
                    ok = true;
                    break;                    
                }
            }
            
            if(!ok)
            {
                return false;
            }
        }
        
        // always check that is is not 0 or 0xf
        if((0 == target) || (0xf == target))
        {
            return false;
        }
            
        
        if(canaddress != target)
        {
            if(embot::hw::resOK != embot::hw::can::setfilters(embot::hw::CAN::one, target))
            {
                return false;
            }
            canbrdinfo.setCANaddress(target);
            canaddress = canbrdinfo.getCANaddress();
        }
                        
        return (target == canaddress);
    }
    
    
    bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    bool process_bl_broadcast_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);  
    bool process_bl_board_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
    bool process_bl_getadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_bl_setadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_bl_setcanaddress(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_bl_gettimeoflife(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    bool process_getfirmwareversion(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
    bool process_setid(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
        
};


bool embot::app::application::theCANparserBasic::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    txframe = false;
    recognised = false;
    
    if(false == embot::prot::can::frameis4board(frame, embot::app::theCANboardInfo::getInstance().cachedCANaddress()))
    {
        recognised = false;
        return recognised;
    }
        
    // now get cls and cmd
    cls = embot::prot::can::frame2clas(frame);
    cmd = embot::prot::can::frame2cmd(frame);
    
//    replies.clear(); i dont want to clear because we may have others inside which i dont want to lose
    
    // the basic can handle only some messages ...
    
    switch(cls)
    {
        case embot::prot::can::Clas::bootloader:
        {
            // only CMD::BROADCAST, CMD::BOARD, CMD::SETCANADDRESS,  CMD::GET_ADDITIONAL_INFO,  CMD::SET_ADDITIONAL_INFO 

            if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::BOARD) == cmd)
            {
                txframe = process_bl_board_appl(frame, replies);   
                recognised = true;
                // then restart ...
                embot::hw::sys::reset();
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::BROADCAST) == cmd)
            {
                txframe = process_bl_broadcast_appl(frame, replies);   
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::SETCANADDRESS) == cmd)
            {
                txframe = process_bl_setcanaddress(frame, replies);  
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::GET_ADDITIONAL_INFO) == cmd)
            {
                txframe = process_bl_getadditionalinfo(frame, replies);   
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::SET_ADDITIONAL_INFO) == cmd)
            {
                txframe = process_bl_setadditionalinfo(frame, replies);
                recognised = true;                
            }                     
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::GET_TIMEOFLIFE) == cmd)
            {
                txframe = process_bl_gettimeoflife(frame, replies);
                recognised = true;                
            }   
            
        } break;
        

        case embot::prot::can::Clas::pollingAnalogSensor:
        {
            // only embot::prot::can::analog::polling::CMD::SET_BOARD_ADX, GET_FIRMWARE_VERSION, ??
            if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::SET_BOARD_ADX) == cmd)
            {
                txframe = process_setid(cls, cmd, frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::GET_FIRMWARE_VERSION) == cmd)
            {
                txframe = process_getfirmwareversion(cls, cmd, frame, replies);
                recognised = true;
            }
 
        } break;

        case embot::prot::can::Clas::pollingMotorControl:
        {
            // only embot::prot::can::mcpollCMD::SET_BOARD_ID, GET_FIRMWARE_VERSION, ??
            if(static_cast<std::uint8_t>(embot::prot::can::motor::polling::CMD::SET_BOARD_ID) == cmd)
            {
                txframe = process_setid(cls, cmd, frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::motor::polling::CMD::GET_FIRMWARE_VERSION) == cmd)
            {
                txframe = process_getfirmwareversion(cls, cmd, frame, replies);
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



bool embot::app::application::theCANparserBasic::Impl::process_bl_broadcast_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_BROADCAST msg;
    msg.load(frame);
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    embot::app::theCANboardInfo::StoredInfo strd = {0};
    canbrdinfo.get(strd);
    
    embot::prot::can::bootloader::Message_BROADCAST::ReplyInfo replyinfo;
    replyinfo.board = static_cast<embot::prot::can::Board>(strd.boardtype);
    replyinfo.process = embot::prot::can::Process::application;
    replyinfo.firmware.major = strd.applicationVmajor;
    replyinfo.firmware.minor = strd.applicationVminor;
    replyinfo.firmware.build = strd.applicationVbuild;
        
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(reply);
        return true;
    }        
    
    return false;
}


bool embot::app::application::theCANparserBasic::Impl::process_bl_board_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_BOARD msg;
    msg.load(frame);
    
    // i dont get any info... i just must restart. 
    embot::hw::sys::reset();  
    
    return false;       
}


bool embot::app::application::theCANparserBasic::Impl::process_bl_getadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO msg;
    msg.load(frame);
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    embot::app::theCANboardInfo::StoredInfo strd = {0};
    canbrdinfo.get(strd);
    
    embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo replyinfo;
    std::memmove(replyinfo.info32, strd.info32, sizeof(replyinfo.info32)); 
   
    std::uint8_t nreplies = msg.numberofreplies();
    for(std::uint8_t i=0; i<nreplies; i++)
    {
        if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
        {
            replies.push_back(reply);
        }
    }
    return true;
        
}


bool embot::app::application::theCANparserBasic::Impl::process_bl_setadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_SET_ADDITIONAL_INFO2 msg;
    msg.load(frame);
    
    if(true == msg.info.valid)
    {   // we have received all the 8 messages in order (important is that the one with data[1] = 0 is the first)
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
        embot::app::theCANboardInfo::StoredInfo strd = {0};
        canbrdinfo.get(strd);    
        std::memmove(strd.info32, msg.info.info32, sizeof(strd.info32));
        canbrdinfo.set(strd);
    }
    return false;        
}


bool embot::app::application::theCANparserBasic::Impl::process_setid(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::shared::Message_SET_ID msg(cl, cm);
    msg.load(frame);
      
    setcanaddress(msg.info.address, 0x0000);
            
    return msg.reply();        
}

bool embot::app::application::theCANparserBasic::Impl::process_getfirmwareversion(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    
    embot::prot::can::shared::Message_GET_VERSION msg(cl, cm);
    msg.load(frame);
      
    embot::prot::can::shared::Message_GET_VERSION::ReplyInfo replyinfo;
    
    replyinfo.board = board;
    replyinfo.firmware = version;
    replyinfo.protocol = canprotocol;
    
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {            
        replies.push_back(reply);
        return true;
    }
    
    return false;
}

bool embot::app::application::theCANparserBasic::Impl::process_bl_setcanaddress(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_SETCANADDRESS msg;
    msg.load(frame);
      
    
    setcanaddress(msg.info.address, msg.info.randominvalidmask);
            
    return msg.reply();        
}

bool embot::app::application::theCANparserBasic::Impl::process_bl_gettimeoflife(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_GET_TIMEOFLIFE msg;
    msg.load(frame);
        
    embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::ReplyInfo replyinfo;
    
    replyinfo.timeoflife = embot::core::now();
   
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {            
        replies.push_back(reply);
        return true;
    }
    
    return false;       
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------




embot::app::application::theCANparserBasic& embot::app::application::theCANparserBasic::getInstance()
{
    static theCANparserBasic* p = new theCANparserBasic();
    return *p;
}

embot::app::application::theCANparserBasic::theCANparserBasic()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
    embot::app::theJumper& thejumper = embot::app::theJumper::getInstance();
}  

    
embot::app::application::theCANparserBasic::~theCANparserBasic() { }
   
        
bool embot::app::application::theCANparserBasic::initialise(const Config &config)
{
    pImpl->config = config;

    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve version of bootloader + address
    embot::app::theCANboardInfo::StoredInfo storedinfo;
    if(true == canbrdinfo.get(storedinfo))
    {
        pImpl->version.major = storedinfo.applicationVmajor;
        pImpl->version.minor = storedinfo.applicationVminor;
        pImpl->version.build = storedinfo.applicationVbuild;
        pImpl->canprotocol.major = storedinfo.protocolVmajor;
        pImpl->canprotocol.minor = storedinfo.protocolVminor;
        pImpl->board = static_cast<embot::prot::can::Board>(storedinfo.boardtype);        
    }
      
    
    return true;
}
  


bool embot::app::application::theCANparserBasic::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


