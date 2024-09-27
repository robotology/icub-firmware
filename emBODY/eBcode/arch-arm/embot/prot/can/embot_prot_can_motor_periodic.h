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
    enum class CMD { 
        none = 0xfe, 
        
        FOC = 0,
        STATUS = 3,
        PRINT = 6,
        ADDITIONAL_STATUS = 12,
        EMSTO2FOC_DESIRED_CURRENT = 15        
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

        private:
        
        std::uint8_t nframes;    
        std::uint8_t framecounter;    
        std::uint8_t nchars;           
        static std::uint8_t textIDmod4;  // 0, 1, 2, 3, 0, 1, 2, etc      
    }; 
   
    
    class Message_EMSTO2FOC_DESIRED_CURRENT : public Message
    {
        public:
            
        struct Info
        {   // it can be a target of current [mA], voltage [%] (expressed as +/-1% -> +/-320), velocity [icubdeg/ms] or position [icubdeg]
            enum class Type : uint8_t { RAW = 0, CURRENT = 1, VOLTAGE = 2, VELOCITY = 3, POSITION = 4 };
            
            int16_t target[4] {0};   
            
            Info() = default;
            
            float get(uint8_t i, Type t)
            {
                return transform(target[i], t);               
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
        };
        
        Info info {};
        
        Message_EMSTO2FOC_DESIRED_CURRENT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);                        
    }; 
    
    
    class Message_FOC : public Message
    {
        public:
            
        struct Info
        {
            uint8_t canaddress {0};
            
            embot::prot::can::motor::Current current {0};           // in [mA]
            embot::prot::can::motor::Velocity velocity {0};         // in [icubdeg/ms]
            embot::prot::can::motor::PositionFull position {0};     // in [icubdeg]
            
            Info() = default;
            
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
        };
        
        Info info {};
        
        Message_FOC() = default;       
        bool load(const Info& inf);            
        bool get(embot::prot::can::Frame &outframe);    
    };        

    
    class Message_STATUS : public Message
    {
        public:
                           
        struct Info
        {
            uint8_t canaddress {0};         
            uint8_t controlmode {0};        // use values in embot::prot::can::motor::polling::ControlMode
            uint8_t quadencoderstate {0};   // treat the bit flags w/ suitable values. for board 2foc use QEstate2FOC
            int16_t pwmfeedback {0};        // [-100, +100] is mapped in [-32000, +32000]
            uint32_t motorfaultstate {0};   // treat the bit flags w/ suitable values. for board 2fos use MotorFaultState2FOC
            Info() = default;
        };
        
        Info info {};
        
        Message_STATUS() = default;       
        bool load(const Info& inf);            
        bool get(embot::prot::can::Frame &outframe);    
    };  

    class Message_ADDITIONAL_STATUS : public Message
    {
        public:
                     
        struct Info
        {
            uint8_t canaddress {0};
            uint8_t tbd[2] {0};
            int16_t temperature {0};
            Info() = default;
        };
        
        Info info {}; 
        
        Message_ADDITIONAL_STATUS() = default;       
        bool load(const Info& inf);            
        bool get(embot::prot::can::Frame &outframe);    
    };  
    
} // namespace embot::prot::can::motor::periodic {    





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
