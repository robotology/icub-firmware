//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 7.5
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Aug 26 15:36:21 2024
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
  real32_T SumI1;                      // '<S122>/SumI1'
  real32_T Integrator;                 // '<S110>/Integrator'
  real32_T Switch;                     // '<S41>/Switch'
  real32_T Integrator_j;               // '<S54>/Integrator'
};

// Block states (default storage) for system '<Root>/FOC inner loop'
struct DW_FOCInnerLoop_T {
  real32_T FilterDifferentiatorTF_states;// '<S103>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S110>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S47>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S54>/Integrator'
  real32_T FilterDifferentiatorTF_tmp; // '<S103>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S47>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S110>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S54>/Integrator'
  uint8_T FilterDifferentiatorTF_icLoad;// '<S103>/Filter Differentiator TF'
  uint8_T Integrator_IC_LOADING;       // '<S110>/Integrator'
  uint8_T FilterDifferentiatorTF_icLoad_c;// '<S47>/Filter Differentiator TF'
  uint8_T Integrator_IC_LOADING_b;     // '<S54>/Integrator'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCE_FOCInnerLoop_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S103>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_o;// '<S47>/Filter Differentiator TF' 
};

extern void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW);
extern void FOCInnerLoop(const SensorsData *rtu_Sensors, const
  ActuatorConfiguration *rtu_ConfigurationParameters, const EstimatedData
  *rtu_Estimates_Inport_3, const Targets *rtu_Targets_Inport_4, const
  ControlOuterOutputs *rtu_OuterOutputs, FOCOutputs *rty_FOCOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW, ZCE_FOCInnerLoop_T
  *localZCE);

#endif                                 // FOCInnerLoop_h_

//
// File trailer for generated code.
//
// [EOF]
//
