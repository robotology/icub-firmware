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

#ifndef __EMBOT_PROT_CAN_MOTOR_POLLING_H_
#define __EMBOT_PROT_CAN_MOTOR_POLLING_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_prot_can.h"
#include "embot_prot_can_motor.h"
#include <cstring>



namespace embot::prot::can::motor::polling {
    
    // the supported commands    
    enum class CMD : uint8_t { 
        none = 0xfe, 
     
        SET_MOTOR_PARAM = 1,
        GET_MOTOR_PARAM = 2,
        
        GET_CONTROL_MODE = 7,
        SET_CONTROL_MODE = 9,
        SET_BOARD_ID = 50, 
        SET_CURRENT_LIMIT = 72, 
        
        SET_POS_PID = 82,
        GET_POS_PID = 83,
        SET_POS_PIDLIMITS = 84,
        GET_POS_PIDLIMITS = 85,        
        
        GET_FIRMWARE_VERSION = 91,
        
        SET_CURRENT_PID = 101,  
        GET_CURRENT_PID = 102,
        SET_CURRENT_PIDLIMITS = 103,       
        GET_CURRENT_PIDLIMITS = 104, 
                   
        SET_VELOCITY_PID = 105,         
        GET_VELOCITY_PID = 106,
        SET_VELOCITY_PIDLIMITS = 107,   
        GET_VELOCITY_PIDLIMITS = 108,  
         
        SET_MOTOR_CONFIG = 119,
        SET_TEMPERATURE_LIMIT = 121,
        GET_TEMPERATURE_LIMIT = 122, 
        GET_MOTOR_CONFIG = 123,
        GET_CURRENT_LIMIT = 124,
        
        SET_PID = 125,
        GET_PID = 126      
    };
    
    // NOTES
    // - not all the commands are supported: only those required for management and those required for MC control of modern boards (foc, amcbldc, etc.)
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
    
} // namespace embot::prot::can::motor::polling {

namespace embot::prot::can::motor::polling {
   
    // types required only by motor::polling

    enum class MotIndex : uint8_t { one = 0, two = 1, three = 2, four = 3, none = 254 };
    MotIndex toMotIndex(uint8_t v);
    uint8_t convert(MotIndex mo);  
    std::string tostring(MotIndex mo);    
    
    
    // ControlMode was in here but was moved embot::prot::can::motor:: because it is used by both polling and periodic classes.
    // but we define it also in here to guarantee backwards compatibility w/ files that use the embot::prot::can::motor::polling namespace
    using ControlMode = embot::prot::can::motor::ControlMode;

} // namespace embot::prot::can::motor::polling {
          

namespace embot::prot::can::motor::polling {
    
    
//    struct InfoGET
//    { 
//        MotIndex motorindex {MotIndex::one};             
//        InfoGET() = default;
//    };
            
    // the management of commands
    
    struct Message_GET_FIRMWARE_VERSION : public embot::prot::can::shared::Message_GET_VERSION
    {
        Message_GET_FIRMWARE_VERSION() : 
            embot::prot::can::shared::Message_GET_VERSION(Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION)) {}
       
    }; 
    
    struct Message_SET_BOARD_ID : public embot::prot::can::shared::Message_SET_ID
    {
        Message_SET_BOARD_ID() : 
            embot::prot::can::shared::Message_SET_ID(Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::SET_BOARD_ID)) {}
       
    }; 
    
    
    struct Message_SET_CONTROL_MODE : public Message
    {                 
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::ControlMode controlmode {embot::prot::can::motor::ControlMode::Idle};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<CONTROL_MODE = ControlMode::") + Converter::tostring(controlmode) + ", MotIndex::" + tostring(motorindex) + ">";               
            }                        
        };
        
        Info info {};
        
        Message_SET_CONTROL_MODE() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
    
    struct Message_GET_CONTROL_MODE : public Message
    {                                    
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<CONTROL_MODE, MotIndex::") + tostring(motorindex) + ">";             
            }  
        };
        
        struct ReplyInfo
        {
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::ControlMode controlmode {embot::prot::can::motor::ControlMode::Idle};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<CONTROL_MODE = ControlMode::") + Converter::tostring(controlmode) + ", MotIndex::" + tostring(motorindex) + ">";               
            }              
        };        
        
        Info info {};
        
        Message_GET_CONTROL_MODE() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };        
    
    struct Message_SET_CURRENT_LIMIT : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            CurrentLimits currents {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<CURRENT_LIMIT = ") + currents.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            } 
        };
        
        Info info {};
        
        Message_SET_CURRENT_LIMIT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_CURRENT_LIMIT : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<CURRENT_LIMIT, MotIndex::") + tostring(motorindex) + ">";             
            } 
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            CurrentLimits currents {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<CURRENT_LIMIT = ") + currents.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                 
        };
        
        Info info {};
        
        Message_GET_CURRENT_LIMIT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };    


    struct Message_SET_POS_PID : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<POS_PID = ") + pid.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                 
        };
        
        Info info {};
        
        Message_SET_POS_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_POS_PID : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<POS_PID, MotIndex::") + tostring(motorindex) + ">";             
            }              
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<POS_PID = ") + pid.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_GET_POS_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    }; 


    struct Message_SET_VELOCITY_PID : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<VELOCITY_PID = ") + pid.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_SET_VELOCITY_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_VELOCITY_PID : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<VELOCITY_PID, MotIndex::") + tostring(motorindex) + ">";             
            } 
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<VELOCITY_PID = ") + pid.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_GET_VELOCITY_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };

    
    struct Message_SET_CURRENT_PID : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<CURRENT_PID = ") + pid.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                 
        };
        
        Info info {};
        
        Message_SET_CURRENT_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_CURRENT_PID : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<CURRENT_PID, MotIndex::") + tostring(motorindex) + ">";             
            }              
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<CURRENT_PID = ") + pid.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_GET_CURRENT_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    }; 
    
    
    struct Message_SET_MOTOR_CONFIG : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            MotorConfig config {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<MOTOR_CONFIG = ") + config.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_SET_MOTOR_CONFIG() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_MOTOR_CONFIG : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<MOTOR_CONFIG, MotIndex::") + tostring(motorindex) + ">";             
            }             
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            MotorConfig config {};           
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<MOTOR_CONFIG = ") + config.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                   
        };
        
        Info info {};
        
        Message_GET_MOTOR_CONFIG() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };
    
    
    
    struct Message_SET_TEMPERATURE_LIMIT : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            int16_t hardwarelimit {0};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<TEMPERATURE_LIMIT = ") + std::to_string(hardwarelimit) + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }               
        };
        
        Info info {};
        
        Message_SET_TEMPERATURE_LIMIT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_TEMPERATURE_LIMIT : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<TEMPERATURE_LIMIT, MotIndex::") + tostring(motorindex) + ">";             
            }           
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            int16_t hardwarelimit {0};           
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<TEMPERATURE_LIMIT = ") + std::to_string(hardwarelimit) + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }              
        };
        
        Info info {};
        
        Message_GET_TEMPERATURE_LIMIT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };   


    struct InfoPIDlimits
    { 
        MotIndex motorindex {MotIndex::one};
        embot::prot::can::motor::PIDlimits pidlimits {};            
        InfoPIDlimits() = default;
    };
    
    
    struct Message_SET_POS_PIDLIMITS : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<POS_PIDLIMITS = ") + "TBD" + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_SET_POS_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_POS_PIDLIMITS : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<POS_PIDLIMIT, MotIndex::") + tostring(motorindex) + ">";             
            } 
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("signal<POS_PIDLIMITS = ") + "TBD" + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }                 
        };
        
        Info info {};
        
        Message_GET_POS_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };     

    
    struct Message_SET_CURRENT_PIDLIMITS : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<CURRENT_PIDLIMITS = ") + "TBD" + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_SET_CURRENT_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_CURRENT_PIDLIMITS : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<CURRENT_PIDLIMIT, MotIndex::") + tostring(motorindex) + ">";             
            } 
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("reply<CURRENT_PIDLIMITS = ") + "TBD" + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_GET_CURRENT_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    }; 


    struct Message_SET_VELOCITY_PIDLIMITS : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            Info() = default;
            std::string to_string() const
            {
                return std::string("set<VELOCITY_PIDLIMITS = ") + "TBD" + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_SET_VELOCITY_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply();   // none        
    };
       

    struct Message_GET_VELOCITY_PIDLIMITS : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};             
            Info() = default;
            std::string to_string() const
            {
                return std::string("get<VELOCITY_PIDLIMIT, MotIndex::") + tostring(motorindex) + ">";             
            } 
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            ReplyInfo() = default;
            std::string to_string() const
            {
                return std::string("signal<VELOCITY_PIDLIMITS = ") + "TBD" + "[?]" + ", MotIndex::" + tostring(motorindex) + ">";               
            }                 
        };
        
        Info info {};
        
        Message_GET_VELOCITY_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };  

    
    struct Message_SET_PID : public Message
    {                     
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::pid::Descriptor descriptor {};            
            Info() = default;
            Info(const embot::prot::can::motor::pid::Descriptor &d, MotIndex m) : descriptor(d), motorindex(m) {}
            std::string to_string() const
            {
                return std::string("set<PID = ") + descriptor.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                 
        };
        
        Info info {};
        
        Message_SET_PID() = default;            
        bool load(const embot::prot::can::Frame &inframe);
            
            
        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info, transmits it and stop 
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address to, Address sender);
        };    

        struct Receiver
        {   // the reveiver use ::parse() to get Info from the inframe, use it to set the required value and stop
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);       
        }; 

        struct Examples
        {
            static void sender();
            static void receiver();
        };
    };
       

    struct Message_GET_PID : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};    
            embot::prot::can::motor::pid::Type type { embot::prot::can::motor::pid::Type::NONE }; 
            embot::prot::can::motor::pid::Param param { embot::prot::can::motor::pid::Param::NONE };  
            
            Info() = default;
            Info(embot::prot::can::motor::pid::Type t, embot::prot::can::motor::pid::Param p, MotIndex m = MotIndex::one) : type(t), param(p), motorindex(m) {}
            std::string to_string() const
            {
                return std::string("get<PID, MotIndex::") + tostring(motorindex) + ", Type::" + pid::Converter::to_string(type) +  ", Param::" + pid::Converter::to_string(param) + ">";             
            }              
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::pid::Descriptor descriptor {};            
            ReplyInfo() = default;
            ReplyInfo(const embot::prot::can::motor::pid::Descriptor &d, MotIndex m) : descriptor(d), motorindex(m) {}
            std::string to_string() const
            {
                return std::string("reply<PID = ") + descriptor.to_string() + ", MotIndex::" + tostring(motorindex) + ">";               
            }                  
        };
        
        Info info {};
        
        Message_GET_PID() = default;            
        bool load(const embot::prot::can::Frame &inframe);                      
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);  
                
        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info, transmits it and waits for a inframe, finally it gets the ReplyInfo w/ ::parse() 
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address to, Address sender);
            static bool parse(ReplyInfo &repinf, const embot::prot::can::Frame &inframe);
        };    

        struct Receiver
        {   // the reveiver use ::parse() to get Info from the inframe, fills the ReplyInfo, prepares an outframe w/ ::form() and transmits it back
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
            static bool form(embot::prot::can::Frame &outframe, const ReplyInfo &repinf, Address to, Address sender);            
        };  

        struct Examples
        {
            static void sender();
            static void receiver();
        };            
      
    }; 


    struct Message_SET_MOTOR_PARAM : public Message
    {                    
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::motorparam::Descriptor descriptor {};            
            Info() = default;
            Info(const embot::prot::can::motor::motorparam::Descriptor &d, MotIndex m) : descriptor(d), motorindex(m) {}
            std::string to_string() const
            {
                return std::string("set<MOTOR_PARAM, MotIndex::") + tostring(motorindex) + "> = (" + descriptor.to_string() +  ")";            
            }                 
        };
                
        Info info {};
       
        Message_SET_MOTOR_PARAM() = default;            
        bool load(const embot::prot::can::Frame &inframe);       


        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info, transmits it and stop 
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address to, Address sender);
        };    

        struct Receiver
        {   // the reveiver use ::parse() to get Info from the inframe, use it to set the required value and stop
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);       
        }; 

        struct Examples
        {
            static void sender();
            static void receiver();
        };            

    };
    
       

    struct Message_GET_MOTOR_PARAM : public Message
    {
        struct Info
        { 
            MotIndex motorindex {MotIndex::one}; 
            embot::prot::can::motor::motorparam::ID id;           
            Info() = default;
            Info(embot::prot::can::motor::motorparam::ID i, MotIndex m) : id(i), motorindex(m) {}
            std::string to_string() const
            {
                return std::string("get<MOTOR_PARAM, MotIndex::") + tostring(motorindex) + ", ID::" + embot::prot::can::motor::motorparam::Converter::to_string(id) +  ">";             
            }              
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::motorparam::Descriptor descriptor {};            
            ReplyInfo() = default;
            ReplyInfo(const embot::prot::can::motor::motorparam::Descriptor &d, MotIndex m) : descriptor(d), motorindex(m) {}
            std::string to_string() const
            {
                return std::string("reply<MOTOR_PARAM, MotIndex::") + tostring(motorindex) + "> = (" + descriptor.to_string() +  ")";               
            }                  
        };
        
        Info info {};
        
        Message_GET_MOTOR_PARAM() = default;            
        bool load(const embot::prot::can::Frame &inframe);            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);   
            

        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info, transmits it and waits for a inframe, finally it gets the ReplyInfo w/ ::parse() 
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address to, Address sender);
            static bool parse(ReplyInfo &repinf, const embot::prot::can::Frame &inframe);
        };    

        struct Receiver
        {   // the reveiver use ::parse() to get Info from the inframe, fills the ReplyInfo, prepares an outframe w/ ::form() and transmits it back
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
            static bool form(embot::prot::can::Frame &outframe, const ReplyInfo &repinf, Address to, Address sender);            
        };  

        struct Examples
        {
            static void sender();
            static void receiver();
        };              
    }; 
    
    
} // namespace embot::prot::can::motor::polling {
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
