//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: kalman_filter_data.cpp
//
// Code generated for Simulink model 'kalman_filter'.
//
// Model version                  : 1.10
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Apr  4 11:53:10 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "kalman_filter.h"
#include "kalman_filter_private.h"

// Invariant block signals (default storage)
const kalmanfilter::KalmanFilter::ConstB_kalman_filter_T kalman_filter_ConstB{
  {
    0.0F,
    0.0F,
    0.0F
  }
  ,                                    // '<S22>/B[k]*u[k]'

  {
    0.0F,
    0.0F,
    0.0F
  }
  ,                                    // '<S23>/Transpose'

  {
    0.0F,
    0.0F,
    0.0F
  }
  ,                                    // '<S23>/Transpose2'
  0.0F
  ,                                    // '<S23>/Product4'

  {
    1.0F,
    0.0F,
    0.0F,
    0.0F,
    1.0F,
    0.0F,
    0.0F,
    0.0F,
    1.0F
  }
  ,                                    // '<S23>/Transpose1'
  0.0F
  // '<S49>/D[k]*u[k]'
};

// Constant parameters (default storage)
const kalmanfilter::KalmanFilter::ConstP_kalman_filter_T kalman_filter_ConstP{
  // Computed Parameter: A_Value
  //  Referenced by: '<S2>/A'

  { 1.0F, 0.0F, 0.0F, 0.001F, 1.0F, 0.0F, 5.0E-7F, 0.001F, 1.0F },

  // Computed Parameter: C_Value
  //  Referenced by: '<S2>/C'

  { 1.0F, 0.0F, 0.0F },

  // Computed Parameter: G_Value
  //  Referenced by: '<S2>/G'

  { 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F }
};

//
// File trailer for generated code.
//
// [EOF]
//
