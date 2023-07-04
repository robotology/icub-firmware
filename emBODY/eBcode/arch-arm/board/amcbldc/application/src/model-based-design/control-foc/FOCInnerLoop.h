//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 5.12
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Tue Jun 27 10:18:32 2023
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
  real32_T SumI1;                      // '<S109>/SumI1'
  real32_T Integrator;                 // '<S97>/Integrator'
  real32_T Switch;                     // '<S33>/Switch'
  real32_T Integrator_j;               // '<S45>/Integrator'
  real32_T Sum3;                       // '<S1>/Sum3'
  B_MovingRMS_control_foc_T MovingRMS1;// '<S1>/Moving RMS'
  B_MovingRMS_control_foc_T MovingRMS; // '<S1>/Moving RMS'
};

// Block states (default storage) for system '<Root>/FOC inner loop'
struct DW_FOCInnerLoop_T {
  real32_T FilterDifferentiatorTF_states;// '<S90>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S97>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S38>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S45>/Integrator'
  real32_T FilterDifferentiatorTF_tmp; // '<S90>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S38>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S97>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S45>/Integrator'
  DW_MovingRMS_control_foc_T MovingRMS1;// '<S1>/Moving RMS'
  DW_MovingRMS_control_foc_T MovingRMS;// '<S1>/Moving RMS'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCV_FOCInnerLoop_T {
  real_T FilterDifferentiatorTF_Reset_ZC;// '<S90>/Filter Differentiator TF'
  real_T FilterDifferentiatorTF_Reset_ZC_c;// '<S38>/Filter Differentiator TF'
};

// Zero-crossing (trigger) state for system '<Root>/FOC inner loop'
struct ZCE_FOCInnerLoop_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S90>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_o;// '<S38>/Filter Differentiator TF' 
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
