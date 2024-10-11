
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_MBD_interface.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"

#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_motor_board.h"

#include <array>

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------

#define USE_MOCK

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


#if defined(USE_MOCK)

#include "embot_app_bldc_MBD_interface_mock.h"

#else

    #warning TODO: include mbd.h
    
#endif


namespace embot::app::bldc::mbd::interface {
 
    void init(IO2 *io2)
    {
#if defined(USE_MOCK)
        embot::app::bldc::mbd::interface::mock::init(io2);
#else     
        #warning TODO: init mbd
#endif        
    }
    
    void tick()
    {
#if defined(USE_MOCK)
        
        embot::app::bldc::mbd::interface::mock::process_01_input();
        embot::app::bldc::mbd::interface::mock::process_02_activity();
        embot::app::bldc::mbd::interface::mock::process_03_output();  
#else           
        #warning TODO: tick mbd
#endif        
    }
    
    
    
#if 0 
    about controlmode
    
    The CAN protocol uses values such as 
    
    ```C++
    namespace embot::prot::can::motor::polling {
    enum class ControlMode : uint8_t { Idle = 0x00, Current = 0x06, ForceIdle = 0x09, SpeedVoltage = 0x0A, OpenLoop = 0x50, HWfault = 0xA0, NotConfigured = 0xB0, none = 254 };
    }
    ```
    
    but the model uses 
    ```C++
    namespace embot::app::bldc::mbd::interface {
    typedef enum {
        ControlModes_NotConfigured = 0,      // Default value
        ControlModes_Idle,              // 1 
        ControlModes_Position,          // 2
        ControlModes_PositionDirect,    // 3
        ControlModes_Current,           // 4
        ControlModes_Velocity,          // 5
        ControlModes_Voltage,           // 6
        ControlModes_HwFaultCM          // 7
    } ControlModes;
    }
    ```
    
    So, we need to map them correctly.
    
    The map to each other is:
    - ControlModes_NotConfigured    (0) <--> ControlMode::NotConfigured (176)
    - ControlModes_Idle             (1) <--> ControlMode::Idle (0) and ControlMode::ForceIdle (9) ....
    - ControlModes_Position         (2) <--> ControlMode::Idle
    - ControlModes_PositionDirect   (3) <--> ControlMode::Idle
    - ControlModes_Current          (4) <--> ControlMode::Current (6) .......
    - ControlModes_Velocity         (5) <--> ControlMode::SpeedVoltage (10) .... 
    - ControlModes_Voltage          (6) <--> ControlMode::OpenLoop (80) .....
    - ControlModes_HwFaultCM        (7) <--> ControlMode::HWfault (160) 
    and
    - ControlModes_NotConfigured    (0) <--> ControlMode::NotConfigured (176)
    - ControlModes_Idle             (1) <--> ControlMode::Idle (0) and ControlMode::ForceIdle (9)
    - ControlModes_Position         (2) <--> ControlMode::?
    - ControlModes_PositionDirect   (3) <--> ControlMode::?
    - ControlModes_Current          (4) <--> ControlMode::Current (6)
    - ControlModes_Velocity         (5) <--> ControlMode::SpeedVoltage (10)
    - ControlModes_Voltage          (6) <--> ControlMode::OpenLoop (80)
    - ControlModes_HwFaultCM        (7) <--> ControlMode::?
    
#endif


#if 0

 removed

    embot::prot::can::motor::ControlMode convert(const ControlModes x)
    {
        embot::prot::can::motor::ControlMode cm {embot::prot::can::motor::ControlMode::Idle};
        
        // w/ lookup table is quick
        
        constexpr uint8_t numOfmbd {8};
        constexpr static embot::prot::can::motor::ControlMode toCANcm[numOfmbd] =
        {
            embot::prot::can::motor::ControlMode::NotConfigured,   // ControlModes_NotConfigured = 0
            embot::prot::can::motor::ControlMode::Idle,            // ControlModes_Idle = 1
            embot::prot::can::motor::ControlMode::Idle,            // ControlModes_Position = 2 we dont have CAN boards controlled in position
            embot::prot::can::motor::ControlMode::Idle,            // ControlModes_PositionDirect = 3 we dont have CAN boards controlled in position direct
            embot::prot::can::motor::ControlMode::Current,         // ControlModes_Current = 4 
            embot::prot::can::motor::ControlMode::SpeedVoltage,    // ControlModes_Velocity = 5 
            embot::prot::can::motor::ControlMode::OpenLoop,        // ControlModes_Voltage = 6 
            embot::prot::can::motor::ControlMode::HWfault          // ControlModes_HwFaultCM = 7            
        };
        
        uint8_t i = static_cast<uint8_t>(x);
        
        if(i<numOfmbd)
        {
            cm = toCANcm[i];
        }
        
        return cm;
    }
    
    struct Pair
    {
        embot::prot::can::motor::ControlMode  cmCAN {embot::prot::can::motor::ControlMode::NotConfigured};    
        embot::app::bldc::mbd::interface::ControlModes cmMBD {embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured}; 
        constexpr Pair() = default; 
        constexpr Pair(embot::prot::can::motor::ControlMode c1, embot::app::bldc::mbd::interface::ControlModes c2)  : cmCAN(c1), cmMBD(c2) {}       
    };

    ControlModes convert(const embot::prot::can::motor::ControlMode cm)
    {
        embot::app::bldc::mbd::interface::ControlModes c { embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured};
        
        constexpr uint8_t numOfcan {8};
        constexpr std::array<Pair, numOfcan> toMBDcm
        {
            Pair{embot::prot::can::motor::ControlMode::Idle,           embot::app::bldc::mbd::interface::ControlModes::ControlModes_Idle},
            Pair{embot::prot::can::motor::ControlMode::Current,        embot::app::bldc::mbd::interface::ControlModes::ControlModes_Current},
            Pair{embot::prot::can::motor::ControlMode::ForceIdle,      embot::app::bldc::mbd::interface::ControlModes::ControlModes_Idle},
            Pair{embot::prot::can::motor::ControlMode::SpeedVoltage,   embot::app::bldc::mbd::interface::ControlModes::ControlModes_Velocity},      
            Pair{embot::prot::can::motor::ControlMode::OpenLoop,       embot::app::bldc::mbd::interface::ControlModes::ControlModes_Voltage},
            Pair{embot::prot::can::motor::ControlMode::HWfault,        embot::app::bldc::mbd::interface::ControlModes::ControlModes_HwFaultCM},
            Pair{embot::prot::can::motor::ControlMode::NotConfigured,  embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured},
            Pair{embot::prot::can::motor::ControlMode::none,           embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured}             
        };
        
        // in here we loop
        for(const auto &i : toMBDcm)
        {
            if(i.cmCAN == cm)
            {
                return i.cmMBD;
            }
        }   

        return embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured;
    }
    
        
    void convert(const embot::prot::can::motor::CurrentLimits &from, SupervisorInputLimits &to)
    {
        // CurrentLimits is in mA, SupervisorInputLimits in A 
        to.nominal = CurrConv::toampere(from.nominal);
        to.overload = CurrConv::toampere(from.overload);
        to.peak = CurrConv::toampere(from.peak);
        to.type = ControlModes::ControlModes_NotConfigured;
    }
    
    void convert(const SupervisorInputLimits &from, embot::prot::can::motor::CurrentLimits &to)
    {
        // CurrentInfo is in mA, SupervisorInputLimits in A 
        to.nominal = CurrConv::tomilliampere(from.nominal);
        to.overload = CurrConv::tomilliampere(from.overload);
        to.peak = CurrConv::tomilliampere(from.peak);
    }
    
    void convert(const embot::prot::can::motor::PID &from, PID &to)
    {       
        to.type = (from.type == embot::prot::can::motor::PIDtype::VEL) ? ControlModes_Velocity : ControlModes_Current;
        to.OutMax = 0;
        to.OutMin = 0;
        to.P = from.get(embot::prot::can::motor::PID::Gain::P);
        to.I = from.get(embot::prot::can::motor::PID::Gain::I);
        to.D = from.get(embot::prot::can::motor::PID::Gain::D);
        to.N = 0;
        to.I0 = 0;
        to.D0 = 0;
        to.shift_factor = from.ks;
    }
    
    void convert(const PID &from, embot::prot::can::motor::PID &to)
    {       
        embot::prot::can::motor::PIDtype pidtype = (from.type == ControlModes_Velocity) ? embot::prot::can::motor::PIDtype::VEL :  embot::prot::can::motor::PIDtype::CURR;        
        to.load(pidtype, from.P, from.I, from.D, from.shift_factor);
    }    
    
    void convert(const embot::prot::can::motor::MotorConfig &from, MotorConfigurationExternal &to)
    {
        // flags
        to.enable_verbosity = from.check(embot::prot::can::motor::MotorConfig::Flag::verbose);
        to.has_hall_sens = from.check(embot::prot::can::motor::MotorConfig::Flag::hasHALLsensor);
        to.has_quadrature_encoder = from.check(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoder);
        to.has_speed_quadrature_encoder = from.check(embot::prot::can::motor::MotorConfig::Flag::hasSpeedEncoder);
        to.use_index = from.check(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoderIndex);
        to.has_temperature_sens = from.check(embot::prot::can::motor::MotorConfig::Flag::hasTempSensor);
        // others
        to.encoder_tolerance = from.rotEncTolerance;
        to.pole_pairs = from.motorPoles;
        to.rotor_encoder_resolution = from.rotorEncoderResolution;
        to.rotor_index_offset = from.rotorIndexOffset;                
    }
    
    void convert(const MotorConfigurationExternal &from, embot::prot::can::motor::MotorConfig &to)
    {
        // flags
        to.flags = 0;
        if(from.enable_verbosity)               to.set(embot::prot::can::motor::MotorConfig::Flag::verbose);
        if(from.has_hall_sens)                  to.set(embot::prot::can::motor::MotorConfig::Flag::hasHALLsensor);
        if(from.has_quadrature_encoder)         to.set(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoder);
        if(from.has_speed_quadrature_encoder)   to.set(embot::prot::can::motor::MotorConfig::Flag::hasSpeedEncoder);
        if(from.use_index)                      to.set(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoderIndex);
        if(from.has_temperature_sens)           to.set(embot::prot::can::motor::MotorConfig::Flag::hasTempSensor);

        // others
        to.rotEncTolerance = from.encoder_tolerance;
        to.motorPoles = from.pole_pairs;
        to.rotorEncoderResolution = from.rotor_encoder_resolution;
        to.rotorIndexOffset = from.rotor_index_offset;                
    }    
    
    
    void convert(const int16_t from, Targets &to)
    {
#if 1
        to.current = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::CURRENT);

        to.voltage = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::VOLTAGE);

        to.velocity = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::VELOCITY);

        to.position = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::POSITION);
        
#else        
//        to.current = to.voltage = to.velocity = to.position = static_cast<float>(from);
        
        // from contains currents in [mA] and we want [A]          
        to.current = 0.001f*static_cast<float>(from);
        
        // from contains voltages expressed as ??? and we want [V]
        // the transformation needs some parameters that depend on the current PID shift factor, so we dont do it in here
        to.voltage = static_cast<float>(from);
        
        // from contains velocity in [icubdegrees/ms] and we want [deg/s]
        constexpr float todeg = 0.0054931640625f; // 1 icub-deg -> 360 deg / pow(2, 16)
        constexpr float cnv = 1000.0f * todeg;  
        to.velocity = cnv*static_cast<float>(from);   
        
        // from contains positions in [icubdegrees], we want [deg] 
        to.position = todeg*static_cast<float>(from);  
#endif        
    }

#endif

    
    // struct Converter
    

    void Converter::tocan(const ControlModes from, embot::prot::can::motor::ControlMode &to)
    {
        embot::prot::can::motor::ControlMode cm {embot::prot::can::motor::ControlMode::Idle};
        
        // w/ lookup table is quick
        
        constexpr uint8_t numOfmbd {8};
        constexpr static embot::prot::can::motor::ControlMode toCANcm[numOfmbd] =
        {
            embot::prot::can::motor::ControlMode::NotConfigured,   // ControlModes_NotConfigured = 0
            embot::prot::can::motor::ControlMode::Idle,            // ControlModes_Idle = 1
            embot::prot::can::motor::ControlMode::Idle,            // ControlModes_Position = 2 we dont have CAN boards controlled in position
            embot::prot::can::motor::ControlMode::Idle,            // ControlModes_PositionDirect = 3 we dont have CAN boards controlled in position direct
            embot::prot::can::motor::ControlMode::Current,         // ControlModes_Current = 4 
            embot::prot::can::motor::ControlMode::SpeedVoltage,    // ControlModes_Velocity = 5 
            embot::prot::can::motor::ControlMode::OpenLoop,        // ControlModes_Voltage = 6 
            embot::prot::can::motor::ControlMode::HWfault          // ControlModes_HwFaultCM = 7            
        };
        
        uint8_t i = static_cast<uint8_t>(from);
        
        if(i<numOfmbd)
        {
            to = toCANcm[i];
        }
    }
    
    void Converter::fromcan(const embot::prot::can::motor::ControlMode from, ControlModes &to)
    {
        to = embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured;
        
        struct Pair
        {
            embot::prot::can::motor::ControlMode  cmCAN {embot::prot::can::motor::ControlMode::NotConfigured};    
            embot::app::bldc::mbd::interface::ControlModes cmMBD {embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured}; 
            constexpr Pair() = default; 
            constexpr Pair(embot::prot::can::motor::ControlMode c1, embot::app::bldc::mbd::interface::ControlModes c2)  : cmCAN(c1), cmMBD(c2) {}       
        };  
        
        constexpr uint8_t numOfcan {8};
        constexpr std::array<Pair, numOfcan> toMBDcm
        {
            Pair{embot::prot::can::motor::ControlMode::Idle,           embot::app::bldc::mbd::interface::ControlModes::ControlModes_Idle},
            Pair{embot::prot::can::motor::ControlMode::Current,        embot::app::bldc::mbd::interface::ControlModes::ControlModes_Current},
            Pair{embot::prot::can::motor::ControlMode::ForceIdle,      embot::app::bldc::mbd::interface::ControlModes::ControlModes_Idle},
            Pair{embot::prot::can::motor::ControlMode::SpeedVoltage,   embot::app::bldc::mbd::interface::ControlModes::ControlModes_Velocity},      
            Pair{embot::prot::can::motor::ControlMode::OpenLoop,       embot::app::bldc::mbd::interface::ControlModes::ControlModes_Voltage},
            Pair{embot::prot::can::motor::ControlMode::HWfault,        embot::app::bldc::mbd::interface::ControlModes::ControlModes_HwFaultCM},
            Pair{embot::prot::can::motor::ControlMode::NotConfigured,  embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured},
            Pair{embot::prot::can::motor::ControlMode::none,           embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured}             
        };
        
        // in here we loop
        for(const auto &i : toMBDcm)
        {
            if(i.cmCAN == from)
            {
                to = i.cmMBD;
                return;
            }
        }   

    }    
    
    void Converter::fromcan(const embot::prot::can::motor::CurrentLimits &from, SupervisorInputLimits &to)
    {
        // CurrentLimits is in mA, SupervisorInputLimits in A 
        to.nominal = millitoampere(from.nominal);
        to.overload = millitoampere(from.overload);
        to.peak = millitoampere(from.peak);
        to.type = ControlModes::ControlModes_NotConfigured;
    }
    
    void Converter::tocan(const SupervisorInputLimits &from, embot::prot::can::motor::CurrentLimits &to)
    {
        // CurrentInfo is in mA, SupervisorInputLimits in A 
        to.nominal = amperetomilli(from.nominal);
        to.overload = amperetomilli(from.overload);
        to.peak = amperetomilli(from.peak);
    }
    
    void Converter::fromcan(const embot::prot::can::motor::PID &from, PID &to)
    {       
        to.type = (from.type == embot::prot::can::motor::PIDtype::VEL) ? ControlModes_Velocity : ControlModes_Current;
        to.OutMax = 0;
        to.OutMin = 0;
        to.P = from.get(embot::prot::can::motor::PID::Gain::P);
        to.I = from.get(embot::prot::can::motor::PID::Gain::I);
        to.D = from.get(embot::prot::can::motor::PID::Gain::D);
        to.N = 0;
        to.I0 = 0;
        to.D0 = 0;
        to.shift_factor = from.ks;
    }
    
    void Converter::tocan(const PID &from, embot::prot::can::motor::PID &to)
    {       
        embot::prot::can::motor::PIDtype pidtype = (from.type == ControlModes_Velocity) ? embot::prot::can::motor::PIDtype::VEL :  embot::prot::can::motor::PIDtype::CURR;        
        to.load(pidtype, from.P, from.I, from.D, from.shift_factor);
    }    
    
    void Converter::fromcan(const embot::prot::can::motor::MotorConfig &from, MotorConfigurationExternal &to)
    {
        // flags
        to.enable_verbosity = from.check(embot::prot::can::motor::MotorConfig::Flag::verbose);
        to.has_hall_sens = from.check(embot::prot::can::motor::MotorConfig::Flag::hasHALLsensor);
        to.has_quadrature_encoder = from.check(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoder);
        to.has_speed_quadrature_encoder = from.check(embot::prot::can::motor::MotorConfig::Flag::hasSpeedEncoder);
        to.use_index = from.check(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoderIndex);
        to.has_temperature_sens = from.check(embot::prot::can::motor::MotorConfig::Flag::hasTempSensor);
        // others
        to.encoder_tolerance = from.rotEncTolerance;
        to.pole_pairs = from.motorPoles;
        to.rotor_encoder_resolution = from.rotorEncoderResolution;
        to.rotor_index_offset = from.rotorIndexOffset;                
    }
    
    void Converter::tocan(const MotorConfigurationExternal &from, embot::prot::can::motor::MotorConfig &to)
    {
        // flags
        to.flags = 0;
        if(from.enable_verbosity)               to.set(embot::prot::can::motor::MotorConfig::Flag::verbose);
        if(from.has_hall_sens)                  to.set(embot::prot::can::motor::MotorConfig::Flag::hasHALLsensor);
        if(from.has_quadrature_encoder)         to.set(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoder);
        if(from.has_speed_quadrature_encoder)   to.set(embot::prot::can::motor::MotorConfig::Flag::hasSpeedEncoder);
        if(from.use_index)                      to.set(embot::prot::can::motor::MotorConfig::Flag::hasRotorEncoderIndex);
        if(from.has_temperature_sens)           to.set(embot::prot::can::motor::MotorConfig::Flag::hasTempSensor);

        // others
        to.rotEncTolerance = from.encoder_tolerance;
        to.motorPoles = from.pole_pairs;
        to.rotorEncoderResolution = from.rotor_encoder_resolution;
        to.rotorIndexOffset = from.rotor_index_offset;                
    }  

    void Converter::tocan(const uint8_t from, embot::prot::can::motor::board::foc::QEstate &to)
    {
        // i assume FOR NOW that they have the same format
        to.load(from);
    }  

    void Converter::tocan(const uint32_t from, embot::prot::can::motor::board::foc::MotorFaultState &to)
    {
        // i assume FOR NOW that they have the same format
        to.load(from);
    }    
    
    
    void Converter::fromcan(const int16_t from, Targets &to)
    {
#if 1
        to.current = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::CURRENT);

        to.voltage = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::VOLTAGE);

        to.velocity = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::VELOCITY);

        to.position = embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::transform(
                        from, 
                        embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info::Type::POSITION);
        
#else        
//        to.current = to.voltage = to.velocity = to.position = static_cast<float>(from);
        
        // from contains currents in [mA] and we want [A]          
        to.current = 0.001f*static_cast<float>(from);
        
        // from contains voltages expressed as ??? and we want [V]
        // the transformation needs some parameters that depend on the current PID shift factor, so we dont do it in here
        to.voltage = static_cast<float>(from);
        
        // from contains velocity in [icubdegrees/ms] and we want [deg/s]
        constexpr float todeg = 0.0054931640625f; // 1 icub-deg -> 360 deg / pow(2, 16)
        constexpr float cnv = 1000.0f * todeg;  
        to.velocity = cnv*static_cast<float>(from);   
        
        // from contains positions in [icubdegrees], we want [deg] 
        to.position = todeg*static_cast<float>(from);  
#endif        
    }    
    
    // struct IO2
    
    void IO2::event_clearall() 
    {
        memset(input->Events, 0, sizeof(input->Events));
        for(auto &i : input->Events)
        {
            i.event_type = EventTypes_None;
        }
        evtpos = 0;
    }
    
    
    int8_t IO2::getposofevent(EventTypes evt, uint8_t motor)
    {
        if(evtpos >= (sizeof(input->Events) / sizeof(ReceivedEvents)))
        {
            return -1;
        }
        
        int8_t r = evtpos;
        evtpos++;
        return r;        
    }
    
    bool IO2::addevent(const ReceivedEvents& re, uint8_t motor)
    {
        int8_t pos = getposofevent(re.event_type, motor);
        if(pos < 0)
        {
            return false;
        }

        input->Events[pos] = re;
        input->Events[pos].motor = motor; // just in case
        
        return true;
    }
    
    bool IO2::event_pushback(const ControlModes cm, uint8_t motor)
    {
        // the way we add an event it is to be clarified w/ mfussi                
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetControlMode;
        re.control_mode_content = cm; 
        re.motor = motor;            
            
        return addevent(re, motor); 

    }
    
    bool IO2::event_pushback(const SupervisorInputLimits &limits, uint8_t motor)
    {
        // ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetLimit;
        re.limits_content = limits;
        re.limits_content.type = get_controlmode(motor); // only in here we can know about control mode
        re.motor = motor; 
            
        addevent(re, motor);
        
        return true;        
    }

    bool IO2::event_pushback(const PID &pid, uint8_t motor)
    {
        //ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetPid;
        re.pid_content = pid;
        re.motor = motor; 
            
        addevent(re, motor);
        
        return true;        
    }
    
    bool IO2::event_pushback(const MotorConfigurationExternal &mc, uint8_t motor)
    {
        // ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetMotorConfig;
        re.motor_config_content = mc;
        re.motor = motor; 
            
        addevent(re, motor);
        
        return true;        
    }
    

    bool IO2::event_pushback(const Targets &t, uint8_t motor)
    {
        // ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetTarget;
        re.targets_content = t;
        // i need to adapt targets_content to the correct format. 
        re.motor = motor; 
            
        addevent(re, motor);   
            
        return true;        
    }
    
    void IO2::set_fault(const bool pressed)
    {
        input->ExternalFlags_j.fault_button = pressed;           
    }
    
    void IO2::set_controlmode(const ControlModes cm, uint8_t motor)
    {
        output->Flags_d[motor].control_mode = cm;
    }
    
    void IO2::set_quadencoderstate(const uint8_t qes, uint8_t motor)
    {
        #warning QES is TODEDONE    
    }

    uint8_t IO2::get_quadencoderstate(uint8_t motor)
    {
        #warning QES is TODEDONE  
        return 0;        
    }
    
    uint32_t IO2::get_motorfaultstate(uint8_t motor)
    {
        uint32_t f {0};
        constexpr uint8_t pos = embot::core::tointegral(embot::prot::can::motor::board::foc::MotorFaultState::Flag::ExternalFaultAsserted);
        
        if(true == get_fault())
        {
            embot::core::binary::bit::set(f, pos);
        }
        return f;       
    }
    
    float IO2::get_pwmfeedback(uint8_t motor) const
    {
        #warning PWMFEEDBACK is TODEDONE  
        return 30.0;         
    }
    
    bool IO2::get_fault() const
    {
        return input->ExternalFlags_j.fault_button;
    }
    
    ControlModes IO2::get_controlmode(uint8_t motor) const
    {
        return output->Flags_d[motor].control_mode;
    }
    
    bool IO2::get_transmitstatus(uint8_t motor) const
    {
       return output->Flags_d[motor].enable_sending_msg_status; 
    }
    
    float IO2::get_current(uint8_t motor) const
    {
        #warning or ... FOCOutputs::Iq_fbk / Iq_rms ??
        return output->Estimates[motor].Iq_filtered; // mettiamo la 
    }

    float IO2::get_velocity(uint8_t motor) const
    {
        return output->Estimates[motor].velocity;
    }

    void IO2::set_velocity(float dps, uint8_t motor)
    {
        output->Estimates[motor].velocity = dps;
    }
    
    float IO2::get_position(uint8_t motor) const
    {
        #warning or ... something else
        return input->SensorData[motor].position;
    }  
    
    void IO2::set_position(const float degrees, uint8_t motor)
    {
        input->SensorData[motor].position = degrees;
    }

    float IO2::get_temperature(uint8_t motor) const
    {
        #warning or ... something else
        return output->Estimates[motor].motor_temperature;
    } 
    
    void IO2::get_current_limits(uint8_t motor, SupervisorInputLimits &cl)
    {
        cl = status->currentlimits[motor];
    }
    

    void IO2::get_current_pid(uint8_t motor, PID &pid)
    {
        pid = status->pidcurrent[motor];
    }    
     
    void IO2::get_velocity_pid(uint8_t motor, PID &pid)
    {
        pid = status->pidvelocity[motor];
    }
    
    void IO2::get_motor_config(uint8_t motor, MotorConfigurationExternal &mc)
    {
        mc = status->motorconfig[motor];
    }    
    
    ExtU_iterative_motion_controller_T * IO2::get_input() const
    {
        return input;
    }
    
    ExtY_iterative_motion_controller_T * IO2::get_output() const
    {
        return output;
    }
    
    Status_iterative_motion_controller_T * IO2::get_status() const
    {
        return status;
    }    
    
} // namespace embot::app::bldc::mbd::interface {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


