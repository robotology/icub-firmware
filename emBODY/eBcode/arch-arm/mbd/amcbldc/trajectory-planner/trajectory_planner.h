//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: trajectory_planner.h
//
// Code generated for Simulink model 'trajectory_planner'.
//
// Model version                  : 2.33
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Oct 21 09:21:58 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef trajectory_planner_h_
#define trajectory_planner_h_
#include "rtwtypes.h"
#include "trajectory_planner_types.h"
#include "zero_crossing_types.h"

// Block signals for model 'trajectory_planner'
struct B_trajectory_planner_c_T {
  real_T Product2;                     // '<S7>/Product2'
  real_T Divide9[4];                   // '<S6>/Divide9'
  real_T OutportBufferForDen[4];
  real32_T In;                         // '<S9>/In'
};

// Block states (default storage) for model 'trajectory_planner'
struct DW_trajectory_planner_f_T {
  real_T UnitDelay_DSTATE;             // '<S4>/Unit Delay'
  real_T DelayInput1_DSTATE;           // '<S8>/Delay Input1'
  real_T DiscreteFilter_states[3];     // '<S4>/Discrete Filter'
  real_T DiscreteFilter_tmp;           // '<S4>/Discrete Filter'
  real32_T DelayInput1_DSTATE_j;       // '<S3>/Delay Input1'
  boolean_T UnitDelay1_DSTATE;         // '<S4>/Unit Delay1'
  uint8_T DiscreteFilter_icLoad;       // '<S4>/Discrete Filter'
};

// Zero-crossing (trigger) state for model 'trajectory_planner'
struct ZCE_trajectory_planner_T {
  ZCSigState DiscreteFilter_Reset_ZCE; // '<S4>/Discrete Filter'
  ZCSigState SampleandHold_Trig_ZCE;   // '<S5>/Sample and Hold'
  ZCSigState ComputeInitState_Trig_ZCE;// '<S4>/Compute Init State'
  ZCSigState ComputeCoefficients_Trig_ZCE;// '<S4>/Compute Coefficients'
};

struct MdlrefDW_trajectory_planner_T {
  B_trajectory_planner_c_T rtb;
  DW_trajectory_planner_f_T rtdw;
  ZCE_trajectory_planner_T rtzce;
};

// Model reference registration function
extern void trajectory_planner_initialize(ZCE_trajectory_planner_T *localZCE);
extern void trajectory_planner_Init(B_trajectory_planner_c_T *localB,
  DW_trajectory_planner_f_T *localDW);
extern void trajectory_planner(const Flags *rtu_Flags, const Targets
  *rtu_Targets, const SensorsData *rtu_SensorData, Targets *rty_Targets_out,
  B_trajectory_planner_c_T *localB, DW_trajectory_planner_f_T *localDW,
  ZCE_trajectory_planner_T *localZCE);

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
//  '<Root>' : 'trajectory_planner'
//  '<S1>'   : 'trajectory_planner/If Action Subsystem2'
//  '<S2>'   : 'trajectory_planner/If Action Subsystem3'
//  '<S3>'   : 'trajectory_planner/If Action Subsystem2/Detect Change'
//  '<S4>'   : 'trajectory_planner/If Action Subsystem2/Discrete Minimum Jerk'
//  '<S5>'   : 'trajectory_planner/If Action Subsystem2/velocity2time'
//  '<S6>'   : 'trajectory_planner/If Action Subsystem2/Discrete Minimum Jerk/Compute Coefficients'
//  '<S7>'   : 'trajectory_planner/If Action Subsystem2/Discrete Minimum Jerk/Compute Init State'
//  '<S8>'   : 'trajectory_planner/If Action Subsystem2/Discrete Minimum Jerk/Detect Change'
//  '<S9>'   : 'trajectory_planner/If Action Subsystem2/velocity2time/Sample and Hold'

#endif                                 // trajectory_planner_h_

//
// File trailer for generated code.
//
// [EOF]
//
