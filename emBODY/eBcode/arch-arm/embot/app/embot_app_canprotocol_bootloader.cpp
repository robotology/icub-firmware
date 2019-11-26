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

#include "embot_app_canprotocol_bootloader.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"

#include <cstring>

#include "embot_hw.h"



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------




namespace embot { namespace app { namespace canprotocol { namespace bootloader {
    
        
    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        static const std::uint64_t blmask256[4] = 
        {
            // bits 0-63            
            (1ULL << static_cast<std::uint8_t>(CMD::BOARD)) | (1ULL << static_cast<std::uint8_t>(CMD::ADDRESS)) | 
            (1ULL << static_cast<std::uint8_t>(CMD::START)) | (1ULL << static_cast<std::uint8_t>(CMD::DATA))    | 
            (1ULL << static_cast<std::uint8_t>(CMD::END))   | (1ULL << static_cast<std::uint8_t>(CMD::GET_ADDITIONAL_INFO)) |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_ADDITIONAL_INFO)) | (1ULL << static_cast<std::uint8_t>(CMD::SETCANADDRESS)),
            // bits 64-127
            0, 
            // bits 128-191
            0, 
            // bits 191-255
            (1ULL << (static_cast<std::uint8_t>(CMD::BROADCAST)-192)) 
        };
        
        std::uint8_t ind = cmd >> 6;
        std::uint8_t pos = cmd & 0x3f;
        if(true == embot::binary::bit::check(blmask256[ind], pos))
        {
            return static_cast<CMD>(cmd);            
        }
        
        return CMD::none;         
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }      
    

    bool Message_BROADCAST::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::BROADCAST) != frame2cmd(inframe))
        {
            return false; 
        }

        return true;
    }  

    bool Message_BROADCAST::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = static_cast<std::uint8_t>(replyinfo.board);
        dd[1] = replyinfo.firmware.major;
        dd[2] = replyinfo.firmware.minor;
        dd[3] = replyinfo.firmware.build;
        
        std::uint8_t datalen = (Process::bootloader == replyinfo.process) ? (3) : (4);
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::BROADCAST), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }   


    bool Message_BOARD::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);
        
        if(static_cast<std::uint8_t>(CMD::BOARD) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.eepromerase = candata.datainframe[0];   

        return true;
    }                    
    
    bool Message_BOARD::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender)
    {
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::BOARD), candata.from, nullptr, 0);
        frame_set_size(outframe, 1);
        return true;
    }     


        
    bool Message_ADDRESS::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::ADDRESS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.datalen = candata.datainframe[0];
        info.address = candata.datainframe[1] | (static_cast<std::uint32_t>(candata.datainframe[2]) << 8) | (static_cast<std::uint32_t>(candata.datainframe[4]) << 16) | (static_cast<std::uint32_t>(candata.datainframe[5]) << 24);
        
        return true;         
    }                    
    
    bool Message_ADDRESS::reply()
    {
        return false;
    } 

    
    bool Message_START::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::START) != frame2cmd(inframe))
        {
            return false; 
        }
      
        return true;         
    }                    

    bool Message_START::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok)
    {
        frame_set_sender(outframe, sender);
        char dd[1] = {1};
        dd[0] = (true == ok) ? (1) : (0);
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::START), candata.from, dd, 1);
        frame_set_size(outframe, 2);
        return true;
    }  
    
    bool Message_DATA::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::DATA) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.size = candata.sizeofdatainframe;
        info.data = &candata.datainframe[0];
        
        return true;         
    }   

    bool Message_DATA::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok)
    {
        frame_set_sender(outframe, sender);
        char dd[1] = {1};
        dd[0] = (true == ok) ? (1) : (0);
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::DATA), candata.from, dd, 1);
        frame_set_size(outframe, 2);
        return true;
    }              
    

        
    bool Message_END::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::END) != frame2cmd(inframe))
        {
            return false; 
        }
      
        return true;         
    }                    
        
    bool Message_END::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok)
    {
        frame_set_sender(outframe, sender);
        char dd[1] = {1};
        dd[0] = (true == ok) ? (1) : (0);
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::END), candata.from, dd, 1);
        frame_set_size(outframe, 2);
        return true;
    }  
    
    
    bool Message_SETCANADDRESS::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SETCANADDRESS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.address = candata.datainframe[0];
        info.randominvalidmask = 0x0000;
        if(3 == inframe.size)
        {
            info.randominvalidmask = candata.datainframe[2];
            info.randominvalidmask <<= 8;
            info.randominvalidmask |= candata.datainframe[1];
        }
      
        return true;         
    }                    
        
    bool Message_SETCANADDRESS::reply()
    {
        return false;
    }    
           
    

    bool Message_GET_ADDITIONAL_INFO::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::GET_ADDITIONAL_INFO) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.thereisnothing = 0;
        
        counter = 0;
      
        return true;         
    }     

    std::uint8_t Message_GET_ADDITIONAL_INFO::numberofreplies()
    {
        return nreplies;
    }    
        
    bool Message_GET_ADDITIONAL_INFO::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        if(counter >= nreplies)
        {
            return false;
        }
        
        frame_set_sender(outframe, sender);
        char dd[7] = {0};
        dd[0] = counter;
        dd[1] = replyinfo.info32[4*counter];
        dd[2] = replyinfo.info32[4*counter+1];
        dd[3] = replyinfo.info32[4*counter+2];
        dd[4] = replyinfo.info32[4*counter+3];

                   
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::GET_ADDITIONAL_INFO), candata.from, dd, 5);
        frame_set_size(outframe, 6);
        
        counter ++;
        
        return true;
    }   
    

//    bool Message_SET_ADDITIONAL_INFO::load(const embot::hw::can::Frame &inframe)
//    {
//        Message::set(inframe);  
//        
//        if(static_cast<std::uint8_t>(CMD::SET_ADDITIONAL_INFO) != frame2cmd(inframe))
//        {
//            return false; 
//        }
//        
//        std::uint8_t counter = candata.datainframe[0];
//        if(counter > 7)
//        {
//            info.offset = 255;
//            return false;
//        }
//        
//        info.offset = 4*counter;
//        info.info04[0] = candata.datainframe[1];
//        info.info04[1] = candata.datainframe[2];
//        info.info04[2] = candata.datainframe[3];
//        info.info04[3] = candata.datainframe[4];
//        
//        return true;         
//    }     
//           
//    bool Message_SET_ADDITIONAL_INFO::reply()
//    {
//        return false;
//    }   


    
    char Message_SET_ADDITIONAL_INFO2::cumulativeinfo32[32] = {0};
    std::uint8_t Message_SET_ADDITIONAL_INFO2::receivedmask = 0;
    
    bool Message_SET_ADDITIONAL_INFO2::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_ADDITIONAL_INFO) != frame2cmd(inframe))
        {
            return false; 
        }
        
        std::uint8_t counter = candata.datainframe[0];
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
        
        embot::binary::bit::set(receivedmask, counter);
        std::memmove(&cumulativeinfo32[4*counter], &candata.datainframe[1], 4);
        
        info.valid = false;
        
        if(0xff == receivedmask)
        {
            std::memmove(info.info32, cumulativeinfo32, sizeof(info.info32));
            info.valid = true;
        }
                    
        return true;         
    }     
           
    bool Message_SET_ADDITIONAL_INFO2::reply()
    {
        return false;
    }   


    bool Message_GET_TIMEOFLIFE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_TIMEOFLIFE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        return true;
    }  

    bool Message_GET_TIMEOFLIFE::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        // we just copy 7 bytes of replyinfo int dd
        std::memmove(dd, &replyinfo.timeoflife, 7);
        std::uint8_t datalen = 7;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::bootloader, static_cast<std::uint8_t>(CMD::GET_TIMEOFLIFE), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }    

}}}} // namespace embot { namespace app { namespace canprotocol { namespace bootloader {


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


