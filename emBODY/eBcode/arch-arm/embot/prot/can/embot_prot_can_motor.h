
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
        Position = 0x01,
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
    
    
    enum class PIDtype : uint8_t 
    { 
        CURR = 0, VEL = 1, POS = 2, VEL_CURR = 3,
        NONE = 15 
    };    
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
                ControlMode::Idle, ControlMode::Position, ControlMode::ForceIdle, ControlMode::Current, 
                ControlMode::SpeedVoltage, ControlMode::OpenLoop, ControlMode::NotConfigured, ControlMode::HWfault 
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
                std::pair(ControlMode::Position, "Position"),
                std::pair(ControlMode::ForceIdle, "ForceIdle"), 
                std::pair(ControlMode::Current, "Current"), 
                std::pair(ControlMode::SpeedVoltage, "SpeedVoltage"),  
                std::pair(ControlMode::OpenLoop, "OpenLoop"), 
                std::pair(ControlMode::NotConfigured, "NotConfigured"), 
                std::pair(ControlMode::HWfault, "HWfault")          
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
            static const char * typ[PIDtypeNumberOf] { "CURR", "VEL", "POS" };
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
The `PIDInfo`, uses [ 1/s] for its `kd`, so the transformation into a floating point gain requires only the `pow(2.0, -ks)`.


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
        int16_t limitonoutput{0}; 
        int16_t limitonintegral {0}; 
        
        constexpr PIDlimits() = default;
        constexpr PIDlimits(PIDtype t, int16_t o, int16_t li, int16_t lo) : type(t), offset(o), limitonintegral(li), limitonoutput(lo) {}
        
        void load(PIDtype t, int16_t o, int16_t li, int16_t lo)
        {
            type = t; limitonintegral = li; limitonoutput = lo; offset = o; 
        }
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
    
    
    namespace pid {
        
        // this namespace manages the types for the SET_PID / GET_PID in floating point 
        
        enum class Type : uint8_t
        {
            CURR = embot::core::tointegral(PIDtype::CURR),    // 0 
            VEL = embot::core::tointegral(PIDtype::VEL),      // 1
            POS = embot::core::tointegral(PIDtype::POS),      // 2
            VEL_CURR = embot::core::tointegral(PIDtype::VEL_CURR), // 3
            NONE = embot::core::tointegral(PIDtype::NONE),    // 15            
        };
        constexpr size_t TypeNumberOf {3};
        
        
        enum class Param : uint8_t 
        { 
            LOCK = 0, UNLOCK = 1,             
            KP = 2, KI = 3, KD = 4, 
            LIMINTEGRAL = 5, LIMOUTPUT = 6,
            KFF = 7, 
            STICTION_UP = 8, STICTION_DOWN = 9, 

            NONE = 15 
        };
        constexpr size_t ParamNumberOf {10};    


        struct Converter
        {
            static std::string to_string(const Type &t)
            {
                static const char * typ[TypeNumberOf] { "CURR", "VEL", "POS" };
                size_t i = embot::core::tointegral(t);
                return (i<PIDtypeNumberOf) ? typ[i] : "NONE";
            }
            
            static std::string to_string(const Param &p)
            {
                static const char * par[ParamNumberOf] { "LOCK", "UNLOCK", "KP", "KI", "KD", "LIMINTEGRAL", "LIMOUTPUT", "KFF", "STICTION_UP", "STICTION_DOWN" };
                size_t i = embot::core::tointegral(p);
                return (i<ParamNumberOf) ? par[i] : "NONE";
            }
        };
        
        struct Descriptor
        {
            // this struct holds the description of the payload of the SET_PID command (and GET_PID replay)
            // the payload is 7 bytes [T|P|x|f32t], where T is 1, P is 1, x is 1 and f32t is 4 
            constexpr Descriptor() = default;
            
            constexpr Descriptor(Type t, Param p, float v)
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

            const Type getType() const
            {
                return type;
            }            

            const Param getParam() const
            {
                return param;
            } 

            const float getValue() const
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
                return std::string("{") + pid::Converter::to_string(type);  + ", " + pid::Converter::to_string(param) + ", " + std::to_string(value) + "}"; 
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
            void load(const uint8_t* data) { memmove(&v, &data[0], v.size); }            
            // we get a mutable reference to T so that we can change it
            T& value() { return v; }    
            // and also a non mutable reference
            const T& value() const { return v; }                      
            // we get the serialization of T to be placed in a can frame
            const uint8_t * serialize() const { return reinterpret_cast<const uint8_t*>(&v); }                         
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
            const I getID() const
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
                return std::string("{") + "tbd" + ", data = TBD" + "}"; 
            } 

            
        private:
            // id_data[0] contains the ID, id_data[1:6] contains up to 6 bytes of data
            uint8_t id_data[7] = {embot::core::tointegral(I::NONE), 0, 0, 0, 0, 0, 0};                                 
        }; 
    
        
    } // namespace generic {     
    
    
    namespace motorparam {
        
        // In here we define a Descriptor able to manipulate [ID|DATA] used in the SET / GET _MOTOR_PARAM messages
                
        enum class ID
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
                return (i<IDnumberOf) ? ids[i] : "NONE";
            }
        };        

        struct vGENERIC
        {
            uint8_t generic[6] {0};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::GENERIC};
        };  static_assert(sizeof(vGENERIC) == 8, "size must be 8");
        
        struct vBEMF
        {
            float kbemf {0.0};
            uint8_t ffu[2] {0};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::BEMF};
        };  static_assert(sizeof(vBEMF) == 8, "size must be 8");
            
        struct vHALL
        {
            enum class SwapMode : uint8_t { none = 0, bc = 1 };
            uint16_t offset {0}; // in degrees
            SwapMode swapmode {SwapMode::none};
            uint8_t ffu[3] {0};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::HALL};
        };  static_assert(sizeof(vHALL) == 8, "size must be 8");   
        
        struct vELECTR_VMAX
        {
            float vmax {0.0};
            uint8_t ffu[2] {0};
            // must have
            uint16_t filler {0};
            static constexpr size_t size {6};
            static constexpr motorparam::ID id {motorparam::ID::ELECTR_VMAX};
        };  static_assert(sizeof(vBEMF) == 8, "size must be 8");
                


        template<typename T>
        using Data = basic::Data<T>;
                
        using Descriptor = basic::Descriptor<motorparam::ID>;
    
        
    } // namespace motorparam {   


    namespace generic { 

        // In here we define a Descriptor able to manipulate generic [ID|DATA] used in the SET / GET messages
                
        enum class ID
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
            _Float16 withvelocity {0.0};
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
