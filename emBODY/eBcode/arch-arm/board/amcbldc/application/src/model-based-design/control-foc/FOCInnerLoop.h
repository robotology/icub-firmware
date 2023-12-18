//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.4
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Thu Dec 14 10:43:43 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_FOCInnerLoop_h_
#define RTW_HEADER_FOCInnerLoop_h_
#include "rtwtypes.h"
#include "control_foc_types.h"
#include "zero_crossing_types.h"

// Block signals for system '<S1>/Moving RMS'
struct B_MovingRMS_control_foc_T {
  real32_T MovingRMS;                  // '<S1>/Moving RMS'
};

// Block states (default storage) for system '<S1>/Moving RMS'
struct DW_MovingRMS_control_foc_T {
  dsp_simulink_MovingRMS_control_foc_T obj;// '<S1>/Moving RMS'
  boolean_T objisempty;                // '<S1>/Moving RMS'
};

// Block signals for system '<Root>/FOC inner loop'
struct B_FOCInnerLoop_T {
  real32_T SumI1;                      // '<S114>/SumI1'
  real32_T Integrator;                 // '<S102>/Integrator'
  real32_T Switch;                     // '<S36>/Switch'
  real32_T Integrator_j;               // '<S48>/Integrator'
  real32_T Sum3;                       // '<S1>/Sum3'
  B_MovingRMS_control_foc_T MovingRMS1;// '<S1>/Moving RMS'
  B_MovingRMS_control_foc_T MovingRMS; // '<S1>/Moving RMS'
};

// Block states (default storage) for system '<Root>/FOC inner loop'
struct DW_FOCInnerLoop_T {
  real32_T FilterDifferentiatorTF_states;// '<S95>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S102>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S41>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S48>/Integrator'
  real32_T FilterDifferentiatorTF_tmp; // '<S95>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S41>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S102>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S48>/Integrator'
  DW_MovingRMS_control_foc_T MovingRMS1;// '<S1>/Moving RMS'
  DW_MovingRMS_control_foc_T MovingRMS;// '<S1>/Moving RMS'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCE_FOCInnerLoop_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S95>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_o;// '<S41>/Filter Differentiator TF' 
};

// Invariant block signals for system '<Root>/FOC inner loop'
struct ConstB_FOCInnerLoop_T {
  real32_T Gain5;                      // '<S1>/Gain5'
  real32_T Sum5;                       // '<S1>/Sum5'
};

extern void control_foc_MovingRMS_Init(DW_MovingRMS_control_foc_T *localDW);
extern void control_foc_MovingRMS(real32_T rtu_0, real32_T rtu_1,
  B_MovingRMS_control_foc_T *localB, DW_MovingRMS_control_foc_T *localDW);
extern void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop(const ConfigurationParameters
  *rtu_ConfigurationParameters, const SensorsData *rtu_Sensors, const
  EstimatedData *rtu_Estimates, const Targets *rtu_Targets, const
  ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs *rty_FOCOutputs,
  B_FOCInnerLoop_T *localB, const ConstB_FOCInnerLoop_T *localC,
  DW_FOCInnerLoop_T *localDW, ZCE_FOCInnerLoop_T *localZCE);
extern void control_foc_MovingRMS_Term(DW_MovingRMS_control_foc_T *localDW);
extern void FOCInnerLoop_Term(DW_FOCInnerLoop_T *localDW);

#endif                                 // RTW_HEADER_FOCInnerLoop_h_

//
// File trailer for generated code.
//
// [EOF]
//
