//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc_private.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 2.96
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Apr  1 11:36:16 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_foc_private_h_
#define RTW_HEADER_control_foc_private_h_
#include "rtwtypes.h"
#include "zero_crossing_types.h"

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

extern const real32_T rtCP_pooled_IgamRjjg0YgF[6];

#define rtCP_IaIbIc0_Gain              rtCP_pooled_IgamRjjg0YgF  // Computed Parameter: rtCP_IaIbIc0_Gain
                                                                 //  Referenced by: '<S1>/Ia+Ib+Ic=0'


// Invariant block signals (default storage)
extern const ConstB_control_foc_h_T control_foc_ConstB;

#endif                                 // RTW_HEADER_control_foc_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
