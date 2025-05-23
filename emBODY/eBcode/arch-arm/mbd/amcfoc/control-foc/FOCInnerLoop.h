//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 8.9
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Fri May 23 10:48:16 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef FOCInnerLoop_h_
#define FOCInnerLoop_h_
#include "rtwtypes.h"
#include "control_foc_types.h"
#include "zero_crossing_types.h"

// Block signals for system '<Root>/FOC inner loop'
struct B_FOCInnerLoop_T {
  real32_T SumI1;                      // '<S127>/SumI1'
  real32_T Integrator;                 // '<S115>/Integrator'
  real32_T Switch;                     // '<S44>/Switch'
  real32_T Integrator_j;               // '<S57>/Integrator'
};

// Block states (default storage) for system '<Root>/FOC inner loop'
struct DW_FOCInnerLoop_T {
  real32_T FilterDifferentiatorTF_states;// '<S108>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S115>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S50>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S57>/Integrator'
  real32_T FilterDifferentiatorTF_tmp; // '<S108>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S50>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S115>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S57>/Integrator'
  uint8_T FilterDifferentiatorTF_icLoad;// '<S108>/Filter Differentiator TF'
  uint8_T Integrator_IC_LOADING;       // '<S115>/Integrator'
  uint8_T FilterDifferentiatorTF_icLoad_c;// '<S50>/Filter Differentiator TF'
  uint8_T Integrator_IC_LOADING_b;     // '<S57>/Integrator'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCE_FOCInnerLoop_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S108>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_o;// '<S50>/Filter Differentiator TF' 
};

extern void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop(const SensorsData *rtu_Sensors, const
  ActuatorConfiguration *rtu_ConfigurationParameters, const Targets
  *rtu_Targets_Inport_4, const ControlOuterOutputs *rtu_OuterOutputs, FOCOutputs
  *rty_FOCOutputs, B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW,
  ZCE_FOCInnerLoop_T *localZCE);

#endif                                 // FOCInnerLoop_h_

//
// File trailer for generated code.
//
// [EOF]
//
