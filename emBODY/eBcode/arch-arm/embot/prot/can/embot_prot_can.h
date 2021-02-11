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

#ifndef _EMBOT_PROT_CAN_H_
#define _EMBOT_PROT_CAN_H_

#include "embot_core.h"


namespace embot { namespace prot { namespace can {
    
    // - useful types 

    struct Frame            
    {
        std::uint32_t       id          {0};
        std::uint8_t        size        {0};
        std::uint8_t        filler[3]   {0};
        std::uint8_t        data[8]     {0};  
        Frame() = default;
        Frame(std::uint32_t i, std::uint8_t s, std::uint8_t *d) : id(i), size(std::min(s, static_cast<std::uint8_t>(8))) 
        {
            if(nullptr != d) { std::memmove(data, d, size); }
        }
    };

    
    enum class Board { mtb = 5, strain = 6, mais = 7, mtb4 = 11, strain2 = 12, rfe = 13, sg3 = 14, psc = 15, mtb4w = 16, pmc = 17, none = 254, unknown = 0xff };
    
    enum class Process { bootloader = 0, application = 1, unknown = 0xff };
           
    struct versionOfBOOTLOADER
    {
        std::uint8_t    major {0};
        std::uint8_t    minor {0};
        versionOfBOOTLOADER() = default;
        constexpr versionOfBOOTLOADER(std::uint8_t ma, std::uint8_t mi) : major(ma), minor(mi) {}
    };
    
    struct versionOfAPPLICATION
    {
        std::uint8_t    major {0};
        std::uint8_t    minor {0};
        std::uint8_t    build {0};
        versionOfAPPLICATION() = default;
        constexpr versionOfAPPLICATION(std::uint8_t ma, std::uint8_t mi, std::uint8_t bu) : major(ma), minor(mi), build(bu) {}
    };    
    
    struct versionOfFIRMWARE
    {
        std::uint8_t    major;
        std::uint8_t    minor;
        std::uint8_t    build;
        versionOfFIRMWARE() : major(0), minor(0), build(0) {}
        versionOfFIRMWARE(std::uint8_t ma, std::uint8_t mi, std::uint8_t bu) : major(ma), minor(mi), build(bu) {}
    };
    
    struct versionOfCANPROTOCOL
    {
        std::uint8_t    major {0};
        std::uint8_t    minor {0};
        versionOfCANPROTOCOL() = default;
        constexpr versionOfCANPROTOCOL(std::uint8_t ma, std::uint8_t mi) : major(ma), minor(mi) {}
    };    
    
    
    // the protocol classes 
    
    enum class Clas { none = 0xfe, 
                      pollingMotorControl = 0, periodicMotorControl = 1, pollingAnalogSensor = 2, 
                      periodicAnalogSensor = 3, periodicSkin = 4, periodicInertialSensor = 5, periodicForFutureUse = 6, bootloader = 7 };
        
    // the commands in each class and the messages which manage their forming / parsing are defined in namespaces ::bootloader, ::motor etc.
      
    // utility functions for a received embot::prot::can::Frame
      
    std::uint8_t frame2sender(const embot::prot::can::Frame &frame);
    std::uint8_t frame2destination(const embot::prot::can::Frame &frame);
    bool frameis4board(const embot::prot::can::Frame &frame, const std::uint8_t boardaddress);
    
    // by these we retrieve the class and the command.
    
    Clas frame2clas(const embot::prot::can::Frame &frame);
    std::uint8_t frame2cmd(const embot::prot::can::Frame &frame);
        
    // by these we get some properties of a frame
   
    bool frameisbootloader(const embot::prot::can::Frame &frame);
    
    bool frameispolling(const embot::prot::can::Frame &frame);
    
    bool frameisperiodic(const embot::prot::can::Frame &frame);
          
    std::uint8_t frame2datasize(const embot::prot::can::Frame &frame);
    
    std::uint8_t* frame2databuffer(embot::prot::can::Frame &frame);
    
       
    bool frame_set_clascmddestinationdata(embot::prot::can::Frame &frame, const Clas cls, const std::uint8_t cmd, const std::uint8_t destination, const void *data, const std::uint8_t size, const std::uint8_t mcindex = 0, bool verify = false);
        
    bool frame_set_sender(embot::prot::can::Frame &frame, std::uint8_t sender, bool verify = false);
    
    bool frame_set_size(embot::prot::can::Frame &frame, std::uint8_t size, bool verify = false);
    

    
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
        
        struct CanData
        {
            std::uint8_t            from {0};
            std::uint8_t            to {0};
            Clas                    clas {Clas::none} ;       
            std::uint8_t            cmd {0}; 
            std::uint8_t*           datainframe {nullptr};
            std::uint8_t            sizeofdatainframe {0};  
            CanData() = default;
            void reset() { from = 0; to = 0; clas = Clas::none; cmd = 0; datainframe = nullptr; sizeofdatainframe = 0; } 
        };

    public:        
        
        bool                    valid {false};
        CanData                 candata {};
        embot::prot::can::Frame canframe {};
    
              
        Message() = default;
           
        // it copies a received frame and it computes other data structure with it. it sets valid = true. after that, if valid == true one can use the public data structures
        void set(const embot::prot::can::Frame &fr);
        
        // it clears dat structure and sets valid = false. nobody should use its dta structures
        void clear();
        
        // it fills data structures with passed values. it creates a frame. it sets valid = true. after that, if valid == true one can use the frame for tx
        void set(std::uint8_t fr, std::uint8_t t, Clas cl, std::uint8_t cm, const void *dat, std::uint8_t siz);
        
        bool isvalid();                
    };
    
    
    
    
}}} // namespace embot { namespace prot { namespace can {



namespace embot { namespace prot { namespace can { namespace shared {
 
    class Message_GET_VERSION : public Message
    {
    public:
            
        struct Info
        { 
            versionOfCANPROTOCOL    requiredprotocol;      
            Info() {}
        };
        
        struct ReplyInfo
        {
            Board                   board;
            versionOfFIRMWARE       firmware;  
            versionOfCANPROTOCOL    protocol;
            ReplyInfo() : board(Board::unknown) {}          
        };
        
        Info info;
        
                
        Message_GET_VERSION(Clas cl, std::uint8_t cm) : cls(cl), cmd(cm) {}
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);    
            
        protected:
            
        Clas            cls;
        std::uint8_t    cmd;                 
    }; 
    

    class Message_SET_ID : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    address;      
            Info() : address(0) {}
        };
        
        
        Info info;
        
                
        Message_SET_ID(Clas cl, std::uint8_t cm) : cls(cl), cmd(cm) {}
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();    
            
        protected:
            
        Clas            cls;
        std::uint8_t    cmd;                 
    };   
      

}}}} // namespace embot { namespace prot { namespace can { namespace shared {


namespace embot { namespace prot { namespace can { namespace analog {

    enum class imuSensor { acc = 0, mag = 1, gyr = 2, eul = 3, qua = 4, lia = 5, grv = 6, status = 15, none = 16 };
    
       
    enum class posTYPE : uint8_t { angleDeciDeg = 0, linearDeciMilliMeter = 1, unknown = 255 };   
    
    enum class posLABEL : uint8_t { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, eight = 8, 
                                    nine = 9, ten = 10, eleven = 11, twelve = 12, thirteen = 13, fourteen = 14, fifteen = 15 };
    
    struct posDES
    {
        posTYPE         type;               // tells if it is an angle, a linear position etc. use an enum class
        posLABEL        startlabel;         // the label of the first value stored inside data[]   
        std::uint8_t    labelsnumberof : 4; // the number of values stored inside data[]. they must have consecutive labels      
        posDES() : type(posTYPE::angleDeciDeg), startlabel(posLABEL::zero), labelsnumberof(2) {} 
        void reset() { type = posTYPE::angleDeciDeg; startlabel = posLABEL::zero; labelsnumberof = 2; }
        bool isvalid() const { if(((type == posTYPE::angleDeciDeg) || (type == posTYPE::linearDeciMilliMeter)) && (labelsnumberof>0)) { return true; } else { return false; } }        
        };  // it must be stored in 16 bits: 8 bits for type, 4 bits for startlabel, 4 bits for labelsnumberof
    
    using deciDeg = std::int16_t;
    using deciMilliMeter = std::int16_t; // +/- 32k ~= +/- 3200 mm = +/- 3.2 m
    
    float deciDeg_export(const deciDeg d); 
    deciDeg deciDeg_import(const float f);
    
    

}}}} // namespace embot { namespace prot { namespace can { namespace analog {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
