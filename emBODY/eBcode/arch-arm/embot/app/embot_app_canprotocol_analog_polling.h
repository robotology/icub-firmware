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

#include "iCubCanProto_analogSensorMessages.h"


namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {
        
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        SET_MATRIX_RC = ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_RC,                 // used by canloader to configure strain
        SET_CH_DAC = ICUBCANPROTO_POL_AS_CMD__SET_CH_DAC,                       // used by canloader to configure strain. in strain2 it manages beta (see AMPLIFIER_GAINOFFSET_SET)
        SET_TXMODE = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE,                       // used to start tx of data for mtb or strain
        SET_CANDATARATE = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE,             // used to configure strain or mais tx rate
        SAVE2EE = ICUBCANPROTO_POL_AS_CMD__SAVE2EE,                             // used by canloader to configure strain
        GET_MATRIX_RC = ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC,                 // used by canloader to configure strain
        GET_CH_DAC = ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC,                       // used by canloader to configure strain. in strain2 it manages offset (see AMPLIFIER_GAINOFFSET_GET)
        GET_CH_ADC = ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC,                       // used by canloader to configure strain. 
                      
        SET_MATRIX_G = ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_G,                   // used by canloader to configure strain
        GET_MATRIX_G = ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G,                   // used by canloader to configure strain                                                                                                      
        SET_CALIB_TARE = ICUBCANPROTO_POL_AS_CMD__SET_CALIB_TARE,               // used by canloader to configure strain
        GET_CALIB_TARE = ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE,               // used by canloader to configure strain 
        SET_CURR_TARE = ICUBCANPROTO_POL_AS_CMD__SET_CURR_TARE,                 // used by canloader to configure strain
        GET_CURR_TARE = ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE,                 // used by canloader to configure strain           
        SET_FULL_SCALES = ICUBCANPROTO_POL_AS_CMD__SET_FULL_SCALES,             // used by canloader to configure strain  
        GET_FULL_SCALES = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES,             // used by reader to properly scaled data received by strain 
        SET_SERIAL_NO = ICUBCANPROTO_POL_AS_CMD__SET_SERIAL_NO,                 // used by canloader to configure strain
        GET_SERIAL_NO = ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO,                 // used by canloader for strain
        GET_EEPROM_STATUS = ICUBCANPROTO_POL_AS_CMD__GET_EEPROM_STATUS,         // used by canloader to configure strain
        GET_FIRMWARE_VERSION = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION,         // basic management.        

        // NEW messages used for a generic AMPLIFIER with a linear transfer function: Vout = gain * Vin + offset. range of Vout is [0, 64k) 
        // these messages are used by strain2 (but not by strain). 
        AMPLIFIER_RESET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_RESET,                         // reset the amplifier (transfer function + others) to default factory values. 
        AMPLIFIER_RANGE_OF_GAIN_GET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_RANGE_OF_GAIN_GET,             // retrieve the allowed limits of the gain 
        AMPLIFIER_RANGE_OF_OFFSET_GET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_RANGE_OF_OFFSET_GET,           // retrieve the allowed limits of the offset        
        AMPLIFIER_GAINOFFSET_GET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_GAINOFFSET_GET,                // get of both gain and offset.   
        AMPLIFIER_GAINOFFSET_SET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_GAINOFFSET_SET,                // set of both gain and offset.  we cannot set them one by one because in PGA308 the offset depends on the gain 
        AMPLIFIER_OFFSET_AUTOCALIB = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_OFFSET_AUTOCALIB,              // it imposes the value of offset (but not of gain) which produces Vout = Vtarget.

        // RESERVED: { 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 } for possible new generic commands for the amplifier
        
        // NEW messages used for managing a specific AMPLIFIER. This is the PGA308 used by the strain2 
        AMPLIFIER_PGA308_CFG1_GET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_PGA308_CFG1_GET,               // of registers of the pg3308 registers managing the amplifier transfer function (gains + offsets).    
        AMPLIFIER_PGA308_CFG1_SET = ICUBCANPROTO_POL_AS_CMD__AMPLIFIER_PGA308_CFG1_SET,               // of registers of the pg3308 registers managing the amplifier transfer function (gains + offsets). 

        // RESERVED: {0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31} for possible new commands specific for extra config of PGA308 or for any new amplifier


        // basic management for all analog-sensor boards
        SET_BOARD_ADX = ICUBCANPROTO_POL_AS_CMD__SET_BOARD_ADX,                      
        
        
        // NEW messages used for IMU and THERMOMETER sensors (strain2 + mtb4)
        IMU_CONFIG_GET = ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_GET,
        IMU_CONFIG_SET = ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_SET,
        IMU_TRANSMIT = ICUBCANPROTO_POL_AS_CMD__IMU_TRANSMIT,
        // RESERVED: { 0x36, 0x37 } for possible new IMU commands
        
        // NEW messages used for THERMOMETER sensors (strain2 + mtb4)
        THERMOMETER_CONFIG_GET = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_CONFIG_GET,
        THERMOMETER_CONFIG_SET = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_CONFIG_SET,
        THERMOMETER_TRANSMIT = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_TRANSMIT,        
        // RESERVED: { 0x3B, 0x3C } for possible new THERMOMETER commands
                
        // NEW messages for strain2 to configure the regulation set to use
        REGULATIONSET_SET = ICUBCANPROTO_POL_AS_CMD__REGULATIONSET_SET,
        REGULATIONSET_GET = ICUBCANPROTO_POL_AS_CMD__REGULATIONSET_GET,
        
        // HOLE: [0x3F, ... , 0x4B]. there are 13 free values ...
        
        // skin messages + legacy acc-gyro messages
        SKIN_OBSOLETE_TACT_SETUP = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP,                     // 0x4C obsolete, but we support it in a basic form
        SKIN_SET_BRD_CFG = ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG,                            // 0x4D used to configure the skin data in mtb + its tx rate
        ACC_GYRO_SETUP = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP,                           // 0x4F used to configure the inertial data in mtb + its tx rate
        SKIN_SET_TRIANG_CFG = ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG,                      // 0x50 used to configure the skin data in mtb

        POS_CONFIG_GET = 0x51,
        POS_CONFIG_SET = 0x52,
        POS_TRANSMIT = 0x53,
        
        // HOLE: [0x54, ... , 0xFD]. there are 170 free values.
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
   
    // some required types
    
    // we keep theoneinuse equal to zero so that we guarantee normal behaviour is case regulationset is zero in can message 
    enum class StrainRegulationSet { theoneinuse = 0, first = 1, second = 2, third = 3 };
    
    enum class StrainRegulationSetMode { temporary = 0,  permanent = 1 };
    
    enum class StrainChannel { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, all = 0xf };
    
    enum class imuFusion { enabled = 1, none = 33 }; // later on we can add the types of fusion we want

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
            
        enum class SkinType { withTemperatureCompensation = 0, palmFingerTip = 1, withoutTempCompensation = 2, testmodeRAW = 7, withTemperatureCompensationV2 = 8, none = 254 };
                        
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
            std::uint8_t    regulationset;  // use StrainRegulationSet
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all
            std::uint16_t   fullscale;            
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), fullscale(0) {}
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
            std::uint8_t    regulationset;  // use StrainRegulationSet
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all           
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t    regulationset;  // use StrainRegulationSet
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all
            std::uint16_t   fullscale;
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), fullscale(0) {}          
        };        
        
        Info info;
        
        Message_GET_FULL_SCALES() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
       
    class Message_GET_CH_DAC : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t    regulationset;  // use StrainRegulationSet
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::al          
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t    regulationset;  // use StrainRegulationSet
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all
            std::uint16_t   offset;
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), offset(0) {}          
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
            std::uint8_t    regulationset;  // use StrainRegulationSet
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all
            std::uint16_t   offset;         
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), offset(0) {}
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
            std::uint8_t        regulationset;  // use StrainRegulationSet
            std::uint8_t        row;            // only 0, 1, 2, 3, 4, 5
            std::uint8_t        col;            // only 0, 1, 2, 3, 4, 5
            std::uint16_t       value;         
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), row(0), col(0), value(0) {}
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
            std::uint8_t        regulationset;  // can be only StrainRegulationSet::theoneinuse (but is not in the can frame)          
            std::uint8_t        gain;       
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), gain(0) {}
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
            std::uint8_t        regulationset;  // use StrainRegulationSet
            std::uint8_t        channel;        // use StrainChannel but not StrainChannel::all
            std::uint16_t       value;          // 
            Info() : mode(Mode::everychannelreset), regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), value(0) {}
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
            std::uint8_t    regulationset;      // use StrainRegulationSet
            std::uint8_t    channel;            // use StrainChannel but not StrainChannel::all 
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset;  // use StrainRegulationSet
            std::uint8_t        channel;        // use StrainChannel but not StrainChannel::all
            std::uint16_t       value;
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), value(0) {}          
        };         
        
        Info info;
        
        Message_GET_CALIB_TARE() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);
            
    };         

    class Message_SET_CURR_TARE : public Message
    {
        public:
             
        enum class Mode { everychannelreset = 0, everychannelnegativeofforcetorque = 1, setchannelwithvalue = 2, unknown = 255 };        
            
        struct Info
        { 
            Mode                mode;
            std::uint8_t        channel;        // use StrainChannel but not StrainChannel::all
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
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all
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
            std::uint8_t    channel;        // use StrainChannel but not StrainChannel::all
            bool            getcalibrated;
            Info() : channel(0), getcalibrated(true) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        channel;    // use StrainChannel but not StrainChannel::all
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
            std::uint8_t    regulationset;      // use StrainRegulationSet
            std::uint8_t    row;                // use 0, 1, 2, 3, 4, 5
            std::uint8_t    col;                // use 0, 1, 2, 3, 4, 5
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), row(0), col(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset;  // use StrainRegulationSet
            std::uint8_t        row;            // use 0, 1, 2, 3, 4, 5
            std::uint8_t        col;            // use 0, 1, 2, 3, 4, 5
            std::uint16_t       value;         
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), row(0), col(0), value(0) {}          
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
            std::uint8_t    regulationset;      // can be only StrainRegulationSet::theoneinuse (but is not in the can frame)
            std::uint8_t    nothing;   
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), nothing(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t    regulationset;      // can be only StrainRegulationSet::theoneinuse (but is not in the can frame)
            std::uint8_t    gain;       
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), gain(0) {}          
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

    class Message_AMPLIFIER_RESET : public Message
    {
        public:
                         
        struct Info
        {
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel (also StrainChannel::all can be used)            
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        Info info;
        
        Message_AMPLIFIER_RESET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none            
    };     
    
    class Message_AMPLIFIER_PGA308_CFG1_SET : public Message
    {
        public:
                         
        struct Info
        {
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // // use StrainChannel (also StrainChannel::all can be used)            
            PGA308cfg1          cfg1;
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        Info info;
        
        Message_AMPLIFIER_PGA308_CFG1_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none            
    }; 

    
    class Message_AMPLIFIER_PGA308_CFG1_GET : public Message
    {
        public:
                       

        struct Info
        { 
            std::uint8_t        regulationset   : 4;        // use StrainRegulationSet
            std::uint8_t        channel         : 4;        // use StrainChannel but not StrainChannel::all        
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset   : 4;        // use StrainRegulationSet
            std::uint8_t        channel         : 4;        // use StrainChannel but not StrainChannel::all           
            PGA308cfg1          cfg1;
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        Info info;
        
        
        Message_AMPLIFIER_PGA308_CFG1_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    }; 
    
    
    
    class Message_AMPLIFIER_OFFSET_AUTOCALIB : public Message
    {
        public:
            
        enum class Mode { oneshot = 0 };
            
        struct Info
        { 
            std::uint8_t        regulationset   : 4;    // use ONLY StrainRegulationSet::theoneinuse [because it starts a acqisition procedure] 
            std::uint8_t        channel         : 4;    // use StrainChannel (also StrainChannel::all can be used)
            Mode                mode;                   // it contains the way the autocalib behaves. so far it is only: oneshot
            std::uint16_t       target;                 // in range [0, 64k). half scale if 32k    
            std::uint16_t       tolerance;              // it must be abs(measure - target) < tolerance
            std::uint8_t        samples2average;        // it specifies how many adc sample to read for getting the average value. if 0 a default is used.            
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(static_cast<std::uint8_t>(StrainChannel::all)), mode(Mode::oneshot), target(32*1024), tolerance(100), samples2average(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset   : 4;    // use ONLY StrainRegulationSet::theoneinuse
            std::uint8_t        channel         : 4;    // use StrainChannel (also StrainChannel::all can be used)
            std::uint8_t        noisychannelmask;       // pos i-th is 1 if abs(maxADC-minADC) > tolerance amongst the samples2average acquisitions. bit 7 set to 1 if first measure, bit 6 if final measure           
            std::uint8_t        algorithmOKmask;        // in pos i-th the boolean result of channel i-th w/ respect to application of algorithm. if it fails, no changes in channel. if ok: changes are applied.          
            std::uint8_t        finalmeasureOKmask;     // in pos i-th the boolean result of channel i-th after autocalib w/ respect to abs(measure - target) < tolerance. 
            std::uint8_t        ffu;                    // for future use
            std::uint16_t       mae;                    // the mean square error for the channel(s) after autocalib = SUM_ch( abs(meas_ch - target) ) / numchannels. if > 64k we saturate.
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(static_cast<std::uint8_t>(StrainChannel::all)), noisychannelmask(0), algorithmOKmask(0), finalmeasureOKmask(0), ffu(0), mae(0) {}
        };        
            
        
        
        Info info;
        
        Message_AMPLIFIER_OFFSET_AUTOCALIB() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);          
    }; 
    
    
    class Message_AMPLIFIER_GAINOFFSET_SET : public Message
    {
        public:
                         
        struct Info
        {
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel (also StrainChannel::all can be used)            
            std::uint8_t        mode;                   // if 0: set values with .gain and .offset. if 1: use hw default values
            std::uint16_t       gain;                   // it is the gain, only positive, where each step is a gaintick of 0.01.
            std::uint16_t       offset;                 // it is teh offset and is in range [0, 64k). 
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), mode(0), gain(1600), offset(32768) {}
        };
        
        Info info;
        
        Message_AMPLIFIER_GAINOFFSET_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none            
    }; 

    
    class Message_AMPLIFIER_GAINOFFSET_GET : public Message
    {
        public:
                       
        struct Info
        { 
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel but not StrainChannel::all
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset   : 4;
            std::uint8_t        channel         : 4;    // use StrainChannel but not StrainChannel::all           
            std::uint16_t       gain;         
            std::uint16_t       offset;
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), gain(0), offset(0) {}
        };
        
        Info info;
        
        
        Message_AMPLIFIER_GAINOFFSET_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    class Message_REGULATIONSET_SET : public Message
    {
        public:
         
        // with this message we command to use a given regulationset (of values only ::one, ::two, ::three) for the specified mode
        // we can actually use also StrainRegulationSet::theoneinuse. in such a case the effect is to use the one in use as permanent.
        // to use it as temporary can be done but ther is no meaning in doing that.
        struct Info
        {   
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet but not StrainRegulationSet::theoneinuse
            std::uint8_t        mode            : 4;    // use StrainRegulationSetMode::temporary or StrainRegulationSetMode::permanent
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), mode(static_cast<std::uint8_t>(StrainRegulationSetMode::temporary)) {}
        };
        
        Info info;
        
        Message_REGULATIONSET_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none            
    }; 

    
    class Message_REGULATIONSET_GET : public Message
    {
        public:
                       
        struct Info
        {   // with this message we ask which value is the regulation set of kind temporary or permanent        
            std::uint8_t        mode;       // use StrainRegulationSetMode::temporary or StrainRegulationSetMode::permanent
            Info() : mode(static_cast<std::uint8_t>(StrainRegulationSetMode::temporary)) {}
        };
        
        struct ReplyInfo
        {   // with this replye we say the value of the regulation set of the kind asked in previous message (temporary or permanent)
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet but not StrainRegulationSet::theoneinuse
            std::uint8_t        mode            : 4;    // use StrainRegulationSetMode::temporary or StrainRegulationSetMode::permanent
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), mode(static_cast<std::uint8_t>(StrainRegulationSetMode::temporary)) {}
        };
        
        Info info;
        
        
        Message_REGULATIONSET_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    
    class Message_AMPLIFIER_RANGE_OF_GAIN_GET : public Message
    {
        public:
                       
        struct Info
        { 
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel but not StrainChannel::all        
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel but not StrainChannel::all
            std::uint16_t       highest;    // in ticks = 0.01            
            std::uint16_t       lowest;     // in ticks = 0.01         
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), highest(0), lowest(0) {}
        };
        
        Info info;
        
        
        Message_AMPLIFIER_RANGE_OF_GAIN_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };     

    
    class Message_AMPLIFIER_RANGE_OF_OFFSET_GET : public Message
    {
        public:
                       
        struct Info
        { 
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel but not StrainChannel::all       
            Info() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t        regulationset   : 4;    // use StrainRegulationSet
            std::uint8_t        channel         : 4;    // use StrainChannel but not StrainChannel::all
            std::uint16_t       highest;    // in value            
            std::uint16_t       lowest;     // in value         
            ReplyInfo() : regulationset(static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse)), channel(0), highest(0), lowest(0) {}
        };
        
        Info info;
        
        
        Message_AMPLIFIER_RANGE_OF_OFFSET_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };     

    
    
    
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
    


    class Message_THERMOMETER_CONFIG_SET : public Message
    {
        public:
            
                                    
        struct Info
        {
            std::uint8_t sensormask;       // none or at most two ... 
            Info() : sensormask(0){}
        };
        
        Info info;
        
        Message_THERMOMETER_CONFIG_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 

    class Message_THERMOMETER_CONFIG_GET : public Message
    {
        public:
                                    
        struct Info
        { 
            std::uint8_t nothing;           
            Info() : nothing(0) {}
        };
        
        struct ReplyInfo
        {
            std::uint8_t sensormask;       
            ReplyInfo() : sensormask(0) {}        
        };        
        
        Info info;
        
        Message_THERMOMETER_CONFIG_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    class Message_THERMOMETER_TRANSMIT : public Message
    {
        public:
        
        // format is data[0] = seconds                        
        struct Info
        {
            bool transmit;
            embot::common::relTime  txperiod;   // if 0, dont transmit. else use usec value.         
            Info() : transmit(false), txperiod(0) {}
        };
        
        Info info;
        
        Message_THERMOMETER_TRANSMIT() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none        
    };  


    
    struct deciDegPOSdescriptor
    {   // it assumes to have raw angle values in range [0, 360] expressed as int16_t in deci-degrees. it returns deci-degrees in open range positive and negative
        // this struct can be transmitted in 3 bytes. from lsb: [rotation: 2 bits, invertdirection: 1 bit, label : 4 bits], zero: two bytes in little endian
        // the bytes [0x01, 0x00, 0x00] tells ... no rotation, no inversion of direction, label 1, no zero. 
        enum class ROT : uint8_t { none = 0, plus180 = 1, plus090 = 2, minus090 = 3 };
        bool        enabled;            // it must be true if teh sensor is to be considered.      
        bool        invertdirection;
        ROT         rotation;   
        posLABEL    label;              // the label to use when transmitting in streaming the value of POS
        deciDeg     zero;               // the zero to be applied to the value
        deciDegPOSdescriptor() { reset(); }
        void reset() { enabled =  true; rotation = ROT::none; invertdirection = false;  label = posLABEL::zero; zero = 0; }
        void load(const uint8_t *bytes)
        { 
            enabled = embot::binary::bit::check(bytes[0], 7);
            invertdirection = embot::binary::bit::check(bytes[0], 6);
            rotation = static_cast<ROT>((bytes[0] >> 4) & 0b0011);           
            label = static_cast<posLABEL>(bytes[0] & 0b1111);
            zero = static_cast<uint16_t>(bytes[1]) | (static_cast<uint16_t>(bytes[2]) << 8);
        }
        void fill(uint8_t *bytes) const 
        {
            if(nullptr != bytes)
            {
                bytes[0] = 0;
                bytes[0] = ((enabled & 0b1) << 7) | ((invertdirection & 0b1) << 6) | ((static_cast<uint8_t>(rotation) & 0b11) << 4) | (static_cast<uint8_t>(label) & 0b1111);
                bytes[1] = zero & 0xff;
                bytes[2] = (zero >> 8);                
            }            
        }
        static const deciDeg rotationmap[4]; // = {0, 1800, 900, -900};
        deciDeg rotateclip(deciDeg v) const { return ( (v+rotationmap[static_cast<uint8_t>(rotation)]) % 3600 ); }
        deciDeg transform(const deciDeg raw) const
        {
            deciDeg z = rotateclip(raw) - rotateclip(zero);
            return (invertdirection) ? (-z) : (+z);
        }
        //uint8_t getlabel() const { return static_cast<uint8_t>(label); } 
    };    


    class Message_POS_CONFIG_SET : public Message
    {
        public:
            
        struct Info
        {   // for now it is dedicated to the case of the psc where we have two values to configure and they are expressed in deciDeg
            posTYPE                 type;    // it contains the type of the sensors. 
            deciDegPOSdescriptor    descriptor[2]; 
            Info() : type(posTYPE::angleDeciDeg) {}            
        };
            
        
        Info info;
        
        Message_POS_CONFIG_SET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none
            
    }; 

    class Message_POS_CONFIG_GET : public Message
    {
        public:
                                    
        struct Info
        {   // it is dedicated to the case of the psc where we have two values to configure and they are expressed in deciDeg
            posTYPE                 type;    // it holds a descriptor of ... presence maybe of it is for future choice of the content
            Info() : type(posTYPE::angleDeciDeg) {}            
        };
        
        struct ReplyInfo
        {   // it is dedicated to the case of the psc where we have two values to configure and they are expressed in deciDeg
            posTYPE                 type;    // it hold a descriptor of ... presence maybe of it is for future choice of the content
            deciDegPOSdescriptor    descriptor[2]; 
            ReplyInfo() : type(posTYPE::angleDeciDeg) {}            
        };
            
        
        Info info;
        
        Message_POS_CONFIG_GET() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };  
    
    class Message_POS_TRANSMIT : public Message
    {
        public:
        
        // format is data[0] = milliseconds                        
        struct Info
        {
            bool transmit;
            embot::common::relTime  txperiod;   // if 0, dont transmit. else use usec value.         
            Info() : transmit(false), txperiod(0) {}
        };
        
        Info info;
        
        Message_POS_TRANSMIT() {}
            
        bool load(const embot::hw::can::Frame &inframe);
            
        bool reply();   // none        
    };            
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
