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
#include "embot_common.h"
#include "embot_binary.h"
#include <new>
#include "embot_hw.h"
#include "embot_hw_flash.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_bootloader.h"
#include "embot_app_canprotocol_motor_polling.h"
#include "embot_app_canprotocol_analog_polling.h"

#include "embot_app_theBootloader.h"
#include "embot_hw_FlashBurner.h"
#include "embot_app_theCANboardInfo.h"
#include "embot_app_theJumper.h"

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
    
    const std::uint32_t InvalidFLASHaddress = 0xffffffff;

    enum class State { Idle = 0, Connected = 1, Updating = 2};
    
    State state;
    
    bool countdownIsActive;
    bool txframe;
    
    embot::app::canprotocol::Clas cls;
    std::uint8_t cmd;
    
    embot::app::canprotocol::versionOfFIRMWARE version;
    embot::app::canprotocol::Board board;
    
    embot::app::canprotocol::bootloader::Message_ADDRESS::Info curr_blrdAddressInfo;
    std::uint8_t curr_blrdAddress_datalenreceivedsofar;
    
    bool eraseAPPLstorage;
    
    embot::hw::can::Frame reply;
    
    embot::hw::FlashBurner* flashburner;

    Impl() 
    {              
        state = State::Idle;
        countdownIsActive = true;
        txframe = false;
        cls = embot::app::canprotocol::Clas::none;
        cmd = 0;
        
        version.major = 0;
        version.minor = 0;
        version.build = 255;
        board = embot::app::canprotocol::Board::unknown;      
        
        eraseAPPLstorage = false;
        
        curr_blrdAddressInfo.address = InvalidFLASHaddress;
        curr_blrdAddressInfo.datalen = 0;
        curr_blrdAddress_datalenreceivedsofar = 0;
        version.build = 255;
        
        
        flashburner = nullptr;       
    }
    
    void setstate(State newstate)
    {
        state = newstate;
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
               
                if(false == embot::binary::bit::check(mask, target))
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
    
    
    bool process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    
    bool process_bl_broadcast(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);    
    bool process_bl_board(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_address(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_data(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_start(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_end(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_getadditionalinfo(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_setadditionalinfo(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    
    bool process_setid(const embot::app::canprotocol::Clas cl, const std::uint8_t cm, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_bl_setcanaddress(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
        
};


bool embot::app::bootloader::theCANparser::Impl::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    txframe = false;
    
    if(false == embot::app::canprotocol::frameis4board(frame, embot::app::theCANboardInfo::getInstance().cachedCANaddress()))
    {
        return false;
    }
    
    if(countdownIsActive)
    {   // do it as quick as possible
        embot::app::theBootloader &bl  = embot::app::theBootloader::getInstance();
        bl.stopcountdown();
        countdownIsActive = false;
    }
    
    // now get cls and cmd
    cls = embot::app::canprotocol::frame2clas(frame);
    cmd = embot::app::canprotocol::frame2cmd(frame);
    
    replies.clear();
    
    // only some states can handle them.
    
    switch(state)
    {
        case Impl::State::Idle:
        {
            // only bootloader::CMD::BROADCAST
            if((cls == embot::app::canprotocol::Clas::bootloader) && (cmd == static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::BROADCAST)))
            {
                txframe = process_bl_broadcast(frame, replies);
                                
                // then go to connected.
                setstate(Impl::State::Connected);
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::BOARD) == cmd)
            {
                txframe = process_bl_board(frame, replies);                
                // then go to update.
                setstate(Impl::State::Updating);
            }
             
        } break;
        
        case Impl::State::Connected:
        {
            // only bootloader::CMD::BOARD (it moves to state updating), bootloader::CMD::BROADCAST, bootloader::CMD::SETCANADDRESS, mcpollCMD::SET_BOARD_ID, aspollCMD::SET_BOARD_ADX,
            
            switch(cls)
            {
                case embot::app::canprotocol::Clas::bootloader:
                {
                    if(static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::BOARD) == cmd)
                    {
                        txframe = process_bl_board(frame, replies);                
                        // then go to update.
                        setstate(Impl::State::Updating);
                    }
                    else if(static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::BROADCAST) == cmd)
                    {
                        txframe = process_bl_broadcast(frame, replies);                                
                    }
                    else if(static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::SETCANADDRESS) == cmd)
                    {
                        txframe = process_bl_setcanaddress(frame, replies);                
                    } 
                    else if(static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::GET_ADDITIONAL_INFO) == cmd)
                    {
                        txframe = process_bl_getadditionalinfo(frame, replies);                
                    } 
                    else if(static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::SET_ADDITIONAL_INFO) == cmd)
                    {
                        txframe = process_bl_setadditionalinfo(frame, replies);                
                    }                     
                } break;
                
                case embot::app::canprotocol::Clas::pollingMotorControl:
                {
                    if(static_cast<std::uint8_t>(embot::app::canprotocol::motor::polling::CMD::SET_BOARD_ID) == cmd)
                    {
                        txframe = process_setid(cls, cmd, frame, replies);
                    }
                    
                } break;
                
                case embot::app::canprotocol::Clas::pollingAnalogSensor:
                {
                    if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_BOARD_ADX) == cmd)
                    {
                        txframe = process_setid(cls, cmd, frame, replies);
                    }                    
                } break;
                
                default:
                {
                }  break;   
            }
            
        } break;

        case Impl::State::Updating:
        {
            // we are in firmware-updating mode. any non specific command will be ignored.
            // only: bootloader::CMD::ADDRESS, bootloader::CMD::DATA, bootloader::CMD::START, bootloader::CMD::END
            // the correct flux would be: { ADDRESS, { DATA }_as_specified_in_address_frame }_as_many_as_lines_in_the_hex_file, START, END.
            // BUT: the canLoader will take care of having a correct flux. only check is: the reply of DATA when it receives the correct number of 
            // bytes. if any wrong one (or lost) the bootloader will fail the fw-udpate but will not be corrupted itself. 
            // conclusion: we keep everything in a single state Impl::State::Updating.
            
            if(embot::app::canprotocol::Clas::bootloader == cls)
            {
                switch(cmd)
                {
                    case static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::ADDRESS):
                    {   // first of the typical sequence for a hex row of 16 bytes. the row is: address, data, data, data 
                        txframe = process_bl_address(frame, replies);
                    } break;
                    
                    case static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::DATA):
                    {   // successive of the typical sequence for a hex row of 16 bytes. the row is: address, data, data, data 
                        txframe = process_bl_data(frame, replies);
                    } break;
                    
                    case static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::START):
                    {   // only one of such messages: it tells that the hex rows are over. time to flush rx data into flash
                        txframe = process_bl_start(frame, replies);
                    } break;                    
                    
                    case static_cast<std::uint8_t>(embot::app::canprotocol::bootloader::CMD::END):
                    {   // only one of such messages: it tells that fw update is over. time to restart.                        
                        txframe = process_bl_end(frame, replies);
                        // simplest and maybe safest way to send ack and restart later is to restart the countdown with 100 ms timeout. 
                        embot::app::theBootloader &bl  = embot::app::theBootloader::getInstance();
                        bl.startcountdown(100*embot::common::time1millisec);                        
                    } break;  
                    
                    default:
                    {
                    }  break;                       
                }

            }

        } break;

        
        default:
        {
            txframe = false;
        } break;
    }    
    
    
    return txframe;
}



bool embot::app::bootloader::theCANparser::Impl::process_bl_broadcast(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_BROADCAST msg;
    msg.load(frame);
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    embot::app::theCANboardInfo::StoredInfo strd = {0};
    canbrdinfo.get(strd);
    
    embot::app::canprotocol::bootloader::Message_BROADCAST::ReplyInfo replyinfo;
    replyinfo.board = static_cast<embot::app::canprotocol::Board>(strd.boardtype);
    
    if(embot::app::canprotocol::Process::application == config.owner)
    {
        replyinfo.process = embot::app::canprotocol::Process::application;
        replyinfo.firmware.major = strd.applicationVmajor;
        replyinfo.firmware.minor = strd.applicationVminor;
        replyinfo.firmware.build = strd.applicationVbuild;        
    }
    else
    {
        replyinfo.process = embot::app::canprotocol::Process::bootloader;
        replyinfo.firmware.major = strd.bootloaderVmajor;
        replyinfo.firmware.minor = strd.bootloaderVminor;
        replyinfo.firmware.build = 255;
    }
        
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(reply);
        return true;
    }        
    
    return false;
}


bool embot::app::bootloader::theCANparser::Impl::process_bl_board(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_BOARD msg;
    msg.load(frame);
    
    // get the eraseeeprom info.    
    eraseAPPLstorage = (1 == msg.info.eepromerase) ? (true) : (false);
    
    // we may erase flash (and eeprom now) but ... we wait.
            
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress()))
    {
        replies.push_back(reply);
        return true;
    }        
    
    return false;       
}

bool embot::app::bootloader::theCANparser::Impl::process_bl_address(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_ADDRESS msg;
    msg.load(frame);
    
    // use what received ...    
    curr_blrdAddressInfo.address = msg.info.address;
    curr_blrdAddressInfo.datalen = msg.info.datalen;
    curr_blrdAddress_datalenreceivedsofar = 0;
    
    return msg.reply();        
}


bool embot::app::bootloader::theCANparser::Impl::process_bl_data(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_DATA msg;
    msg.load(frame);
    
    // use what received ...       
    if((curr_blrdAddress_datalenreceivedsofar+msg.info.size) <= curr_blrdAddressInfo.datalen)
    {   // ok, i can pass data to the flashburner
        
        // the flashburner object takes care of accepting data, to buffer it in efficient modes, and to write it to flash.
        // the first time it needs to write a page of flash, it also erases all the flash pages used by the application.
        flashburner->add(curr_blrdAddressInfo.address+curr_blrdAddress_datalenreceivedsofar, msg.info.size, msg.info.data);
        
        // now i increment data received so far.
        curr_blrdAddress_datalenreceivedsofar += msg.info.size;
        
        if(curr_blrdAddress_datalenreceivedsofar == curr_blrdAddressInfo.datalen)
        {   // i send ok only if the bytes receive so far are exactly equal to the ones we are expecting in total
            
            if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), true))
            {
                replies.push_back(reply);
                return true;
            }            
            return false;                
        }
        
        return false;
    }
    
    // do i return nothing or a nack??? i send nothing.
    return false;    
}


bool embot::app::bootloader::theCANparser::Impl::process_bl_start(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{        
    embot::app::canprotocol::bootloader::Message_START msg;
    msg.load(frame);
    
    // it flushes ...   
    flashburner->flush();
    
    // it erases userdefineddata 
    if(true == eraseAPPLstorage)
    {
        eraseAPPLstorage = false;
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
        canbrdinfo.erase();
    }
        
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), true))
    {
        replies.push_back(reply);
        return true;
    } 
    return false;    
}


bool embot::app::bootloader::theCANparser::Impl::process_bl_end(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_END msg;
    msg.load(frame);
        
    if(true == msg.reply(reply, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), true))
    {
        replies.push_back(reply);
        return true;
    } 
    return false;     
}

bool embot::app::bootloader::theCANparser::Impl::process_bl_getadditionalinfo(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_GET_ADDITIONAL_INFO msg;
    msg.load(frame);
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    embot::app::theCANboardInfo::StoredInfo strd = {0};
    canbrdinfo.get(strd);
    
    embot::app::canprotocol::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo replyinfo;
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


bool embot::app::bootloader::theCANparser::Impl::process_bl_setadditionalinfo(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_SET_ADDITIONAL_INFO2 msg;
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


bool embot::app::bootloader::theCANparser::Impl::process_setid(const embot::app::canprotocol::Clas cl, const std::uint8_t cm, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::shared::Message_SET_ID msg(cl, cm);
    msg.load(frame);
      
    setcanaddress(msg.info.address, 0x0000);
            
    return msg.reply();        
}

bool embot::app::bootloader::theCANparser::Impl::process_bl_setcanaddress(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::bootloader::Message_SETCANADDRESS msg;
    msg.load(frame);
      
    
    setcanaddress(msg.info.address, msg.info.randominvalidmask);
            
    return msg.reply();        
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------




embot::app::bootloader::theCANparser& embot::app::bootloader::theCANparser::getInstance()
{
    static theCANparser* p = new theCANparser();
    return *p;
}

embot::app::bootloader::theCANparser::theCANparser()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
    embot::app::theJumper& thejumper = embot::app::theJumper::getInstance();
}  

    
embot::app::bootloader::theCANparser::~theCANparser() { }

        
bool embot::app::bootloader::theCANparser::initialise(Config &config)
{
    pImpl->config = config;
    pImpl->setstate(Impl::State::Idle);
        
    if(embot::app::canprotocol::Process::unknown == pImpl->config.owner)
    {
        pImpl->config.owner = embot::app::canprotocol::Process::bootloader;
    }

    if(embot::app::canprotocol::Process::application == pImpl->config.owner)
    {
        pImpl->countdownIsActive = false;            
    }
    else
    {
        pImpl->countdownIsActive = true;            
    }    
           
    if(nullptr == pImpl->flashburner)
    {
        std::uint32_t flashstart = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
        std::uint32_t flashsize = embot::hw::flash::getpartition(embot::hw::FLASH::application).maxsize;

        if(embot::app::canprotocol::Process::application == pImpl->config.owner)
        {
            flashstart = embot::hw::flash::getpartition(embot::hw::FLASH::bootloader).address;
            flashsize = embot::hw::flash::getpartition(embot::hw::FLASH::bootloader).maxsize;            
        }
        else
        {
            flashstart = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
            flashsize = embot::hw::flash::getpartition(embot::hw::FLASH::application).maxsize;            
        }
        
        pImpl->flashburner = new embot::hw::FlashBurner(flashstart, flashsize);
    }        
    
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve version of bootloader + address
    embot::app::theCANboardInfo::StoredInfo storedinfo;
    if(true == canbrdinfo.get(storedinfo))
    {
        pImpl->board = static_cast<embot::app::canprotocol::Board>(storedinfo.boardtype);
        

        if(embot::app::canprotocol::Process::application == pImpl->config.owner)
        {
            pImpl->version.major = storedinfo.applicationVmajor;
            pImpl->version.minor = storedinfo.applicationVminor;
            pImpl->version.build = storedinfo.applicationVbuild; 
        }            
        else
        {
            pImpl->version.major = storedinfo.bootloaderVmajor;
            pImpl->version.minor = storedinfo.bootloaderVminor;
            pImpl->version.build = 0;   
        }
    }
      
    
    return true;
}
  


bool embot::app::bootloader::theCANparser::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


