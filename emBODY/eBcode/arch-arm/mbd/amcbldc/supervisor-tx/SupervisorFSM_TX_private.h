//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX_private.h
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 7.4
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar 13 10:35:18 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_TX_private_h_
#define RTW_HEADER_SupervisorFSM_TX_private_h_
#include "rtwtypes.h"
#include "SupervisorFSM_TX_types.h"

// Block signals for model 'SupervisorFSM_TX'
struct B_SupervisorFSM_TX_c_T {
  boolean_T ev_foc;                    // '<Root>/SupervisorFSM_TX'
  boolean_T ev_status;                 // '<Root>/SupervisorFSM_TX'
};

// Block states (default storage) for model 'SupervisorFSM_TX'
struct DW_SupervisorFSM_TX_f_T {
  uint32_T ev_focEventCounter;         // '<Root>/SupervisorFSM_TX'
  uint32_T ev_statusEventCounter;      // '<Root>/SupervisorFSM_TX'
  boolean_T DelayInput1_DSTATE;        // '<S1>/Delay Input1'
  boolean_T DelayInput1_DSTATE_d;      // '<S2>/Delay Input1'
  uint8_T is_active_c3_SupervisorFSM_TX;// '<Root>/SupervisorFSM_TX'
};

// Real-time Model Data Structure
struct tag_RTM_SupervisorFSM_TX_T {
  const char_T **errorStatus;
};

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

struct MdlrefDW_SupervisorFSM_TX_T {
  RT_MODEL_SupervisorFSM_TX_T rtm;
};

extern MdlrefDW_SupervisorFSM_TX_T SupervisorFSM_TX_MdlrefDW;

// Block signals (default storage)
extern B_SupervisorFSM_TX_c_T SupervisorFSM_TX_B;

// Block states (default storage)
extern DW_SupervisorFSM_TX_f_T SupervisorFSM_TX_DW;

#endif                                // RTW_HEADER_SupervisorFSM_TX_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
