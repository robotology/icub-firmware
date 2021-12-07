//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity_private.h
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 2.29
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec  7 09:16:35 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_estimation_velocity_private_h_
#define RTW_HEADER_estimation_velocity_private_h_
#include "rtwtypes.h"

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

extern const real32_T rtCP_pooled_cBWEzfqKWJF1[32];

#define rtCP_Constant_Value            rtCP_pooled_cBWEzfqKWJF1  // Computed Parameter: rtCP_Constant_Value
                                                                 //  Referenced by: '<S1>/Constant'

#endif                             // RTW_HEADER_estimation_velocity_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
