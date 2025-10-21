//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: position_velocity_cascade.h
//
// Code generated for Simulink model 'position_velocity_cascade'.
//
// Model version                  : 10.15
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Oct 21 09:21:42 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef position_velocity_cascade_h_
#define position_velocity_cascade_h_
#include "rtwtypes.h"
#include "position_velocity_cascade_types.h"
#include "zero_crossing_types.h"

// Block signals for model 'position_velocity_cascade'
struct B_position_velocity_cascade_c_T {
  real32_T DiscreteTimeIntegrator;     // '<S1>/Discrete-Time Integrator'
};

// Block states (default storage) for model 'position_velocity_cascade'
struct DW_position_velocity_cascade_f_T {
  real32_T FilterDifferentiatorTF_states;// '<S49>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<Root>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S56>/Integrator'
  real32_T FilterDifferentiatorTF_states_i;// '<S103>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_i;        // '<S110>/Integrator'
  real32_T FilterDifferentiatorTF_states_c;// '<S157>/Filter Differentiator TF'
  real32_T UnitDelay1_DSTATE;          // '<Root>/Unit Delay1'
  real32_T Integrator_DSTATE_b;        // '<S164>/Integrator'
  real32_T DiscreteTimeIntegrator_DSTATE;// '<S1>/Discrete-Time Integrator'
  ControlModes DelayInput1_DSTATE;     // '<S17>/Delay Input1'
  real32_T FilterDifferentiatorTF_tmp; // '<S49>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_m;// '<S103>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_p;// '<S157>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S56>/Integrator'
  int8_T Integrator_PrevResetState_n;  // '<S110>/Integrator'
  int8_T Integrator_PrevResetState_c;  // '<S164>/Integrator'
  int8_T DiscreteTimeIntegrator_PrevResetState;// '<S1>/Discrete-Time Integrator' 
  uint8_T DiscreteTimeIntegrator_SYSTEM_ENABLE;// '<S1>/Discrete-Time Integrator' 
  boolean_T Memory_PreviousInput;      // '<S9>/Memory'
};

// Zero-crossing (trigger) state for model 'position_velocity_cascade'
struct ZCE_position_velocity_cascade_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S49>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_m;// '<S103>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_e;// '<S157>/Filter Differentiator TF' 
};

struct MdlrefDW_position_velocity_cascade_T {
  B_position_velocity_cascade_c_T rtb;
  DW_position_velocity_cascade_f_T rtdw;
  ZCE_position_velocity_cascade_T rtzce;
};

// Model reference registration function
extern void position_velocity_cascade_initialize(ZCE_position_velocity_cascade_T
  *localZCE);
extern void position_velocity_cascade_Init(DW_position_velocity_cascade_f_T
  *localDW);
extern void position_velocity_cascade_Enable(DW_position_velocity_cascade_f_T
  *localDW);
extern void position_velocity_cascade_Disable(B_position_velocity_cascade_c_T
  *localB, DW_position_velocity_cascade_f_T *localDW);
extern void position_velocity_cascade(const EstimatedData *rtu_Estimates, const
  Targets *rtu_Targets, const real32_T *rtu_JointData_position, const
  ActuatorConfiguration *rtu_ActuatorCfg, const real32_T
  *rtu_SensorsData_motorsensors_qencoder_rotor_angle, const boolean_T
  *rtu_Flags_enable_thermal_protection, const ControlModes
  *rtu_Flags_control_mode, ControlOuterOutputs *rty_OuterOutputs,
  B_position_velocity_cascade_c_T *localB, DW_position_velocity_cascade_f_T
  *localDW, ZCE_position_velocity_cascade_T *localZCE);

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S4>/Data Type Propagation' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Propagation' : Unused code path elimination
//  Block '<S49>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S67>/Kt' : Eliminated nontunable gain of 1
//  Block '<S103>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S121>/Kt' : Eliminated nontunable gain of 1
//  Block '<S157>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S175>/Kt' : Eliminated nontunable gain of 1


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
//  '<Root>' : 'position_velocity_cascade'
//  '<S1>'   : 'position_velocity_cascade/Current Limiter'
//  '<S2>'   : 'position_velocity_cascade/Enabling Logic'
//  '<S3>'   : 'position_velocity_cascade/Position PID control'
//  '<S4>'   : 'position_velocity_cascade/Saturation Dynamic'
//  '<S5>'   : 'position_velocity_cascade/Saturation Dynamic1'
//  '<S6>'   : 'position_velocity_cascade/Velocity PID control'
//  '<S7>'   : 'position_velocity_cascade/Current Limiter/Compare To Zero'
//  '<S8>'   : 'position_velocity_cascade/Current Limiter/Compare To Zero1'
//  '<S9>'   : 'position_velocity_cascade/Current Limiter/S-R Flip-Flop'
//  '<S10>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant'
//  '<S11>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant1'
//  '<S12>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant2'
//  '<S13>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant3'
//  '<S14>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant4'
//  '<S15>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant5'
//  '<S16>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant6'
//  '<S17>'  : 'position_velocity_cascade/Enabling Logic/Detect Change'
//  '<S18>'  : 'position_velocity_cascade/Position PID control/Position PID'
//  '<S19>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID'
//  '<S20>'  : 'position_velocity_cascade/Position PID control/Position PID/Anti-windup'
//  '<S21>'  : 'position_velocity_cascade/Position PID control/Position PID/D Gain'
//  '<S22>'  : 'position_velocity_cascade/Position PID control/Position PID/External Derivative'
//  '<S23>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter'
//  '<S24>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter ICs'
//  '<S25>'  : 'position_velocity_cascade/Position PID control/Position PID/I Gain'
//  '<S26>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain'
//  '<S27>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain Fdbk'
//  '<S28>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator'
//  '<S29>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator ICs'
//  '<S30>'  : 'position_velocity_cascade/Position PID control/Position PID/N Copy'
//  '<S31>'  : 'position_velocity_cascade/Position PID control/Position PID/N Gain'
//  '<S32>'  : 'position_velocity_cascade/Position PID control/Position PID/P Copy'
//  '<S33>'  : 'position_velocity_cascade/Position PID control/Position PID/Parallel P Gain'
//  '<S34>'  : 'position_velocity_cascade/Position PID control/Position PID/Reset Signal'
//  '<S35>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation'
//  '<S36>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation Fdbk'
//  '<S37>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum'
//  '<S38>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum Fdbk'
//  '<S39>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode'
//  '<S40>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode Sum'
//  '<S41>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Integral'
//  '<S42>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Ngain'
//  '<S43>'  : 'position_velocity_cascade/Position PID control/Position PID/postSat Signal'
//  '<S44>'  : 'position_velocity_cascade/Position PID control/Position PID/preInt Signal'
//  '<S45>'  : 'position_velocity_cascade/Position PID control/Position PID/preSat Signal'
//  '<S46>'  : 'position_velocity_cascade/Position PID control/Position PID/Anti-windup/Passthrough'
//  '<S47>'  : 'position_velocity_cascade/Position PID control/Position PID/D Gain/External Parameters'
//  '<S48>'  : 'position_velocity_cascade/Position PID control/Position PID/External Derivative/Error'
//  '<S49>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S50>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S51>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S52>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter ICs/Internal IC - Filter'
//  '<S53>'  : 'position_velocity_cascade/Position PID control/Position PID/I Gain/External Parameters'
//  '<S54>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain/Passthrough'
//  '<S55>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S56>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator/Discrete'
//  '<S57>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator ICs/Internal IC'
//  '<S58>'  : 'position_velocity_cascade/Position PID control/Position PID/N Copy/External Parameters'
//  '<S59>'  : 'position_velocity_cascade/Position PID control/Position PID/N Gain/External Parameters'
//  '<S60>'  : 'position_velocity_cascade/Position PID control/Position PID/P Copy/Disabled'
//  '<S61>'  : 'position_velocity_cascade/Position PID control/Position PID/Parallel P Gain/External Parameters'
//  '<S62>'  : 'position_velocity_cascade/Position PID control/Position PID/Reset Signal/External Reset'
//  '<S63>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation/Passthrough'
//  '<S64>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation Fdbk/Passthrough'
//  '<S65>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum/Sum_PID'
//  '<S66>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum Fdbk/Enabled'
//  '<S67>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode/Enabled'
//  '<S68>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S69>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Integral/TsSignalSpecification'
//  '<S70>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Ngain/Passthrough'
//  '<S71>'  : 'position_velocity_cascade/Position PID control/Position PID/postSat Signal/Feedback_Path'
//  '<S72>'  : 'position_velocity_cascade/Position PID control/Position PID/preInt Signal/Internal PreInt'
//  '<S73>'  : 'position_velocity_cascade/Position PID control/Position PID/preSat Signal/Feedback_Path'
//  '<S74>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Anti-windup'
//  '<S75>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/D Gain'
//  '<S76>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/External Derivative'
//  '<S77>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter'
//  '<S78>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter ICs'
//  '<S79>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/I Gain'
//  '<S80>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain'
//  '<S81>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S82>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator'
//  '<S83>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator ICs'
//  '<S84>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Copy'
//  '<S85>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Gain'
//  '<S86>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/P Copy'
//  '<S87>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Parallel P Gain'
//  '<S88>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Reset Signal'
//  '<S89>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation'
//  '<S90>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation Fdbk'
//  '<S91>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum'
//  '<S92>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum Fdbk'
//  '<S93>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode'
//  '<S94>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode Sum'
//  '<S95>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Integral'
//  '<S96>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Ngain'
//  '<S97>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/postSat Signal'
//  '<S98>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/preInt Signal'
//  '<S99>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/preSat Signal'
//  '<S100>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Anti-windup/Passthrough'
//  '<S101>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/D Gain/External Parameters'
//  '<S102>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/External Derivative/Error'
//  '<S103>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S104>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S105>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S106>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S107>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/I Gain/External Parameters'
//  '<S108>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S109>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S110>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator/Discrete'
//  '<S111>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S112>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Copy/External Parameters'
//  '<S113>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Gain/External Parameters'
//  '<S114>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/P Copy/Disabled'
//  '<S115>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S116>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Reset Signal/External Reset'
//  '<S117>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation/Passthrough'
//  '<S118>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S119>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum/Sum_PID'
//  '<S120>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S121>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode/Enabled'
//  '<S122>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S123>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Integral/TsSignalSpecification'
//  '<S124>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S125>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S126>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/preInt Signal/Internal PreInt'
//  '<S127>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S128>' : 'position_velocity_cascade/Velocity PID control/Anti-windup'
//  '<S129>' : 'position_velocity_cascade/Velocity PID control/D Gain'
//  '<S130>' : 'position_velocity_cascade/Velocity PID control/External Derivative'
//  '<S131>' : 'position_velocity_cascade/Velocity PID control/Filter'
//  '<S132>' : 'position_velocity_cascade/Velocity PID control/Filter ICs'
//  '<S133>' : 'position_velocity_cascade/Velocity PID control/I Gain'
//  '<S134>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain'
//  '<S135>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain Fdbk'
//  '<S136>' : 'position_velocity_cascade/Velocity PID control/Integrator'
//  '<S137>' : 'position_velocity_cascade/Velocity PID control/Integrator ICs'
//  '<S138>' : 'position_velocity_cascade/Velocity PID control/N Copy'
//  '<S139>' : 'position_velocity_cascade/Velocity PID control/N Gain'
//  '<S140>' : 'position_velocity_cascade/Velocity PID control/P Copy'
//  '<S141>' : 'position_velocity_cascade/Velocity PID control/Parallel P Gain'
//  '<S142>' : 'position_velocity_cascade/Velocity PID control/Reset Signal'
//  '<S143>' : 'position_velocity_cascade/Velocity PID control/Saturation'
//  '<S144>' : 'position_velocity_cascade/Velocity PID control/Saturation Fdbk'
//  '<S145>' : 'position_velocity_cascade/Velocity PID control/Sum'
//  '<S146>' : 'position_velocity_cascade/Velocity PID control/Sum Fdbk'
//  '<S147>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode'
//  '<S148>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode Sum'
//  '<S149>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Integral'
//  '<S150>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Ngain'
//  '<S151>' : 'position_velocity_cascade/Velocity PID control/postSat Signal'
//  '<S152>' : 'position_velocity_cascade/Velocity PID control/preInt Signal'
//  '<S153>' : 'position_velocity_cascade/Velocity PID control/preSat Signal'
//  '<S154>' : 'position_velocity_cascade/Velocity PID control/Anti-windup/Passthrough'
//  '<S155>' : 'position_velocity_cascade/Velocity PID control/D Gain/External Parameters'
//  '<S156>' : 'position_velocity_cascade/Velocity PID control/External Derivative/Error'
//  '<S157>' : 'position_velocity_cascade/Velocity PID control/Filter/Disc. Trapezoidal Filter'
//  '<S158>' : 'position_velocity_cascade/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S159>' : 'position_velocity_cascade/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S160>' : 'position_velocity_cascade/Velocity PID control/Filter ICs/Internal IC - Filter'
//  '<S161>' : 'position_velocity_cascade/Velocity PID control/I Gain/External Parameters'
//  '<S162>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain/Passthrough'
//  '<S163>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S164>' : 'position_velocity_cascade/Velocity PID control/Integrator/Discrete'
//  '<S165>' : 'position_velocity_cascade/Velocity PID control/Integrator ICs/Internal IC'
//  '<S166>' : 'position_velocity_cascade/Velocity PID control/N Copy/External Parameters'
//  '<S167>' : 'position_velocity_cascade/Velocity PID control/N Gain/External Parameters'
//  '<S168>' : 'position_velocity_cascade/Velocity PID control/P Copy/Disabled'
//  '<S169>' : 'position_velocity_cascade/Velocity PID control/Parallel P Gain/External Parameters'
//  '<S170>' : 'position_velocity_cascade/Velocity PID control/Reset Signal/External Reset'
//  '<S171>' : 'position_velocity_cascade/Velocity PID control/Saturation/Passthrough'
//  '<S172>' : 'position_velocity_cascade/Velocity PID control/Saturation Fdbk/Passthrough'
//  '<S173>' : 'position_velocity_cascade/Velocity PID control/Sum/Sum_PID'
//  '<S174>' : 'position_velocity_cascade/Velocity PID control/Sum Fdbk/Enabled'
//  '<S175>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode/Enabled'
//  '<S176>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode Sum/Tracking Mode'
//  '<S177>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S178>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Ngain/Passthrough'
//  '<S179>' : 'position_velocity_cascade/Velocity PID control/postSat Signal/Feedback_Path'
//  '<S180>' : 'position_velocity_cascade/Velocity PID control/preInt Signal/Internal PreInt'
//  '<S181>' : 'position_velocity_cascade/Velocity PID control/preSat Signal/Feedback_Path'

#endif                                 // position_velocity_cascade_h_

//
// File trailer for generated code.
//
// [EOF]
//
