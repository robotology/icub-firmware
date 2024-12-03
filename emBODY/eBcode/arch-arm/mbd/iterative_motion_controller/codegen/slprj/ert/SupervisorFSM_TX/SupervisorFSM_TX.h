//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX.h
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 8.2
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Thu Oct 10 13:02:14 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef SupervisorFSM_TX_h_
#define SupervisorFSM_TX_h_
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

struct MdlrefDW_SupervisorFSM_TX_T {
  B_SupervisorFSM_TX_c_T rtb;
  DW_SupervisorFSM_TX_f_T rtdw;
};

extern void SupervisorFSM_TX_Init(BUS_MESSAGES_TX *rty_MessagesTx);
extern void SupervisorFSM_TX(const SensorsData *rtu_SensorsData, const
  EstimatedData *rtu_EstimatedData, const Flags *rtu_Flags, const FOCOutputs
  *rtu_ControlOutputs, const boolean_T *rtu_ExternalFlags_fault_button,
  BUS_MESSAGES_TX *rty_MessagesTx, BUS_STATUS_TX *rty_StatusTx,
  B_SupervisorFSM_TX_c_T *localB, DW_SupervisorFSM_TX_f_T *localDW);

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'SupervisorFSM_TX'
//  '<S1>'   : 'SupervisorFSM_TX/Detect Change'
//  '<S2>'   : 'SupervisorFSM_TX/Detect Change1'
//  '<S3>'   : 'SupervisorFSM_TX/SupervisorFSM_TX'

#endif                                 // SupervisorFSM_TX_h_

//
// File trailer for generated code.
//
// [EOF]
//
