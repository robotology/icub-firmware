//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_private.h
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 5.32
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Tue Jul  8 15:26:44 2025
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
  // Expression: EstimationVelocityModes.Disabled
  //  Referenced by: '<S1>/Constant'

  EstimationVelocityModes Constant_Value;

  // Expression: EstimationVelocityModes.MovingAverage
  //  Referenced by: '<S2>/Velocity Estimation Mode'

  EstimationVelocityModes VelocityEstimationMode_Value;
};

extern const uint32_T rtCP_pooled_UdZPJ5vElovg;

#define rtCP_Velocityestimationwindow_Value rtCP_pooled_UdZPJ5vElovg// Computed Parameter: rtCP_Velocityestimationwindow_Value
                                                                    //  Referenced by: '<S2>/Velocity estimation window'


// Constant parameters (default storage)
extern const ConstP_motion_controller_T motion_controller_ConstP;

#endif                                 // motion_controller_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
