
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


    
    // struct Converter
    

    void Converter::tocan(const embot::app::bldc::mbd::interface::ControlModes from, embot::prot::can::motor::ControlMode &to)
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
    
    void Converter::fromcan(const embot::prot::can::motor::ControlMode from, embot::app::bldc::mbd::interface::ControlModes &to)
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
    
    void Converter::fromcan(const embot::prot::can::motor::CurrentLimits &from, embot::app::bldc::mbd::interface::SupervisorInputLimits &to)
    {
        // CurrentLimits is in mA, SupervisorInputLimits in A 
        to.nominal = millitoampere(from.nominal);
        to.overload = millitoampere(from.overload);
        to.peak = millitoampere(from.peak);
        to.type = ControlModes::ControlModes_NotConfigured;
    }
    
    void Converter::tocan(const embot::app::bldc::mbd::interface::SupervisorInputLimits &from, embot::prot::can::motor::CurrentLimits &to)
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
    
    void Converter::tocan(const embot::app::bldc::mbd::interface::PID &from, embot::prot::can::motor::PID &to)
    {       
        embot::prot::can::motor::PIDtype pidtype = (from.type == ControlModes_Velocity) ? embot::prot::can::motor::PIDtype::VEL :  embot::prot::can::motor::PIDtype::CURR;        
        to.load(pidtype, from.P, from.I, from.D, from.shift_factor);
    }    
    
    void Converter::fromcan(const embot::prot::can::motor::MotorConfig &from, embot::app::bldc::mbd::interface::MotorConfigurationExternal &to)
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
    
    void Converter::tocan(const embot::app::bldc::mbd::interface::MotorConfigurationExternal &from, embot::prot::can::motor::MotorConfig &to)
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
    
    
    void Converter::fromcan(const int16_t from, embot::app::bldc::mbd::interface::Targets &to)
    {

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
           
    }    
    
    // struct IO2
    
    void IO2::event_clearall() 
    {
        memset(input->EventsList, 0, sizeof(input->EventsList));
        for(auto &i : input->EventsList)
        {
            i.event_type = EventTypes_None;
        }
        evtpos = 0;
    }
    
    
    int8_t IO2::getposofevent(embot::app::bldc::mbd::interface::EventTypes evt, uint8_t motor)
    {
        if(evtpos >= (sizeof(input->EventsList) / sizeof(ReceivedEvents)))
        {
            return -1;
        }
        
        int8_t r = evtpos;
        evtpos++;
        return r;        
    }
    
    bool IO2::addevent(const embot::app::bldc::mbd::interface::ReceivedEvents& re, uint8_t motor)
    {
        int8_t pos = getposofevent(re.event_type, motor);
        if(pos < 0)
        {
            return false;
        }

        input->EventsList[pos] = re;
        input->EventsList[pos].motor_id = motor; // just in case
        
        return true;
    }
    
    bool IO2::event_pushback(const embot::app::bldc::mbd::interface::ControlModes cm, uint8_t motor)
    {
        // the way we add an event it is to be clarified w/ mfussi                
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetControlMode;
        re.control_mode_content = cm; 
        re.motor_id = motor;            
            
        return addevent(re, motor); 

    }
    
    bool IO2::event_pushback(const embot::app::bldc::mbd::interface::SupervisorInputLimits &limits, uint8_t motor)
    {
        // ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetLimit;
        re.limits_content = limits;
        re.limits_content.type = get_controlmode(motor); // only in here we can know about control mode
        re.motor_id = motor; 
            
        addevent(re, motor);
        
        return true;        
    }

    bool IO2::event_pushback(const embot::app::bldc::mbd::interface::PID &pid, uint8_t motor)
    {
        //ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetPid;
        re.pid_content = pid;
        re.motor_id = motor; 
            
        addevent(re, motor);
        
        return true;        
    }
    
    bool IO2::event_pushback(const embot::app::bldc::mbd::interface::MotorConfigurationExternal &mc, uint8_t motor)
    {
        // ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetMotorConfig;
        re.motor_config_content = mc;
        re.motor_id = motor; 
            
        addevent(re, motor);
        
        return true;        
    }
    

    bool IO2::event_pushback(const embot::app::bldc::mbd::interface::Targets &t, uint8_t motor)
    {
        // ControlModes cm = get_controlmode(motor);
        ReceivedEvents re {};                
        re.event_type = EventTypes_SetTarget;
        re.targets_content = t;
        // i need to adapt targets_content to the correct format. 
        re.motor_id = motor; 
            
        addevent(re, motor);   
            
        return true;        
    }
    
    void IO2::set_fault(const bool pressed)
    {
        input->ExternalFlags_j.fault_button = pressed;           
    }
    
    void IO2::set_controlmode(const embot::app::bldc::mbd::interface::ControlModes cm, uint8_t motor)
    {
        output->Flags_d[motor].control_mode = cm;
    }
 
    void IO2::set_temperature(float celsiusdegrees, uint8_t motor)
    {
        float c = celsiusdegrees;
    }
    
    
    bool IO2::get_fault() const
    {
        return input->ExternalFlags_j.fault_button;
    }
    
    ControlModes IO2::get_controlmode(uint8_t motor) const
    {
        return output->Flags_d[motor].control_mode;
    }
    
    
    bool IO2::get_transmit(embot::prot::can::motor::periodic::CMD cmd, uint8_t motor) const
    {
        bool r {false};
        
        switch(cmd)
        {
            default: {} break;
                
            case embot::prot::can::motor::periodic::CMD::STATUS:
            {
                r = output->Flags_d[motor].enable_sending_msg_status;
            } break;
            
            case embot::prot::can::motor::periodic::CMD::FOC:
            {
                r = output->Flags_d[motor].enable_sending_msg_status;
            } break; 
            case embot::prot::can::motor::periodic::CMD::ADDITIONAL_STATUS:
            {
                r = false;
            } break;                
        }
        
        return r;
        
    }
    
    void IO2::set(const canFOCinfo &info, uint8_t motor)
    {
        state.messages[motor]->foc.current = info.current;
        state.messages[motor]->foc.velocity = info.velocity;
        state.messages[motor]->foc.position = info.position;
    }
    
    void IO2::get(canFOCinfo &info, uint8_t motor) const
    {
        info.current = state.messages[motor]->foc.current;
        info.velocity = state.messages[motor]->foc.velocity;
        info.position = state.messages[motor]->foc.position;
    }
    
    
    void IO2::set(const canSTATUSinfo &info, uint8_t motor)
    {
        set_controlmode(info.controlmode, motor);
        #warning QES is TODEDONE // info.quadencoderstate;
        embot::prot::can::motor::board::foc::QEstate qes {0};
        qes.load(info.quadencoderstate);    // info.motorfaultstate
        // nad now use qes
        state.messages[motor]->status.flags.ExternalFaultAsserted = get_fault();
        state.messages[motor]->status.pwm_fbk = info.pwmfeedback;

    }
    
    void IO2::get(canSTATUSinfo &info, uint8_t motor) const
    {
        #warning QES is TODEDONE
        info.controlmode = get_controlmode(motor);
        info.quadencoderstate = 0;
        info.pwmfeedback = state.messages[motor]->status.pwm_fbk;       
        embot::prot::can::motor::board::foc::MotorFaultState mfs {0};
        if(true == get_fault())
        {
            mfs.set(embot::prot::can::motor::board::foc::MotorFaultState::Flag::ExternalFaultAsserted);
        }
        info.motorfaultstate = mfs.get();
        
    }   
    

    float IO2::get_temperature(uint8_t motor) const
    {
        return 66.6;
//        return output->Estimates[motor].motor_temperature;
    } 
    
    void IO2::get_current_limits(uint8_t motor, embot::app::bldc::mbd::interface::SupervisorInputLimits &cl)
    {   
        cl.overload = state.actcfg[motor]->thresholds.motorOverloadCurrents;
        cl.peak = state.actcfg[motor]->thresholds.motorPeakCurrents;
        cl.nominal = state.actcfg[motor]->thresholds.motorNominalCurrents; 
    }
    

    void IO2::get_current_pid(uint8_t motor, PID &pid)
    {  
        pid = state.actcfg[motor]->pids.currentPID;       
    }    
     
    void IO2::get_velocity_pid(uint8_t motor, PID &pid)
    {
        pid = state.actcfg[motor]->pids.velocityPID;
    }
    
    void IO2::get_motor_config(uint8_t motor, embot::app::bldc::mbd::interface::MotorConfigurationExternal &mc)
    {
        mc = state.actcfg[motor]->motor.externals;
    }    
    
    ExtU_iterative_motion_controller_T * IO2::get_input() const
    {
        return input;
    }
    
    ExtY_iterative_motion_controller_T * IO2::get_output() const
    {
        return output;
    }
    

    State& IO2::get_state()
    {
        return state;
    }      
    
} // namespace embot::app::bldc::mbd::interface {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


