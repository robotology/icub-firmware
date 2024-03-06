//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX.h
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 7.4
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar  6 15:00:19 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_TX_h_
#define RTW_HEADER_SupervisorFSM_TX_h_
#include "rtwtypes.h"
#include "SupervisorFSM_TX_types.h"

extern void SupervisorFSM_TX_Init(BUS_MESSAGES_TX *rty_MessagesTx);
extern void SupervisorFSM_TX(const SensorsData *rtu_SensorsData, const
  EstimatedData *rtu_EstimatedData, const Flags *rtu_Flags, const ControlOutputs
  *rtu_ControlOutputs, BUS_MESSAGES_TX *rty_MessagesTx, BUS_STATUS_TX
  *rty_StatusTx);

// Model reference registration function
extern void SupervisorFSM_TX_initialize(const char_T **rt_errorStatus);

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

#endif                                 // RTW_HEADER_SupervisorFSM_TX_h_

//
// File trailer for generated code.
//
// [EOF]
//
