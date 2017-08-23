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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_CANPROTOCOL_BOOTLOADER_H_
#define _EMBOT_APP_CANPROTOCOL_BOOTLOADER_H_

#include "embot_common.h"

#include "embot_app_canprotocol.h"


namespace embot { namespace app { namespace canprotocol { namespace bootloader {
    
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        BOARD = 0x00, ADDRESS = 0x01, START = 0x02, DATA = 0x03, END = 0x04, 
        GET_ADDITIONAL_INFO = 12, SET_ADDITIONAL_INFO = 13, SETCANADDRESS = 0x32,         
        BROADCAST = 0xff        
    };

    // NOTES
    // - the message canprotocol::bootloader::SETCANADDRESS is added so that we can have a common (improved)
    //   message to change address. the improved feature allow random address assignement which is useful if
    //   two cna boards happen to have the same id.    

    
    // some utilities    
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);
    
}}}} // namespace embot { namespace app { namespace canprotocol { namespace bootloader {    
    


namespace embot { namespace app { namespace canprotocol { namespace bootloader {
        
    // the management of commands
    
    class Message_BROADCAST : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        struct ReplyInfo
        {
            Board               board;
            Process             process;
            versionOfFIRMWARE   firmware;  
            ReplyInfo() : board(Board::unknown), process(Process::application) {}          
        };
        
        Info info;
                    
        Message_BROADCAST(){}
            
        bool load(const embot::hw::can::Frame &inframe);            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);   // boardtype, etc.
        
    };
    
    
    
    class Message_BOARD : public Message
    {
        public:
            
        struct Info
        { 
            bool eepromerase;  
            Info() : eepromerase(false) {}
        };
        
        Info info;
        
        Message_BOARD(){}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender);   // ack/nack        
    };
    
    
    class Message_ADDRESS : public Message
    {
        public:
            
        struct Info
        {
            std::uint8_t    datalen;
            std::uint32_t   address;  
            Info() : datalen(0), address(0) {}
        };
        
        Info info;
                    
        Message_ADDRESS(){}
            
        bool load(const embot::hw::can::Frame &inframe); 
            
        bool reply();   // none
        
    };
    
    
    class Message_START : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        Info info;
        
        Message_START(){}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok);   // ack/nack
        
    };   


    class Message_DATA : public Message
    {
        public:
            
        struct Info
        {
            std::uint8_t*   data;
            std::uint8_t    size;  
            Info() : data(nullptr), size(0) {}
        };
        
        Info info;
                    
        Message_DATA(){}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok);  //ack but only when te cumulative Info::size reach Message_ADDRESS::Info::datalen
        
    }; 


    class Message_END: public Message
    {
        public:

        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        Info info;
        
        Message_END(){}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok);  //ack/nack
        
    };  
    

    class Message_GET_ADDITIONAL_INFO : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        Info info;
        
        struct ReplyInfo
        {
            char    info32[32];      
        };
        
                
        Message_GET_ADDITIONAL_INFO() : counter(0) {}
            
        bool load(const embot::hw::can::Frame &inframe);
         
        // we have multiple frames ... best way is to pass a vector which is resized according to the needs.
        // but ... memory allocation, heap, embedded systems ... i prefer to give back a frame at a time.            
        std::uint8_t numberofreplies();
        // i give the frames in order until max number.    
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);  

        private:
        
        std::uint8_t counter;
        const std::uint8_t nreplies = 8;
    };    

//    class Message_SET_ADDITIONAL_INFO : public Message
//    {
//        public:
//            
//        struct Info
//        {
//            std::uint8_t    offset;     // 0, 4, 8, 16, 20, 24, 28. 255 is a non-valid value 
//            char            info04[4];  
//            Info() : offset(255) { }
//        };
//        
//        Info info;
//        
//                
//        Message_SET_ADDITIONAL_INFO() {}
//            
//        bool load(const embot::hw::can::Frame &inframe);
//         
//        // we need to receive 8 frames to be able to have a complete 32 bytes long info32. we operate in memory-less mode.
//        // each time we recover only a fraction of info32[].
//        // we could operate differently, but maybe for later: use of a static char info32[32] array and a static flag08.
//        // when we received counter = 0, then we reset info32[] and flag08. then we fill the 4 bytes into info32 and set the
//        // bit of pos counter. when all bits are on, we fill Info::info32[] and set Info::ready = true.
//            
//        bool reply(); // none 

//    };  


    class Message_SET_ADDITIONAL_INFO2 : public Message
    {
        public:
            
        struct Info
        {
            bool        valid;     
            char        info32[32];  
            Info() : valid(false) { }
        };
        
        Info info;
        
                
        Message_SET_ADDITIONAL_INFO2() {}
            
        bool load(const embot::hw::can::Frame &inframe);
         
        // we need to receive 8 frames to be able to have a complete 32 bytes long info32. we operate in memory-less mode.
        // each time we recover only a fraction of info32[].
        // we could operate differently, but maybe for later: use of a static char info32[32] array and a static flag08.
        // when we received counter = 0, then we reset info32[] and flag08. then we fill the 4 bytes into info32 and set the
        // bit of pos counter. when all bits are on, we fill Info::info32[] and set Info::ready = true.
            
        bool reply(); // none 
            
        private:
            
        static char cumulativeinfo32[32];
        static std::uint8_t receivedmask;
    };  

    
    class Message_SETCANADDRESS : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    address;            // if id is 255, then the board assign it randomly as best as it can.
            std::uint16_t   randominvalidmask;  // but not assign if address is set inside this mask
            Info() : address(0), randominvalidmask(0) {}
        };
        
        Info info;
        
        Message_SETCANADDRESS(){}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };    
    
}}}} // namespace embot { namespace app { namespace canprotocol { namespace bootloader {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
