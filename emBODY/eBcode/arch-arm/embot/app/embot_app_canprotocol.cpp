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

#include "embot_app_canprotocol.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <cstring>

#include "embot_hw.h"



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace app { namespace canprotocol {
    
    Clas frame2clas(const embot::hw::can::Frame &frame)
    {
        std::uint8_t t = (frame.id & 0x00000700) >> 8;
        if(6 == t)
        {
            return Clas::none;
        }
        return(static_cast<Clas>(t));
    }
    
    std::uint8_t frame2sender(const embot::hw::can::Frame &frame)
    {
        std::uint8_t t = (frame.id & 0x000000F0) >> 4;
        return(t);
    }
    
    
    bool frameisbootloader(const embot::hw::can::Frame &frame)
    {
        return (Clas::bootloader == frame2clas(frame)) ? true : (false);
    }
    
    bool frameispolling(const embot::hw::can::Frame &frame)
    {
        bool ret = false;
        switch(frame2clas(frame))
        {
            case Clas::pollingMotorControl:         ret = true;     break;
            case Clas::pollingAnalogSensor:         ret = true;     break;
            case Clas::pollingSkin:                 ret = true;     break;
            default:                                ret = false;    break;            
        }
        return ret;
    }    
    
    bool frameisperiodic(const embot::hw::can::Frame &frame)
    {
        bool ret = false;
        switch(frame2clas(frame))
        {
            case Clas::periodicMotorControl:        ret = true;     break;
            case Clas::periodicAnalogSensor:        ret = true;     break;
            case Clas::periodicInertialSensor:      ret = true;     break;
            default:                                ret = false;    break;            
        }
        return ret;
    }
    
    std::uint8_t frame2destination(const embot::hw::can::Frame &frame)
    {   // for not periodic messages . if periodic .... destination is 0xf
        if(frameisperiodic(frame))
        {
            return 0xf;
        }
        std::uint8_t t = (frame.id & 0x0000000F);
        return(t);
    }
    
    std::uint8_t frame2cmd(const embot::hw::can::Frame &frame)
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
    
    const std::uint8_t frame2datasize(const embot::hw::can::Frame &frame)
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
    
    std::uint8_t* frame2databuffer(embot::hw::can::Frame &frame)
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
    
    bool frame_set_clascmddestinationdata(embot::hw::can::Frame &frame, const Clas cls, const std::uint8_t cmd, const std::uint8_t destination, const void *data, const std::uint8_t sizeofdatainframe, const std::uint8_t mcindex, bool verify)
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
            case Clas::pollingSkin:      
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
    
    bool frame_set_sender(embot::hw::can::Frame &frame, std::uint8_t sender, bool verify)
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
    
    bool frame_set_size(embot::hw::can::Frame &frame, std::uint8_t size, bool verify)
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
    
            
        void Message::set(const embot::hw::can::Frame &fr)
        {
            frame = fr;
            
            data.clas = frame2clas(frame);
            data.cmd = frame2cmd(frame);
            data.from = frame2sender(frame);
            data.to = frame2destination(frame);              
            data.datainframe = frame2databuffer(frame);
            data.sizeofdatainframe = frame2datasize(frame);   

            valid = true;
        }
        
        void Message::clear()
        {
            data.reset();
            
            std::memset(&frame, 0, sizeof(frame));
            
            valid = false;
        } 
        

        void Message::set(std::uint8_t fr, std::uint8_t t, Clas cl, std::uint8_t ty, const void *dat, std::uint8_t siz)
        {
            data.from = fr;
            data.to = t;
            data.clas = cl;
            data.cmd = ty;
            // ok, now frame ...
            std::memset(&frame, 0, sizeof(frame));
            
            frame_set_sender(frame, data.from);
            frame_set_clascmddestinationdata(frame, data.clas, data.cmd, data.to, data.datainframe, data.sizeofdatainframe);
            
            data.datainframe = frame2databuffer(frame);
            data.sizeofdatainframe = frame2datasize(frame);
            
            valid = true;
        }
        
        bool Message::isvalid()
        {
            return valid;
        }


            
        bool Message_bldr_BROADCAST::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame); 
            
            if(static_cast<std::uint8_t>(bldrCMD::BROADCAST) != frame2cmd(frame))
            {
                return false; 
            }

            return true;
        }  

        bool Message_bldr_BROADCAST::reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const ReplyInfo &replyinfo)
        {
            std::uint8_t dd[7] = {0};
            dd[0] = static_cast<std::uint8_t>(replyinfo.board);
            dd[1] = replyinfo.firmware.major;
            dd[2] = replyinfo.firmware.minor;
            dd[3] = replyinfo.firmware.build;
            
            std::uint8_t datalen = (Process::bootloader == replyinfo.process) ? (3) : (4);
            
            frame_set_sender(frame, sender);
            frame_set_clascmddestinationdata(frame, Clas::bootloader, static_cast<std::uint8_t>(bldrCMD::BROADCAST), data.from, dd, datalen);
            frame_set_size(frame, datalen+1);
            return true;
        }            
        
    
            
        bool Message_bldr_BOARD::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);
            
            if(static_cast<std::uint8_t>(bldrCMD::BOARD) != frame2cmd(frame))
            {
                return false; 
            }
            
            info.eepromerase = data.datainframe[0];   

            return true;
        }                    
        
        bool Message_bldr_BOARD::reply(embot::hw::can::Frame &frame, const std::uint8_t sender)
        {
            frame_set_sender(frame, sender);
            frame_set_clascmddestinationdata(frame, Clas::bootloader, static_cast<std::uint8_t>(bldrCMD::BOARD), data.from, nullptr, 0);
            frame_set_size(frame, 1);
            return true;
        }     
    

            
        bool Message_bldr_ADDRESS::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(bldrCMD::ADDRESS) != frame2cmd(frame))
            {
                return false; 
            }
            
            info.datalen = data.datainframe[0];
            info.address = data.datainframe[1] | (static_cast<std::uint32_t>(data.datainframe[2]) << 8) | (static_cast<std::uint32_t>(data.datainframe[4]) << 16) | (static_cast<std::uint32_t>(data.datainframe[5]) << 24);
            
            return true;         
        }                    
        
        bool Message_bldr_ADDRESS::reply()
        {
            return false;
        } 

        
        bool Message_bldr_START::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(bldrCMD::START) != frame2cmd(frame))
            {
                return false; 
            }
          
            return true;         
        }                    

        bool Message_bldr_START::reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const bool ok)
        {
            frame_set_sender(frame, sender);
            char dd[2] = {0x02, 0x01};
            frame_set_clascmddestinationdata(frame, Clas::bootloader, static_cast<std::uint8_t>(bldrCMD::START), data.from, dd, 2);
            frame_set_size(frame, 3);
            return true;
        }  
        
        bool Message_bldr_DATA::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(bldrCMD::DATA) != frame2cmd(frame))
            {
                return false; 
            }
            
            info.size = data.sizeofdatainframe;
            info.data = &data.datainframe[0];
            
            return true;         
        }   

        bool Message_bldr_DATA::reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const bool ok)
        {
            frame_set_sender(frame, sender);
            char dd[1] = {ok};
            frame_set_clascmddestinationdata(frame, Clas::bootloader, static_cast<std::uint8_t>(bldrCMD::DATA), data.from, dd, 1);
            frame_set_size(frame, 2);
            return true;
        }              
        

            
        bool Message_bldr_END::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(bldrCMD::END) != frame2cmd(frame))
            {
                return false; 
            }
          
            return true;         
        }                    
            
        bool Message_bldr_END::reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const bool ok)
        {
            frame_set_sender(frame, sender);
            char dd[1] = {0x1};
            frame_set_clascmddestinationdata(frame, Clas::bootloader, static_cast<std::uint8_t>(bldrCMD::END), data.from, dd, 1);
            frame_set_size(frame, 2);
            return true;
        }  
        
        
        bool Message_bldr_SETID::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(bldrCMD::SETID) != frame2cmd(frame))
            {
                return false; 
            }
            
            info.id = data.datainframe[0];
            info.randominvalidmask = 0x0000;
            if(3 == frame.size)
            {
                info.randominvalidmask = data.datainframe[2];
                info.randominvalidmask << 8;
                info.randominvalidmask |= data.datainframe[1];
            }
          
            return true;         
        }                    
            
        bool Message_bldr_SETID::reply()
        {
            return false;
        }    
        
        
        
        bool Message_anypoll_SETID::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(anypollCMD::SETID) != frame2cmd(frame))
            {
                return false; 
            }
            
            info.id = data.datainframe[0];
          
            return true;         
        }                    
            
        bool Message_anypoll_SETID::reply()
        {
            return false;
        }   
        
        
        bool Message_base_GET_FIRMWARE_VERSION::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(this->cmd != frame2cmd(frame))
            {
                return false; 
            }
            
            info.requiredprotocol.major = data.datainframe[0];
            info.requiredprotocol.minor = data.datainframe[1];
          
            return true;         
        }                    
            
        bool Message_base_GET_FIRMWARE_VERSION::reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const ReplyInfo &replyinfo)
        {
            frame_set_sender(frame, sender);
            char dd[7] = {0};
            dd[0] = static_cast<std::uint8_t>(replyinfo.board);
            dd[1] = replyinfo.firmware.major;
            dd[2] = replyinfo.firmware.minor;
            dd[3] = replyinfo.firmware.build;
            dd[4] = replyinfo.protocol.major;
            dd[5] = replyinfo.protocol.minor;
            dd[6] = ((replyinfo.protocol.major == info.requiredprotocol.major) && (replyinfo.protocol.minor >= info.requiredprotocol.minor) ) ? (1) : (0);;
                       
            frame_set_clascmddestinationdata(frame, this->cls, this->cmd, data.from, dd, 7);
            frame_set_size(frame, 8);
            return true;
        } 


        bool Message_mcpoll_GET_ADDITIONAL_INFO::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(mcpollCMD::GET_ADDITIONAL_INFO) != frame2cmd(frame))
            {
                return false; 
            }
            
            info.thereisnothing = 0;
            
            counter = 0;
          
            return true;         
        }     

        std::uint8_t Message_mcpoll_GET_ADDITIONAL_INFO::numberofreplies()
        {
            return nreplies;
        }    
            
        bool Message_mcpoll_GET_ADDITIONAL_INFO::reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const ReplyInfo &replyinfo)
        {
            if(counter >= nreplies)
            {
                return false;
            }
            
            frame_set_sender(frame, sender);
            char dd[7] = {0};
            dd[0] = counter;
            dd[1] = replyinfo.info32[4*counter];
            dd[2] = replyinfo.info32[4*counter+1];
            dd[3] = replyinfo.info32[4*counter+2];
            dd[4] = replyinfo.info32[4*counter+3];

                       
            frame_set_clascmddestinationdata(frame, Clas::pollingMotorControl, static_cast<std::uint8_t>(mcpollCMD::GET_ADDITIONAL_INFO), data.from, dd, 5);
            frame_set_size(frame, 6);
            
            counter ++;
            
            return true;
        }   
        

        bool Message_mcpoll_SET_ADDITIONAL_INFO::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(mcpollCMD::SET_ADDITIONAL_INFO) != frame2cmd(frame))
            {
                return false; 
            }
            
            std::uint8_t counter = data.datainframe[0];
            if(counter > 7)
            {
                info.offset = 255;
                return false;
            }
            
            info.offset = 4*counter;
            info.info04[0] = data.datainframe[1];
            info.info04[1] = data.datainframe[2];
            info.info04[2] = data.datainframe[3];
            info.info04[3] = data.datainframe[4];
            
            return true;         
        }     
               
        bool Message_mcpoll_SET_ADDITIONAL_INFO::reply()
        {
            return false;
        }   


        
        char Message_mcpoll_SET_ADDITIONAL_INFO2::cumulativeinfo32[32] = {0};
        std::uint8_t Message_mcpoll_SET_ADDITIONAL_INFO2::receivedmask = 0;
        
        bool Message_mcpoll_SET_ADDITIONAL_INFO2::load(const embot::hw::can::Frame &frame)
        {
            Message::set(frame);  
            
            if(static_cast<std::uint8_t>(mcpollCMD::SET_ADDITIONAL_INFO) != frame2cmd(frame))
            {
                return false; 
            }
            
            std::uint8_t counter = data.datainframe[0];
            if(counter > 7)
            {
                return false;
            }
            
            if(0 == counter)
            {
                info.valid = false;
                receivedmask = 0;
                std::memset(cumulativeinfo32, 0, sizeof(cumulativeinfo32));                
            }
            
            embot::common::bit::set(receivedmask, counter);
            std::memmove(&cumulativeinfo32[4*counter], &data.datainframe[1], 4);
            
            if(0xff == receivedmask)
            {
                std::memmove(info.info32, cumulativeinfo32, sizeof(info.info32));
                info.valid = true;
            }
                        
            return true;         
        }     
               
        bool Message_mcpoll_SET_ADDITIONAL_INFO2::reply()
        {
            return false;
        }         
        
}}} // namespace embot { namespace app { namespace canprotocol {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


