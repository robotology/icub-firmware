//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: position_velocity_cascade.h
//
// Code generated for Simulink model 'position_velocity_cascade'.
//
// Model version                  : 9.22
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Mon Aug 11 10:31:19 2025
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

// Zero-crossing (trigger) state for model 'position_velocity_cascade'
struct ZCE_position_velocity_cascade_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZCE;// '<S50>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_m;// '<S104>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset_ZCE_e;// '<S158>/Filter Differentiator TF' 
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
  Targets *rtu_Targets, const real32_T *rtu_JointData_position, const real32_T
  *rtu_ActuatorCfg_thresholds_jntVelMax, const real32_T
  *rtu_ActuatorCfg_thresholds_motorNominalCurrents, const real32_T
  *rtu_ActuatorCfg_thresholds_motorPeakCurrents, const real32_T
  *rtu_ActuatorCfg_pids_currentPID_I, const real32_T
  *rtu_ActuatorCfg_pids_velocityPID_P, const real32_T
  *rtu_ActuatorCfg_pids_velocityPID_I, const real32_T
  *rtu_ActuatorCfg_pids_velocityPID_D, const real32_T
  *rtu_ActuatorCfg_pids_velocityPID_N, const real32_T
  *rtu_ActuatorCfg_pids_positionPID_P, const real32_T
  *rtu_ActuatorCfg_pids_positionPID_I, const real32_T
  *rtu_ActuatorCfg_pids_positionPID_D, const real32_T
  *rtu_ActuatorCfg_pids_positionPID_N, const ReferenceEncoder
  *rtu_ActuatorCfg_motor_reference_encoder, const real32_T
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
//  '<S17>'  : 'position_velocity_cascade/Enabling Logic/Compare To Constant7'
//  '<S18>'  : 'position_velocity_cascade/Enabling Logic/Detect Change'
//  '<S19>'  : 'position_velocity_cascade/Position PID control/Position PID'
//  '<S20>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID'
//  '<S21>'  : 'position_velocity_cascade/Position PID control/Position PID/Anti-windup'
//  '<S22>'  : 'position_velocity_cascade/Position PID control/Position PID/D Gain'
//  '<S23>'  : 'position_velocity_cascade/Position PID control/Position PID/External Derivative'
//  '<S24>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter'
//  '<S25>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter ICs'
//  '<S26>'  : 'position_velocity_cascade/Position PID control/Position PID/I Gain'
//  '<S27>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain'
//  '<S28>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain Fdbk'
//  '<S29>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator'
//  '<S30>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator ICs'
//  '<S31>'  : 'position_velocity_cascade/Position PID control/Position PID/N Copy'
//  '<S32>'  : 'position_velocity_cascade/Position PID control/Position PID/N Gain'
//  '<S33>'  : 'position_velocity_cascade/Position PID control/Position PID/P Copy'
//  '<S34>'  : 'position_velocity_cascade/Position PID control/Position PID/Parallel P Gain'
//  '<S35>'  : 'position_velocity_cascade/Position PID control/Position PID/Reset Signal'
//  '<S36>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation'
//  '<S37>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation Fdbk'
//  '<S38>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum'
//  '<S39>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum Fdbk'
//  '<S40>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode'
//  '<S41>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode Sum'
//  '<S42>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Integral'
//  '<S43>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Ngain'
//  '<S44>'  : 'position_velocity_cascade/Position PID control/Position PID/postSat Signal'
//  '<S45>'  : 'position_velocity_cascade/Position PID control/Position PID/preInt Signal'
//  '<S46>'  : 'position_velocity_cascade/Position PID control/Position PID/preSat Signal'
//  '<S47>'  : 'position_velocity_cascade/Position PID control/Position PID/Anti-windup/Passthrough'
//  '<S48>'  : 'position_velocity_cascade/Position PID control/Position PID/D Gain/External Parameters'
//  '<S49>'  : 'position_velocity_cascade/Position PID control/Position PID/External Derivative/Error'
//  '<S50>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S51>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S52>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S53>'  : 'position_velocity_cascade/Position PID control/Position PID/Filter ICs/Internal IC - Filter'
//  '<S54>'  : 'position_velocity_cascade/Position PID control/Position PID/I Gain/External Parameters'
//  '<S55>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain/Passthrough'
//  '<S56>'  : 'position_velocity_cascade/Position PID control/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S57>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator/Discrete'
//  '<S58>'  : 'position_velocity_cascade/Position PID control/Position PID/Integrator ICs/Internal IC'
//  '<S59>'  : 'position_velocity_cascade/Position PID control/Position PID/N Copy/External Parameters'
//  '<S60>'  : 'position_velocity_cascade/Position PID control/Position PID/N Gain/External Parameters'
//  '<S61>'  : 'position_velocity_cascade/Position PID control/Position PID/P Copy/Disabled'
//  '<S62>'  : 'position_velocity_cascade/Position PID control/Position PID/Parallel P Gain/External Parameters'
//  '<S63>'  : 'position_velocity_cascade/Position PID control/Position PID/Reset Signal/External Reset'
//  '<S64>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation/Passthrough'
//  '<S65>'  : 'position_velocity_cascade/Position PID control/Position PID/Saturation Fdbk/Passthrough'
//  '<S66>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum/Sum_PID'
//  '<S67>'  : 'position_velocity_cascade/Position PID control/Position PID/Sum Fdbk/Enabled'
//  '<S68>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode/Enabled'
//  '<S69>'  : 'position_velocity_cascade/Position PID control/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S70>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Integral/TsSignalSpecification'
//  '<S71>'  : 'position_velocity_cascade/Position PID control/Position PID/Tsamp - Ngain/Passthrough'
//  '<S72>'  : 'position_velocity_cascade/Position PID control/Position PID/postSat Signal/Feedback_Path'
//  '<S73>'  : 'position_velocity_cascade/Position PID control/Position PID/preInt Signal/Internal PreInt'
//  '<S74>'  : 'position_velocity_cascade/Position PID control/Position PID/preSat Signal/Feedback_Path'
//  '<S75>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Anti-windup'
//  '<S76>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/D Gain'
//  '<S77>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/External Derivative'
//  '<S78>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter'
//  '<S79>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter ICs'
//  '<S80>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/I Gain'
//  '<S81>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain'
//  '<S82>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S83>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator'
//  '<S84>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator ICs'
//  '<S85>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Copy'
//  '<S86>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Gain'
//  '<S87>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/P Copy'
//  '<S88>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Parallel P Gain'
//  '<S89>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Reset Signal'
//  '<S90>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation'
//  '<S91>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation Fdbk'
//  '<S92>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum'
//  '<S93>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum Fdbk'
//  '<S94>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode'
//  '<S95>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode Sum'
//  '<S96>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Integral'
//  '<S97>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Ngain'
//  '<S98>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/postSat Signal'
//  '<S99>'  : 'position_velocity_cascade/Position PID control/Position-Direct PID/preInt Signal'
//  '<S100>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/preSat Signal'
//  '<S101>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Anti-windup/Passthrough'
//  '<S102>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/D Gain/External Parameters'
//  '<S103>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/External Derivative/Error'
//  '<S104>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S105>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S106>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S107>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S108>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/I Gain/External Parameters'
//  '<S109>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S110>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S111>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator/Discrete'
//  '<S112>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S113>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Copy/External Parameters'
//  '<S114>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/N Gain/External Parameters'
//  '<S115>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/P Copy/Disabled'
//  '<S116>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S117>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Reset Signal/External Reset'
//  '<S118>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation/Passthrough'
//  '<S119>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S120>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum/Sum_PID'
//  '<S121>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S122>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode/Enabled'
//  '<S123>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S124>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Integral/TsSignalSpecification'
//  '<S125>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S126>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S127>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/preInt Signal/Internal PreInt'
//  '<S128>' : 'position_velocity_cascade/Position PID control/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S129>' : 'position_velocity_cascade/Velocity PID control/Anti-windup'
//  '<S130>' : 'position_velocity_cascade/Velocity PID control/D Gain'
//  '<S131>' : 'position_velocity_cascade/Velocity PID control/External Derivative'
//  '<S132>' : 'position_velocity_cascade/Velocity PID control/Filter'
//  '<S133>' : 'position_velocity_cascade/Velocity PID control/Filter ICs'
//  '<S134>' : 'position_velocity_cascade/Velocity PID control/I Gain'
//  '<S135>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain'
//  '<S136>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain Fdbk'
//  '<S137>' : 'position_velocity_cascade/Velocity PID control/Integrator'
//  '<S138>' : 'position_velocity_cascade/Velocity PID control/Integrator ICs'
//  '<S139>' : 'position_velocity_cascade/Velocity PID control/N Copy'
//  '<S140>' : 'position_velocity_cascade/Velocity PID control/N Gain'
//  '<S141>' : 'position_velocity_cascade/Velocity PID control/P Copy'
//  '<S142>' : 'position_velocity_cascade/Velocity PID control/Parallel P Gain'
//  '<S143>' : 'position_velocity_cascade/Velocity PID control/Reset Signal'
//  '<S144>' : 'position_velocity_cascade/Velocity PID control/Saturation'
//  '<S145>' : 'position_velocity_cascade/Velocity PID control/Saturation Fdbk'
//  '<S146>' : 'position_velocity_cascade/Velocity PID control/Sum'
//  '<S147>' : 'position_velocity_cascade/Velocity PID control/Sum Fdbk'
//  '<S148>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode'
//  '<S149>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode Sum'
//  '<S150>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Integral'
//  '<S151>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Ngain'
//  '<S152>' : 'position_velocity_cascade/Velocity PID control/postSat Signal'
//  '<S153>' : 'position_velocity_cascade/Velocity PID control/preInt Signal'
//  '<S154>' : 'position_velocity_cascade/Velocity PID control/preSat Signal'
//  '<S155>' : 'position_velocity_cascade/Velocity PID control/Anti-windup/Passthrough'
//  '<S156>' : 'position_velocity_cascade/Velocity PID control/D Gain/External Parameters'
//  '<S157>' : 'position_velocity_cascade/Velocity PID control/External Derivative/Error'
//  '<S158>' : 'position_velocity_cascade/Velocity PID control/Filter/Disc. Trapezoidal Filter'
//  '<S159>' : 'position_velocity_cascade/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S160>' : 'position_velocity_cascade/Velocity PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S161>' : 'position_velocity_cascade/Velocity PID control/Filter ICs/Internal IC - Filter'
//  '<S162>' : 'position_velocity_cascade/Velocity PID control/I Gain/External Parameters'
//  '<S163>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain/Passthrough'
//  '<S164>' : 'position_velocity_cascade/Velocity PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S165>' : 'position_velocity_cascade/Velocity PID control/Integrator/Discrete'
//  '<S166>' : 'position_velocity_cascade/Velocity PID control/Integrator ICs/Internal IC'
//  '<S167>' : 'position_velocity_cascade/Velocity PID control/N Copy/External Parameters'
//  '<S168>' : 'position_velocity_cascade/Velocity PID control/N Gain/External Parameters'
//  '<S169>' : 'position_velocity_cascade/Velocity PID control/P Copy/Disabled'
//  '<S170>' : 'position_velocity_cascade/Velocity PID control/Parallel P Gain/External Parameters'
//  '<S171>' : 'position_velocity_cascade/Velocity PID control/Reset Signal/External Reset'
//  '<S172>' : 'position_velocity_cascade/Velocity PID control/Saturation/Passthrough'
//  '<S173>' : 'position_velocity_cascade/Velocity PID control/Saturation Fdbk/Passthrough'
//  '<S174>' : 'position_velocity_cascade/Velocity PID control/Sum/Sum_PID'
//  '<S175>' : 'position_velocity_cascade/Velocity PID control/Sum Fdbk/Enabled'
//  '<S176>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode/Enabled'
//  '<S177>' : 'position_velocity_cascade/Velocity PID control/Tracking Mode Sum/Tracking Mode'
//  '<S178>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S179>' : 'position_velocity_cascade/Velocity PID control/Tsamp - Ngain/Passthrough'
//  '<S180>' : 'position_velocity_cascade/Velocity PID control/postSat Signal/Feedback_Path'
//  '<S181>' : 'position_velocity_cascade/Velocity PID control/preInt Signal/Internal PreInt'
//  '<S182>' : 'position_velocity_cascade/Velocity PID control/preSat Signal/Feedback_Path'

#endif                                 // position_velocity_cascade_h_

//
// File trailer for generated code.
//
// [EOF]
//
