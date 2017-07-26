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

#include <vector>

namespace embot { namespace app { namespace canprotocol {
    
    enum class Clas { none = 0xfe, pollingMotorControl = 0, periodicMotorControl = 1, pollingAnalogSensor = 2, periodicAnalogSensor = 3, periodicSkin = 4, periodicInertialSensor = 5, bootloader = 7 };
    
    enum class anypollCMD { none = 0xfe, SETID = 0x32 };
    
    // Clas::pollingMotorControl (0)
    enum class mcpollCMD { none = 0xfe, SET_BOARD_ID = 50, GET_FIRMWARE_VERSION = 91, };
    
    // Clas::periodicMotorControl (1)
    enum class mcperCMD { none = 0xfe, PRINT = 6 };

  
    // Clas::pollingAnalogSensor (2)
    enum class aspollCMD {  none = 0xfe, 
                            GET_FIRMWARE_VERSION = 0x1C, SET_BOARD_ADX = 0x32,                                              // basic
                            SET_TXMODE = 0x07,                                                                              // mtb, strain 
                            SKIN_SET_BRD_CFG = 77, ACC_GYRO_SETUP = 79, SKIN_SET_TRIANG_CFG = 80,                           // mtb         
                            SET_CANDATARATE = 0x08,                                                                         // strain, mais  
                            GET_FULL_SCALES = 0x18,                                                                         // strain   
                            SET_FULL_SCALES = 0x17,                                                                         // strain canloader  
                            SAVE2EE = 0x09, GET_EEPROM_STATUS = 0x1B,                                                       // strain canloader
                            SET_SERIAL_NO = 0x19, GET_SERIAL_NO = 0x1A,                                                     // strain canloader
                            GET_AMP_GAIN = 0x1D, SET_AMP_GAIN = 0x1E,                                                       // 6sg ... sg6_get_amp_gain(), sg6_set_amp_gain()
                            GET_CH_ADC = 0x0C,                                                                              // strain canloader
                            SET_CH_DAC_offset = 0x04, GET_CH_DAC_offset = 0x0B,                                             // strain canloader
                            SET_MATRIX_RC = 0x03, GET_MATRIX_RC = 0x0A,                                                     // strain canloader
                            SET_MATRIX_G = 0x11, GET_MATRIX_G = 0x12,                                                       // strain canloader                                               
                            SET_CALIB_TARE_bias = 0x13, GET_CALIB_TARE_bias = 0x14,                                         // strain canloader 
                            SET_CURR_TARE_bias = 0x15, GET_CURR_TARE_bias = 0x16,                                           // strain canloader 

    // TBD: ... maybe not for strain2                    
                            SET_RESOLUTION = 0x10                                                                           // mais
    };
    
    // missing opcodes from ems application and from canloader are:
    // SET_IIR = 0x01
    // SELECT_ACTIVE_CH = 0x05
    // FILTER_EN = 0x0D
    // MUX_EN = 0x0E
    // MUX_NUM = 0x0F
    // 
    

    // Clas::periodicAnalogSensor (3) 
    enum class asperCMD {   none = 0xfe, 
                            DONTUSEIT_DIGITAL_GYROSCOPE = 0, DONTUSEIT_DIGITAL_ACCELEROMETER = 1,        
                            UNCALIBFORCE_VECTOR_DEBUGMODE = 0x08, UNCALIBTORQUE_VECTOR_DEBUGMODE = 0x09,                    // strain 
                            FORCE_VECTOR = 0x0A, TORQUE_VECTOR = 0x0B,                                                      // strain
    // TBD: ... maybe not for strain2       
                            HES0TO6 = 0x0C, HES7TO14 = 0x0D                                                                 // mais
    };
    
    // Clas::periodicSkin (4) 
    enum class skperCMD { none = 0xfe, TRG00 = 0, TRG01 = 1, TRG02 = 2, TRG03 = 3, TRG04 = 4, TRG05 = 5, TRG06 = 6, TRG07 = 7, TRG08 = 8, TRG09 = 9, 
                          TRG10 = 10, TRG11 = 11, TRG12 = 12, TRG13 = 13, TRG14 = 14, TRG15 = 15 };
    
    // Clas::periodicInertialSensor (5) 
    enum class isperCMD { none = 0xfe, DIGITAL_GYROSCOPE = 0, DIGITAL_ACCELEROMETER = 1 };
    
    // Clas::bootloader (7)    
    enum class bldrCMD { none = 0xfe, BROADCAST = 0xff, BOARD = 0x00, ADDRESS = 0x01, START = 0x02, DATA = 0x03, END = 0x04, GET_ADDITIONAL_INFO = 12, SET_ADDITIONAL_INFO = 13, SETCANADDRESS = 0x32 };

    
    bldrCMD cmd2bldr(std::uint8_t cmd);
    aspollCMD cmd2aspoll(std::uint8_t cmd);
    mcpollCMD cmd2mcpoll(std::uint8_t cmd);
    anypollCMD cmd2anypoll(std::uint8_t cmd);
    
    
    enum class Board { mtb = 5, strain = 6, mais = 7, mtb4 = 11, strain2 = 12, none = 254, unknown = 0xff };
    
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
    
    Clas frame2clas(const embot::hw::can::Frame &frame);
    std::uint8_t frame2cmd(const embot::hw::can::Frame &frame);
    
    // all the rest
    std::uint8_t frame2sender(const embot::hw::can::Frame &frame);
    
    bool frameisbootloader(const embot::hw::can::Frame &frame);
    
    bool frameispolling(const embot::hw::can::Frame &frame);
    
    bool frameisperiodic(const embot::hw::can::Frame &frame);
    
    std::uint8_t frame2destination(const embot::hw::can::Frame &frame);
    
    
    bool frameis4board(const embot::hw::can::Frame &frame, const std::uint8_t boardaddress);
    
    
    
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
            Board               board;
            Process             process;
            versionOfFIRMWARE   firmware;  
            ReplyInfo() : board(Board::unknown), process(Process::application) {}          
        };
        
        Info info;
                    
        Message_bldr_BROADCAST(){}
            
        bool load(const embot::hw::can::Frame &inframe);            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);   // boardtype, etc.
        
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
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender);   // ack/nack        
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
            
        bool load(const embot::hw::can::Frame &inframe); 
            
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
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok);   // ack/nack
        
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
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok);  //ack but only when te cumulative Info::size reach Message_bldr_ADDRESS::Info::datalen
        
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
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const bool ok);  //ack/nack
        
    };  
    

    class Message_bldr_GET_ADDITIONAL_INFO : public Message
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
        
                
        Message_bldr_GET_ADDITIONAL_INFO() : counter(0) {}
            
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

    class Message_bldr_SET_ADDITIONAL_INFO : public Message
    {
        public:
            
        struct Info
        {
            std::uint8_t    offset;     // 0, 4, 8, 16, 20, 24, 28. 255 is a non-valid value 
            char            info04[4];  
            Info() : offset(255) { }
        };
        
        Info info;
        
                
        Message_bldr_SET_ADDITIONAL_INFO() {}
            
        bool load(const embot::hw::can::Frame &inframe);
         
        // we need to receive 8 frames to be able to have a complete 32 bytes long info32. we operate in memory-less mode.
        // each time we recover only a fraction of info32[].
        // we could operate differently, but maybe for later: use of a static char info32[32] array and a static flag08.
        // when we received counter = 0, then we reset info32[] and flag08. then we fill the 4 bytes into info32 and set the
        // bit of pos counter. when all bits are on, we fill Info::info32[] and set Info::ready = true.
            
        bool reply(); // none 

    };  


    class Message_bldr_SET_ADDITIONAL_INFO2 : public Message
    {
        public:
            
        struct Info
        {
            bool        valid;     
            char        info32[32];  
            Info() : valid(false) { }
        };
        
        Info info;
        
                
        Message_bldr_SET_ADDITIONAL_INFO2() {}
            
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

    
    class Message_bldr_SETCANADDRESS : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    address;            // if id is 255, then the board assign it randomly as best as it can.
            std::uint16_t   randominvalidmask;  // but not assign if address is set inside this mask
            Info() : address(0), randominvalidmask(0) {}
        };
        
        Info info;
        
        Message_bldr_SETCANADDRESS(){}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };
        
   
 
    class Message_base_GET_FIRMWARE_VERSION : public Message
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
        
                
        Message_base_GET_FIRMWARE_VERSION(Clas cl, std::uint8_t cm) : cls(cl), cmd(cm) {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);    
            
        protected:
            
        Clas            cls;
        std::uint8_t    cmd;                 
    }; 
    
    class Message_mcpoll_GET_FIRMWARE_VERSION : public Message_base_GET_FIRMWARE_VERSION
    {
        public:
            
        Message_mcpoll_GET_FIRMWARE_VERSION() : 
            Message_base_GET_FIRMWARE_VERSION(Clas::pollingMotorControl, static_cast<std::uint8_t>(mcpollCMD::GET_FIRMWARE_VERSION)) {}
       
    }; 
    
    class Message_aspoll_GET_FIRMWARE_VERSION : public Message_base_GET_FIRMWARE_VERSION
    {
        public:
            
        Message_aspoll_GET_FIRMWARE_VERSION() : 
            Message_base_GET_FIRMWARE_VERSION(Clas::pollingAnalogSensor, static_cast<std::uint8_t>(aspollCMD::GET_FIRMWARE_VERSION)) {}
       
    }; 



    class Message_base_SET_ID : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t    address;      
            Info() : address(0) {}
        };
        
        
        Info info;
        
                
        Message_base_SET_ID(Clas cl, std::uint8_t cm) : cls(cl), cmd(cm) {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();    
            
        protected:
            
        Clas            cls;
        std::uint8_t    cmd;                 
    };   
    
    class Message_aspoll_SET_BOARD_ADX : public Message_base_SET_ID
    {
        public:
            
        Message_aspoll_SET_BOARD_ADX() : 
            Message_base_SET_ID(Clas::pollingAnalogSensor, static_cast<std::uint8_t>(aspollCMD::SET_BOARD_ADX)) {}
       
    }; 

    class Message_mcpoll_SET_BOARD_ID : public Message_base_SET_ID
    {
        public:
            
        Message_mcpoll_SET_BOARD_ID() : 
            Message_base_SET_ID(Clas::pollingMotorControl, static_cast<std::uint8_t>(mcpollCMD::SET_BOARD_ID)) {}
       
    }; 
    
    
    class Message_aspoll_SET_TXMODE : public Message
    {
        public:
            
        Board board;    // strain, strain2, mtb, mtb4 (but also mais could be ...).
            
        // use it if we have a Board::strain or Board::strain2
        enum class StrainMode { txCalibrated = 0, acquireOnly = 1, txUncalibrated = 3, txAll = 4, none = 254 };
            
        struct Info
        { 
            bool            transmit;  
            StrainMode      strainmode;            
            Info() : transmit(false), strainmode(StrainMode::none) {}
        };
        
        Info info;
        
        Message_aspoll_SET_TXMODE(Board brd) : board(brd) {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };
    
    
    class Message_aspoll_SKIN_SET_BRD_CFG : public Message
    {
        public:
            
        enum class SkinType { withTemperatureCompensation = 0, palmFingerTip = 1, withoutTempCompensation = 2, testmodeRAW = 7, none = 254 };
                        
        struct Info
        { 
            SkinType                    skintype;  
            std::uint8_t                noload; 
            embot::common::relTime      txperiod;              
            Info() : skintype(SkinType::none), noload(0), txperiod(50*embot::common::time1millisec) {}
        };
        
        Info info;
        
        Message_aspoll_SKIN_SET_BRD_CFG() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };    
    
    class Message_aspoll_SKIN_SET_TRIANG_CFG : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                trgStart;  
            std::uint8_t                trgEnd;  
            std::uint8_t                shift; 
            bool                        enabled;
            std::uint16_t               cdcOffset;
            Info() : trgStart(0), trgEnd(0), shift(0), enabled(false), cdcOffset(0) {}
        };
        
        Info info;
        
        Message_aspoll_SKIN_SET_TRIANG_CFG() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };  


    class Message_aspoll_SET_CANDATARATE : public Message
    {
        public:
                        
            
        struct Info
        { 
            embot::common::relTime  txperiod;           
            Info() : txperiod(10*embot::common::time1millisec) {}
        };
        
        Info info;
        
        Message_aspoll_SET_CANDATARATE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    
    

    class Message_aspoll_SET_FULL_SCALES : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t    channel;
            std::uint16_t   fullscale;            
            Info() : channel(0), fullscale(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_FULL_SCALES() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    
    
    
    class Message_aspoll_GET_FULL_SCALES : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t  channel;           
            Info() : channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            std::uint16_t       fullscale;
            ReplyInfo() : channel(0), fullscale(0) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_FULL_SCALES() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    
    class Message_aspoll_GET_AMP_GAIN : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t  channel;           
            Info() : channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            std::uint16_t       gain1;
            std::uint16_t       gain2;
            ReplyInfo() : channel(0), gain1(0), gain2(0) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_AMP_GAIN() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  

    class Message_aspoll_SET_AMP_GAIN : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        channel;
            std::uint16_t       gain1;
            std::uint16_t       gain2;           
            Info() : channel(0), gain1(0), gain2(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_AMP_GAIN() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 
    
    class Message_aspoll_GET_CH_DAC_offset : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t  channel;           
            Info() : channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            std::uint16_t       offset;
            ReplyInfo() : channel(0), offset(0) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_CH_DAC_offset() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  

    
    class Message_aspoll_SET_CH_DAC_offset : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        channel;
            std::uint16_t       offset;         
            Info() : channel(0), offset(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_CH_DAC_offset() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };


    class Message_aspoll_SET_MATRIX_RC : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        row;
            std::uint8_t        col;
            std::uint16_t       value;         
            Info() : row(0), col(0), value(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_MATRIX_RC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };      
    
    class Message_aspoll_SET_MATRIX_G : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        gain;       
            Info() : gain(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_MATRIX_G() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 


    class Message_aspoll_SET_CALIB_TARE_bias : public Message
    {
        public:
             
        enum class Mode { def = 1, reset = 0, useval = 2, unknown = 255 };        
            
        struct Info
        { 
            Mode                mode;
            std::uint8_t        channel;
            std::uint16_t       value;         
            Info() : mode(Mode::def), channel(0), value(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_CALIB_TARE_bias() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    

    class Message_aspoll_GET_CALIB_TARE_bias : public Message
    {
        public:
             
        struct Info
        { 
            std::uint8_t    channel; 
            Info() : channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            std::uint16_t       value;
            ReplyInfo() : channel(0), value(0) {}          
        };         
        
        Info info;
        
        Message_aspoll_GET_CALIB_TARE_bias() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);
            
    };         

    class Message_aspoll_SET_CURR_TARE_bias : public Message
    {
        public:
             
        enum class Mode { def = 1, reset = 0, useval = 2, unknown = 255 };        
            
        struct Info
        { 
            Mode                mode;
            std::uint8_t        channel;
            std::uint16_t       value;         
            Info() : mode(Mode::def), channel(0), value(0) {}
        };
        
        Info info;
        
        Message_aspoll_SET_CURR_TARE_bias() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    

    class Message_aspoll_GET_CURR_TARE_bias : public Message
    {
        public:
             
        struct Info
        { 
            std::uint8_t    channel; 
            Info() : channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            std::uint16_t       value;
            ReplyInfo() : channel(0), value(0) {}          
        };         
        
        Info info;
        
        Message_aspoll_GET_CURR_TARE_bias() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);
            
    };      
    
    class Message_aspoll_GET_CH_ADC : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t    channel;   
            bool            getrawvalue;
            Info() : channel(0), getrawvalue(true) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            bool                valueisraw;
            std::uint16_t       adcvalue;
            ReplyInfo() : channel(0), valueisraw(true), adcvalue(0) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_CH_ADC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    

    class Message_aspoll_GET_MATRIX_RC : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t    row;   
            std::uint8_t    col;
            Info() : row(0), col(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        row;
            std::uint8_t        col;
            std::uint16_t       value;         
            ReplyInfo() : row(0), col(0), value(0) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_MATRIX_RC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    }; 


    class Message_aspoll_GET_MATRIX_G : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t    nothing;   
            Info() : nothing(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t    gain;       
            ReplyInfo() : gain(0) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_MATRIX_G() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };          
    
    class Message_aspoll_SAVE2EE : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t  nothing;           
            Info() : nothing(0) {}
        };
        
        struct ReplyInfo
        {
            bool ok;
            ReplyInfo() : ok(true) {}          
        };        
        
        Info info;
        
        Message_aspoll_SAVE2EE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    
    class Message_aspoll_GET_EEPROM_STATUS : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t  nothing;           
            Info() : nothing(0) {}
        };
        
        struct ReplyInfo
        {
            bool saved;
            ReplyInfo() : saved(true) {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_EEPROM_STATUS() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  


    class Message_aspoll_SET_SERIAL_NO : public Message
    {
        public:
                        
            
        struct Info
        { 
            char serial[7];        
            Info() {}
        };
        
        Info info;
        
        Message_aspoll_SET_SERIAL_NO() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };


    class Message_aspoll_GET_SERIAL_NO : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t nothing;           
            Info() : nothing(0) {}
        };
        
        struct ReplyInfo
        {
            char        serial[7];
            ReplyInfo() {}          
        };        
        
        Info info;
        
        Message_aspoll_GET_SERIAL_NO() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };      
    
    
    class Message_skper_TRG : public Message
    {
        public:
            
        enum class ErrorInTriangleBit { noack = 0, notconnected = 1 };
            
        struct Info
        { 
            std::uint8_t                the12s[12];
            std::uint16_t               outofrangemaskofthe12s;         // bit in pos x is 1 if the12s[x] is out of range.
            std::uint16_t               notconnectedmaskofthe12s;       // 0 no error, 1 error of no ack, 2 error of none connected. 
            std::uint16_t               notackmaskofthe12s;
            std::uint8_t                trianglenum;
            std::uint8_t                canaddress;
            Info() { std::memset(the12s, 0, sizeof(the12s)); trianglenum = canaddress = outofrangemaskofthe12s = notconnectedmaskofthe12s = notackmaskofthe12s = 0;}
        };
        
        Info info;
        
        Message_skper_TRG() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe0, embot::hw::can::Frame &outframe1);        
    };

    class Message_mcper_PRINT : public Message
    {
        public:
            
        struct Info
        { 
            char                        text[31];   // 30 chars + '0' : at most 5 can messages ('0' is not transmitted)
            std::uint8_t                canaddress;
            Info() { std::memset(text, 0, sizeof(text)); canaddress = 0; }
        };
        
        Info info;
        
        Message_mcper_PRINT() { framecounter = nframes = nchars = 0; }
            
        bool load(const Info& inf);
            
       // bool get(std::vector<embot::hw::can::Frame> &frames); 
            
        std::uint8_t numberofframes();
        bool get(embot::hw::can::Frame &outframe);  

        private:
        
        std::uint8_t nframes;    
        std::uint8_t framecounter;    
        std::uint8_t nchars;           
        static std::uint8_t textIDmod4;  // 0, 1, 2, 3, 0, 1, 2, etc      
    }; 

    class Message_aspoll_ACC_GYRO_SETUP : public Message
    {
        public:
            
        enum class InertialTypeBit { analogaccelerometer = 0, 
                                     internaldigitalaccelerometer = 1, 
                                     externaldigitalgyroscope = 2, 
                                     externaldigitalaccelerometer = 3, 
                                     none = 255 };
        
        enum class InertialType {   none = 0, 
                                    analogaccelerometer = 0x01, 
                                    internaldigitalaccelerometer = 0x02, 
                                    externaldigitalgyroscope = 0x04, 
                                    externaldigitalaccelerometer = 0x08 };
        
        struct Info
        { 
            std::uint8_t                maskoftypes;  
            embot::common::relTime      txperiod;              
            Info() : maskoftypes(0), txperiod(50*embot::common::time1millisec) {}
        };
        
        Info info;
        
        Message_aspoll_ACC_GYRO_SETUP() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };     

    class Message_isper_DIGITAL_GYROSCOPE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_isper_DIGITAL_GYROSCOPE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe);        
    };
    
    class Message_isper_DIGITAL_ACCELEROMETER : public Message
    {
        public:
                        
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_isper_DIGITAL_ACCELEROMETER() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe);        
    }; 


    class Message_asper_UNCALIBFORCE_VECTOR_DEBUGMODE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_asper_UNCALIBFORCE_VECTOR_DEBUGMODE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe);        
    };    
    
    
    class Message_asper_UNCALIBTORQUE_VECTOR_DEBUGMODE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_asper_UNCALIBTORQUE_VECTOR_DEBUGMODE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe);        
    };    

    
    class Message_asper_FORCE_VECTOR : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_asper_FORCE_VECTOR() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe);        
    };    
    
    
    class Message_asper_TORQUE_VECTOR : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_asper_TORQUE_VECTOR() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe);        
    };    

    
    
}}} // namespace embot { namespace app { namespace canprotocol {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
