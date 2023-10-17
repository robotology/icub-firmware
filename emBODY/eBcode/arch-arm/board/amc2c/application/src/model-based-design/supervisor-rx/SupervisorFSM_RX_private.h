//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX_private.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 6.4
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Mon Oct 16 10:07:15 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_RX_private_h_
#define RTW_HEADER_SupervisorFSM_RX_private_h_
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"
#include "SupervisorFSM_RX.h"

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

// Invariant block signals (default storage)
extern const ConstB_SupervisorFSM_RX_h_T SupervisorFSM_RX_ConstB;

#endif                                // RTW_HEADER_SupervisorFSM_RX_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
