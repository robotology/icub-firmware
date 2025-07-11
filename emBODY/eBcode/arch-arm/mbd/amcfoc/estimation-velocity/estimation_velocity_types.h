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
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Tue Jul  8 13:04:57 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef estimation_velocity_types_h_
#define estimation_velocity_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_EstimationVelocityModes_
#define DEFINED_TYPEDEF_FOR_EstimationVelocityModes_

typedef enum {
  EstimationVelocityModes_Disabled = 0,// Default value
  EstimationVelocityModes_MovingAverage,
  EstimationVelocityModes_LeastSquares
} EstimationVelocityModes;

#endif

#ifndef struct_dsp_simulink_QRSolver_estimation_velocity_T
#define struct_dsp_simulink_QRSolver_estimation_velocity_T

struct dsp_simulink_QRSolver_estimation_velocity_T
{
  int32_T isInitialized;
};

#endif                    // struct_dsp_simulink_QRSolver_estimation_velocity_T
#endif                                 // estimation_velocity_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
