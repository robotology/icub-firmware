//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_types.h
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 4.6
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Fri May 30 15:27:43 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef motion_controller_types_h_
#define motion_controller_types_h_
#include "rtwtypes.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"
#ifndef DEFINED_TYPEDEF_FOR_DriverSensors_
#define DEFINED_TYPEDEF_FOR_DriverSensors_

struct DriverSensors
{
  // power supply voltage
  real32_T Vcc;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

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

#endif

#ifndef DEFINED_TYPEDEF_FOR_SensorsData_
#define DEFINED_TYPEDEF_FOR_SensorsData_

struct SensorsData
{
  // position encoders
  real32_T position;
  DriverSensors driversensors;
  MotorSensors motorsensors;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Thresholds_
#define DEFINED_TYPEDEF_FOR_Thresholds_

struct Thresholds
{
  // Can be only non-negative
  real32_T jntVelMax;

  // Current that can be kept for an indefinite period of time w/o damaging the motor
  // Expressed in [A] as all the internal computations are done this way
  // Can be only non-negative
  real32_T motorNominalCurrents;

  // Current that can be applied for a short period of time
  // Expressed in [A] as all the internal computations are done this way
  // Can be only non-negative
  real32_T motorPeakCurrents;

  // Currents over this threshold can instantaneously damages the motor
  // Expressed in [A] as all the internal computations are done this way
  // Can be only non-negative
  real32_T motorOverloadCurrents;

  // Expressed in ticks
  // Max value is 32000
  // Can be only non-negative
  uint32_T motorPwmLimit;

  // The critical temperature of the motor that triggers i2t current protection. 
  real32_T motorCriticalTemperature;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlModes_
#define DEFINED_TYPEDEF_FOR_ControlModes_

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

#endif

#ifndef DEFINED_TYPEDEF_FOR_PID_
#define DEFINED_TYPEDEF_FOR_PID_

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

#endif

#ifndef DEFINED_TYPEDEF_FOR_PIDsConfiguration_
#define DEFINED_TYPEDEF_FOR_PIDsConfiguration_

struct PIDsConfiguration
{
  PID currentPID;
  PID velocityPID;
  PID positionPID;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfigurationExternal_
#define DEFINED_TYPEDEF_FOR_MotorConfigurationExternal_

struct MotorConfigurationExternal
{
  boolean_T enable_verbosity;
  boolean_T has_hall_sens;
  boolean_T has_quadrature_encoder;
  boolean_T has_speed_quadrature_encoder;
  boolean_T has_temperature_sens;
  uint8_T encoder_tolerance;
  uint8_T pole_pairs;
  int16_T rotor_encoder_resolution;
  int16_T rotor_index_offset;
  boolean_T use_index;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfiguration_
#define DEFINED_TYPEDEF_FOR_MotorConfiguration_

struct MotorConfiguration
{
  MotorConfigurationExternal externals;
  real32_T Kbemf;
  real32_T Rphase;
  real32_T Imin;
  real32_T Imax;
  real32_T Vmax;
  real32_T resistance;
  real32_T inductance;
  real32_T thermal_resistance;
  real32_T thermal_time_constant;
  real32_T hall_sensors_offset;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ActuatorConfiguration_
#define DEFINED_TYPEDEF_FOR_ActuatorConfiguration_

struct ActuatorConfiguration
{
  Thresholds thresholds;
  PIDsConfiguration pids;
  MotorConfiguration motor;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimationVelocityModes_
#define DEFINED_TYPEDEF_FOR_EstimationVelocityModes_

typedef enum {
  EstimationVelocityModes_Disabled = 0,// Default value
  EstimationVelocityModes_MovingAverage,
  EstimationVelocityModes_LeastSquares
} EstimationVelocityModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimationConfiguration_
#define DEFINED_TYPEDEF_FOR_EstimationConfiguration_

struct EstimationConfiguration
{
  real32_T environment_temperature;
  real32_T current_rms_lambda;
  EstimationVelocityModes velocity_est_mode;
  uint32_T velocity_est_window;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_GlobalConfiguration_
#define DEFINED_TYPEDEF_FOR_GlobalConfiguration_

struct GlobalConfiguration
{
  EstimationConfiguration estimation;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimatedData_
#define DEFINED_TYPEDEF_FOR_EstimatedData_

struct EstimatedData
{
  // velocity
  real32_T velocity;

  // filtered motor current
  real32_T Iq_filtered;

  // motor temperature
  real32_T motor_temperature;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ExternalFlags_
#define DEFINED_TYPEDEF_FOR_ExternalFlags_

struct ExternalFlags
{
  // External Fault Button (1 == pressed)
  boolean_T fault_button;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EventTypes_
#define DEFINED_TYPEDEF_FOR_EventTypes_

typedef enum {
  EventTypes_None = 0,                 // Default value
  EventTypes_SetLimit,
  EventTypes_SetControlMode,
  EventTypes_SetMotorConfig,
  EventTypes_SetPid,
  EventTypes_SetTarget
} EventTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Targets_
#define DEFINED_TYPEDEF_FOR_Targets_

struct Targets
{
  real32_T position;
  real32_T velocity;
  real32_T current;
  real32_T voltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SupervisorInputLimits_
#define DEFINED_TYPEDEF_FOR_SupervisorInputLimits_

struct SupervisorInputLimits
{
  real32_T overload;
  real32_T peak;
  real32_T nominal;
  ControlModes type;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ReceivedEvents_
#define DEFINED_TYPEDEF_FOR_ReceivedEvents_

struct ReceivedEvents
{
  uint8_T motor_id;
  EventTypes event_type;
  Targets targets_content;
  PID pid_content;
  ControlModes control_mode_content;
  SupervisorInputLimits limits_content;
  MotorConfigurationExternal motor_config_content;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_FOCOutputs_
#define DEFINED_TYPEDEF_FOR_FOCOutputs_

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

#endif

#ifndef DEFINED_TYPEDEF_FOR_HardwareFaults_
#define DEFINED_TYPEDEF_FOR_HardwareFaults_

struct HardwareFaults
{
  boolean_T overcurrent;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Flags_
#define DEFINED_TYPEDEF_FOR_Flags_

struct Flags
{
  boolean_T enable_sending_msg_status;
  HardwareFaults hw_faults;
  boolean_T enable_thermal_protection;

  // control mode
  ControlModes control_mode;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlOuterOutputs_
#define DEFINED_TYPEDEF_FOR_ControlOuterOutputs_

struct ControlOuterOutputs
{
  boolean_T vel_en;
  boolean_T cur_en;
  boolean_T out_en;
  boolean_T pid_reset;
  real32_T motorcurrent;
  real32_T current_limiter;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_FOCSlowInputs_
#define DEFINED_TYPEDEF_FOR_FOCSlowInputs_

struct FOCSlowInputs
{
  GlobalConfiguration global_configuration;
  ActuatorConfiguration actuator_configuration;
  EstimatedData estimated_data;
  Targets targets;
  ControlOuterOutputs control_outer_outputs;
};

#endif
#endif                                 // motion_controller_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
