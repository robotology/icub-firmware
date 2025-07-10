//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: position_velocity_cascade_types.h
//
// Code generated for Simulink model 'position_velocity_cascade'.
//
// Model version                  : 9.22
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Tue Jul  8 15:26:09 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef position_velocity_cascade_types_h_
#define position_velocity_cascade_types_h_
#include "rtwtypes.h"
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

#ifndef DEFINED_TYPEDEF_FOR_EstimatedData_
#define DEFINED_TYPEDEF_FOR_EstimatedData_

struct EstimatedData
{
  // velocity
  real32_T rotor_velocity;

  // filtered motor current
  real32_T Iq_filtered;

  // motor temperature
  real32_T motor_temperature;

  // velocity
  real32_T joint_velocity;
};

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

#ifndef DEFINED_TYPEDEF_FOR_ReferenceEncoder_
#define DEFINED_TYPEDEF_FOR_ReferenceEncoder_

typedef enum {
  ReferenceEncoder_Motor = 0,          // Default value
  ReferenceEncoder_Joint
} ReferenceEncoder;

#endif

#ifndef DEFINED_TYPEDEF_FOR_CalibrationTypes_
#define DEFINED_TYPEDEF_FOR_CalibrationTypes_

typedef enum {
  CalibrationTypes_None = 0,           // Default value
  CalibrationTypes_Search_Index,
  CalibrationTypes_Full_Calibration
} CalibrationTypes;

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
#endif                                 // position_velocity_cascade_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
