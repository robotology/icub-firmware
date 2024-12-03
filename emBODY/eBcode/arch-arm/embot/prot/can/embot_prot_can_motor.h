
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

#include <algorithm>
#include <array>
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
    std::string tostring(const MotIndex mo);    
    
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
    constexpr uint8_t ControlModeNumberOf {8};    
    
    
    enum class PIDtype : uint8_t { CURR = 0, VEL = 1, NONE = 7 };
    constexpr uint8_t PIDtypeNumberOf {3}; 
    
    
        
    struct Converter
    {
        // 1 icub-deg -> 360 deg / pow(2, 16)
        static constexpr float icubdeg2deg = 0.0054931640625f;
        static constexpr float deg2icubdeg = 182.044444444444f;
        static constexpr float milli2amp = 0.001f; 
        static constexpr float amp2milli = 1000.0f; 
        static constexpr float vel2dps = 1000.0f * Converter::icubdeg2deg; 
        static constexpr float dps2vel = 0.001f * Converter::deg2icubdeg; 
        
               
        
        static constexpr uint8_t convert(ControlMode cm)
        {
            return embot::core::tointegral(cm);
        }
        
        static constexpr ControlMode convert(uint8_t v)
        {
            constexpr std::array<ControlMode, ControlModeNumberOf> values { 
                ControlMode::Idle, ControlMode::ForceIdle, ControlMode::Current, ControlMode::SpeedVoltage, 
                ControlMode::OpenLoop, ControlMode::NotConfigured, ControlMode::HWfault, ControlMode::none 
            };  
            
            ControlMode r {ControlMode::none};            
            if(std::end(values) != std::find_if(values.begin(), values.end(), [&](const auto& val){ return convert(val) == v; }))
            {
                r = static_cast<ControlMode>(v);
            }
            
            return r;                        
        }
        
        static constexpr ControlMode toControlMode(uint8_t v) { return convert(v); }     


        static std::string tostring(ControlMode cm)
        {
            constexpr std::array<std::pair<ControlMode, const char *>, ControlModeNumberOf> pairs { 
                std::pair(ControlMode::Idle, "Idle"), 
                std::pair(ControlMode::ForceIdle, "ForceIdle"), 
                std::pair(ControlMode::Current, "Current"), 
                std::pair(ControlMode::SpeedVoltage, "SpeedVoltage"),  
                std::pair(ControlMode::OpenLoop, "OpenLoop"), 
                std::pair(ControlMode::NotConfigured, "NotConfigured"), 
                std::pair(ControlMode::HWfault, "HWfault"), 
                std::pair(ControlMode::none, "none")                
            };

            auto pp = std::find_if(pairs.begin(), pairs.end(), [&](const auto& p){ return p.first == cm; });            
            return (std::end(pairs) != pp) ? pp->second : "none";
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
            constexpr float toperc = 1.0f/320.f;
            float r = static_cast<float>(cp) * toperc;
            return std::clamp(r, -100.0f, +100.0f);
        } 

        static constexpr PWM to_can_percentage(const float &p)
        {           
            constexpr float toPWM = 320.f;
            float r = p*toPWM;
            return std::clamp(static_cast<PWM>(r), static_cast<PWM>(-32000), static_cast<PWM>(+32000));            
        }
        
        static std::string to_string(const PIDtype &t)
        {
            static const char * typ[8] { "CURR", "VEL", "unknown", "unknown", "unknown", "unknown", "unknown", "NONE" };
            return typ[embot::core::tointegral(t)];
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
        enum class Gain : uint8_t { P = 0, I = 1, D = 2 };
        static constexpr uint8_t maxshift {15};
        
        PIDtype type {PIDtype::NONE};         
        int16_t kp {0}; // proportional gain
        int16_t ki {0}; // integral gain
        int16_t kd {0}; // derivative gain
        uint8_t ks {0}; // shift factor w/ values in range [0, 15]. it gives a scalefactor of pow(2, -ks) that we apply to kx
        
        constexpr PID() = default;
        constexpr PID(PIDtype t, int16_t p, int16_t i, int16_t d, uint8_t s) : type(t), kp(p), ki(i), kd(d), ks(s) {}
        
        constexpr bool isvalid() const { return ks<=maxshift; }
        
        constexpr float scalefactor() const 
        {   // pow(2, -ks) implemented as 1/(1<<ks) if ks is in range [0, 15], else very small, so  0.0
            // alternate implementation is: static_cast<float>(0x1 << (15-ks))/ (32.0f*1024.0f)
            float r = 0.0f; 
            if(isvalid()) { r = 1.0 / static_cast<float>(1 << ks);    }
            return r;
        }
        
        constexpr float inversescalefactor() const 
        {
            float r = 0.0f; 
            if(isvalid())
            { 
                r = static_cast<float>(1 << ks);    
            }
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
            return (PIDtype::VEL == type) ? milli2ampere*scalefactor()*v :  scalefactor()*v;
        }
        
        void load(PIDtype t, float p, float i, float d, uint8_t shift)
        {
            type = t;
            ks = std::min(shift, maxshift);
            
            constexpr float ampere2milli = 1000.0f;
            float correctionfactor = (PIDtype::VEL == type) ? ampere2milli : 1.0f;
            float invscalefactor = inversescalefactor(); // it depends from ks, so call it after ks = ....
            kp = static_cast<int16_t>(p * invscalefactor * correctionfactor);
            ki = static_cast<int16_t>(i * invscalefactor * correctionfactor);
            kd = static_cast<int16_t>(d * invscalefactor * correctionfactor);
        }  
        
        std::string to_string() const
        {
            return std::string("{") + Converter::to_string(type) + " (kp, ki, kp, ks) = (" + std::to_string(kp) + ", " + std::to_string(ki) + ", " + std::to_string(kd) + ", " + std::to_string(ks) + ")}"; 
        }
    };  
  

    struct PIDlimits
    {
        PIDtype type {PIDtype::NONE};           
        int16_t offset {0}; 
        int16_t limitonoutput{0}; // derivative gain
        int16_t limitonintegral {0}; // integral gain
        
        constexpr PIDlimits() = default;
        constexpr PIDlimits(PIDtype t, int16_t o, int16_t li, int16_t lo) : type(t), offset(o), limitonintegral(li), limitonoutput(lo) {}
        
        // note:
        // so far, we dont use PIDlimits so we dont provide any getter /setter function 
        // the way its members are used may follow the same rules of PID, so we may need PID::ks to scale     

        std::string to_string() const
        {
            return std::string("{") + Converter::to_string(type) + " (offset, limitonputput, limitonintegral) = (" + std::to_string(offset) + ", " + std::to_string(limitonoutput) + ", " + std::to_string(limitonintegral) + ")}"; 
        }           
    };   


    struct CurrentLimits
    {   
        uint8_t x {0};
        embot::prot::can::motor::Current nominal {0};
        embot::prot::can::motor::Current peak {0};
        embot::prot::can::motor::Current overload {0};
        
        void load(float n, float p, float o)
        {
            constexpr float ampere2milli = 1000.0f;
            nominal = ampere2milli * n;
            peak = ampere2milli * p;
            overload = ampere2milli * o;
        }
        
        std::string to_string() const
        {
            return std::string("{(nominal, peak, overload) = (") + std::to_string(nominal) + ", " + std::to_string(peak) + ", " + std::to_string(overload) + ") [mA]}"; 
        }
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

        std::string to_string() const
        {
            char fl[48] = {0};
            snprintf(fl, sizeof(fl), "enco=%d:hall=%d:tsen=%d:indx=%d:spen=%d:verb=%d", 
                                      check(Flag::hasRotorEncoder), check(Flag::hasHALLsensor), 
                                      check(Flag::hasTempSensor), check(Flag::hasRotorEncoderIndex), 
                                      check(Flag::hasSpeedEncoder), check(Flag::verbose));
            return std::string("{(flags, rotorEncoderResolution, rotorIndexOffset, motorPoles, rotEncTolerance) = (") +
                                  std::string(fl) + ", " + std::to_string(rotorEncoderResolution) + ", " + std::to_string(rotorIndexOffset) 
                                  + ", " + std::to_string(motorPoles) 
                                  + ", " + std::to_string(rotEncTolerance) 
                                  + ")}"; 
        }
        
    };    
    
} // namespace embot::prot::can::motor {


          
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
