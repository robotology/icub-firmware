//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity_private.h
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 4.0
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Mon Mar 13 14:26:33 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_estimation_velocity_private_h_
#define RTW_HEADER_estimation_velocity_private_h_
#include "rtwtypes.h"
#include "estimation_velocity_types.h"

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         (*((rtm)->errorStatus))
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    (*((rtm)->errorStatus) = (val))
#endif

#ifndef rtmGetErrorStatusPointer
#define rtmGetErrorStatusPointer(rtm)  (rtm)->errorStatus
#endif

#ifndef rtmSetErrorStatusPointer
#define rtmSetErrorStatusPointer(rtm, val) ((rtm)->errorStatus = (val))
#endif

extern const real32_T rtCP_pooled_Az3IVI54Pn7X[32];

#define rtCP_Constant_Value_c          rtCP_pooled_Az3IVI54Pn7X  // Computed Parameter: rtCP_Constant_Value_c
                                                                 //  Referenced by: '<S1>/Constant'

#endif                             // RTW_HEADER_estimation_velocity_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
