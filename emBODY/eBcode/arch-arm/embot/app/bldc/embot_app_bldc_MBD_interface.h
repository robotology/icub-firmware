
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_MBD_INTERFACE_H_
#define __EMBOT_APP_BLDC_MBD_INTERFACE_H_

#include "embot_core.h"

//using namespace std;
namespace embot::app::bldc::mbd::interface {
    
// Questa struct è l'input che il MBD legge ad ogni tick

using boolean_T = bool;
using real32_T = float;    
using uint8_T = uint8_t;
using int16_T = std::int16_t;    

struct DriverSensors
{
  // power supply voltage
  real32_T Vcc;
};

struct MotorSensors
{
  real32_T Iabc[3];

  // electrical angle = angle * pole_pairs
  real32_T angle;
  real32_T temperature;
  real32_T voltage;
  real32_T current;
  uint8_T hallABC;
};




typedef enum {
  EventTypes_None = 0,                 // Default value
  EventTypes_SetLimit,
  EventTypes_SetControlMode,
  EventTypes_SetMotorConfig,
  EventTypes_SetPid,
  EventTypes_SetTarget
} EventTypes;

struct ExternalFlags
{
  // External Fault Button (1 == pressed)
  boolean_T fault_button;
};

struct Targets
{
  real32_T position;
  real32_T velocity;
  real32_T current;
  real32_T voltage;
};

typedef enum {
  ControlModes_NotConfigured = 0,      // Default value
  ControlModes_Idle, 
  ControlModes_Position,
  ControlModes_PositionDirect,
  ControlModes_Current,
  ControlModes_Velocity,
  ControlModes_Voltage,
  ControlModes_HwFaultCM
} ControlModes;

struct PID
{
  ControlModes type;
  real32_T OutMax;
  real32_T OutMin;
  real32_T P;
  real32_T I;
  real32_T D;
  real32_T N;
  real32_T I0;
  real32_T D0;
  uint8_T shift_factor;
};

struct SupervisorInputLimits
{
  real32_T overload;
  real32_T peak;
  real32_T nominal;
  ControlModes type;
};

struct MotorConfigurationExternal
{
  boolean_T enable_verbosity;
  boolean_T has_hall_sens;
  boolean_T has_quadrature_encoder;
  boolean_T has_speed_quadrature_encoder;
    // was: boolean_T has_torque_sens; BUT it is a mistake. it must be: boolean_T has_temperature_sens;
  boolean_T has_temperature_sens;
  uint8_T encoder_tolerance;
  uint8_T pole_pairs;
  int16_T rotor_encoder_resolution;
  int16_T rotor_index_offset;
  boolean_T use_index;
};

struct ReceivedEvents // ho usato il plurale erroneamente, l'evento dovrebbe rimanere uno
{
  uint8_t motor; // marco.accame: to be added. it helps to address the event to the right motor
  EventTypes event_type;
  Targets targets_content;
  PID pid_content;
  ControlModes control_mode_content;
  SupervisorInputLimits limits_content;
  MotorConfigurationExternal motor_config_content;
};

enum { MAX_EVENTS_PER_TICK = 4 };

struct SensorsData
{
  // position encoders
  real32_T position;
  DriverSensors driversensors;
  MotorSensors motorsensors;
};

struct ExtU_iterative_motion_controller_T {
  SensorsData SensorData[2];           // '<Root>/SensorData'
  ReceivedEvents Events[(2 * MAX_EVENTS_PER_TICK)];// '<Root>/Events'
  ExternalFlags ExternalFlags_j;       // '<Root>/ExternalFlags'
};


// output

struct ActuatorConfiguration
{
    uint32_t tbd;
    
};

struct HardwareFaults
{
    uint32_t tbd;
};

struct Flags
{
  boolean_T enable_sending_msg_status;
  HardwareFaults hw_faults;
  boolean_T enable_thermal_protection;

  // control mode
  ControlModes control_mode;
};


struct FOCOutputs
{
  // control effort (quadrature)
  real32_T Vq;

  // control effort (3-phases)
  real32_T Vabc[3];

  // quadrature current
  real32_T Iq_fbk;

  // direct current
  real32_T Id_fbk;

  // RMS of Iq
  real32_T Iq_rms;

  // RMS of Id
  real32_T Id_rms;
};

struct EstimatedData
{
  // velocity
  real32_T velocity;

  // filtered motor current
  real32_T Iq_filtered;

  // motor temperature
  real32_T motor_temperature;
};





// External outputs (root outports fed by signals with default storage)
struct ExtY_iterative_motion_controller_T {
  FOCOutputs FOCOutputs_h[2];          // '<Root>/FOCOutputs'
  EstimatedData Estimates[2];          // '<Root>/Estimates'
  Flags Flags_d[2];                    // '<Root>/Flags'

  // Questo actuatorConfiguration non credo serva strettamente per 
  // il firmware di alto livello ma potrebbe essere utile per debugging
  ActuatorConfiguration ConfigurationParameters[2];// '<Root>/ConfigurationParameters' 
};


// marco.accame: questa la ho definita io sulla base di cosa mi serve
struct Status_iterative_motion_controller_T {
    PID pidcurrent[2];
    PID pidvelocity[2];
    SupervisorInputLimits currentlimits[2];
    MotorConfigurationExternal motorconfig[2];
    MotorSensors motorsensors[2];
    float target[2];
};


struct IO
{
    ExtU_iterative_motion_controller_T input {};
    ExtY_iterative_motion_controller_T output {};
    Status_iterative_motion_controller_T status {};       
};

}



#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_prot_can.h"
#include "embot_prot_can_bootloader.h"
#include "embot_prot_can_motor.h"
#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_analog_periodic.h"
#include "embot_prot_can_analog_polling.h"
#include "embot_prot_can_motor_board.h"

namespace embot::app::bldc::mbd::interface {

//    // conversion functions

//    // from mbd to can
//    embot::prot::can::motor::ControlMode convert(const ControlModes x); 
//    void convert(const SupervisorInputLimits &from, embot::prot::can::motor::CurrentLimits &to); 
//    void convert(const PID &from, embot::prot::can::motor::PID &to);  
//    void convert(const MotorConfigurationExternal &from, embot::prot::can::motor::MotorConfig &to);    
//    
//    // from can to mbd
//    ControlModes convert(const embot::prot::can::motor::ControlMode cm);
//    void convert(const embot::prot::can::motor::CurrentLimits &from, SupervisorInputLimits &to); 
//    void convert(const embot::prot::can::motor::PID &from, PID &to);
//    void convert(const embot::prot::can::motor::MotorConfig &from, MotorConfigurationExternal &to);
//    void convert(const int16_t from, Targets &to);
    
//    struct CurrConv
//    {
//        static float toampere(int16_t ma)
//        {
//            return 0.001 * static_cast<float>(ma);
//        }
//        static int16_t tomilliampere(float a)
//        {
//            return static_cast<int16_t>(1000.0f * a);
//        }
//    };
    
    struct Converter
    {
        // from mbd to can
        static void tocan(const ControlModes from, embot::prot::can::motor::ControlMode &to); 
        static void tocan(const SupervisorInputLimits &from, embot::prot::can::motor::CurrentLimits &to); 
        static void tocan(const PID &from, embot::prot::can::motor::PID &to);  
        static void tocan(const MotorConfigurationExternal &from, embot::prot::can::motor::MotorConfig &to);    
        static void tocan(const uint8_t from, embot::prot::can::motor::board::foc::QEstate &to);
        static void tocan(const uint32_t from, embot::prot::can::motor::board::foc::MotorFaultState &to); 
        // from can to mbd
        static void fromcan(const embot::prot::can::motor::ControlMode from, ControlModes &to);
        static void fromcan(const embot::prot::can::motor::CurrentLimits &from, SupervisorInputLimits &to); 
        static void fromcan(const embot::prot::can::motor::PID &from, PID &to);
        static void fromcan(const embot::prot::can::motor::MotorConfig &from, MotorConfigurationExternal &to);
        static void fromcan(const int16_t from, Targets &to);  

        // utilities
        static float millitoampere(int16_t ma)
        {
            return 0.001 * static_cast<float>(ma);
        }
        
        static int16_t amperetomilli(float a)
        {
            return static_cast<int16_t>(1000.0f * a);
        }                
    };
    
    // load into mbd
    
//    bool load(embot::app::bldc::mbd::interface::ControlModes cm, uint8_t motor);

    struct IO2
    {
        IO2() = default;
        IO2(ExtU_iterative_motion_controller_T *i, ExtY_iterative_motion_controller_T *o, Status_iterative_motion_controller_T *s)
           : input(i), output(o), status(s) {}
        constexpr bool isvalid() const { return (nullptr != input) && (nullptr != output) && (nullptr != status); }
       
        // events: the events are pushed back inside a fifo so that
        // the MBD can evaluate them. the fifo has a limited capacity,
        // so the pushback() returns false when capacity is reached
        void event_clearall();
        bool event_pushback(const ControlModes cm, uint8_t motor);
        bool event_pushback(const SupervisorInputLimits &limits, uint8_t motor); 
        bool event_pushback(const PID &pid, uint8_t motor);
        bool event_pushback(const MotorConfigurationExternal &mc, uint8_t motor);
        bool event_pushback(const Targets &t, uint8_t motor);
        
        // setter
        void set_fault(const bool pressed);
        void set_controlmode(const ControlModes cm, uint8_t motor);
        void set_quadencoderstate(const uint8_t qes, uint8_t motor); 
        void set_position(const float degrees, uint8_t motor);  
        void set_velocity(float dps, uint8_t motor);        
        
        // getter
        bool get_fault() const;
        ControlModes get_controlmode(uint8_t motor) const;
        uint8_t get_quadencoderstate(uint8_t motor);
        uint32_t get_motorfaultstate(uint8_t motor);
        
        bool get_transmitstatus(uint8_t motor) const; 
        float get_current(uint8_t motor) const;
        float get_velocity(uint8_t motor) const;
        float get_position(uint8_t motor) const;
        float get_temperature(uint8_t motor) const;
        float get_pwmfeedback(uint8_t motor) const;
        // to be developed because the values are not in ExtU or ExtY.
        // they may be inside ... 
        void get_current_limits(uint8_t motor, SupervisorInputLimits &cl);
        void get_current_pid(uint8_t motor, PID &pid);
        void get_velocity_pid(uint8_t motor, PID &pid);
        void get_motor_config(uint8_t motor, MotorConfigurationExternal &mc);
        
        // to be used w/ extreme care. 
        ExtU_iterative_motion_controller_T * get_input() const;
        ExtY_iterative_motion_controller_T * get_output() const;
        Status_iterative_motion_controller_T * get_status() const;

    private:
        
        // -- pointers to mbd data.       
        // they point to the global variables inside the generated code
        ExtU_iterative_motion_controller_T * input {nullptr};
        ExtY_iterative_motion_controller_T * output {nullptr};  
        Status_iterative_motion_controller_T * status {nullptr};

    private:
        
        // -- handling of event queue
        uint8_t evtpos {0};        
        int8_t getposofevent(EventTypes evt, uint8_t motor);
        bool addevent(const ReceivedEvents& re, uint8_t motor);
        
        // - others
        
    };
    

// better using this that will be moved inside something maybe
struct IO2manager : public IO2
{
    IO2manager() = default;

    //  in here: the conversion and load methods        
       
};

} // namespace embot::app::bldc::mbd::interface {


namespace embot::app::bldc::mbd::interface {
    
    void init(IO2 *io2);
    void tick();
    
} // namespace embot::app::bldc::mbd::interface {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
