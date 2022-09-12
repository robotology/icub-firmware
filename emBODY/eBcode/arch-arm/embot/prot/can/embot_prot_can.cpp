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

#include "embot_prot_can.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace prot { namespace can {
    
         
    Clas frame2clas(const embot::prot::can::Frame &frame)
    {
        std::uint8_t t = (frame.id & 0x00000700) >> 8;
        if(6 == t)
        {
            return Clas::none;
        }
        return(static_cast<Clas>(t));
    }
    
    std::uint8_t frame2sender(const embot::prot::can::Frame &frame)
    {
        std::uint8_t t = (frame.id & 0x000000F0) >> 4;
        return(t);
    }
        
    bool frameisbootloader(const embot::prot::can::Frame &frame)
    {
        return (Clas::bootloader == frame2clas(frame)) ? true : (false);
    }
    
    bool frameispolling(const embot::prot::can::Frame &frame)
    {
        bool ret = false;
        switch(frame2clas(frame))
        {
            case Clas::pollingMotorControl:         ret = true;     break;
            case Clas::pollingAnalogSensor:         ret = true;     break;
            default:                                ret = false;    break;            
        }
        return ret;
    }    
    
    bool frameisperiodic(const embot::prot::can::Frame &frame)
    {
        bool ret = false;
        switch(frame2clas(frame))
        {
            case Clas::periodicMotorControl:        ret = true;     break;
            case Clas::periodicAnalogSensor:        ret = true;     break;
            case Clas::periodicInertialSensor:      ret = true;     break;
            case Clas::periodicSkin:                ret = true;     break;
            case Clas::periodicBattery:             ret = true;     break;
            default:                                ret = false;    break;            
        }
        return ret;
    }
    
    std::uint8_t frame2destination(const embot::prot::can::Frame &frame)
    {   // for not periodic messages . if periodic .... destination is 0xf
        if(frameisperiodic(frame))
        {
            return 0xf;
        }
        std::uint8_t t = (frame.id & 0x0000000F);
        return(t);
    }
    
    bool frameis4board(const embot::prot::can::Frame &frame, const std::uint8_t boardaddress)
    {
        if(frameisperiodic(frame))
        {   // we dont accept any periodic
            return false;
        }
        // for all others destination is in id
        std::uint8_t t = (frame.id & 0x0000000F); 
        if(0xf == t)
        {   // broadcast
            return true;
        }
        if((0xf & boardaddress) == t)
        {   // matches the address
            return true;
        }
        return false;        
    }
    
    std::uint8_t frame2cmd(const embot::prot::can::Frame &frame)
    {  
        if(frameisperiodic(frame))
        {
            return (frame.id & 0x0000000F);
        }
        if(frameisbootloader(frame))
        {
            return frame.data[0];
        }
        return (frame.data[0] & 0x7F);
    }
    
    std::uint8_t frame2datasize(const embot::prot::can::Frame &frame)
    {  
        if(0 == frame.size)
        {
            return 0;
        }
        if(frameisperiodic(frame))
        {
            return (frame.size);
        }
        return (frame.size-1);
    }
    
    std::uint8_t* frame2databuffer(embot::prot::can::Frame &frame)
    {  
        if(0 == frame.size)
        {
            return nullptr;
        }
        
        if(frameisperiodic(frame))
        {
            return &frame.data[0];
        }
        
        return &frame.data[1];
    }
    
    bool frame_set_clascmddestinationdata(embot::prot::can::Frame &frame, const Clas cls, const std::uint8_t cmd, const std::uint8_t destination, const void *data, const std::uint8_t sizeofdatainframe, const std::uint8_t mcindex, bool verify)
    {
        if(Clas::none == cls)
        {
            return false;
        }
        
        bool ret = false;
        
        // set cls ... it is always in id-0x00000700
        frame.id &= ~0x00000700;
        frame.id |= ((static_cast<std::uint32_t>(cls) & 0xf) << 8);
        
        // set cmd and destination. their location dends on clas
        switch(cls)
        {
            case Clas::pollingMotorControl:
            case Clas::pollingAnalogSensor:   
            case Clas::bootloader:
            {
                // destination is in id-0xf, cmd is in data[0]
                frame.id &= ~0x0000000F;
                frame.id |= (destination & 0xF);
                
                if(Clas::bootloader == cls)
                {
                    frame.data[0] = cmd;
                }
                else if(Clas::pollingMotorControl == cls)
                {
                    frame.data[0] = ((mcindex&0x1) << 7) | (cmd & 0x7F);
                }
                else
                {
                    frame.data[0] = cmd & 0x7F;
                }
                
                std::uint8_t s = (sizeofdatainframe>7) ? 7 : sizeofdatainframe;
                if((nullptr != data) && (s>0))
                {
                    std::memmove(&frame.data[1], data, s);
                }
                
                ret = true;
            } break;

            case Clas::periodicMotorControl:
            case Clas::periodicAnalogSensor: 
            case Clas::periodicInertialSensor:    
            case Clas::periodicSkin:  
            case Clas::periodicBattery:
            {
                // destination is not present, cmd is in 0x0000000F
                frame.id &= ~0x0000000F;
                frame.id |= (cmd & 0xF);
                
                std::uint8_t s = (sizeofdatainframe>8) ? 8 : sizeofdatainframe;
                if((nullptr != data) && (s>0))
                {
                    std::memmove(&frame.data[0], data, s);
                }
                
                ret = true;

            } break;            
            
            default:    
            {
                ret =  false;
            } break;            
        }
        
        if((true == ret) && (true == verify))
        {
            // i check it           
            if(cls != frame2clas(frame))
            {
                return false;
            }           
            if(cmd != frame2cmd(frame))
            {
                return false;
            }
            if((!frameisperiodic(frame)) && (destination == frame2destination(frame)))
            {
                return false;
            }            
        }
        
        return ret;
    }
    
    bool frame_set_sender(embot::prot::can::Frame &frame, std::uint8_t sender, bool verify)
    {
        frame.id &= ~0x000000F0;
        frame.id |= (static_cast<std::uint32_t>(sender & 0xF) << 4);
        if(verify)
        {
            if(sender != frame2sender(frame))
            {
                return false;
            }
        }
        return true;
    }
    
    bool frame_set_size(embot::prot::can::Frame &frame, std::uint8_t size, bool verify)
    {
        frame.size = (size > 8) ? (8) : (size);
        if(verify)
        {
            if(size != frame.size)
            {
                return false;
            }
        }
        return true;
    }
                
    void Message::set(const embot::prot::can::Frame &fr)
    {
        canframe = fr;
        
        candata.clas = frame2clas(canframe);
        candata.cmd = frame2cmd(canframe);
        candata.from = frame2sender(canframe);
        candata.to = frame2destination(canframe);              
        candata.datainframe = frame2databuffer(canframe);
        candata.sizeofdatainframe = frame2datasize(canframe);   

        valid = true;
    }
    
    void Message::clear()
    {
        candata.reset();        
        std::memset(&canframe, 0, sizeof(canframe));        
        valid = false;
    } 
    

    void Message::set(std::uint8_t fr, std::uint8_t t, Clas cl, std::uint8_t cm, const void *dat, std::uint8_t siz)
    {
        candata.clas = cl;
        candata.cmd = cm;
        candata.from = fr;
        candata.to = t;
        // we miss: candata.datainframe and candata.sizeofdatainframe .... but we compute them by loading something into the canframe and ....
        // ok, now canframe ...
        std::memset(&canframe, 0, sizeof(canframe));
        
        frame_set_sender(canframe, candata.from);
        frame_set_clascmddestinationdata(canframe, candata.clas, candata.cmd, candata.to, dat, siz);
        frame_set_size(canframe, siz);
        
        // ok, now we can compute ... candata.datainframe and candata.sizeofdatainframe
        candata.datainframe = frame2databuffer(canframe);
        candata.sizeofdatainframe = frame2datasize(canframe);
        
        valid = true;
    }
    
    bool Message::isvalid()
    {
        return valid;
    }

}}} // namespace embot { namespace prot { namespace can {



namespace embot { namespace prot { namespace can { namespace shared {
    
    bool Message_GET_VERSION::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(this->cmd != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.requiredprotocol.major = candata.datainframe[0];
        info.requiredprotocol.minor = candata.datainframe[1];
      
        return true;         
    }                    
        
    bool Message_GET_VERSION::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        frame_set_sender(outframe, sender);
        char dd[7] = {0};
        dd[0] = static_cast<std::uint8_t>(replyinfo.board);
        dd[1] = replyinfo.firmware.major;
        dd[2] = replyinfo.firmware.minor;
        dd[3] = replyinfo.firmware.build;
        dd[4] = replyinfo.protocol.major;
        dd[5] = replyinfo.protocol.minor;
        dd[6] = ((replyinfo.protocol.major == info.requiredprotocol.major) && (replyinfo.protocol.minor >= info.requiredprotocol.minor) ) ? (1) : (0);;
                   
        frame_set_clascmddestinationdata(outframe, this->cls, this->cmd, candata.from, dd, 7);
        frame_set_size(outframe, 8);
        return true;
    } 
    

    bool Message_SET_ID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(this->cmd != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.address = candata.datainframe[0];
      
        return true;         
    }                    
        
    bool Message_SET_ID::reply()
    {
        return false;
    } 
    
        
}}}} // namespace embot { namespace prot { namespace can { namespace shared {        

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


