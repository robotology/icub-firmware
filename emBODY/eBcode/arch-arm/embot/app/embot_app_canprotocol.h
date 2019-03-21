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

#include "embot_hw_can.h"




namespace embot { namespace app { namespace canprotocol {
    
    // - useful types 
    
    enum class Board { mtb = 5, strain = 6, mais = 7, mtb4 = 11, strain2 = 12, rfe = 13, sg3 = 14, psc = 15, mtb4w = 16, none = 254, unknown = 0xff };
    
    enum class Process { bootloader = 0, application = 1, unknown = 0xff };
           
    struct versionOfBOOTLOADER
    {
        std::uint8_t    major;
        std::uint8_t    minor;
        versionOfBOOTLOADER() : major(0), minor(0) {}
        versionOfBOOTLOADER(std::uint8_t ma, std::uint8_t mi) : major(ma), minor(mi) {}
    };
    
    struct versionOfAPPLICATION
    {
        std::uint8_t    major;
        std::uint8_t    minor;
        std::uint8_t    build;
        versionOfAPPLICATION() : major(0), minor(0), build(0) {}
        versionOfAPPLICATION(std::uint8_t ma, std::uint8_t mi, std::uint8_t bu) : major(ma), minor(mi), build(bu) {}
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
        std::uint8_t    major;
        std::uint8_t    minor;
        versionOfCANPROTOCOL() : major(0), minor(0) {}
        versionOfCANPROTOCOL(std::uint8_t ma, std::uint8_t mi) : major(ma), minor(mi) {}
    };    
    
    
    // the protocol classes 
    
    enum class Clas { none = 0xfe, 
                      pollingMotorControl = 0, periodicMotorControl = 1, pollingAnalogSensor = 2, 
                      periodicAnalogSensor = 3, periodicSkin = 4, periodicInertialSensor = 5, bootloader = 7 };
        
    // the commands in each class and the messages which manage their forming / parsing are defined in namespaces ::bootloader, ::motor etc.
    
   
    // utility functions for a received embot::hw::can::Frame
      
    std::uint8_t frame2sender(const embot::hw::can::Frame &frame);
    std::uint8_t frame2destination(const embot::hw::can::Frame &frame);
    bool frameis4board(const embot::hw::can::Frame &frame, const std::uint8_t boardaddress);
    
    // by these we retrieve the class and the command.
    
    Clas frame2clas(const embot::hw::can::Frame &frame);
    std::uint8_t frame2cmd(const embot::hw::can::Frame &frame);
        
    // by these we get some properties of a frame
   
    bool frameisbootloader(const embot::hw::can::Frame &frame);
    
    bool frameispolling(const embot::hw::can::Frame &frame);
    
    bool frameisperiodic(const embot::hw::can::Frame &frame);
    
    
        
    
    std::uint8_t frame2datasize(const embot::hw::can::Frame &frame);
    
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
        
        struct CanData
        {
            std::uint8_t            from;
            std::uint8_t            to;
            Clas                    clas;       
            std::uint8_t            cmd;       // use they are: ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID, etc.
            std::uint8_t*           datainframe;
            std::uint8_t            sizeofdatainframe;  
            CanData() { reset(); }
            void reset() { from = 0; to = 0; clas = Clas::none; cmd = 0; datainframe = nullptr; sizeofdatainframe = 0; } 
        };

    public:        
        
        bool                    valid;
        CanData                 candata;
        embot::hw::can::Frame   canframe;
    
              
        Message() { clear(); }
           
        // it copies a received frame and it computes other data structure with it. it sets valid = true. after that, if valid == true one can use the public data structures
        void set(const embot::hw::can::Frame &fr);
        
        // it clears dat structure and sets valid = false. nobody should use its dta structures
        void clear();
        
        // it fills data structures with passed values. it creates a frame. it sets valid = true. after that, if valid == true one can use the frame for tx
        void set(std::uint8_t fr, std::uint8_t t, Clas cl, std::uint8_t cm, const void *dat, std::uint8_t siz);
        
        bool isvalid();
                
    };
    
    
    
    
}}} // namespace embot { namespace app { namespace canprotocol {



namespace embot { namespace app { namespace canprotocol { namespace shared {
 
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
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);    
            
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
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();    
            
        protected:
            
        Clas            cls;
        std::uint8_t    cmd;                 
    };   
      

}}}} // namespace embot { namespace app { namespace canprotocol { namespace shared {


namespace embot { namespace app { namespace canprotocol { namespace analog {

    enum class imuSensor { acc = 0, mag = 1, gyr = 2, eul = 3, qua = 4, lia = 5, grv = 6, status = 15, none = 16 };
    
    
    
    enum class posType : uint8_t { angleDeciDeg = 0, unknown = 15 };               
    struct posDES
    {
        posType         type;               // tells if it is an angle, a linear position etc. use an enum class
        std::uint8_t    indexoffirst : 4;   // the index of the first value stored inside data[]   
        std::uint8_t    numberof : 4;       // the number of values stored inside data[]      
        posDES() : type(posType::angleDeciDeg), indexoffirst(0), numberof(2) {} 
        void reset() { type = posType::angleDeciDeg; indexoffirst = 0; numberof = 2; }
        bool isvalid() const { if((type == posType::angleDeciDeg) && (numberof>0)) { return true; } else { return false; } }        
    };  // it must be stored in 16 bits
    
//    enum class posType : uint8_t { angleDdec = 0, angleD = 1,  angleDcen = 2, raw = 14, unknown = 15 };            
//    enum class posFormat : uint8_t { I16 = 0, U08 = 1, I08 = 2, U16 = 3, U32 = 4, I32 = 5, unknown = 15 };        
//    struct posDescriptor
//    {
//        posType         type;               // tells if it is an angle, a linear position etc. use an enum class
//        posFormat       format;             // tells a format
//        std::uint8_t    indexoffirst : 4;   // the index of the first value stored inside data[]   
//        std::uint8_t    numberof : 4;       // the number of values stored inside data[]      
//        posDescriptor() : type(posType::angleDdec), format(posFormat::I16), indexoffirst(0), numberof(2) {} 
//        void reset() { type = posType::angleDdec; format = posFormat::I16; indexoffirst = 0; numberof = 2; }
//        bool isvalid() const { if((type == posType::angleDdec) && (format == posFormat::I16) && (numberof>0)) { return true; } else { return false; } }
//        
//    };  // it must be stored in 16 bits
    
    
//    enum class posTYPE : uint8_t { angleDegDec = 0 , angleDecCen = 1, unknown = 15 };
//    struct posDESC
//    {
//        posTYPE type;
//        posDESC() : type(posTYPE::angleDegDec) {}
//        float tofloat(void *data) const 
//        { 
//            float r = 0; 
//            if((type == posTYPE::angleDegDec) && (data != nullptr)) { int16_t *v = reinterpret_cast<int16_t*>(data); r = *v; r/=10; }
//            return r;
//        }
//        void todata(const float v, void* data) const 
//        {
//            if((type == posTYPE::angleDegDec) && (data != nullptr))
//            {
//                uint8_t *b = reinterpret_cast<uint8_t*>(data);
//                int16_t vv = v * 10.0;
//                b[0] = vv & 0xff;
//                b[1] = vv >> 8;
//            }
//        }
//        uint8_t getsize() const { if((type == posTYPE::angleDegDec)) { return 2; } else { return 0; } }
//    };
    
    
    

}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
