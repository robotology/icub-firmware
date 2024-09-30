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
     
        GET_CONTROL_MODE = 7,
        SET_CONTROL_MODE = 9,
        SET_BOARD_ID = 50, 
        SET_CURRENT_LIMIT = 72, 
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
        GET_CURRENT_LIMIT = 124 
      
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
        };
        
        struct ReplyInfo
        {
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::ControlMode controlmode {embot::prot::can::motor::ControlMode::Idle};            
            ReplyInfo() = default;
        };        
        
        Info info {};
        
        Message_GET_CONTROL_MODE() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);            
    };      

    struct CurrentLimits
    {   
        uint8_t x {0};
        embot::prot::can::motor::Current nominal {0};
        embot::prot::can::motor::Current peak {0};
        embot::prot::can::motor::Current overload {0};
    };     
    
    struct Message_SET_CURRENT_LIMIT : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            CurrentLimits currents {};            
            Info() = default;
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            CurrentLimits currents {};            
            ReplyInfo() = default;
        };
        
        Info info {};
        
        Message_GET_CURRENT_LIMIT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };    

//#if 0
//## The `PIDInfo` explained
//The `PIDInfo` contains the PID gains (proportional, integral and derivative) transported by iCub CAN protocol
//that uses a limited number of bytes.
//The gains are expressed with `int16_t` values, so in range [-32K, +32K-1]. 
//The protocol contains also a parameter `ks` in range [0, 15] that scales the three gains with a factor 
//`pow(2.0f, -ks)`, so that:

//- `ks` = 0 does not scale the three gains that assume only integer values in [-32768.0, +32767.0], 
//- `ks` = 1 allows 0.5 granularity in range [-16384.0, +16383.5],
//- `ks` = 2 allows 0.25 granularity in range [-8192.0, +8191.75],
//- ...
//- `ks` = 15 allows 0.000030517578125 granularity in range [-1.0, +0.999969482421875]

//It is important also to tell the measurement units of the gains in case of velocity and current PID.

//### Velocity PID

//The velocity PID transforms velocity [deg/s] into current [A], so the measurement unit of the proportional gain is [A/(deg/s)].

//```mermaid
//flowchart LR
//    id1((velocity)) --[deg/s]--> velocityPID
//    velocityPID --[A]--> id2((current))
//```
//The `PIDInfo`, uses [milli-A / (deg/s)] for its `kd`, so the transformation into a floating point gain requires a further 0.001 conversion factor after the `pow(2.0, -ks)`.

//### Current PID

//The current PID transforms current [A] into voltage [V], so the measurement unit of the proportional gain is [V/A].

//The `PIDInfo`, uses [V/A] for its `kd`, so the transformation into a floating point gain requires only the `pow(2.0, -ks)`.

//```mermaid
//flowchart LR
//    id1((current)) --[A]--> currentPID
//    currentPID --[V]--> id2((voltage))
//```

//#endif
//    struct PIDInfo
//    {
//        enum class Type : uint8_t { CURR = 0, VEL = 1, NONE = 7 };
//        enum class Gain : uint8_t { P = 0, I = 1, D = 2 };
//        Type type {Type::NONE};         
//        int16_t kp {0}; // proportional gain
//        int16_t ki {0}; // integral gain
//        int16_t kd {0}; // derivative gain
//        uint8_t ks {0}; // shift factor w/ values in range [0, 15]. it gives a scalefactor of pow(2, -ks) that we apply to kx
//        constexpr bool isvalid() const { return ks<16; }
//        constexpr float scalefactor() const 
//        {   // pow(2, -ks) implemented as 1/(1<<ks) if ks is in range [0, 15], else very small, so  0.0
//            // alternate implementation is: static_cast<float>(0x1 << (15-ks))/ (32.0f*1024.0f)
//            float r = 0.0f; 
//            if(isvalid()) { r = 1.0 / static_cast<float>(1 << ks);    }
//            return r;
//        }
//        constexpr float get(Gain g) const 
//        {
//            float v {0};
//            switch(g)
//            {
//                case Gain::P: { v = static_cast<float>(kp); } break;
//                case Gain::I: { v = static_cast<float>(ki); } break;
//                case Gain::D: { v = static_cast<float>(kd); } break;
//                default: {} break;
//            };
//            constexpr float milli2ampere = 0.001f;
//            return (Type::VEL == type) ? milli2ampere*scalefactor()*v :  scalefactor()*v;
//        }
//    };

    struct Message_SET_CURRENT_PID : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            Info() = default;
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            ReplyInfo() = default;
        };
        
        Info info {};
        
        Message_GET_CURRENT_PID() = default;
            
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PID pid {};            
            ReplyInfo() = default;
        };
        
        Info info {};
        
        Message_GET_VELOCITY_PID() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };

    struct MotorConfig 
    {
        enum class Flag : uint8_t
        {
            hasRotorEncoder         = 0,
            hasHALLsensor           = 1,
            hasTempSensor           = 2,
            hasRotorEncoderIndex    = 3,
            hasSpeedEncoder         = 4,
            verbose                 = 5,
            ffu6                    = 6,
            ffu7                    = 7
        };
        
        uint8_t flags {0}; // use struct MotorConfig::Flag to manage it
        int16_t rotorEncoderResolution {0};
        int16_t rotorIndexOffset {0};
        uint8_t motorPoles {0};
        uint8_t rotEncTolerance {0};        

        MotorConfig() = default; 

        void set(Flag f)
        {
            embot::core::binary::bit::set(flags, embot::core::tointegral(f));
        }
        
        bool check(Flag f) const
        {
            return embot::core::binary::bit::check(flags, embot::core::tointegral(f));
        }        
    };

    struct Message_SET_MOTOR_CONFIG : public Message
    {                  
        struct Info
        { 
            MotIndex motorindex {MotIndex::one};
            MotorConfig config {};            
            Info() = default;
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            MotorConfig config {};           
            ReplyInfo() = default;
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            int16_t hardwarelimit {0};           
            ReplyInfo() = default;
        };
        
        Info info {};
        
        Message_GET_TEMPERATURE_LIMIT() = default;
            
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            ReplyInfo() = default;
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
        };
        
        struct ReplyInfo
        { 
            MotIndex motorindex {MotIndex::one};
            embot::prot::can::motor::PIDlimits pidlimits {};            
            ReplyInfo() = default;
        };
        
        Info info {};
        
        Message_GET_VELOCITY_PIDLIMITS() = default;
            
        bool load(const embot::prot::can::Frame &inframe);
            
        bool reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo);       
    };    
    
} // namespace embot::prot::can::motor::polling {
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
