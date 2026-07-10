//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: position_velocity_cascade_types.h
//
// Code generated for Simulink model 'position_velocity_cascade'.
//
// Model version                  : 10.15
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:00:57 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef POSITION_VELOCITY_CASCADE_TYPES_H_
#define POSITION_VELOCITY_CASCADE_TYPES_H_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_MOTORCONFIGURATIONEXTERNAL_
#define DEFINED_TYPEDEF_FOR_MOTORCONFIGURATIONEXTERNAL_

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

#ifndef DEFINED_TYPEDEF_FOR_REFERENCEENCODER_
#define DEFINED_TYPEDEF_FOR_REFERENCEENCODER_

typedef enum {
  ReferenceEncoder_Motor = 0,          // Default value
  ReferenceEncoder_Joint
} ReferenceEncoder;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MOTORCONFIGURATION_
#define DEFINED_TYPEDEF_FOR_MOTORCONFIGURATION_

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
  boolean_T hall_sensors_swapBC;
  ReferenceEncoder reference_encoder;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CONTROLMODES_
#define DEFINED_TYPEDEF_FOR_CONTROLMODES_

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

#ifndef DEFINED_TYPEDEF_FOR_PIDSCONFIGURATION_
#define DEFINED_TYPEDEF_FOR_PIDSCONFIGURATION_

struct PIDsConfiguration
{
  PID currentPID;
  PID velocityPID;
  PID positionPID;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_THRESHOLDS_
#define DEFINED_TYPEDEF_FOR_THRESHOLDS_

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

#ifndef DEFINED_TYPEDEF_FOR_ACTUATORCONFIGURATION_
#define DEFINED_TYPEDEF_FOR_ACTUATORCONFIGURATION_

struct ActuatorConfiguration
{
  Thresholds thresholds;
  PIDsConfiguration pids;
  MotorConfiguration motor;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CALIBRATIONTYPES_
#define DEFINED_TYPEDEF_FOR_CALIBRATIONTYPES_

typedef enum {
  CalibrationTypes_None = 0,           // Default value
  CalibrationTypes_Search_Index,
  CalibrationTypes_Full_Calibration
} CalibrationTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_CONTROLOUTEROUTPUTS_
#define DEFINED_TYPEDEF_FOR_CONTROLOUTEROUTPUTS_

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

#ifndef DEFINED_TYPEDEF_FOR_ESTIMATEDDATA_
#define DEFINED_TYPEDEF_FOR_ESTIMATEDDATA_

struct EstimatedData
{
  // Speed of the rotor BEFORE the reduction stage
  real32_T rotor_velocity;

  // filtered motor current
  real32_T Iq_filtered;

  // motor temperature
  real32_T motor_temperature;

  // Speed of the rotor AFTER the reduction stage
  real32_T joint_velocity;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_TARGETS_
#define DEFINED_TYPEDEF_FOR_TARGETS_

struct Targets
{
  // Target time for position control
  real32_T trajectory_time;
  real32_T position;
  real32_T velocity;
  real32_T current;
  real32_T voltage;
};

#endif
#endif                                 // POSITION_VELOCITY_CASCADE_TYPES_H_

//
// File trailer for generated code.
//
// [EOF]
//
