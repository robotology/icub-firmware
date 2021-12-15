//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity_types.h
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 2.29
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec 14 19:25:56 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_estimation_velocity_types_h_
#define RTW_HEADER_estimation_velocity_types_h_
#include "rtwtypes.h"

// Model Code Variants
#ifndef DEFINED_TYPEDEF_FOR_JointPositions_
#define DEFINED_TYPEDEF_FOR_JointPositions_

struct JointPositions
{
  // joint positions
  real32_T position;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

// electrical angle = angle * num_poles
struct MotorSensors
{
  real32_T Iabc[3];
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

#ifndef DEFINED_TYPEDEF_FOR_EstimatedData_
#define DEFINED_TYPEDEF_FOR_EstimatedData_

struct EstimatedData
{
  // velocities
  JointVelocities jointvelocities;
};

#endif

#ifndef struct_dsp_simulink_QRSolver_estimat_T
#define struct_dsp_simulink_QRSolver_estimat_T

struct dsp_simulink_QRSolver_estimat_T
{
  int32_T isInitialized;
};

#endif                                // struct_dsp_simulink_QRSolver_estimat_T
#endif                               // RTW_HEADER_estimation_velocity_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
