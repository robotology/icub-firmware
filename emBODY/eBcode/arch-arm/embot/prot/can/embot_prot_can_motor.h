
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
#include <limits>
#include <type_traits>


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
    
    // marco.accame on 23 nov 26: 
    // note 666: 
    // the simulink model now uses ControlMode::SpeedVoltage (0x0A) as a velocity control mode that has a current stage after it
    // moreover, it uses for it a velocity PID labeled PIDtype::CURR
    // after PR #721 of icub-firmware however this role of velocity->current is taken by ControlMode::SpeedCurrent (0x0B) and PIDtype::VEL_CURR
    // so we should align.      
    enum class ControlMode : uint8_t { 
        // in here the ones used by modern MC boards 
        Idle = 0x00,            // the Idle control mode we use in simulink model
        Position = 0x01,
        ForceIdle = 0x09,       // assimilated to Idle control mode, possibly used as command and not to report a state
        Current = 0x06,         // the Current control mode we use in simulink model
        SpeedVoltage = 0x0A,    // the Velocity control mode we use in simulink mode (but see note 666) 
        SpeedCurrent = 0x0B,    // the velocity control mode of 2FOC whose velocity PID (PIDtype::VEL_CURR) outputs a current
        OpenLoop = 0x50,        // the Voltage control mode we use in simulink model
        NotConfigured = 0xB0,   // the NotConfigured control mode we use in simulink model
        HWfault = 0xA0,         // the HwFaultCM control mode we use in simulink model
        // in here the ones that are not used but that were somewhere defined over the time
        // omissis 
        // in here something else
        none = 254 
    };  
    constexpr uint8_t ControlModeNumberOf {9};    
    
    
    enum class PIDtype : uint8_t 
    { 
        CURR = 0,       // current -> voltage
        VEL = 1,        // velocity -> current on the MBD boards (amc, amcbldc, ...), velocity -> voltage on 2FOC (ControlMode::SpeedVoltage)
        POS = 2,        // position -> velocity
        VEL_CURR = 3,   // velocity -> current on 2FOC (ControlMode::SpeedCurrent) and not used on MBD boards
        NONE = 15 
    };    
    constexpr uint8_t PIDtypeNumberOf {4}; 
                    
        
    struct Converter
    {
        // 1 icub-deg -> 360 deg / pow(2, 16)
        static constexpr float icubdeg2deg = 0.0054931640625f;
        static constexpr float deg2icubdeg = 182.044444444444f;
        static constexpr float milli2amp = 0.001f; 
        static constexpr float amp2milli = 1000.0f; 
        static constexpr float vel2dps = 1000.0f * Converter::icubdeg2deg; 
        static constexpr float dps2vel = 0.001f * Converter::deg2icubdeg; 
        
        // they are kept for the code which uses them, but the implementation is now embot::prot::can::saturate<>() 
        template<typename T>
        static constexpr T saturate(float v, bool &ok)
        {
            return embot::prot::can::saturate<T>(v, ok);
        }
        
        template<typename T>
        static constexpr T saturate(float v)
        {
            return embot::prot::can::saturate<T>(v);
        }
        
               
        
        static constexpr uint8_t convert(ControlMode cm)
        {
            return embot::core::tointegral(cm);
        }
        
        static constexpr ControlMode convert(uint8_t v)
        {
            constexpr std::array<ControlMode, ControlModeNumberOf> values { 
                ControlMode::Idle, ControlMode::Position, ControlMode::ForceIdle, ControlMode::Current, 
                ControlMode::SpeedVoltage, ControlMode::SpeedCurrent, ControlMode::OpenLoop, ControlMode::NotConfigured, ControlMode::HWfault 
            };  
            
            // a plain loop instead of std::find_if(), which is constexpr only since C++20
            ControlMode r {ControlMode::none};            
            for(const auto &val : values)
            {
                if(convert(val) == v)
                {
                    r = val;
                    break;
                }
            }
            
            return r;                        
        }
        
        static constexpr ControlMode toControlMode(uint8_t v) { return convert(v); }     


        static std::string tostring(ControlMode cm)
        {
            constexpr std::array<std::pair<ControlMode, const char *>, ControlModeNumberOf> pairs { 
                std::pair(ControlMode::Idle, "Idle"), 
                std::pair(ControlMode::Position, "Position"),
                std::pair(ControlMode::ForceIdle, "ForceIdle"), 
                std::pair(ControlMode::Current, "Current"), 
                std::pair(ControlMode::SpeedVoltage, "SpeedVoltage"),  
                std::pair(ControlMode::SpeedCurrent, "SpeedCurrent"),  
                std::pair(ControlMode::OpenLoop, "OpenLoop"), 
                std::pair(ControlMode::NotConfigured, "NotConfigured"), 
                std::pair(ControlMode::HWfault, "HWfault")          
            };

            auto pp = std::find_if(pairs.begin(), pairs.end(), [&](const auto& p){ return p.first == cm; });            
            return (std::end(pairs) != pp) ? pp->second : "none";
        }           
        
        
        // current in CAN is in [mA]. it saturates at +/-32767 mA
        static constexpr Current to_can_current(const float amp)
        {
            return saturate<Current>(amp2milli*amp);
        } 
        
        static constexpr float to_ampere(const Current mil)
        {
            return static_cast<float>(mil) * milli2amp;
        }
        
        // position in CAN is in [icubdegree], where 1 icubdegree -> 360 deg / pow(2, 16)
        // note: there is no saturation. Position covers [-180, +180) deg, and a value out of it wraps around modulo 360 deg 
        // (e.g., +190 deg -> -170 deg), which is the expected behaviour for an angle. strictly speaking the conversion of an
        // out of range float into int16_t is undefined behaviour in C++, but on Cortex-M it gives exactly the wrap around,
        // as the compiler converts into int32_t first (VCVT) and then keeps the 16 less significant bits.
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
            return saturate<PositionFull>(deg*deg2icubdeg);
        } 
        
        static constexpr float fullposition_to_degree(const PositionFull idg)
        {
            return static_cast<float>(idg) * icubdeg2deg;
        }
        
        // velocity in CAN is [icubdegree/ms], normal unit is degree/sec or dps. it saturates at +/-32767 icubdegree/ms (about +/-180000 deg/s)
        static constexpr Velocity to_can_velocity(const float dps)
        {
            return saturate<Velocity>(dps*dps2vel);
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

        // it clamps to [-100%, +100%] -> [-32000, +32000]. the clamp is done on the float before the conversion,
        // so that a value out of range (e.g., 125%) cannot wrap around in int16_t. a NaN gives 0
        static constexpr PWM to_can_percentage(const float &p)
        {           
            constexpr float toPWM = 320.f;
            const float r = std::clamp(p*toPWM, -32000.0f, +32000.0f);
            return saturate<PWM>(r);            
        }
        
        static std::string to_string(const PIDtype &t)
        {
            static const char * typ[PIDtypeNumberOf] { "CURR", "VEL", "POS", "VEL_CURR" };
            size_t p = embot::core::tointegral(t);
            return (p<PIDtypeNumberOf) ? typ[p] : "NONE";
        }
    
    };      
    
    
#if 0
## The `embot::prot::can::motor::PID` explained
The `embot::prot::can::motor::PID` contains the PID gains (proportional, integral and derivative) transported by iCub CAN protocol
that uses a limited number of bytes.
The gains are expressed with `int16_t` values, so in range [-32K, +32K-1]. 
The protocol contains also a parameter `ks` in range [0, 15] that scales the three gains with a factor 
`pow(2.0f, -ks)`, so that:

- `ks` = 0 does not scale the three gains that assume only integer values in [-32768.0, +32767.0], 
- `ks` = 1 allows 0.5 granularity in range [-16384.0, +16383.5],
- `ks` = 2 allows 0.25 granularity in range [-8192.0, +8191.75],
- ...
- `ks` = 15 allows 0.000030517578125 granularity in range [-1.0, +0.999969482421875]

It is important also to tell the measurement units of the gains in case of position, velocity and current PID.
    
### Position PID

The position PID transforms position [deg] into velocity [deg/s], so the measurement unit of the proportional gain is [1/s].

```mermaid
flowchart LR
    id1((position)) --[deg]--> positionPID
    positionPID --[deg/s]--> id2((velocity))
```
The `PIDInfo`, uses [ 1/s] for its `kp`, so the transformation into a floating point gain requires only the `pow(2.0, -ks)`.


### Velocity PID

The velocity PID transforms velocity [deg/s] into current [A], so the measurement unit of the proportional gain is [A/(deg/s)].

```mermaid
flowchart LR
    id1((velocity)) --[deg/s]--> velocityPID
    velocityPID --[A]--> id2((current))
```
The `PIDInfo`, uses [milli-A / (deg/s)] for its `kp`, so the transformation into a floating point gain requires a further 0.001 conversion factor after the `pow(2.0, -ks)`.

### Current PID

The current PID transforms current [A] into voltage [V], so the measurement unit of the proportional gain is [V/A].

The `PIDInfo`, uses [V/A] for its `kp`, so the transformation into a floating point gain requires only the `pow(2.0, -ks)`.

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
            // the gains of PIDs which output a current are in [milli-A / ...]: correctionfactor() removes the milli
            // the multiplication order is (milli2ampere * scalefactor()) * v, as it has always been, so that VEL gives the very same floats
            return (1.0f / correctionfactor(type)) * scalefactor() * v;
        }
        
        // it converts a float into int16_t w/ truncation toward zero (as static_cast<> does) but also w/ saturation.
        // ok becomes false if the value is NaN or out of range [-32768, +32767]. it never sets ok to true.
        static constexpr int16_t toint16(float v, bool &ok)
        {
            return Converter::saturate<int16_t>(v, ok);
        }
        
        // the factor that the gains get beyond pow(2, ks): the velocity PID uses [milli-A / (deg/s)] in its int16_t gains
        static constexpr float correctionfactor(PIDtype t)
        {   // the velocity PIDs which output a current (VEL as used by the MBD boards, VEL_CURR) keep their int16_t gains in [milli-A / (deg/s)]
            constexpr float ampere2milli = 1000.0f;
            return ((PIDtype::VEL == t) || (PIDtype::VEL_CURR == t)) ? ampere2milli : 1.0f;
        }
        
        // it loads the gains w/ a given shift factor ks. 
        // the values are truncated toward zero exactly as before, but if any of them does not fit into int16_t 
        // it is saturated and the function returns false
        constexpr bool load(PIDtype t, float p, float i, float d, uint8_t shift)
        {
            type = t;
            ks = std::min(shift, maxshift);
            
            const float factor = inversescalefactor() * correctionfactor(type); // inversescalefactor() depends from ks, so call it after ks = ....
            bool ok {true};
            kp = toint16(p * factor, ok);
            ki = toint16(i * factor, ok);
            kd = toint16(d * factor, ok);
            return ok;
        } 
        
        // it loads the gains choosing the largest ks in [0, maxshift] for which all of them fit into int16_t, 
        // so that we have the best resolution. if they do not fit even w/ ks = 0 they are saturated and the function returns false.
        // note: as there is a single ks for the three gains, a gain much smaller than the others may become zero
        constexpr bool load(PIDtype t, float p, float i, float d)
        {
            auto absolute = [](float v) { return (v < 0.0f) ? -v : v; };
            const float maxgain = std::max({absolute(p), absolute(i), absolute(d)}) * correctionfactor(t);
            
            uint8_t shift {0};
            for(uint8_t s = maxshift; s > 0; s--)
            {
                if((maxgain * static_cast<float>(1 << s)) < 32768.0f)
                {
                    shift = s;
                    break;
                }
            }
            return load(t, p, i, d, shift);
        } 
        
        std::string to_string() const
        {
            return std::string("{") + Converter::to_string(type) + " (kp, ki, kd, ks) = (" + std::to_string(kp) + ", " + std::to_string(ki) + ", " + std::to_string(kd) + ", " + std::to_string(ks) + ")}"; 
        }
    };  
  

    struct PIDlimits
    {
        PIDtype type {PIDtype::NONE};           
        int16_t offset {0}; 
        int16_t limitonoutput{0}; 
        int16_t limitonintegral {0}; 
        
        constexpr PIDlimits() = default;
        // attention: the ctor (and load()) takes (offset, limitonintegral, limitonoutput), 
        // whilst the members and the CAN payload have the order (offset, limitonoutput, limitonintegral)
        constexpr PIDlimits(PIDtype t, int16_t o, int16_t li, int16_t lo) : type(t), offset(o), limitonoutput(lo), limitonintegral(li) {}
        
        void load(PIDtype t, int16_t o, int16_t li, int16_t lo)
        {
            type = t; limitonintegral = li; limitonoutput = lo; offset = o; 
        }
        // note:
        // so far, we dont use PIDlimits so we dont provide any getter /setter function 
        // the way its members are used may follow the same rules of PID, so we may need PID::ks to scale     

        std::string to_string() const
        {
            return std::string("{") + Converter::to_string(type) + " (offset, limitonoutput, limitonintegral) = (" + std::to_string(offset) + ", " + std::to_string(limitonoutput) + ", " + std::to_string(limitonintegral) + ")}"; 
        }           
    };   


    struct CurrentLimits
    {   
        uint8_t x {0};
        embot::prot::can::motor::Current nominal {0};
        embot::prot::can::motor::Current peak {0};
        embot::prot::can::motor::Current overload {0};
        
        constexpr CurrentLimits() = default;
        constexpr CurrentLimits(uint8_t xx, embot::prot::can::motor::Current n, embot::prot::can::motor::Current p, embot::prot::can::motor::Current o) 
            : x(xx), nominal(n), peak(p), overload(o) {}
        
        // it loads the currents expressed in [A]. if any of them does not fit into [mA] as int16_t it is saturated and the function returns false
        constexpr bool load(float n, float p, float o)
        {
            constexpr float ampere2milli = 1000.0f;
            bool ok {true};
            nominal = Converter::saturate<embot::prot::can::motor::Current>(ampere2milli * n, ok);
            peak = Converter::saturate<embot::prot::can::motor::Current>(ampere2milli * p, ok);
            overload = Converter::saturate<embot::prot::can::motor::Current>(ampere2milli * o, ok);
            return ok;
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

        constexpr MotorConfig() = default; 
        constexpr MotorConfig(uint8_t f, int16_t resolution, int16_t indexoffset, uint8_t poles, uint8_t tolerance) 
            : flags(f), rotorEncoderResolution(resolution), rotorIndexOffset(indexoffset), motorPoles(poles), rotEncTolerance(tolerance) {}

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
    
    
    namespace pid {
        
        // this namespace manages the types for the SET_PID / GET_PID in floating point 
        
        enum class Type : uint8_t
        {
            CURR = embot::core::tointegral(PIDtype::CURR),          // 0 
            VEL = embot::core::tointegral(PIDtype::VEL),            // 1
            POS = embot::core::tointegral(PIDtype::POS),            // 2
            VEL_CURR = embot::core::tointegral(PIDtype::VEL_CURR),  // 3
            NONE = embot::core::tointegral(PIDtype::NONE),          // 15            
        };
        constexpr size_t TypeNumberOf {4};
        
        
        enum class Param : uint8_t 
        { 
            LOCK = 0, UNLOCK = 1,             
            KP = 2, KI = 3, KD = 4, 
            LIMINTEGRAL = 5, LIMOUTPUT = 6,
            KFF = 7, 

            NONE = 15 
        };
        constexpr size_t ParamNumberOf {8};


        struct Converter
        {
            static std::string to_string(const Type &t)
            {
                static const char * typ[TypeNumberOf] { "CURR", "VEL", "POS", "VEL_CURR" };
                size_t i = embot::core::tointegral(t);
                return (i<TypeNumberOf) ? typ[i] : "NONE";
            }
            
            static std::string to_string(const Param &p)
            {
                static const char * par[ParamNumberOf] { "LOCK", "UNLOCK", "KP", "KI", "KD", "LIMINTEGRAL", "LIMOUTPUT", "KFF" };
                size_t i = embot::core::tointegral(p);
                return (i<ParamNumberOf) ? par[i] : "NONE";
            }
        };
        
        struct Descriptor
        {
            // this struct holds the description of the payload of the SET_PID command (and GET_PID replay)
            // the payload is 7 bytes [T|P|x|f32t], where T is 1, P is 1, x is 1 and f32t is 4 
            constexpr Descriptor() = default;
            
            Descriptor(Type t, Param p, float v)
            {
                load(t, p, v);
            }
            
            void clear()
            {
                load(Type::NONE, Param::NONE, 0.0);
            }
                
            void load(Type t, Param p, float v)
            { 
                type = t; param = p; value = v;
                data[0] = embot::core::tointegral(type);
                data[1] = embot::core::tointegral(param);
                data[2] = 0;
                memmove(&data[3], &value, sizeof(float));
            }
            
            // it loads the bytes of payload[1:7] of a received can frame
            void load(void *stream)
            {
                if(nullptr == stream)
                {
                    return;
                }
                memmove(data, stream, sizeof(data));
                type = static_cast<Type>(data[0]);
                param = static_cast<Param>(data[1]);
                ffu = data[2];
                memmove(&value, &data[3], sizeof(float));
            }   

            Type getType() const
            {
                return type;
            }            

            Param getParam() const
            {
                return param;
            } 

            float getValue() const
            {
                return value;
            } 
            
            const uint8_t * getstream() const
            {
                return data;
            }

            constexpr size_t getsizeofstream() const 
            {
                return sizeof(data);
            }
            
            constexpr bool isvalid() const { return (Type::NONE != type) && (Param::NONE != param); }     

            std::string to_string() const
            {
                return std::string("{") + pid::Converter::to_string(type) + ", " + pid::Converter::to_string(param) + ", " + std::to_string(value) + "}"; 
            }  
        
        private:
            uint8_t data[7] = {embot::core::tointegral(Type::NONE), embot::core::tointegral(Param::NONE), 0, 0, 0, 0, 0}; 
            Type type {Type::NONE};  
            Param param {Param::NONE};
            uint8_t ffu {0};
            float value {0.0};             
        };
    
    } // namespace pid {
    
    
    namespace basic {
        
        using ID = uint8_t;
        constexpr ID IDnone {0xff};
        
        struct vTEMPLATE
        {
            uint8_t generic[6] {0};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr ID id {IDnone};
        };  static_assert(sizeof(vTEMPLATE) == 8, "size must be 8");

        template<typename T>    
        struct Data 
        {   // this template struct contains a T that models the 6 bytes of DATA of a SET / GET message
            // DATA must be 6 bytes accomodated inside a struct of size 8. 
            // T must have some properties:          
            static_assert(sizeof(T) == 8, "sizeof(T) must be 8"); 
            static_assert(T::size == 6, "T::size must be 6");         
            
            T v {};
                
            Data() = default;
            Data(const uint8_t* data) { load(data); }    
                                   
            // we load data into T
            void load(const uint8_t* data) { memmove(static_cast<void*>(&v), &data[0], v.size); }            
            // we get a mutable reference to T so that we can change it
            T& value() { return v; }    
            // and also a non mutable reference
            const T& value() const { return v; }                      
            // we get the serialization of T to be placed in a can frame
            const uint8_t * serialize() const { return reinterpret_cast<const uint8_t*>(&v); }

            std::string to_string() const 
            {
                return v.to_string();
            }
        };
                
        template<typename I>
        struct Descriptor
        { 
            // this struct holds the description of the information transported by the SET command (and the reply to GET)
            // it manages: ID + DATA[6], where the format of DATA[6] depends on ID
               
            // basic ctor 
            constexpr Descriptor() = default;
            
            constexpr Descriptor(I id, const uint8_t *data)
            {
                load(id, data);
            }
            
            // clear to default values
            void clear()
            {
                memset(id_data, 0, sizeof(id_data));
                id_data[0] = embot::core::tointegral(I::NONE);
            }
            
            // it loads the bytes of payload[1:7] of a received can frame
            bool load(const uint8_t *stream)
            {
                if((nullptr == stream) || (I::NONE == static_cast<I>(stream[0])))
                {
                    return false;
                }
                memmove(id_data, stream, sizeof(id_data)); 
                return true;            
            }
            
            // fills the object w/ [id|data] so that we can later get the stream to fill a frame we wnat to transmit
            // data should be taken from .... Data::serialize()
            bool load(I id, const uint8_t *data)
            {
                if((I::NONE == id) || (nullptr == data)) 
                {
                    return false;
                }
                id_data[0] = embot::core::tointegral(id);
                memmove(&id_data[1], data, sizeof(id_data)-1);  
                return true;
            }
            
            // verify validity
            bool isvalid() const
            {
                return id_data[0] != embot::core::tointegral(I::NONE);
            }
            
            // retrieves the ID
            I getID() const
            {
                return static_cast<I>(id_data[0]);
            }
            
            // retrieves only the data (not the ID)
            const uint8_t * getdata() const
            {
                return &id_data[1];
            }        
            
            // retrieves all the seven bytes of the stream [id|data]
            const uint8_t * getstream() const
            {
                return id_data;
            }
                 
            constexpr size_t getsizeofstream() const 
            {
                return sizeof(id_data);
            }
            std::string to_string() const
            {
                char ss[40] = {0};
                snprintf(ss, sizeof(ss), "id = %02x, data = %02x %02x %02x %02x %02x %02x", id_data[0], id_data[1], id_data[2], id_data[3], id_data[4], id_data[5], id_data[6]);
                return std::string("{") + ss + "}"; 
            } 
            
        private:
            // id_data[0] contains the ID, id_data[1:6] contains up to 6 bytes of data
            uint8_t id_data[7] = {embot::core::tointegral(I::NONE), 0, 0, 0, 0, 0, 0};                                 
        }; 
    
        
    } // namespace generic {     
    
    
    namespace motorparam {
        
        // In here we define a Descriptor able to manipulate [ID|DATA] used in the SET / GET _MOTOR_PARAM messages
                
        enum class ID : uint8_t 
        {
            GENERIC = 0x00,
            BEMF = 0x01,
            HALL = 0x02,
            ELECTR_VMAX = 0x03,
            NONE = basic::IDnone
        };
        constexpr size_t IDnumberOf {4}; 
                                    
        constexpr ID toID(const uint8_t id)
        {
            return (id < IDnumberOf) ? static_cast<ID>(id) : ID::NONE;
        }   

        struct Converter
        {
            static std::string to_string(const ID &id)
            {
                static const char * ids[IDnumberOf] { "GENERIC", "BEMF", "HALL", "ELECTR_VMAX" };
                size_t i = embot::core::tointegral(id);
                if(i<IDnumberOf)    return std::string("ID::") + ids[i];
                else                return std::string("ID::NONE");
            }
        };        

        struct vGENERIC
        {
            uint8_t generic[6] {0};
            // must have
            uint16_t filler {0};
            
            std::string to_string() const
            {
                char ss[40] = {0};
                snprintf(ss, sizeof(ss), "%02x %02x %02x %02x %02x %02x", generic[0], generic[1], generic[2], generic[3], generic[4], generic[5]);                
                return std::string("[motorparam::ID::GENERIC |") + ss + "]";
            }
            
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::GENERIC};
        };  static_assert(sizeof(vGENERIC) == 8, "size must be 8");
        
        struct vBEMF
        {
            float kbemf {0.0};
            uint8_t ffu[2] {0};
            // must have
            uint16_t filler {0};

            std::string to_string() const
            {
                //return std::string("[") + Converter::to_string(id) + " | kbemf = " + std::to_string(kbemf) + "]";
                return std::string("[motorparam::ID::BEMF |") + " kbemf = " + std::to_string(kbemf) + "]";
            }
            
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::BEMF};            
        };  static_assert(sizeof(vBEMF) == 8, "size must be 8");
            
        struct vHALL
        {
            enum class SwapMode : uint8_t { none = 0, bc = 1 }; 
            static constexpr uint8_t numberof_swapmodes = 2;
            int16_t offset {0}; // in degrees. it is an int16_t as in the protocol document
            SwapMode swapmode {SwapMode::none};
            uint8_t ffu[3] {0};
            // must have
            uint16_t filler {0};

            std::string to_string() const
            {
                const char *sm[numberof_swapmodes] = {"none", "bc"};
                const char *s = (embot::core::tointegral(swapmode) < numberof_swapmodes) ? sm[embot::core::tointegral(swapmode)] : "unknown";
                return std::string("[motorparam::ID::HALL |") + " offset = " + std::to_string(offset) + " swapmode = SwapMode::" + s + "]";
            }
            
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::HALL};                        
        };  static_assert(sizeof(vHALL) == 8, "size must be 8");   
        
        struct vELECTR_VMAX
        {
            float vmax {0.0};
            uint8_t ffu[2] {0};
            // must have
            uint16_t filler {0};
            
            std::string to_string() const
            {
                //return std::string("[") + Converter::to_string(id) + " | kbemf = " + std::to_string(kbemf) + "]";
                return std::string("[motorparam::ID::ELECTR_VMAX |") + " vmax = " + std::to_string(vmax) + "]";
            }
            
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::ELECTR_VMAX};
        };  static_assert(sizeof(vELECTR_VMAX) == 8, "size must be 8");
                


        template<typename T>
        using Data = basic::Data<T>;
                
        using Descriptor = basic::Descriptor<motorparam::ID>;
    
        
    } // namespace motorparam {   


    namespace generic { 

        // In here we define a Descriptor able to manipulate generic [ID|DATA] used in the SET / GET messages
                
        enum class ID : uint8_t
        {
            GENERIC = 0x00,
            TargetPOS = 0x01,
            NONE = basic::IDnone
        };
        constexpr size_t IDnumberOf {2}; 
                                    
        constexpr ID toID(const uint8_t id)
        {
            return (id < IDnumberOf) ? static_cast<ID>(id) : ID::NONE;
        }   

        struct Converter
        {
            static std::string to_string(const ID &id)
            {
                static const char * ids[IDnumberOf] { "GENERIC", "TargetPOS" };
                size_t i = embot::core::tointegral(id);
                return (i<IDnumberOf) ? ids[i] : "NONE";
            }
        };        

        struct vGENERIC
        {
            uint8_t generic[6] {0};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr generic::ID id {generic::ID::GENERIC};
        };  static_assert(sizeof(vGENERIC) == 8, "size must be 8");
        
        struct vTargetPOS
        {
            float position {0.0};
            _Float16 withvelocity {};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr generic::ID id {generic::ID::TargetPOS};
        };  static_assert(sizeof(vTargetPOS) == 8, "size must be 8");
            
    

        template<typename T>
        using Data = basic::Data<T>;
                
        using Descriptor = basic::Descriptor<generic::ID>;        

    } // namespace generic {         

 
    
} // namespace embot::prot::can::motor {


          
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
