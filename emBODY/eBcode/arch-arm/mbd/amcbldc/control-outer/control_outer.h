//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 8.0
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Wed Jun  4 17:58:22 2025
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
  real32_T FilterDifferentiatorTF_states;// '<S50>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<Root>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S57>/Integrator'
  real32_T FilterDifferentiatorTF_states_i;// '<S104>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_i;        // '<S111>/Integrator'
  real32_T FilterDifferentiatorTF_states_c;// '<S158>/Filter Differentiator TF'
  real32_T UnitDelay1_DSTATE;          // '<Root>/Unit Delay1'
  real32_T Integrator_DSTATE_b;        // '<S165>/Integrator'
  real32_T DiscreteTimeIntegrator_DSTATE;// '<S1>/Discrete-Time Integrator'
  ControlModes DelayInput1_DSTATE;     // '<S18>/Delay Input1'
  real32_T FilterDifferentiatorTF_tmp; // '<S50>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_m;// '<S104>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_p;// '<S158>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S57>/Integrator'
  int8_T Integrator_PrevResetState_n;  // '<S111>/Integrator'
  int8_T Integrator_PrevResetState_c;  // '<S165>/Integrator'
  int8_T DiscreteTimeIntegrator_PrevResetState;// '<S1>/Discrete-Time Integrator' 
  uint8_T DiscreteTimeIntegrator_SYSTEM_ENABLE;// '<S1>/Discrete-Time Integrator' 
  boolean_T Memory_PreviousInput;      // '<S9>/Memory'
};

// Zero-crossing (trigger) state for model 'control_outer'
struct ZCE_control_outer_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S50>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_m;// '<S104>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_e;// '<S158>/Filter Differentiator TF' 
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
//  Block '<S50>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S68>/Kt' : Eliminated nontunable gain of 1
//  Block '<S104>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S122>/Kt' : Eliminated nontunable gain of 1
//  Block '<S158>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S176>/Kt' : Eliminated nontunable gain of 1


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
//  '<S45>'  : 'control_outer/Position PID control/Position PID/preInt Signal'
//  '<S46>'  : 'control_outer/Position PID control/Position PID/preSat Signal'
//  '<S47>'  : 'control_outer/Position PID control/Position PID/Anti-windup/Passthrough'
//  '<S48>'  : 'control_outer/Position PID control/Position PID/D Gain/External Parameters'
//  '<S49>'  : 'control_outer/Position PID control/Position PID/External Derivative/Error'
//  '<S50>'  : 'control_outer/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S51>'  : 'control_outer/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S52>'  : 'control_outer/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S53>'  : 'control_outer/Position PID control/Position PID/Filter ICs/Internal IC - Filter'
//  '<S54>'  : 'control_outer/Position PID control/Position PID/I Gain/External Parameters'
//  '<S55>'  : 'control_outer/Position PID control/Position PID/Ideal P Gain/Passthrough'
//  '<S56>'  : 'control_outer/Position PID control/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S57>'  : 'control_outer/Position PID control/Position PID/Integrator/Discrete'
//  '<S58>'  : 'control_outer/Position PID control/Position PID/Integrator ICs/Internal IC'
//  '<S59>'  : 'control_outer/Position PID control/Position PID/N Copy/External Parameters'
//  '<S60>'  : 'control_outer/Position PID control/Position PID/N Gain/External Parameters'
//  '<S61>'  : 'control_outer/Position PID control/Position PID/P Copy/Disabled'
//  '<S62>'  : 'control_outer/Position PID control/Position PID/Parallel P Gain/External Parameters'
//  '<S63>'  : 'control_outer/Position PID control/Position PID/Reset Signal/External Reset'
//  '<S64>'  : 'control_outer/Position PID control/Position PID/Saturation/Passthrough'
//  '<S65>'  : 'control_outer/Position PID control/Position PID/Saturation Fdbk/Passthrough'
//  '<S66>'  : 'control_outer/Position PID control/Position PID/Sum/Sum_PID'
//  '<S67>'  : 'control_outer/Position PID control/Position PID/Sum Fdbk/Enabled'
//  '<S68>'  : 'control_outer/Position PID control/Position PID/Tracking Mode/Enabled'
//  '<S69>'  : 'control_outer/Position PID control/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S70>'  : 'control_outer/Position PID control/Position PID/Tsamp - Integral/TsSignalSpecification'
//  '<S71>'  : 'control_outer/Position PID control/Position PID/Tsamp - Ngain/Passthrough'
//  '<S72>'  : 'control_outer/Position PID control/Position PID/postSat Signal/Feedback_Path'
//  '<S73>'  : 'control_outer/Position PID control/Position PID/preInt Signal/Internal PreInt'
//  '<S74>'  : 'control_outer/Position PID control/Position PID/preSat Signal/Feedback_Path'
//  '<S75>'  : 'control_outer/Position PID control/Position-Direct PID/Anti-windup'
//  '<S76>'  : 'control_outer/Position PID control/Position-Direct PID/D Gain'
//  '<S77>'  : 'control_outer/Position PID control/Position-Direct PID/External Derivative'
//  '<S78>'  : 'control_outer/Position PID control/Position-Direct PID/Filter'
//  '<S79>'  : 'control_outer/Position PID control/Position-Direct PID/Filter ICs'
//  '<S80>'  : 'control_outer/Position PID control/Position-Direct PID/I Gain'
//  '<S81>'  : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain'
//  '<S82>'  : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S83>'  : 'control_outer/Position PID control/Position-Direct PID/Integrator'
//  '<S84>'  : 'control_outer/Position PID control/Position-Direct PID/Integrator ICs'
//  '<S85>'  : 'control_outer/Position PID control/Position-Direct PID/N Copy'
//  '<S86>'  : 'control_outer/Position PID control/Position-Direct PID/N Gain'
//  '<S87>'  : 'control_outer/Position PID control/Position-Direct PID/P Copy'
//  '<S88>'  : 'control_outer/Position PID control/Position-Direct PID/Parallel P Gain'
//  '<S89>'  : 'control_outer/Position PID control/Position-Direct PID/Reset Signal'
//  '<S90>'  : 'control_outer/Position PID control/Position-Direct PID/Saturation'
//  '<S91>'  : 'control_outer/Position PID control/Position-Direct PID/Saturation Fdbk'
//  '<S92>'  : 'control_outer/Position PID control/Position-Direct PID/Sum'
//  '<S93>'  : 'control_outer/Position PID control/Position-Direct PID/Sum Fdbk'
//  '<S94>'  : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode'
//  '<S95>'  : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode Sum'
//  '<S96>'  : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Integral'
//  '<S97>'  : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Ngain'
//  '<S98>'  : 'control_outer/Position PID control/Position-Direct PID/postSat Signal'
//  '<S99>'  : 'control_outer/Position PID control/Position-Direct PID/preInt Signal'
//  '<S100>' : 'control_outer/Position PID control/Position-Direct PID/preSat Signal'
//  '<S101>' : 'control_outer/Position PID control/Position-Direct PID/Anti-windup/Passthrough'
//  '<S102>' : 'control_outer/Position PID control/Position-Direct PID/D Gain/External Parameters'
//  '<S103>' : 'control_outer/Position PID control/Position-Direct PID/External Derivative/Error'
//  '<S104>' : 'control_outer/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S105>' : 'control_outer/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S106>' : 'control_outer/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S107>' : 'control_outer/Position PID control/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S108>' : 'control_outer/Position PID control/Position-Direct PID/I Gain/External Parameters'
//  '<S109>' : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S110>' : 'control_outer/Position PID control/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S111>' : 'control_outer/Position PID control/Position-Direct PID/Integrator/Discrete'
//  '<S112>' : 'control_outer/Position PID control/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S113>' : 'control_outer/Position PID control/Position-Direct PID/N Copy/External Parameters'
//  '<S114>' : 'control_outer/Position PID control/Position-Direct PID/N Gain/External Parameters'
//  '<S115>' : 'control_outer/Position PID control/Position-Direct PID/P Copy/Disabled'
//  '<S116>' : 'control_outer/Position PID control/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S117>' : 'control_outer/Position PID control/Position-Direct PID/Reset Signal/External Reset'
//  '<S118>' : 'control_outer/Position PID control/Position-Direct PID/Saturation/Passthrough'
//  '<S119>' : 'control_outer/Position PID control/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S120>' : 'control_outer/Position PID control/Position-Direct PID/Sum/Sum_PID'
//  '<S121>' : 'control_outer/Position PID control/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S122>' : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode/Enabled'
//  '<S123>' : 'control_outer/Position PID control/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S124>' : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Integral/TsSignalSpecification'
//  '<S125>' : 'control_outer/Position PID control/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S126>' : 'control_outer/Position PID control/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S127>' : 'control_outer/Position PID control/Position-Direct PID/preInt Signal/Internal PreInt'
//  '<S128>' : 'control_outer/Position PID control/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S129>' : 'control_outer/Velocity PID control/Anti-windup'
//  '<S130>' : 'control_outer/Velocity PID control/D Gain'
//  '<S131>' : 'control_outer/Velocity PID control/External Derivative'
//  '<S132>' : 'control_outer/Velocity PID control/Filter'
//  '<S133>' : 'control_outer/Velocity PID control/Filter ICs'
//  '<S134>' : 'control_outer/Velocity PID control/I Gain'
//  '<S135>' : 'control_outer/Velocity PID control/Ideal P Gain'
//  '<S136>' : 'control_outer/Velocity PID control/Ideal P Gain Fdbk'
//  '<S137>' : 'control_outer/Velocity PID control/Integrator'
//  '<S138>' : 'control_outer/Velocity PID control/Integrator ICs'
//  '<S139>' : 'control_outer/Velocity PID control/N Copy'
//  '<S140>' : 'control_outer/Velocity PID control/N Gain'
//  '<S141>' : 'control_outer/Velocity PID control/P Copy'
//  '<S142>' : 'control_outer/Velocity PID control/Parallel P Gain'
//  '<S143>' : 'control_outer/Velocity PID control/Reset Signal'
//  '<S144>' : 'control_outer/Velocity PID control/Saturation'
//  '<S145>' : 'control_outer/Velocity PID control/Saturation Fdbk'
//  '<S146>' : 'control_outer/Velocity PID control/Sum'
//  '<S147>' : 'control_outer/Velocity PID control/Sum Fdbk'
//  '<S148>' : 'control_outer/Velocity PID control/Tracking Mode'
//  '<S149>' : 'control_outer/Velocity PID control/Tracking Mode Sum'
//  '<S150>' : 'control_outer/Velocity PID control/Tsamp - Integral'
//  '<S151>' : 'control_outer/Velocity PID control/Tsamp - Ngain'
//  '<S152>' : 'control_outer/Velocity PID control/postSat Signal'
//  '<S153>' : 'control_outer/Velocity PID control/preInt Signal'
//  '<S154>' : 'control_outer/Velocity PID control/preSat Signal'
//  '<S155>' : 'control_outer/Velocity PID control/Anti-windup/Passthrough'
//  '<S156>' : 'control_outer/Velocity PID control/D Gain/External Parameters'
//  '<S157>' : 'control_outer/Velocity PID control/External Derivative/Error'
//  '<S158>' : 'control_outer/Velocity PID control/Filter/Disc. Trapezoidal Filter'
//  '<S159>' : 'control_outer/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S160>' : 'control_outer/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S161>' : 'control_outer/Velocity PID control/Filter ICs/Internal IC - Filter'
//  '<S162>' : 'control_outer/Velocity PID control/I Gain/External Parameters'
//  '<S163>' : 'control_outer/Velocity PID control/Ideal P Gain/Passthrough'
//  '<S164>' : 'control_outer/Velocity PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S165>' : 'control_outer/Velocity PID control/Integrator/Discrete'
//  '<S166>' : 'control_outer/Velocity PID control/Integrator ICs/Internal IC'
//  '<S167>' : 'control_outer/Velocity PID control/N Copy/External Parameters'
//  '<S168>' : 'control_outer/Velocity PID control/N Gain/External Parameters'
//  '<S169>' : 'control_outer/Velocity PID control/P Copy/Disabled'
//  '<S170>' : 'control_outer/Velocity PID control/Parallel P Gain/External Parameters'
//  '<S171>' : 'control_outer/Velocity PID control/Reset Signal/External Reset'
//  '<S172>' : 'control_outer/Velocity PID control/Saturation/Passthrough'
//  '<S173>' : 'control_outer/Velocity PID control/Saturation Fdbk/Passthrough'
//  '<S174>' : 'control_outer/Velocity PID control/Sum/Sum_PID'
//  '<S175>' : 'control_outer/Velocity PID control/Sum Fdbk/Enabled'
//  '<S176>' : 'control_outer/Velocity PID control/Tracking Mode/Enabled'
//  '<S177>' : 'control_outer/Velocity PID control/Tracking Mode Sum/Tracking Mode'
//  '<S178>' : 'control_outer/Velocity PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S179>' : 'control_outer/Velocity PID control/Tsamp - Ngain/Passthrough'
//  '<S180>' : 'control_outer/Velocity PID control/postSat Signal/Feedback_Path'
//  '<S181>' : 'control_outer/Velocity PID control/preInt Signal/Internal PreInt'
//  '<S182>' : 'control_outer/Velocity PID control/preSat Signal/Feedback_Path'

#endif                                 // control_outer_h_

//
// File trailer for generated code.
//
// [EOF]
//
