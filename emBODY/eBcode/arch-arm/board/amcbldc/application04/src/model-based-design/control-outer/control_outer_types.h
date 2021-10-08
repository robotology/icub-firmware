//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: control_outer_types.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 1.100
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:44:08 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_outer_types_h_
#define RTW_HEADER_control_outer_types_h_
#include "rtwtypes.h"

// Model Code Variants
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
  ControlModes_Torque,
  ControlModes_HwFaultCM
} ControlModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Flags_
#define DEFINED_TYPEDEF_FOR_Flags_

struct Flags
{
  // control mode
  ControlModes control_mode;

  // PID reset
  boolean_T PID_reset;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_JointLimits_
#define DEFINED_TYPEDEF_FOR_JointLimits_

struct JointLimits
{
  // joint limits
  real32_T limits[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_VelocityLimits_
#define DEFINED_TYPEDEF_FOR_VelocityLimits_

struct VelocityLimits
{
  // velocity limits
  real32_T limits[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfig_
#define DEFINED_TYPEDEF_FOR_MotorConfig_

struct MotorConfig
{
  real32_T Imin;
  real32_T Imax;
  boolean_T has_hall_sens;
  uint8_T pole_pairs;
  real32_T reduction;
  real32_T Kp;
  real32_T Ki;
  boolean_T has_speed_sens;
  real32_T Kbemf;
  real32_T Rphase;
  real32_T Vmax;
  real32_T Vcc;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_PIDConfig_
#define DEFINED_TYPEDEF_FOR_PIDConfig_

struct PIDConfig
{
  real32_T OutMax;
  real32_T OutMin;
  real32_T P;
  real32_T I;
  real32_T D;
  real32_T N;
  real32_T I0;
  real32_T D0;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ConfigurationParameters_
#define DEFINED_TYPEDEF_FOR_ConfigurationParameters_

struct ConfigurationParameters
{
  JointLimits jointlimits;
  VelocityLimits velocitylimits;
  MotorConfig motorconfig;
  PIDConfig PosLoopPID;
  PIDConfig VelLoopPID;
  PIDConfig DirLoopPID;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_JointPositions_
#define DEFINED_TYPEDEF_FOR_JointPositions_

struct JointPositions
{
  // joint positions
  real32_T position;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Thresholds_
#define DEFINED_TYPEDEF_FOR_Thresholds_

struct Thresholds
{
  real32_T current_low;
  real32_T current_high;
  real32_T voltage_low;
  real32_T voltage_high;
  real32_T temperature_low;
  real32_T temperature_high;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

struct MotorSensors
{
  real32_T Iabc[3];
  real32_T angle;
  real32_T omega;
  real32_T temperature;
  real32_T voltage;
  Thresholds threshold;
  real32_T current;
  uint8_T hallABC;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SensorsData_
#define DEFINED_TYPEDEF_FOR_SensorsData_

struct SensorsData
{
  // position encoders
  JointPositions jointpositions;
  MotorSensors motorsensors;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_JointVelocities_
#define DEFINED_TYPEDEF_FOR_JointVelocities_

struct JointVelocities
{
  // joint velocities
  real32_T velocity;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorCurrent_
#define DEFINED_TYPEDEF_FOR_MotorCurrent_

struct MotorCurrent
{
  // motor current
  real32_T current;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorVoltage_
#define DEFINED_TYPEDEF_FOR_MotorVoltage_

struct MotorVoltage
{
  // motor voltage
  real32_T voltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Targets_
#define DEFINED_TYPEDEF_FOR_Targets_

struct Targets
{
  JointPositions jointpositions;
  JointVelocities jointvelocities;
  MotorCurrent motorcurrent;
  MotorVoltage motorvoltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlOuterOutputs_
#define DEFINED_TYPEDEF_FOR_ControlOuterOutputs_

struct ControlOuterOutputs
{
  boolean_T vel_en;
  boolean_T cur_en;
  boolean_T out_en;
  MotorCurrent motorcurrent;
};

#endif
#endif                                 // RTW_HEADER_control_outer_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
