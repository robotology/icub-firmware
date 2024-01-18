//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.16
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Mon Jan 15 18:21:34 2024
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

extern "C"
{

#include "rt_nonfinite.h"

}

#include "rtGetNaN.h"
#include "zero_crossing_types.h"

// Block signals for system '<Root>/FOC inner loop'
struct B_FOCInnerLoop_T {
  real32_T SumI1;                      // '<S119>/SumI1'
  real32_T Integrator;                 // '<S107>/Integrator'
  real32_T Switch;                     // '<S41>/Switch'
  real32_T Integrator_j;               // '<S53>/Integrator'
  real32_T Sum3;                       // '<S1>/Sum3'
};

// Block states (default storage) for system '<Root>/FOC inner loop'
struct DW_FOCInnerLoop_T {
  real32_T FilterDifferentiatorTF_states;// '<S100>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S107>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S46>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S53>/Integrator'
  real32_T FilterDifferentiatorTF_tmp; // '<S100>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S46>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S107>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S53>/Integrator'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCE_FOCInnerLoop_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S100>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_o;// '<S46>/Filter Differentiator TF' 
};

// Invariant block signals for system '<Root>/FOC inner loop'
struct ConstB_FOCInnerLoop_T {
  real32_T Gain5;                      // '<S1>/Gain5'
  real32_T Sum5;                       // '<S1>/Sum5'
};

extern void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop(const ConfigurationParameters
  *rtu_ConfigurationParameters, const SensorsData *rtu_Sensors, const
  EstimatedData *rtu_Estimates, const Targets *rtu_Targets, const
  ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs *rty_FOCOutputs,
  B_FOCInnerLoop_T *localB, const ConstB_FOCInnerLoop_T *localC,
  DW_FOCInnerLoop_T *localDW, ZCE_FOCInnerLoop_T *localZCE);

#endif                                 // RTW_HEADER_FOCInnerLoop_h_

//
// File trailer for generated code.
//
// [EOF]
//
