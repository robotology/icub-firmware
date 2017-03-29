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

#ifndef _EMBOT_APP_CANPROTOCOL_H_
#define _EMBOT_APP_CANPROTOCOL_H_

#include "embot_common.h"

#include "embot_hw.h"

#include <cstring>

namespace embot { namespace app { namespace canprotocol {
    
    enum class Clas { pollingMotorControl = 0, periodicMotorControl = 1, pollingAnalogSensor = 2, periodicAnalogSensor = 3, pollingSkin = 4, periodicInertialSensor = 5, bootloader = 7, none = 255 };

    enum class bldrCMD { BROADCAST = 0xff, BOARD = 0x00, ADDRESS = 0x01, START = 0x02, DATA = 0x03, END = 0x04, SETID = 0x32 };
    
    enum class anypollCMD { SETID = 0x32 };
    
    enum class mcpollCMD { GET_ADDITIONAL_INFO = 12, SET_ADDITIONAL_INFO = 13, GET_FW_VERSION = 91 };
    
    enum class aspollCMD { GET_FW_VERSION = 0x1C };
    
    
    enum class Board { mtb = 0x05, strain = 0x06, mais = 0x07, mtb4 = 0x66, strain2 = 0x67, unknown = 0xff };
    
    Clas frame2clas(const embot::hw::can::Frame &frame);
    std::uint8_t frame2cmd(const embot::hw::can::Frame &frame);
    
    // all teh rest
    std::uint8_t frame2sender(const embot::hw::can::Frame &frame);
    
    bool frameisbootloader(const embot::hw::can::Frame &frame);
    
    bool frameispolling(const embot::hw::can::Frame &frame);
    
    bool frameisperiodic(const embot::hw::can::Frame &frame);
    
    std::uint8_t frame2destination(const embot::hw::can::Frame &frame);
    
    
    
    const std::uint8_t frame2datasize(const embot::hw::can::Frame &frame);
    
    std::uint8_t* frame2databuffer(embot::hw::can::Frame &frame);
    
    bool frame_set_clascmddestinationdata(embot::hw::can::Frame &frame, const Clas cls, const std::uint8_t cmd, const std::uint8_t destination, const void *data, const std::uint8_t size, const std::uint8_t mcindex = 0, bool verify = false);
        
    bool frame_set_sender(embot::hw::can::Frame &frame, std::uint8_t sender, bool verify = false);
    
    bool frame_set_size(embot::hw::can::Frame &frame, std::uint8_t size, bool verify = false);
    

    
    // this class is used for receiving but also for transmitting.
    // receiving: Message msg; msg.set(rxframe); if(msg.isvalid()) { use its data ..}
    // transmitting: Message msg; msg.set(......); if(msg.isvalid()) { use its frame }
    //
    // example of bootloader or polling messages used by a can board
    // derive a bootloaderWhateverMessage from Message.
    // use method load(frame) which calls Message::set(frame) to fill a frame in the class
    // use the Info data struct to retrieve params.
    // use bool reply(frametx) to see if a reply is needed and get the frame to transmit.
    // for the case of GET_ADDITIONAL_INFO ...  we get a vector<frame> to transmit ...
    // for the case of SET_ADDITIONAL_INFO ... we need to store data across rxframes somehow ... 
    
    
    // example of broadcast message
    // derive a braodcastASsomethingMessage from Message.
    // use method load(some values / Info&, senderaddress) to fill whatever we need and to form a frame 
    // use bool transmit(frametx) to see if a tx message is ready and use it.
    
    // we can decide which classes to use in parsing using { cls = frame2clas(frame), cmd = frame2cmd(frame) }
    
    // we can decide whcih class to use in forming because ... well we must know it.
    
    
    
    class Message
    {
    public:
        
        struct Data
        {
            std::uint8_t            from;
            std::uint8_t            to;
            Clas                    clas;       
            std::uint8_t            cmd;       // use they are: ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID, etc.
            std::uint8_t*           datainframe;
            std::uint8_t            sizeofdatainframe;  
            Data() { reset(); }
            void reset() { from = 0; to = 0; clas = Clas::none; cmd = 0; datainframe = nullptr; sizeofdatainframe = 0; } 
        };

    public:        
        
        bool                    valid;
        Data                    data;
        embot::hw::can::Frame   frame;
    
              
        Message() { clear(); }
           
        // it copies a received frame and it computes other data structure with it. it sets valid = true. after that, if valid == true one can use the public data structures
        void set(const embot::hw::can::Frame &fr);
        
        // it clears dat structure and sets valid = false. nobody should use its dta structures
        void clear();
        
        // it fills data structures with passed values. it creates a frame. it sets valid = true. after that, if valid == true one can use the frame for tx
        void set(std::uint8_t fr, std::uint8_t t, Clas cl, std::uint8_t ty, const void *dat, std::uint8_t siz);
        
        bool isvalid();
                
    };
    
    
 
   

    
    class Message_bldr_BROADCAST : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        struct ReplyInfo
        {
            Board           board;
            std::uint8_t    major;
            std::uint8_t    minor;
            std::uint8_t    build;  
            ReplyInfo() : board(Board::unknown), major(0), minor(0), build(255) {}          
        };
        
        Info info;
                    
        Message_bldr_BROADCAST(){}
            
        bool load(const embot::hw::can::Frame &frame);            
        bool reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const ReplyInfo &replyinfo);   // boardtype, etc.
        
    };
    
    class Message_bldr_BOARD : public Message
    {
        public:
            
        struct Info
        { 
            bool eepromerase;  
            Info() : eepromerase(false) {}
        };
        
        Info info;
        
        Message_bldr_BOARD(){}
            
        bool load(const embot::hw::can::Frame &frame);
            
        bool reply(embot::hw::can::Frame &frame, const std::uint8_t sender);   // ack/nack        
    };
    
    
    class Message_bldr_ADDRESS : public Message
    {
        public:
            
        struct Info
        {
            std::uint8_t    datalen;
            std::uint32_t   address;  
            Info() : datalen(0), address(0) {}
        };
        
        Info info;
                    
        Message_bldr_ADDRESS(){}
            
        bool load(const embot::hw::can::Frame &frame); 
            
        bool reply();   // none
        
    };
    
    
    class Message_bldr_START : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        Info info;
        
        Message_bldr_START(){}
            
        bool load(const embot::hw::can::Frame &frame);
            
        bool reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const bool ok);   // ack/nack
        
    };   


    class Message_bldr_DATA : public Message
    {
        public:
            
        struct Info
        {
            std::uint8_t*   data;
            std::uint8_t    size;  
            Info() : data(nullptr), size(0) {}
        };
        
        Info info;
                    
        Message_bldr_DATA(){}
            
        bool load(const embot::hw::can::Frame &frame);
            
        bool reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const bool ok);  //ack but only when te cumulative Info::size reach Message_bldr_ADDRESS::Info::datalen
        
    }; 


    class Message_bldr_END: public Message
    {
        public:

        struct Info
        { 
            std::uint8_t    thereisnothing;  
            Info() : thereisnothing(0) {}
        };
        
        Info info;
        
        Message_bldr_END(){}
            
        bool load(const embot::hw::can::Frame &frame);
            
        bool reply(embot::hw::can::Frame &frame, const std::uint8_t sender, const bool ok);  //ack/nack
        
    };  
    
    
    class Message_bldr_SETID : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    id;                 // if id is 255, then the board assign it randomly as best as it can.
            std::uint16_t   randominvalidmask;  // but not assign if address is set inside this mask
            Info() : id(0), randominvalidmask(0) {}
        };
        
        Info info;
        
        Message_bldr_SETID(){}
            
        bool load(const embot::hw::can::Frame &frame);
            
        bool reply();   // none
        
    };
        
    
    class Message_anypoll_SETID : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    id;  
            Info() : id(0) {}
        };
        
        Info info;
        
        Message_anypoll_SETID(){}
            
        bool load(const embot::hw::can::Frame &frame);
            
        bool reply();   // none
        
    };
    
}}} // namespace embot { namespace app { namespace canprotocol {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
