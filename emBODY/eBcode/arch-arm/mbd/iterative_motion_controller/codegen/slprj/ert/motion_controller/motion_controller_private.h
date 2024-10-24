//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_private.h
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 3.12
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Thu Oct 10 12:24:40 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef motion_controller_private_h_
#define motion_controller_private_h_
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "motion_controller_types.h"

// Constant parameters (default storage)
struct ConstP_motion_controller_T {
  // Expression: EstimationVelocityModes.MovingAverage
  //  Referenced by: '<S2>/Velocity Estimation Mode'

  EstimationVelocityModes VelocityEstimationMode_Value;
};

// Constant parameters (default storage)
extern const ConstP_motion_controller_T motion_controller_ConstP;

#endif                                 // motion_controller_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
