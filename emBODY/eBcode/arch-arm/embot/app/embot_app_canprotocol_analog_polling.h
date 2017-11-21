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

#ifndef _EMBOT_APP_CANPROTOCOL_ANALOG_POLLING_H_
#define _EMBOT_APP_CANPROTOCOL_ANALOG_POLLING_H_

#include "embot_common.h"
#include "embot_binary.h"

#include "embot_app_canprotocol.h"


namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {
        
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        SET_MATRIX_RC = 0x03,                           // used by canloader to configure strain
        SET_CH_DAC = 0x04,                              // used by canloader to configure strain
        SET_TXMODE = 0x07,                              // used to start tx of data for mtb or strain
        SET_CANDATARATE = 0x08,                         // used to configure strain or mais tx rate
        SAVE2EE = 0x09,                                 // used by canloader to configure strain
        GET_MATRIX_RC = 0x0A,                           // used by canloader to configure strain
        GET_CH_DAC = 0x0B,                              // used by canloader to configure strain
        GET_CH_ADC = 0x0C,                              // used by canloader to configure strain
        
              

        SET_MATRIX_G = 0x11,                            // used by canloader to configure strain
        GET_MATRIX_G = 0x12,                            // used by canloader to configure strain                                                                                                      
        SET_CALIB_TARE = 0x13,                          // used by canloader to configure strain
        GET_CALIB_TARE = 0x14,                          // used by canloader to configure strain 
        SET_CURR_TARE = 0x15,                           // used by canloader to configure strain
        GET_CURR_TARE = 0x16,                           // used by canloader to configure strain           
        SET_FULL_SCALES = 0x17,                         // used by canloader to configure strain  
        GET_FULL_SCALES = 0x18,                         // used by reader to properly scaled data received by strain 
        SET_SERIAL_NO = 0x19,                           // used by canloader to configure strain
        GET_SERIAL_NO = 0x1A,                           // used by canloader for strain
        GET_EEPROM_STATUS = 0x1B,                       // used by canloader to configure strain
        GET_FIRMWARE_VERSION = 0x1C,                    // basic management.
        GET_AMP_GAIN = 0x1D,                            // used by canloader to configure 6sg 
        SET_AMP_GAIN = 0x1E,                            // used by canloader to configure 6sg

        SET_BOARD_ADX = 0x32,                           // basic management
        
        
        
        SKIN_OBSOLETE_TACT_SETUP = 76, // 0x4C

        SKIN_SET_BRD_CFG = 77,                          // 0x4D used to configure the skin data in mtb + its tx rate
        ACC_GYRO_SETUP = 79,                            // 0x4F used to configure the inertial data in mtb + its tx rate
        SKIN_SET_TRIANG_CFG = 80,                       // 0x50 used to configure the skin data in mtb  
        
        
        // messages used for strain2: 
        STRAIN2_AMPLIFIER_RESET = 0x20,                 // reset the amplifier (transfer function + others) to default factory values. 
        STRAIN2_AMPLIFIER_CFG1_SET = 0x21,              // config of the amplifier transfer function (gains + offsets). 
        STRAIN2_AMPLIFIER_CFG1_GET = 0x22,              // config of the amplifier transfer function (gains + offsets). 
        STRAIN2_AMPLIFIER_AUTOCALIB = 0x23,             // it starts a calibration procedure for the amplifier(s). 
        
        // messages used for IMU and TERMOMETER sensors (strain2 + mtb4)
        IMU_CONFIG_SET = 0x24,
        IMU_CONFIG_GET = 0x25,
        IMU_TRANSMIT = 0x26,
        
        TERMOMETER_CONFIG_SET = 0x27,
        TERMOMETER_CONFIG_GET = 0x28,
        TERMOMETER_TRANSMIT = 0x29        
    };
    
    
    // NOTES
    // - only some messages are managed. there are basic messages (those for canloader), some for mtb, others for strain
    // - the analog polling class contains an heterogeneous set of messages: 
    //   1. basic management: GET_FIRMWARE_VERSION and SET_BOARD_ADX (whose functionalities are sadly duplicated in motor polling)
    //   2. configuration of analog sensor boards (mais and strain and 6sg)
    //   3. configuration of skin and inertial acquisition.
    // - message SET_RESOLUTION (0x10) is not implemented yet because only mais uses it.
    // - messages SET_IIR = 0x01, SELECT_ACTIVE_CH = 0x05, FILTER_EN = 0x0D, MUX_EN = 0x0E, MUX_NUM = 0x0F are not implemented 
    //   because they seem to be unúsed in strain.
    // - there are hole in values: 2, [0x1f, 0x31], [0x33, 0x4C], [0x51, ->], where we can add extra messages to configure new boards.
    
    
    // some utilities    
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {



 
namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {
         
    // the management of commands
    
    class Message_GET_FIRMWARE_VERSION : public embot::app::canprotocol::shared::Message_GET_VERSION
    {
        public:
            
        Message_GET_FIRMWARE_VERSION() : 
            embot::app::canprotocol::shared::Message_GET_VERSION(Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION)) {}
       
    }; 
    
    
    class Message_SET_BOARD_ADX : public embot::app::canprotocol::shared::Message_SET_ID
    {
        public:
            
        Message_SET_BOARD_ADX() : 
            embot::app::canprotocol::shared::Message_SET_ID(Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::SET_BOARD_ADX)) {}
       
    };  



    class Message_SET_TXMODE : public Message
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
        
        Message_SET_TXMODE(Board brd) : board(brd) {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };
    
    
    class Message_SKIN_OBSOLETE_TACT_SETUP : public Message
    {   // marco.accame on 02 nov 17: this message is obsolete. in here i implement only the features used by an old test program.
        // as such:
        // D0: opcode (=0x4c)
        // D1-nib0: resolution fixed to 8 bit (=0x1)
        // D1-nib1: conf fixed to SINGLE (=0x0)
        // D2: accuracy fixe to high (=0x01)
        // D3: period fixed to the default one of 40 ms (=0x01)
        // [D4, D5]: cdc offset in little endian FIXED = 0x2200
        // [D6, D7]: not used in here. it contains the value of teh period in case D3 is = 0x03. 
        public:
            

        struct Info
        { 
            std::uint16_t               cdcOffset;  
            embot::common::relTime      txperiod;              
            Info() : cdcOffset(0x2200), txperiod(40*embot::common::time1millisec) {}
        };
        
        Info info;
        
        Message_SKIN_OBSOLETE_TACT_SETUP() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };    
    
    class Message_SKIN_SET_BRD_CFG : public Message
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
        
        Message_SKIN_SET_BRD_CFG() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };    
    
    class Message_SKIN_SET_TRIANG_CFG : public Message
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
        
        Message_SKIN_SET_TRIANG_CFG() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };  


    class Message_SET_CANDATARATE : public Message
    {
        public:
                        
            
        struct Info
        { 
            embot::common::relTime  txperiod;           
            Info() : txperiod(10*embot::common::time1millisec) {}
        };
        
        Info info;
        
        Message_SET_CANDATARATE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    
    

    class Message_SET_FULL_SCALES : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t    channel;
            std::uint16_t   fullscale;            
            Info() : channel(0), fullscale(0) {}
        };
        
        Info info;
        
        Message_SET_FULL_SCALES() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    
    
    
    class Message_GET_FULL_SCALES : public Message
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
        
        Message_GET_FULL_SCALES() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    
    class Message_GET_AMP_GAIN : public Message
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
            std::uint16_t       gain0;
            std::uint16_t       gain1;
            ReplyInfo() : channel(0), gain0(0), gain1(0) {}          
        };        
        
        Info info;
        
        Message_GET_AMP_GAIN() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  

    class Message_SET_AMP_GAIN : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        channel;
            std::uint16_t       gain0;
            std::uint16_t       gain1;           
            Info() : channel(0), gain0(0), gain1(0) {}
        };
        
        Info info;
        
        Message_SET_AMP_GAIN() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 
    
    class Message_GET_CH_DAC : public Message
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
        
        Message_GET_CH_DAC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  

    
    class Message_SET_CH_DAC : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        channel;
            std::uint16_t       offset;         
            Info() : channel(0), offset(0) {}
        };
        
        Info info;
        
        Message_SET_CH_DAC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };


    class Message_SET_MATRIX_RC : public Message
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
        
        Message_SET_MATRIX_RC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };      
    
    class Message_SET_MATRIX_G : public Message
    {
        public:
                        
            
        struct Info
        { 
            std::uint8_t        gain;       
            Info() : gain(0) {}
        };
        
        Info info;
        
        Message_SET_MATRIX_G() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 


    class Message_SET_CALIB_TARE : public Message
    {
        public:
             
        enum class Mode { everychannelreset = 0, everychannelnegativeofadc = 1, setchannelwithvalue = 2, unknown = 255 };        
            
        struct Info
        { 
            Mode                mode;
            std::uint8_t        channel;
            std::uint16_t       value;         
            Info() : mode(Mode::everychannelreset), channel(0), value(0) {}
        };
        
        Info info;
        
        Message_SET_CALIB_TARE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    

    class Message_GET_CALIB_TARE : public Message
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
        
        Message_GET_CALIB_TARE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);
            
    };         

    class Message_SET_CURR_TARE : public Message
    {
        public:
             
        enum class Mode { everychannelreset = 0, everychannelnegativeoftorqueforce = 1, setchannelwithvalue = 2, unknown = 255 };        
            
        struct Info
        { 
            Mode                mode;
            std::uint8_t        channel;
            std::uint16_t       value;         
            Info() : mode(Mode::everychannelreset), channel(0), value(0) {}
        };
        
        Info info;
        
        Message_SET_CURR_TARE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };    

    class Message_GET_CURR_TARE : public Message
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
        
        Message_GET_CURR_TARE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);
            
    };      
    
    class Message_GET_CH_ADC : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t    channel;   
            bool            getcalibrated;
            Info() : channel(0), getcalibrated(true) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;
            bool                valueiscalibrated;
            std::uint16_t       adcvalue;
            ReplyInfo() : channel(0), valueiscalibrated(true), adcvalue(0) {}          
        };        
        
        Info info;
        
        Message_GET_CH_ADC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    

    class Message_GET_MATRIX_RC : public Message
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
        
        Message_GET_MATRIX_RC() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    }; 


    class Message_GET_MATRIX_G : public Message
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
        
        Message_GET_MATRIX_G() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };          
    
    class Message_SAVE2EE : public Message
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
        
        Message_SAVE2EE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    
    class Message_GET_EEPROM_STATUS : public Message
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
        
        Message_GET_EEPROM_STATUS() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  


    class Message_SET_SERIAL_NO : public Message
    {
        public:
                        
            
        struct Info
        { 
            char serial[7];        
            Info() {}
        };
        
        Info info;
        
        Message_SET_SERIAL_NO() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    };


    class Message_GET_SERIAL_NO : public Message
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
        
        Message_GET_SERIAL_NO() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };      

    
    class Message_ACC_GYRO_SETUP : public Message
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
        
        Message_ACC_GYRO_SETUP() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };  

    // format of the 7 remaining bytes (B = byte, N = nibble)
    //  [1          ][2         ][3         ][4         ][5             ][6             ][7             ]
    //   set channel  GD                      GI S GO     Voffsetcoarse    Vzerodac
    //  B1-N1 -> set: it is the set of all possible 0, 1, 2 configuration set. value 15 means: default value inside the micro
    //  B1-N0 -> channel: it is one of the six channels 0, 1, 2, 3, 4, 5.
    //  B2,B3 -> fine gain GD using little endian ordering.
    //  B4-N1 -> front end gain GI.
    //  B4-N0 -> bit 0x8 gives the sign S (0 is +1), bits 0x7 give the output gain GO
    //  Vout = ( ( (1-2*S)*Vin + Voffsetcoarse )*GI + Vzerodac )*GD*G0
    //  the values to use for GD, GI, S, GO, Voffsetcoarse, and Vzerodac are those found in the datasheet of 
    //  the programmable amplifier PGA308 by Texas Instruments.  
    
    struct PGA308cfg1
    { 
        std::uint16_t       GD;
        std::uint8_t        GI          : 4; 
        std::uint8_t        S           : 1;
        std::uint8_t        GO          : 3; 
        std::uint8_t        Voffsetcoarse;
        std::uint16_t       Vzerodac;
        PGA308cfg1() : GD(0), GI(0), S(0), GO(0), Voffsetcoarse(0), Vzerodac(0) {}
    };  

    class Message_STRAIN2_AMPLIFIER_RESET : public Message
    {
        public:
                         
        struct Info
        {
            std::uint8_t        set         : 4;
            std::uint8_t        channel     : 4;    // if 0xf we mean every channel            
            Info() : set(0), channel(0) {}
        };
        
        Info info;
        
        Message_STRAIN2_AMPLIFIER_RESET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none            
    };     
    
    class Message_STRAIN2_AMPLIFIER_CFG1_SET : public Message
    {
        public:
                         
        struct Info
        {
            std::uint8_t        set         : 4;
            std::uint8_t        channel     : 4;    // if 0xf we mean every channel            
            PGA308cfg1          cfg1;
            Info() : set(0), channel(0) {}
        };
        
        Info info;
        
        Message_STRAIN2_AMPLIFIER_CFG1_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none            
    }; 

    
    class Message_STRAIN2_AMPLIFIER_CFG1_GET : public Message
    {
        public:
                       

        struct Info
        { 
            std::uint8_t        set         : 4;
            std::uint8_t        channel     : 4;    // if 0xf we mean every channel     
            Info() : set(0), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        set         : 4;
            std::uint8_t        channel     : 4;            
            PGA308cfg1          cfg1;
            ReplyInfo() : set(0), channel(0) {}
        };
        
        Info info;
        
        
        Message_STRAIN2_AMPLIFIER_CFG1_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    }; 
    
    
    
    class Message_STRAIN2_AMPLIFIER_AUTOCALIB : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t        set         : 4;
            std::uint8_t        channel     : 4;    // if 0xf we mean every channel
            std::uint16_t       target;             // in range [0, 64k). half scale if 32k    
            std::uint16_t       tolerance;          // it must be abs(measure - target) < tolerance            
            Info() : set(0), channel(0xf), target(32*1024), tolerance(100) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        set         : 4;
            std::uint8_t        channel     : 4;            
            std::uint8_t        resultmask;     // in pos i-th the boolean result of channel i-th
            std::uint32_t       mae;            // the mean square error for the channel(s) after autocalib = SUM_ch( abs(meas_ch - target) ) / numchannels
            ReplyInfo() : set(0), channel(0xf), resultmask(0) {}
        };        
            
        
        
        Info info;
        
        Message_STRAIN2_AMPLIFIER_AUTOCALIB() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);          
    }; 
    
    enum class imuFusion { enabled = 1, none = 33 }; // later on we can add the types of fusion we want
    
    class Message_IMU_CONFIG_SET : public Message
    {
        public:
            
                                    
        struct Info
        {
            std::uint16_t sensormask;       // combination of ... 0x0001 << embot::app::canprotocol::analog::imuSensor values
            imuFusion fusion;                  // with an enum we can add later on as many options we want. 
            std::uint32_t ffu_ranges_measureunits;
            Info() : sensormask(0), fusion(imuFusion::none), ffu_ranges_measureunits(0) {}
            void enable(embot::app::canprotocol::analog::imuSensor s) 
            { 
                if(embot::app::canprotocol::analog::imuSensor::none != s)
                    embot::binary::bit::set(sensormask, static_cast<std::uint8_t>(s)); 
            }
            bool enabled(embot::app::canprotocol::analog::imuSensor s)
            {
                return embot::binary::bit::check(sensormask, static_cast<std::uint8_t>(s));
            }
        };
        
        Info info;
        
        Message_IMU_CONFIG_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 

    class Message_IMU_CONFIG_GET : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t nothing;           
            Info() : nothing(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint16_t sensormask;       // combination of ... 0x0001 << embot::app::canprotocol::analog::imuSensor values
            imuFusion fusion;                  // with an enum we can add later on as many options we want. 
            std::uint32_t ffu_ranges_measureunits;
            ReplyInfo() : sensormask(0), fusion(imuFusion::none), ffu_ranges_measureunits(0) {}        
        };        
        
        Info info;
        
        Message_IMU_CONFIG_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    class Message_IMU_TRANSMIT : public Message
    {
        public:
        
        // format is data[0] = millisec
                        
        struct Info
        {
            bool transmit;
            embot::common::relTime  txperiod;   // if 0, dont transmit. else use usec value.         
            Info() : transmit(false), txperiod(0) {}
        };
        
        Info info;
        
        Message_IMU_TRANSMIT() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
        
    };    
    
    
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
