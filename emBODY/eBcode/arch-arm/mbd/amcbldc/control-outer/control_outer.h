//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 7.6
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Oct  7 15:56:09 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef control_outer_h_
#define control_outer_h_
#include "rtwtypes.h"
#include "control_outer_types.h"
#include "zero_crossing_types.h"

// Block signals for model 'control_outer'
struct B_control_outer_c_T {
  real32_T DiscreteTimeIntegrator;     // '<S1>/Discrete-Time Integrator'
};

// Block states (default storage) for model 'control_outer'
struct DW_control_outer_f_T {
  real32_T FilterDifferentiatorTF_states;// '<S49>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<Root>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S56>/Integrator'
  real32_T FilterDifferentiatorTF_states_i;// '<S101>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_i;        // '<S108>/Integrator'
  real32_T FilterDifferentiatorTF_states_c;// '<S153>/Filter Differentiator TF'
  real32_T UnitDelay1_DSTATE;          // '<Root>/Unit Delay1'
  real32_T Integrator_DSTATE_b;        // '<S160>/Integrator'
  real32_T DiscreteTimeIntegrator_DSTATE;// '<S1>/Discrete-Time Integrator'
  ControlModes DelayInput1_DSTATE;     // '<S18>/Delay Input1'
  real32_T FilterDifferentiatorTF_tmp; // '<S49>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_m;// '<S101>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_p;// '<S153>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S56>/Integrator'
  int8_T Integrator_PrevResetState_n;  // '<S108>/Integrator'
  int8_T Integrator_PrevResetState_c;  // '<S160>/Integrator'
  int8_T DiscreteTimeIntegrator_PrevResetState;// '<S1>/Discrete-Time Integrator' 
  uint8_T DiscreteTimeIntegrator_SYSTEM_ENABLE;// '<S1>/Discrete-Time Integrator' 
  boolean_T Memory_PreviousInput;      // '<S9>/Memory'
};

// Zero-crossing (trigger) state for model 'control_outer'
struct ZCE_control_outer_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S49>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_m;// '<S101>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_e;// '<S153>/Filter Differentiator TF' 
};

struct MdlrefDW_control_outer_T {
  B_control_outer_c_T rtb;
  DW_control_outer_f_T rtdw;
  ZCE_control_outer_T rtzce;
};

// Model reference registration function
extern void control_outer_initialize(ZCE_control_outer_T *localZCE);
extern void control_outer_Init(DW_control_outer_f_T *localDW);
extern void control_outer_Enable(DW_control_outer_f_T *localDW);
extern void control_outer_Disable(B_control_outer_c_T *localB,
  DW_control_outer_f_T *localDW);
extern void control_outer(const Flags *rtu_Flags, const ActuatorConfiguration
  *rtu_ConfigurationParameters, const EstimatedData *rtu_Estimates, const
  SensorsData *rtu_Sensors, const Targets *rtu_Targets, ControlOuterOutputs
  *rty_OuterOutputs, B_control_outer_c_T *localB, DW_control_outer_f_T *localDW,
  ZCE_control_outer_T *localZCE);

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S4>/Data Type Propagation' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Propagation' : Unused code path elimination
//  Block '<S49>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S67>/Kt' : Eliminated nontunable gain of 1
//  Block '<S101>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S119>/Kt' : Eliminated nontunable gain of 1
//  Block '<S153>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S171>/Kt' : Eliminated nontunable gain of 1


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
//  '<Root>' : 'control_outer'
//  '<S1>'   : 'control_outer/Current Limiter'
//  '<S2>'   : 'control_outer/Enabling Logic'
//  '<S3>'   : 'control_outer/Position PID control'
//  '<S4>'   : 'control_outer/Saturation Dynamic'
//  '<S5>'   : 'control_outer/Saturation Dynamic1'
//  '<S6>'   : 'control_outer/Velocity PID control'
//  '<S7>'   : 'control_outer/Current Limiter/Compare To Zero'
//  '<S8>'   : 'control_outer/Current Limiter/Compare To Zero1'
//  '<S9>'   : 'control_outer/Current Limiter/S-R Flip-Flop'
//  '<S10>'  : 'control_outer/Enabling Logic/Compare To Constant'
//  '<S11>'  : 'control_outer/Enabling Logic/Compare To Constant1'
//  '<S12>'  : 'control_outer/Enabling Logic/Compare To Constant2'
//  '<S13>'  : 'control_outer/Enabling Logic/Compare To Constant3'
//  '<S14>'  : 'control_outer/Enabling Logic/Compare To Constant4'
//  '<S15>'  : 'control_outer/Enabling Logic/Compare To Constant5'
//  '<S16>'  : 'control_outer/Enabling Logic/Compare To Constant6'
//  '<S17>'  : 'control_outer/Enabling Logic/Compare To Constant7'
//  '<S18>'  : 'control_outer/Enabling Logic/Detect Change'
//  '<S19>'  : 'control_outer/Position PID control/Position PID'
//  '<S20>'  : 'control_outer/Position PID control/Position-Direct PID'
//  '<S21>'  : 'control_outer/Position PID control/Position PID/Anti-windup'
//  '<S22>'  : 'control_outer/Position PID control/Position PID/D Gain'
//  '<S23>'  : 'control_outer/Position PID control/Position PID/External Derivative'
//  '<S24>'  : 'control_outer/Position PID control/Position PID/Filter'
//  '<S25>'  : 'control_outer/Position PID control/Position PID/Filter ICs'
//  '<S26>'  : 'control_outer/Position PID control/Position PID/I Gain'
//  '<S27>'  : 'control_outer/Position PID control/Position PID/Ideal P Gain'
//  '<S28>'  : 'control_outer/Position PID control/Position PID/Ideal P Gain Fdbk'
//  '<S29>'  : 'control_outer/Position PID control/Position PID/Integrator'
//  '<S30>'  : 'control_outer/Position PID control/Position PID/Integrator ICs'
//  '<S31>'  : 'control_outer/Position PID control/Position PID/N Copy'
//  '<S32>'  : 'control_outer/Position PID control/Position PID/N Gain'
//  '<S33>'  : 'control_outer/Position PID control/Position PID/P Copy'
//  '<S34>'  : 'control_outer/Position PID control/Position PID/Parallel P Gain'
//  '<S35>'  : 'control_outer/Position PID control/Position PID/Reset Signal'
//  '<S36>'  : 'control_outer/Position PID control/Position PID/Saturation'
//  '<S37>'  : 'control_outer/Position PID control/Position PID/Saturation Fdbk'
//  '<S38>'  : 'control_outer/Position PID control/Position PID/Sum'
//  '<S39>'  : 'control_outer/Position PID control/Position PID/Sum Fdbk'
//  '<S40>'  : 'control_outer/Position PID control/Position PID/Tracking Mode'
//  '<S41>'  : 'control_outer/Position PID control/Position PID/Tracking Mode Sum'
//  '<S42>'  : 'control_outer/Position PID control/Position PID/Tsamp - Integral'
//  '<S43>'  : 'control_outer/Position PID control/Position PID/Tsamp - Ngain'
//  '<S44>'  : 'control_outer/Position PID control/Position PID/postSat Signal'
//  '<S45>'  : 'control_outer/Position PID control/Position PID/preSat Signal'
//  '<S46>'  : 'control_outer/Position PID control/Position PID/Anti-windup/Passthrough'
//  '<S47>'  : 'control_outer/Position PID control/Position PID/D Gain/External Parameters'
//  '<S48>'  : 'control_outer/Position PID control/Position PID/External Derivative/Error'
//  '<S49>'  : 'control_outer/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S50>'  : 'control_outer/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S51>'  : 'control_outer/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S52>'  : 'control_outer/Position PID control/Position PID/Filter ICs/Internal IC - Filter'
//  '<S53>'  : 'control_outer/Position PID control/Position PID/I Gain/External Parameters'
//  '<S54>'  : 'control_outer/Position PID control/Position PID/Ideal P Gain/Passthrough'
//  '<S55>'  : 'control_outer/Position PID control/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S56>'  : 'control_outer/Position PID control/Position PID/Integrator/Discrete'
//  '<S57>'  : 'control_outer/Position PID control/Position PID/Integrator ICs/Internal IC'
//  '<S58>'  : 'control_outer/Position PID control/Position PID/N Copy/External Parameters'
//  '<S59>'  : 'control_outer/Position PID control/Position PID/N Gain/External Parameters'
//  '<S60>'  : 'control_outer/Position PID control/Position PID/P Copy/Disabled'
//  '<S61>'  : 'control_outer/Position PID control/Position PID/Parallel P Gain/External Parameters'
//  '<S62>'  : 'control_outer/Position PID control/Position PID/Reset Signal/External Reset'
//  '<S63>'  : 'control_outer/Position PID control/Position PID/Saturation/Passthrough'
//  '<S64>'  : 'control_outer/Position PID control/Position PID/Saturation Fdbk/Passthrough'
//  '<S65>'  : 'control_outer/Position PID control/Position PID/Sum/Sum_PID'
//  '<S66>'  : 'control_outer/Position PID control/Position PID/Sum Fdbk/Enabled'
//  '<S67>'  : 'control_outer/Position PID control/Position PID/Tracking Mode/Enabled'
//  '<S68>'  : 'control_outer/Position PID control/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S69>'  : 'control_outer/Position PID control/Position PID/Tsamp - Integral/TsSignalSpecification'
//  '<S70>'  : 'control_outer/Position PID control/Position PID/Tsamp - Ngain/Passthrough'
//  '<S71>'  : 'control_outer/Position PID control/Position PID/postSat Signal/Feedback_Path'
//  '<S72>'  : 'control_outer/Position PID control/Position PID/preSat Signal/Feedback_Path'
//  '<S73>'  : 'control_outer/Position PID control/Position-Direct PID/Anti-windup'
//  '<S74>'  : 'control_outer/Position PID control/Position-Direct PID/D Gain'
//  '<S75>'  : 'control_outer/Position PID control/Position-Direct PID/External Derivative'
//  '<S76>'  : 'control_outer/Position PID control/Position-Direct PID/Filter'
//  '<S77>'  : 'control_outer/Position PID control/Position-Direct PID/Filter ICs'
//  '<S78>'  : 'control_outer/Position PID control/Position-Direct PID/I Gain'
//  '<S79>'  : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain'
//  '<S80>'  : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S81>'  : 'control_outer/Position PID control/Position-Direct PID/Integrator'
//  '<S82>'  : 'control_outer/Position PID control/Position-Direct PID/Integrator ICs'
//  '<S83>'  : 'control_outer/Position PID control/Position-Direct PID/N Copy'
//  '<S84>'  : 'control_outer/Position PID control/Position-Direct PID/N Gain'
//  '<S85>'  : 'control_outer/Position PID control/Position-Direct PID/P Copy'
//  '<S86>'  : 'control_outer/Position PID control/Position-Direct PID/Parallel P Gain'
//  '<S87>'  : 'control_outer/Position PID control/Position-Direct PID/Reset Signal'
//  '<S88>'  : 'control_outer/Position PID control/Position-Direct PID/Saturation'
//  '<S89>'  : 'control_outer/Position PID control/Position-Direct PID/Saturation Fdbk'
//  '<S90>'  : 'control_outer/Position PID control/Position-Direct PID/Sum'
//  '<S91>'  : 'control_outer/Position PID control/Position-Direct PID/Sum Fdbk'
//  '<S92>'  : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode'
//  '<S93>'  : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode Sum'
//  '<S94>'  : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Integral'
//  '<S95>'  : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Ngain'
//  '<S96>'  : 'control_outer/Position PID control/Position-Direct PID/postSat Signal'
//  '<S97>'  : 'control_outer/Position PID control/Position-Direct PID/preSat Signal'
//  '<S98>'  : 'control_outer/Position PID control/Position-Direct PID/Anti-windup/Passthrough'
//  '<S99>'  : 'control_outer/Position PID control/Position-Direct PID/D Gain/External Parameters'
//  '<S100>' : 'control_outer/Position PID control/Position-Direct PID/External Derivative/Error'
//  '<S101>' : 'control_outer/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S102>' : 'control_outer/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S103>' : 'control_outer/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S104>' : 'control_outer/Position PID control/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S105>' : 'control_outer/Position PID control/Position-Direct PID/I Gain/External Parameters'
//  '<S106>' : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S107>' : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S108>' : 'control_outer/Position PID control/Position-Direct PID/Integrator/Discrete'
//  '<S109>' : 'control_outer/Position PID control/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S110>' : 'control_outer/Position PID control/Position-Direct PID/N Copy/External Parameters'
//  '<S111>' : 'control_outer/Position PID control/Position-Direct PID/N Gain/External Parameters'
//  '<S112>' : 'control_outer/Position PID control/Position-Direct PID/P Copy/Disabled'
//  '<S113>' : 'control_outer/Position PID control/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S114>' : 'control_outer/Position PID control/Position-Direct PID/Reset Signal/External Reset'
//  '<S115>' : 'control_outer/Position PID control/Position-Direct PID/Saturation/Passthrough'
//  '<S116>' : 'control_outer/Position PID control/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S117>' : 'control_outer/Position PID control/Position-Direct PID/Sum/Sum_PID'
//  '<S118>' : 'control_outer/Position PID control/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S119>' : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode/Enabled'
//  '<S120>' : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S121>' : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Integral/TsSignalSpecification'
//  '<S122>' : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S123>' : 'control_outer/Position PID control/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S124>' : 'control_outer/Position PID control/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S125>' : 'control_outer/Velocity PID control/Anti-windup'
//  '<S126>' : 'control_outer/Velocity PID control/D Gain'
//  '<S127>' : 'control_outer/Velocity PID control/External Derivative'
//  '<S128>' : 'control_outer/Velocity PID control/Filter'
//  '<S129>' : 'control_outer/Velocity PID control/Filter ICs'
//  '<S130>' : 'control_outer/Velocity PID control/I Gain'
//  '<S131>' : 'control_outer/Velocity PID control/Ideal P Gain'
//  '<S132>' : 'control_outer/Velocity PID control/Ideal P Gain Fdbk'
//  '<S133>' : 'control_outer/Velocity PID control/Integrator'
//  '<S134>' : 'control_outer/Velocity PID control/Integrator ICs'
//  '<S135>' : 'control_outer/Velocity PID control/N Copy'
//  '<S136>' : 'control_outer/Velocity PID control/N Gain'
//  '<S137>' : 'control_outer/Velocity PID control/P Copy'
//  '<S138>' : 'control_outer/Velocity PID control/Parallel P Gain'
//  '<S139>' : 'control_outer/Velocity PID control/Reset Signal'
//  '<S140>' : 'control_outer/Velocity PID control/Saturation'
//  '<S141>' : 'control_outer/Velocity PID control/Saturation Fdbk'
//  '<S142>' : 'control_outer/Velocity PID control/Sum'
//  '<S143>' : 'control_outer/Velocity PID control/Sum Fdbk'
//  '<S144>' : 'control_outer/Velocity PID control/Tracking Mode'
//  '<S145>' : 'control_outer/Velocity PID control/Tracking Mode Sum'
//  '<S146>' : 'control_outer/Velocity PID control/Tsamp - Integral'
//  '<S147>' : 'control_outer/Velocity PID control/Tsamp - Ngain'
//  '<S148>' : 'control_outer/Velocity PID control/postSat Signal'
//  '<S149>' : 'control_outer/Velocity PID control/preSat Signal'
//  '<S150>' : 'control_outer/Velocity PID control/Anti-windup/Passthrough'
//  '<S151>' : 'control_outer/Velocity PID control/D Gain/External Parameters'
//  '<S152>' : 'control_outer/Velocity PID control/External Derivative/Error'
//  '<S153>' : 'control_outer/Velocity PID control/Filter/Disc. Trapezoidal Filter'
//  '<S154>' : 'control_outer/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S155>' : 'control_outer/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S156>' : 'control_outer/Velocity PID control/Filter ICs/Internal IC - Filter'
//  '<S157>' : 'control_outer/Velocity PID control/I Gain/External Parameters'
//  '<S158>' : 'control_outer/Velocity PID control/Ideal P Gain/Passthrough'
//  '<S159>' : 'control_outer/Velocity PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S160>' : 'control_outer/Velocity PID control/Integrator/Discrete'
//  '<S161>' : 'control_outer/Velocity PID control/Integrator ICs/Internal IC'
//  '<S162>' : 'control_outer/Velocity PID control/N Copy/External Parameters'
//  '<S163>' : 'control_outer/Velocity PID control/N Gain/External Parameters'
//  '<S164>' : 'control_outer/Velocity PID control/P Copy/Disabled'
//  '<S165>' : 'control_outer/Velocity PID control/Parallel P Gain/External Parameters'
//  '<S166>' : 'control_outer/Velocity PID control/Reset Signal/External Reset'
//  '<S167>' : 'control_outer/Velocity PID control/Saturation/Passthrough'
//  '<S168>' : 'control_outer/Velocity PID control/Saturation Fdbk/Passthrough'
//  '<S169>' : 'control_outer/Velocity PID control/Sum/Sum_PID'
//  '<S170>' : 'control_outer/Velocity PID control/Sum Fdbk/Enabled'
//  '<S171>' : 'control_outer/Velocity PID control/Tracking Mode/Enabled'
//  '<S172>' : 'control_outer/Velocity PID control/Tracking Mode Sum/Tracking Mode'
//  '<S173>' : 'control_outer/Velocity PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S174>' : 'control_outer/Velocity PID control/Tsamp - Ngain/Passthrough'
//  '<S175>' : 'control_outer/Velocity PID control/postSat Signal/Feedback_Path'
//  '<S176>' : 'control_outer/Velocity PID control/preSat Signal/Feedback_Path'

#endif                                 // control_outer_h_

//
// File trailer for generated code.
//
// [EOF]
//
