
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_MBD_API_H_
#define __EMBOT_APP_BLDC_MBD_API_H_

#include "embot_core.h"

#include "stdint.h" // because i am annoyed by seeing file <cstdint> non recognised.



//#define MBD_API_VERSION_draft

#define MBD_API_VERSION_1_0

#if defined(MBD_API_VERSION_1_0)
//#define MBD_API_standalone_usage
#endif

#if defined(MBD_API_VERSION_draft)

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

} // namespace embot::app::bldc::mbd::interface {

#endif // #if defined(MBD_API_VERSION_draft)

#if defined(MBD_API_VERSION_1_0)


#if defined(MBD_API_standalone_usage)

    // in standalone usage we need to cut out everything but the two .h files inside the folder mbd-api-v1  
    using boolean_T = bool;
    using real32_T = float;    
    using uint8_T = uint8_t;
    using uint32_T = uint32_t;
    using int16_T = std::int16_t;    

    #define N_MOTORS 2
    #define MAX_EVENTS_PER_TICK 4


#include "mbd-api-v1/iterative_motion_controller_types.h"
#include "mbd-api-v1/iterative_motion_controller.h"

#else

#include "iterative_motion_controller_types.h"
#include "iterative_motion_controller.h"
#endif

namespace embot::app::bldc::mbd::interface {
    
    //  the types inside _types.h that we need 
    using MotorConfigurationExtSet = MotorConfigurationExtSet;
    using DriverSensors = DriverSensors;
    using SensorsData = SensorsData;
    using ExternalFlags = ExternalFlags;
    using EventTypes = EventTypes;
    using Targets = Targets;
    using ControlModes = ControlModes;
    using PID = PID;
    using SupervisorInputLimits = SupervisorInputLimits;
    using MotorConfigurationExternal = MotorConfigurationExternal;
    using ReceivedEvents = ReceivedEvents;
    using FOCOutputs = FOCOutputs;
    using EstimatedData = EstimatedData;
    using HardwareFaults = HardwareFaults;
    using Flags = Flags;
    using Thresholds = Thresholds;
    using PIDsConfiguration = PIDsConfiguration;
    using MotorConfiguration = MotorConfiguration;
    using ActuatorConfiguration = ActuatorConfiguration; // contains: 
//    using BUS_MSG_FOC = BUS_MSG_FOC;
    using MCControlModes = MCControlModes;
//    using BUS_FLAGS_TX = BUS_FLAGS_TX;
//    using BUS_MSG_STATUS = BUS_MSG_STATUS;
    using BUS_MESSAGES_TX = BUS_MESSAGES_TX;
    using BUS_STATUS_TX = BUS_STATUS_TX;
//    using EstimationVelocityModes = EstimationVelocityModes;
//    using RT_MODEL_iterative_motion_controller_T = RT_MODEL_iterative_motion_controller_T;
    
    // derived types
    using ExtU_iterative_motion_controller_T = ExtU_iterative_motion_controller_T;
    using ExtY_iterative_motion_controller_T = ExtY_iterative_motion_controller_T;
    


#if 0

note:

    cambi in output
    
    ExtY_iterative_motion_controller_T ha aggiunto:

        BUS_MESSAGES_TX Messages[N_MOTORS];  // '<Root>/Messages'
        BUS_STATUS_TX MessagesFlags[N_MOTORS];// '<Root>/MessagesFlags'
   
   ed ha modificato:
        ActuatorConfiguration ConfigurationParameters[N_MOTORS];// '<Root>/ConfigurationParameters'    
   dove forse stanno le cose che ci servono..     

#endif

#if 0    
    ActuatorConfiguration contains:
    - Thresholds:
      - jntVelMax
      - motorNominalCurrents
      - motorPeakCurrents, etc....
      
    - PIDsConfiguration:
      - currentPID,
      - velocityPID
      - positionPID
      
    - MotorConfiguration:
      - MotorConfigurationExternal
      - Kbemf, Rphase, etc.
#endif


    
    struct State
    {
        static constexpr uint8_t nmot {2};
        ActuatorConfiguration* actcfg[nmot] {nullptr, nullptr};
        BUS_MESSAGES_TX* messages[nmot] {nullptr, nullptr};
        BUS_STATUS_TX* messagesflags[nmot] {nullptr, nullptr};
        
        State() = default;
        
        State(ExtY_iterative_motion_controller_T &out)
        {
            load(out);         
        }
        
        void load(ExtY_iterative_motion_controller_T &out)
        {
            for(uint8_t m=0; m<nmot; m++)
            {
                actcfg[m] = &out.ConfigurationParameters[m];
                messages[m] = &out.Messages[m];
                messagesflags[m] = &out.MessagesFlags[m];
           }           
        }
        
        constexpr bool isvalid() const
        {
            return nullptr != actcfg[0];
        }
    };


} // namespace embot::app::bldc::mbd::interface {

#endif // #if defined(MBD_API_VERSION_1_0)

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
