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

#ifndef __EMBOT_PROT_CAN_MOTOR_PERIODIC_H_
#define __EMBOT_PROT_CAN_MOTOR_PERIODIC_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_prot_can.h"
#include "embot_prot_can_motor.h"

#include <cstring>


    
namespace embot::prot::can::motor::periodic {
  
    // the supported commands    
    enum class CMD : uint8_t { 
        none = 0xfe, 
        
        FOC = 0,
        STATUS = 3,
        PRINT = 6,
        DEBUG = 9,
        ADDITIONAL_STATUS = 12,
        EMSTO2FOC_DESIRED_CURRENT = 15,
        TARGETS = 15                        // alias of EMSTO2FOC_DESIRED_CURRENT
    };

    
    // some utilities
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);    
       
} // namespace embot::prot::can::motor::periodic {
       

  
  
namespace embot::prot::can::motor::periodic {
         
    // the management of commands
    
    struct Message_PRINT : public Message
    {
        struct Info
        { 
            char                        text[31];   // 30 chars + '0' : at most 5 can messages ('0' is not transmitted)
            std::uint8_t                canaddress;
            Info() { std::memset(text, 0, sizeof(text)); canaddress = 0; }
        };
        
        Info info;
        
        Message_PRINT() { framecounter = nframes = nchars = 0; }
            
        bool load(const Info& inf);
            
       // bool get(std::vector<embot::prot::can::Frame> &frames); 
            
        std::uint8_t numberofframes();
        bool get(embot::prot::can::Frame &outframe);  


        static constexpr uint8_t maxchars {30};         // at most 30 chars ...
        static constexpr uint8_t charsperframe {6};     // ... with 6 chars per frame ...
        static constexpr uint8_t maxframes {5};         // ... so at most 5 frames

        // the content of a single PRINT frame
        struct Fragment
        {
            uint8_t canaddress {0};         // the sender
            uint8_t textID {0};             // the id of the text, it is the same for all the frames of a text
            uint8_t index {0};              // the position of the frame inside the text, in range [0, maxframes)
            bool last {false};              // true if it is the last frame of the text
            uint8_t nchars {0};             // the number of chars in this frame, in range [0, charsperframe]
            char chars[charsperframe] {0};  // the chars (not null terminated)
            constexpr Fragment() = default;
        };

        struct Sender
        {   // the sender calls ::numberofframes() and then ::form() for every frameindex in [0, numberofframes()) 
            // w/ the same textID, and transmits every outframe in sequence. 
            // textID should change from one text to the next (e.g., 0, 1, 2, 3, 0, ...) so that the receiver can detect lost frames
            static uint8_t numberofframes(const Info &inf);
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, uint8_t frameindex, uint8_t textID, Address sender);
        };

        struct Receiver
        {   // the receiver calls ::parse() for every PRINT frame to get a Fragment and passes it to an Assembler, 
            // which returns true and fills the Info when the text is complete.
            // as the frames of different senders may interleave, use one Assembler for each sender address
            static bool parse(Fragment &fragment, const embot::prot::can::Frame &inframe);

            class Assembler
            {
            public:
                constexpr Assembler() = default;
                // it returns true when fragment completes a text and in such a case it fills info. 
                // a fragment out of sequence discards the text in progress
                bool add(const Fragment &fragment, Info &info);
                void reset();
            private:
                char buffer[maxchars] {0};
                uint8_t length {0};
                uint8_t expectedindex {0};
                uint8_t textID {0};
                uint8_t canaddress {0};
                bool active {false};
            };
        };

        private:
        
        std::uint8_t nframes;    
        std::uint8_t framecounter;    
        std::uint8_t nchars;           
        static std::uint8_t textIDmod4;  // 0, 1, 2, 3, 0, 1, 2, etc      
    }; 
    
    
//    template< typename T >
//    std::string int_to_hex( T i )
//    {
//      std::stringstream stream;
//      stream << "0x" 
//             << std::setfill ('0') << std::setw(sizeof(T)*2) 
//             << std::hex << i;
//      return stream.str();
//    }    
       
    struct Message_DEBUG : public Message
    {
        struct Info
        { 
            uint64_t                    payload {0};   // the full payload
            std::uint8_t                canaddress {0};
            constexpr Info() = default;
            constexpr Info(uint64_t p, uint8_t a) : payload(p), canaddress(a) {}
            std::string to_string() const
            {
                return std::string("sig<DEBUG = (") 
                                    + std::to_string(payload) 
//                                    + int_to_hex(payload) 
                                    + ")"
                                   + ">";               
            }
        };
        
        Info info {};
        
        Message_DEBUG() = default;
            
        bool load(const Info& inf);            
        bool get(embot::prot::can::Frame &outframe);  

        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info and transmits it (periodic messages have no destination)
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address sender);
        };

        struct Receiver
        {   // the receiver use ::parse() to get Info from the inframe. info.canaddress is filled w/ the address of the sender
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
        };
    }; 
    
    class Message_EMSTO2FOC_DESIRED_CURRENT : public Message
    {
        public:
            
        struct Info
        {   // it can be a target of current [mA], voltage [%] (expressed as +/-1% -> +/-320), velocity [icubdeg/ms] or position [icubdeg]
            enum class Type : uint8_t { RAW = 0, CURRENT = 1, VOLTAGE = 2, VELOCITY = 3, POSITION = 4 };
            static constexpr uint8_t numberoftargets {4};
            
            // the type of the targets depends on the control mode imposed to the boards by a previous SET_CONTROL_MODE.
            // the control modes which do not use a setpoint (Idle, ForceIdle, NotConfigured, HWfault, none) give Type::RAW
            static constexpr Type toType(embot::prot::can::motor::ControlMode cm)
            {
                Type t {Type::RAW};
                switch(cm)
                {
                    case embot::prot::can::motor::ControlMode::Current:         { t = Type::CURRENT; } break;
                    case embot::prot::can::motor::ControlMode::SpeedVoltage:    { t = Type::VELOCITY; } break;
                    case embot::prot::can::motor::ControlMode::OpenLoop:        { t = Type::VOLTAGE; } break;
                    case embot::prot::can::motor::ControlMode::Position:        { t = Type::POSITION; } break;
                    default:                                                    { t = Type::RAW; } break;
                }
                return t;
            }

            uint8_t canaddress {0};                 // address of the sender (typically the ems = 0). filled by load(const Frame&)
            int16_t target[numberoftargets] {0};    // target[i] is for the board w/ address i+1

            constexpr Info() = default;

            // explicit ctor w/ 0 raw values 
            constexpr explicit Info(uint8_t adr)
                : canaddress(adr), target{0, 0, 0, 0} {}
                    
            // explicit ctor w/ raw values (already in CAN units)
            constexpr Info(uint8_t adr, int16_t t0, int16_t t1, int16_t t2, int16_t t3)
                : canaddress(adr), target{t0, t1, t2, t3} {}

            float get(uint8_t i, Type t) const
            {
                return (i < numberoftargets) ? transform(target[i], t) : 0.0f;
            }

            // it sets target[i] from a value expressed in physical units: [A], [%], [deg/s], [deg] or raw
            void set(uint8_t i, float value, Type t)
            {
                if(i < numberoftargets)
                {
                    target[i] = untransform(value, t);
                }
            }

            // the inverse of transform(): from physical units to CAN units
            static constexpr int16_t untransform(float v, Type t)
            {
                int16_t r {0};
                switch(t)
                {
                    case Type::CURRENT:
                    {   // [A] -> [mA]
                        r = embot::prot::can::motor::Converter::to_can_current(v);
                    } break;
                    case Type::VOLTAGE:
                    {   // [-100%, +100%] -> [-32000, +32000]
                        r = embot::prot::can::motor::Converter::to_can_percentage(v);
                    } break;
                    case Type::VELOCITY:
                    {   // [deg/s] -> [icubdeg/ms]
                        r = embot::prot::can::motor::Converter::to_can_velocity(v);
                    } break;
                    case Type::POSITION:
                    {   // [deg] -> [icubdeg]
                        r = embot::prot::can::motor::Converter::to_can_position(v);
                    } break;
                    case Type::RAW:
                    default:
                    {   // clamp to avoid UB in the float -> int16_t conversion
                        r = static_cast<int16_t>(std::clamp(v, -32768.0f, 32767.0f));
                    } break;
                }
                return r;
            }

            static constexpr float transform(int16_t val, Type t)
            {
                
                float v = static_cast<float>(val);
                switch(t)
                {
                    default: 
                    {
                    } break;
                    case Type::VELOCITY:
                    {   // i want to have [deg/s]. i have [icubdeg/ms] ->
                        v = embot::prot::can::motor::Converter::to_degreepersecond(val);                       
                    } break;
                    case Type::CURRENT:
                    {   // i want to have [A]. i have [mA] ->
                        v = embot::prot::can::motor::Converter::to_ampere(val);                       
                    } break;
                    case Type::VOLTAGE:
                    {   // i want to have [-100%, +100%]. i have [-32000, +32000] -> 
                        v = embot::prot::can::motor::Converter::to_percentage(val);    
                    } break; 
                    case Type::POSITION:
                    {   // i want to have [deg]. i have [icubdeg] ->
                        v = embot::prot::can::motor::Converter::position_to_degree(val);;                      
                    } break;                     
                }
                return v;                
            } 

            std::string to_string() const
            {
                return std::string("sig<TARGETS = (") 
                                   + std::to_string(target[0]) + ", " + std::to_string(target[1]) + ", " 
                                   + std::to_string(target[2]) + ", " + std::to_string(target[3]) + ")"
                                   + ">";               
            }                
        };
        
        Info info {};
        
        Message_EMSTO2FOC_DESIRED_CURRENT() = default;

        // decoding: it parses a received frame and fills info
        bool load(const embot::prot::can::Frame &inframe);

        // encoding: it loads info and then get() forms the frame to transmit
        bool load(const Info& inf);
        bool get(embot::prot::can::Frame &outframe);

        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info and transmits it (periodic messages have no destination)
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address sender);
        };

        struct Receiver
        {   // the receiver use ::parse() to get Info from the inframe. info.canaddress is filled w/ the address of the sender
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
        };
    };

    // Message_EMSTO2FOC_DESIRED_CURRENT transmits in broadcast the targets for the boards w/ can address 1, 2, 3 and 4: 
    // target[i] is for the board w/ address i+1. the targets are all of the same type, which each board knows 
    // from the control mode imposed by a previous SET_CONTROL_MODE. hence a more expressive alias:
    using Message_TARGETS = Message_EMSTO2FOC_DESIRED_CURRENT;
    
    
    class Message_FOC : public Message
    {
        public:
            
        struct Info
        {
            uint8_t canaddress {0};
            
            embot::prot::can::motor::Current current {0};           // in [mA]
            embot::prot::can::motor::Velocity velocity {0};         // in [icubdeg/ms]
            embot::prot::can::motor::PositionFull position {0};     // in [icubdeg]
            
            constexpr Info() = default;
            // explicit ctor w/ values already in CAN units
            constexpr Info(uint8_t adr, embot::prot::can::motor::Current c, embot::prot::can::motor::Velocity v, embot::prot::can::motor::PositionFull p)
                : canaddress(adr), current(c), velocity(v), position(p) {}
            
            void setCurrent(float curr)
            {
                current = embot::prot::can::motor::Converter::to_can_current(curr); 
            }
            void setVelocity(float vel)
            {
                velocity = embot::prot::can::motor::Converter::to_can_velocity(vel); 
            }
            void setPosition(float pos)
            {
                position = embot::prot::can::motor::Converter::to_can_fullposition(pos);
            }

            // getters in physical units, useful after decoding
            float getCurrent() const    // [A]
            {
                return embot::prot::can::motor::Converter::to_ampere(current);
            }
            float getVelocity() const   // [deg/s]
            {
                return embot::prot::can::motor::Converter::to_degreepersecond(velocity);
            }
            float getPosition() const   // [deg]
            {
                return embot::prot::can::motor::Converter::fullposition_to_degree(position);
            }

            std::string to_string() const
            {
                return std::string("sig<FOC = (") 
                                    + "current [mA] = " + std::to_string(current) + ", " + 
                                    + "velocity [icubdeg/ms] = " + std::to_string(velocity) + ", " 
                                    + "position [icubdeg] = " + std::to_string(position) + ")"
                                    + ">";               
            }             
        };
        
        Info info {};
        
        Message_FOC() = default;

        // encoding
        bool load(const Info& inf);
        bool get(embot::prot::can::Frame &outframe);

        // decoding: it parses a received frame and fills info
        bool load(const embot::prot::can::Frame &inframe);

        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info and transmits it (periodic messages have no destination)
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address sender);
        };

        struct Receiver
        {   // the receiver use ::parse() to get Info from the inframe. info.canaddress is filled w/ the address of the sender
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
        };
    };

    
    class Message_STATUS : public Message
    {
        public:
                           
        struct Info
        {
            uint8_t canaddress {0};         
            uint8_t controlmode {0};        // use values in embot::prot::can::motor::ControlMode
            uint8_t quadencoderstate {0};   // treat the bit flags w/ suitable values. for board 2foc use embot::prot::can::motor::board::foc::QEstate
            int16_t pwmfeedback {0};        // [-100, +100] is mapped in [-32000, +32000]
            uint32_t motorfaultstate {0};   // treat the bit flags w/ suitable values. for board 2fos use embot::prot::can::motor::board::foc::MotorFaultState
            constexpr Info() = default;
            constexpr Info(uint8_t adr, uint8_t cm, uint8_t qes, int16_t pwm, uint32_t mfs)
                : canaddress(adr), controlmode(cm), quadencoderstate(qes), pwmfeedback(pwm), motorfaultstate(mfs) {}

            // getters in more usable form, useful after decoding
            embot::prot::can::motor::ControlMode getControlMode() const
            {
                return embot::prot::can::motor::Converter::toControlMode(controlmode);
            }
            float getPWM() const    // [-100%, +100%]
            {
                return embot::prot::can::motor::Converter::to_percentage(pwmfeedback);
            }

            std::string to_string() const
            {
                return std::string("sig<STATUS = (")
                    + "ctrlmode = " + embot::prot::can::motor::Converter::tostring(static_cast<embot::prot::can::motor::ControlMode>(controlmode)) + ", "  
                                    + "quadencoderstate [flags] = " + std::to_string(quadencoderstate) + ", " 
                                    + "pwmfeedback [-32000, +32000] = " + std::to_string(pwmfeedback) + ", " 
                                    + "motorfaultstate flags = " + std::to_string(motorfaultstate) + ")"
                                    + ">";               
            }             
        };
        
        Info info {};
        
        Message_STATUS() = default;

        // encoding
        bool load(const Info& inf);
        bool get(embot::prot::can::Frame &outframe);

        // decoding: it parses a received frame and fills info
        bool load(const embot::prot::can::Frame &inframe);

        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info and transmits it (periodic messages have no destination)
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address sender);
        };

        struct Receiver
        {   // the receiver use ::parse() to get Info from the inframe. info.canaddress is filled w/ the address of the sender
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
        };
    };

    class Message_ADDITIONAL_STATUS : public Message
    {
        public:
                     
        struct Info
        {
            uint8_t canaddress {0};
            uint8_t tbd[2] {0};
            int16_t temperature {0};
            constexpr Info() = default;
            constexpr Info(uint8_t adr, uint8_t tbd0, uint8_t tbd1, int16_t temp)
                : canaddress(adr), tbd{tbd0, tbd1}, temperature(temp) {}
            
            std::string to_string() const
            {
                return std::string("sig<ADDITIONAL_STATUS = (") 
                    + "temperature = " + std::to_string(temperature) + ")"
                                    + ">";               
            }             
        };
        
        Info info {}; 
        
        Message_ADDITIONAL_STATUS() = default;

        // encoding
        bool load(const Info& inf);
        bool get(embot::prot::can::Frame &outframe);

        // decoding: it parses a received frame and fills info
        bool load(const embot::prot::can::Frame &inframe);

        struct Sender
        {   // the sender use ::form() to prepare an outframe from an Info and transmits it (periodic messages have no destination)
            static bool form(embot::prot::can::Frame &outframe, const Info &inf, Address sender);
        };

        struct Receiver
        {   // the receiver use ::parse() to get Info from the inframe. info.canaddress is filled w/ the address of the sender
            static bool parse(Info &info, const embot::prot::can::Frame &inframe);
        };
    };
    
} // namespace embot::prot::can::motor::periodic {    





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
