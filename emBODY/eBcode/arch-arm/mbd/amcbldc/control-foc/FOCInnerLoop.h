//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 10.9
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:00:35 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef FOCINNERLOOP_H_
#define FOCINNERLOOP_H_
#include "rtwtypes.h"
#include "control_foc_types.h"
#include "zero_crossing_types.h"

// Block signals for system '<Root>/FOC inner loop'
struct B_FOCInnerLoop_T {
  real32_T SumI1;                      // '<S138>/SumI1'
  real32_T Integrator;                 // '<S126>/Integrator'
  real32_T Switch;                     // '<S47>/Switch'
  real32_T Integrator_j;               // '<S60>/Integrator'
};

// Block states (default storage) for system '<Root>/FOC inner loop'
struct DW_FOCInnerLoop_T {
  real32_T FilterDifferentiatorTF_states;// '<S119>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S126>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S53>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S60>/Integrator'
  real32_T FilterDifferentiatorTF_tmp; // '<S119>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S53>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S126>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S60>/Integrator'
  uint8_T FilterDifferentiatorTF_icLoad;// '<S119>/Filter Differentiator TF'
  uint8_T Integrator_IC_LOADING;       // '<S126>/Integrator'
  uint8_T FilterDifferentiatorTF_icLoad_c;// '<S53>/Filter Differentiator TF'
  uint8_T Integrator_IC_LOADING_b;     // '<S60>/Integrator'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCE_FOCInnerLoop_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S119>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_o;// '<S53>/Filter Differentiator TF' 
};

extern void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop(const SensorsData *rtu_Sensors_Inport_1, const
  ActuatorConfiguration *rtu_ConfigurationParameters, const EstimatedData
  *rtu_Estimates_Inport_3, const Targets *rtu_Targets_Inport_4, const
  ControlOuterOutputs *rtu_OuterOutputs, FOCOutputs *rty_FOCOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW, ZCE_FOCInnerLoop_T
  *localZCE);

#endif                                 // FOCINNERLOOP_H_

//
// File trailer for generated code.
//
// [EOF]
//
