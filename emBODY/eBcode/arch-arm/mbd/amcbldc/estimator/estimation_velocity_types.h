//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity_types.h
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 8.11
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:00:44 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef ESTIMATION_VELOCITY_TYPES_H_
#define ESTIMATION_VELOCITY_TYPES_H_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_ESTIMATIONVELOCITYMODES_
#define DEFINED_TYPEDEF_FOR_ESTIMATIONVELOCITYMODES_

typedef enum {
  EstimationVelocityModes_Disabled = 0,// Default value
  EstimationVelocityModes_MovingAverage,
  EstimationVelocityModes_LeastSquares
} EstimationVelocityModes;

#endif

#ifndef STRUCT_DSP_SIMULINK_QRSOLVER_ESTIMATION_VELOCITY_T
#define STRUCT_DSP_SIMULINK_QRSOLVER_ESTIMATION_VELOCITY_T

struct dsp_simulink_QRSolver_estimation_velocity_T
{
  int32_T isInitialized;
};

#endif                    // STRUCT_DSP_SIMULINK_QRSOLVER_ESTIMATION_VELOCITY_T
#endif                                 // ESTIMATION_VELOCITY_TYPES_H_

//
// File trailer for generated code.
//
// [EOF]
//
