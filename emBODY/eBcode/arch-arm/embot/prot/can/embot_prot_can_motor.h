
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_PROT_CAN_MOTOR_H_
#define __EMBOT_PROT_CAN_MOTOR_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_prot_can.h"

#include <cstring>


namespace embot::prot::can::motor {
    
    // what about MotIndex and ControlMode ?? i can add them in here and keep in polling.cpp the implementation just to avoid ....
    
    // current is transported in [mA]
    using Current = int16_t; 
    
    // voltage is transported in percentage w/ [-100, + 100] mapped in [-32000, + 32000]
    using Voltage = int16_t;
    using PWM = Voltage;
    
    // position is expressed in icubdegree in range [-180, + 180]
    using Position = int16_t;
    
    // position is expressed in icubdegree in full range.
    using PositionFull = int32_t;
    
    // velocity is expressed in icubdegree / ms 
    using Velocity = int16_t;
    
    
    enum class MotIndex : uint8_t { one = 0, two = 1, three = 2, four = 3, none = 254 };
    
    MotIndex toMotIndex(uint8_t v);
    uint8_t convert(MotIndex mo);  
    std::string tostring(MotIndex mo);    
    
    enum class ControlMode : uint8_t { 
        // in here the ones used by modern MC boards 
        Idle = 0x00,            // the Idle control mode we use in simulink model
        ForceIdle = 0x09,       // assimilated to Idle control mode, possibly used as command and not to report a state
        Current = 0x06,         // the Current control mode we use in simulink model
        SpeedVoltage = 0x0A,    // the Velocity control mode we use in simulink model
        OpenLoop = 0x50,        // the Voltage control mode we use in simulink model
        NotConfigured = 0xB0,   // the NotConfigured control mode we use in simulink model
        HWfault = 0xA0,         // the HwFaultCM control mode we use in simulink model
        // in here the ones that are not used but that were somewhere defined over the time
        // omissis 
        // in here something else
        none = 254 
    };     
    
        
    struct Converter
    {
        // 1 icub-deg -> 360 deg / pow(2, 16)
        static constexpr float icubdeg2deg = 0.0054931640625f;
        static constexpr float deg2icubdeg = 182.044444444444f;
        static constexpr float milli2amp = 0.001f; 
        static constexpr float amp2milli = 1000.0f; 
        static constexpr float vel2dps = 1000.0f * Converter::icubdeg2deg; 
        static constexpr float dps2vel = 0.001f * Converter::deg2icubdeg; 
        
        
        static constexpr ControlMode toControlMode(uint8_t v)
        {
            ControlMode cm {ControlMode::none};
            
            switch(v)
            {
                case 0x00:
                {
                    cm = ControlMode::Idle;
                } break;

                case 0x06:
                {
                    cm = ControlMode::Current;
                } break;
                
                case 0x09:
                {
                    cm = ControlMode::ForceIdle;
                } break;            

                case 0x0A:
                {
                    cm = ControlMode::SpeedVoltage;
                } break;
                  
                case 0x50:
                {
                    cm = ControlMode::OpenLoop;
                } break; 

                case 0xA0:
                {
                    cm = ControlMode::HWfault;
                } break; 

                case 0xB0:
                {
                    cm = ControlMode::NotConfigured;
                } break; 
                
                default:
                {
                    cm = ControlMode::none;
                } break;

            }  

            return cm;        
        }
        
        static constexpr uint8_t convert(ControlMode cm)
        {
            return embot::core::tointegral(cm);
        }
        
        static std::string tostring(ControlMode cm)
        {

            switch(cm)
            {
                case ControlMode::Idle:
                {
                    return "Idle";
                } break;

                case ControlMode::Current:
                {
                    return "Current";
                } break;

                case ControlMode::ForceIdle:
                {
                    return "ForceIdle";
                } break;
                
                case ControlMode::SpeedVoltage:
                {
                    return "SpeedVoltage";
                } break;

                case ControlMode::OpenLoop:
                {
                    return "OpenLoop";
                } break;

                case ControlMode::HWfault:
                {
                    return "HWfault";
                } break;

                case ControlMode::NotConfigured:
                {
                    return "NotConfigured";
                } break;
                
                default:
                {
                    return "none";
                } break;

            } 

            return "none";
        }          
        
        // current in CAN is in [mA]
        static constexpr Current to_can_current(const float amp)
        {
            return static_cast<Current>(amp2milli*amp);
        } 
        
        static constexpr float to_ampere(const Current mil)
        {
            return static_cast<float>(mil) * milli2amp;
        }
        
        // position in CAN is in [icubdegree], where 1 icubdegree -> 360 deg / pow(2, 16)
        static constexpr Position to_can_position(const float deg)
        {
            return static_cast<Position>(deg*deg2icubdeg);
        } 
        
        static constexpr float position_to_degree(const Position idg)
        {
            return static_cast<float>(idg) * icubdeg2deg;
        }        
 
        static constexpr PositionFull to_can_fullposition(const float deg)
        {
            return static_cast<PositionFull>(deg*deg2icubdeg);
        } 
        
        static constexpr float fullposition_to_degree(const PositionFull idg)
        {
            return static_cast<float>(idg) * icubdeg2deg;
        }
        
        // velocity in CAN is [icubdegree/ms], normal unit is degree/sec or dps
        static constexpr Velocity to_can_velocity(const float dps)
        {
            return static_cast<Velocity>(dps*dps2vel);
        }
        
        static constexpr float to_degreepersecond(const Velocity cv)
        { 
            return static_cast<float>(cv) * vel2dps;
        }
        
        static constexpr float to_percentage(const PWM &cp)
        {
            PWM v = std::clamp(v, static_cast<PWM>(-32000), static_cast<PWM>(+32000));
            constexpr float toperc = 1.0f/320.f;
            return static_cast<float>(v) * toperc;
        } 

        static constexpr PWM to_can_percentage(const float &p)
        {
            float v = std::clamp(p, static_cast<float>(-100.0f), static_cast<float>(+100.0f));
            constexpr float toPWM = 320.f;
            return static_cast<PWM>(v*toPWM);
        }
    
    };      
    
    
#if 0
## The `PIDInfo` explained
The `PIDInfo` contains the PID gains (proportional, integral and derivative) transported by iCub CAN protocol
that uses a limited number of bytes.
The gains are expressed with `int16_t` values, so in range [-32K, +32K-1]. 
The protocol contains also a parameter `ks` in range [0, 15] that scales the three gains with a factor 
`pow(2.0f, -ks)`, so that:

- `ks` = 0 does not scale the three gains that assume only integer values in [-32768.0, +32767.0], 
- `ks` = 1 allows 0.5 granularity in range [-16384.0, +16383.5],
- `ks` = 2 allows 0.25 granularity in range [-8192.0, +8191.75],
- ...
- `ks` = 15 allows 0.000030517578125 granularity in range [-1.0, +0.999969482421875]

It is important also to tell the measurement units of the gains in case of velocity and current PID.

### Velocity PID

The velocity PID transforms velocity [deg/s] into current [A], so the measurement unit of the proportional gain is [A/(deg/s)].

```mermaid
flowchart LR
    id1((velocity)) --[deg/s]--> velocityPID
    velocityPID --[A]--> id2((current))
```
The `PIDInfo`, uses [milli-A / (deg/s)] for its `kd`, so the transformation into a floating point gain requires a further 0.001 conversion factor after the `pow(2.0, -ks)`.

### Current PID

The current PID transforms current [A] into voltage [V], so the measurement unit of the proportional gain is [V/A].

The `PIDInfo`, uses [V/A] for its `kd`, so the transformation into a floating point gain requires only the `pow(2.0, -ks)`.

```mermaid
flowchart LR
    id1((current)) --[A]--> currentPID
    currentPID --[V]--> id2((voltage))
```

#endif

    struct PID
    {
        enum class Type : uint8_t { CURR = 0, VEL = 1, NONE = 7 };
        enum class Gain : uint8_t { P = 0, I = 1, D = 2 };
        
        Type type {Type::NONE};         
        int16_t kp {0}; // proportional gain
        int16_t ki {0}; // integral gain
        int16_t kd {0}; // derivative gain
        uint8_t ks {0}; // shift factor w/ values in range [0, 15]. it gives a scalefactor of pow(2, -ks) that we apply to kx
        
        constexpr PID() = default;
        constexpr PID(Type t, int16_t p, int16_t i, int16_t d, uint8_t s) : type(t), kp(p), ki(i), kd(d), ks(s) {}
        
        constexpr bool isvalid() const { return ks<16; }
        
        constexpr float scalefactor() const 
        {   // pow(2, -ks) implemented as 1/(1<<ks) if ks is in range [0, 15], else very small, so  0.0
            // alternate implementation is: static_cast<float>(0x1 << (15-ks))/ (32.0f*1024.0f)
            float r = 0.0f; 
            if(isvalid()) { r = 1.0 / static_cast<float>(1 << ks);    }
            return r;
        }
        
        constexpr float get(Gain g) const 
        {
            float v {0};
            switch(g)
            {
                case Gain::P: { v = static_cast<float>(kp); } break;
                case Gain::I: { v = static_cast<float>(ki); } break;
                case Gain::D: { v = static_cast<float>(kd); } break;
                default: {} break;
            };
            constexpr float milli2ampere = 0.001f;
            return (Type::VEL == type) ? milli2ampere*scalefactor()*v :  scalefactor()*v;
        }
    };  
  
  
    
} // namespace embot::prot::can::motor {


          
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
