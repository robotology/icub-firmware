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

#ifndef _EMBOT_PROT_CAN_MOTOR_H_
#define _EMBOT_PROT_CAN_MOTOR_H_

#include "embot_core.h"

#include "embot_prot_can.h"

#include <cstring>



namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
    
    // the supported commands    
    enum class CMD : uint8_t { 
        none = 0xfe, 
     
        GET_CONTROL_MODE = 0x07,
        SET_CONTROL_MODE = 0x09,
        SET_BOARD_ID = 50, GET_FIRMWARE_VERSION = 91 
    };
    
    // NOTES
    // - only a commands subset is supported. at date of 23 aug 17 they are those required by canloader for analog sensor boards
    // - commands ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO (12) and ICUBCANPROTO_POL_MC_CMD__SET_ADDITIONAL_INFO (13)
    //   are not used by canloder which uses those in the bootloader class. hence we dont support them in motor polling class. 
    // - command ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID (5) is supported but ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID (51) is not.
    //   note that in class analog polling we also support a SET_BOARD_ADX of the same value (0x32) only because there is a
    //   ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION. it would have been much better having a single command in class bootloader. 
    //   well: i added a canprotocol::bootloader::CMD::SETCANADDRESS as well.
    // - command ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION (91) is supported as well its (sic!) dual command in analog polling
    //   class ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION (0x1c = 28). even in this case a single command in class bootloader would have been
    //   a better choice. in this case i did not implement it.   
    
    // some utilities
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);    
    
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace polling {

namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
   
    // some required types

    enum class MotIndex : uint8_t { one = 0, two = 1, none = 254 };
    MotIndex toMotIndex(uint8_t v);
    uint8_t convert(MotIndex mo);  
    std::string tostring(MotIndex mo);    
    
    enum class ControlMode : uint8_t { Idle = 0x00, Current = 0x06, SpeedVoltage = 0x0A, OpenLoop = 0x50, none = 254 };         
    ControlMode toControlMode(uint8_t v);
    uint8_t convert(ControlMode cm); 
    std::string tostring(ControlMode cm);      

}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
          

namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
            
    // the management of commands
    
    class Message_GET_FIRMWARE_VERSION : public embot::prot::can::shared::Message_GET_VERSION
    {
        public:
            
        Message_GET_FIRMWARE_VERSION() : 
            embot::prot::can::shared::Message_GET_VERSION(Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION)) {}
       
    }; 
    
    class Message_SET_BOARD_ID : public embot::prot::can::shared::Message_SET_ID
    {
        public:
            
        Message_SET_BOARD_ID() : 
            embot::prot::can::shared::Message_SET_ID(Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::SET_BOARD_ID)) {}
       
    }; 
    
    
    class Message_SET_CONTROL_MODE : public Message
    {
        public:
                                   
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            ControlMode controlmode {ControlMode::Idle};            
            Info() = default;
        };
        
        Info info {};
        
        Message_SET_CONTROL_MODE() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
    
    class Message_GET_CONTROL_MODE : public Message
    {
        public:
                                    
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
        };
        
        struct ReplyInfo
        {
            MotIndex motorindex {MotIndex::one};
            ControlMode controlmode {ControlMode::Idle};            
            ReplyInfo() = default;
        };        
        
        Info info {};
        
        Message_GET_CONTROL_MODE() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };      

    
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
