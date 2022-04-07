//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 2.43
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri May  6 11:29:59 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_outer_h_
#define RTW_HEADER_control_outer_h_
#include <cmath>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "control_outer_types.h"
#include "rtGetNaN.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"

// Block states (default storage) for model 'control_outer'
#ifndef control_outer_MDLREF_HIDE_CHILD_

struct DW_control_outer_f_T {
  real32_T FilterDifferentiatorTF_states;// '<S46>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<Root>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S53>/Integrator'
  real32_T FilterDifferentiatorTF_states_i;// '<S96>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_i;        // '<S103>/Integrator'
  real32_T FilterDifferentiatorTF_states_c;// '<S148>/Filter Differentiator TF'
  real32_T UnitDelay1_DSTATE;          // '<Root>/Unit Delay1'
  real32_T Integrator_DSTATE_b;        // '<S155>/Integrator'
  real32_T DelayInput1_DSTATE;         // '<S8>/Delay Input1'
  real32_T DiscreteFilter_states;      // '<S1>/Discrete Filter'
  real32_T DiscreteFilter_denStates;   // '<S1>/Discrete Filter'
  ControlModes DelayInput1_DSTATE_f;   // '<S2>/Delay Input1'
  real32_T FilterDifferentiatorTF_tmp; // '<S46>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_m;// '<S96>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_p;// '<S148>/Filter Differentiator TF'
  real32_T DiscreteFilter_tmp;         // '<S1>/Discrete Filter'
  int8_T Integrator_PrevResetState;    // '<S53>/Integrator'
  int8_T Integrator_PrevResetState_n;  // '<S103>/Integrator'
  int8_T Integrator_PrevResetState_c;  // '<S155>/Integrator'
};

#endif                                 //control_outer_MDLREF_HIDE_CHILD_

// Zero-crossing (trigger) state for model 'control_outer'
#ifndef control_outer_MDLREF_HIDE_CHILD_

struct ZCE_control_outer_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZC;// '<S46>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset__m;// '<S96>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset__e;// '<S148>/Filter Differentiator TF' 
  ZCSigState DiscreteFilter_Reset_ZCE; // '<S1>/Discrete Filter'
};

#endif                                 //control_outer_MDLREF_HIDE_CHILD_

#ifndef control_outer_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_control_outer_T {
  const char_T **errorStatus;
};

#endif                                 //control_outer_MDLREF_HIDE_CHILD_

#ifndef control_outer_MDLREF_HIDE_CHILD_

struct MdlrefDW_control_outer_T {
  RT_MODEL_control_outer_T rtm;
};

#endif                                 //control_outer_MDLREF_HIDE_CHILD_

extern void control_outer_Init(void);
extern void control_outer(const Flags *rtu_Flags, const ConfigurationParameters *
  rtu_ConfigurationParameters, const Targets *rtu_Targets, const SensorsData
  *rtu_Sensors, const EstimatedData *rtu_Estimates, ControlOuterOutputs
  *rty_OuterOutputs);

// Model reference registration function
extern void control_outer_initialize(const char_T **rt_errorStatus);

#ifndef control_outer_MDLREF_HIDE_CHILD_

extern MdlrefDW_control_outer_T control_outer_MdlrefDW;

#endif                                 //control_outer_MDLREF_HIDE_CHILD_

#ifndef control_outer_MDLREF_HIDE_CHILD_

// Block states (default storage)
extern DW_control_outer_f_T control_outer_DW;

// Previous zero-crossings (trigger) states
extern ZCE_control_outer_T control_outer_PrevZCX;

#endif                                 //control_outer_MDLREF_HIDE_CHILD_

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S9>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Propagation' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Propagation' : Unused code path elimination
//  Block '<S46>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S64>/Kt' : Eliminated nontunable gain of 1
//  Block '<S96>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S114>/Kt' : Eliminated nontunable gain of 1
//  Block '<S148>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S166>/Kt' : Eliminated nontunable gain of 1


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
//  '<S2>'   : 'control_outer/Detect Change'
//  '<S3>'   : 'control_outer/Enabling Logic'
//  '<S4>'   : 'control_outer/Position loop'
//  '<S5>'   : 'control_outer/Saturation Dynamic'
//  '<S6>'   : 'control_outer/Vel PID control'
//  '<S7>'   : 'control_outer/Current Limiter/Compare To Zero'
//  '<S8>'   : 'control_outer/Current Limiter/Detect Change'
//  '<S9>'   : 'control_outer/Current Limiter/Saturation Dynamic'
//  '<S10>'  : 'control_outer/Enabling Logic/Compare To Constant'
//  '<S11>'  : 'control_outer/Enabling Logic/Compare To Constant1'
//  '<S12>'  : 'control_outer/Enabling Logic/Compare To Constant2'
//  '<S13>'  : 'control_outer/Enabling Logic/Compare To Constant3'
//  '<S14>'  : 'control_outer/Enabling Logic/Compare To Constant4'
//  '<S15>'  : 'control_outer/Enabling Logic/Compare To Constant5'
//  '<S16>'  : 'control_outer/Enabling Logic/Compare To Constant6'
//  '<S17>'  : 'control_outer/Enabling Logic/Compare To Constant7'
//  '<S18>'  : 'control_outer/Position loop/Position PID'
//  '<S19>'  : 'control_outer/Position loop/Position-Direct PID'
//  '<S20>'  : 'control_outer/Position loop/Position PID/Anti-windup'
//  '<S21>'  : 'control_outer/Position loop/Position PID/D Gain'
//  '<S22>'  : 'control_outer/Position loop/Position PID/Filter'
//  '<S23>'  : 'control_outer/Position loop/Position PID/Filter ICs'
//  '<S24>'  : 'control_outer/Position loop/Position PID/I Gain'
//  '<S25>'  : 'control_outer/Position loop/Position PID/Ideal P Gain'
//  '<S26>'  : 'control_outer/Position loop/Position PID/Ideal P Gain Fdbk'
//  '<S27>'  : 'control_outer/Position loop/Position PID/Integrator'
//  '<S28>'  : 'control_outer/Position loop/Position PID/Integrator ICs'
//  '<S29>'  : 'control_outer/Position loop/Position PID/N Copy'
//  '<S30>'  : 'control_outer/Position loop/Position PID/N Gain'
//  '<S31>'  : 'control_outer/Position loop/Position PID/P Copy'
//  '<S32>'  : 'control_outer/Position loop/Position PID/Parallel P Gain'
//  '<S33>'  : 'control_outer/Position loop/Position PID/Reset Signal'
//  '<S34>'  : 'control_outer/Position loop/Position PID/Saturation'
//  '<S35>'  : 'control_outer/Position loop/Position PID/Saturation Fdbk'
//  '<S36>'  : 'control_outer/Position loop/Position PID/Sum'
//  '<S37>'  : 'control_outer/Position loop/Position PID/Sum Fdbk'
//  '<S38>'  : 'control_outer/Position loop/Position PID/Tracking Mode'
//  '<S39>'  : 'control_outer/Position loop/Position PID/Tracking Mode Sum'
//  '<S40>'  : 'control_outer/Position loop/Position PID/Tsamp - Integral'
//  '<S41>'  : 'control_outer/Position loop/Position PID/Tsamp - Ngain'
//  '<S42>'  : 'control_outer/Position loop/Position PID/postSat Signal'
//  '<S43>'  : 'control_outer/Position loop/Position PID/preSat Signal'
//  '<S44>'  : 'control_outer/Position loop/Position PID/Anti-windup/Passthrough'
//  '<S45>'  : 'control_outer/Position loop/Position PID/D Gain/External Parameters'
//  '<S46>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S47>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S48>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S49>'  : 'control_outer/Position loop/Position PID/Filter ICs/Internal IC - Filter'
//  '<S50>'  : 'control_outer/Position loop/Position PID/I Gain/External Parameters'
//  '<S51>'  : 'control_outer/Position loop/Position PID/Ideal P Gain/Passthrough'
//  '<S52>'  : 'control_outer/Position loop/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S53>'  : 'control_outer/Position loop/Position PID/Integrator/Discrete'
//  '<S54>'  : 'control_outer/Position loop/Position PID/Integrator ICs/Internal IC'
//  '<S55>'  : 'control_outer/Position loop/Position PID/N Copy/External Parameters'
//  '<S56>'  : 'control_outer/Position loop/Position PID/N Gain/External Parameters'
//  '<S57>'  : 'control_outer/Position loop/Position PID/P Copy/Disabled'
//  '<S58>'  : 'control_outer/Position loop/Position PID/Parallel P Gain/External Parameters'
//  '<S59>'  : 'control_outer/Position loop/Position PID/Reset Signal/External Reset'
//  '<S60>'  : 'control_outer/Position loop/Position PID/Saturation/Passthrough'
//  '<S61>'  : 'control_outer/Position loop/Position PID/Saturation Fdbk/Passthrough'
//  '<S62>'  : 'control_outer/Position loop/Position PID/Sum/Sum_PID'
//  '<S63>'  : 'control_outer/Position loop/Position PID/Sum Fdbk/Enabled'
//  '<S64>'  : 'control_outer/Position loop/Position PID/Tracking Mode/Enabled'
//  '<S65>'  : 'control_outer/Position loop/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S66>'  : 'control_outer/Position loop/Position PID/Tsamp - Integral/Passthrough'
//  '<S67>'  : 'control_outer/Position loop/Position PID/Tsamp - Ngain/Passthrough'
//  '<S68>'  : 'control_outer/Position loop/Position PID/postSat Signal/Feedback_Path'
//  '<S69>'  : 'control_outer/Position loop/Position PID/preSat Signal/Feedback_Path'
//  '<S70>'  : 'control_outer/Position loop/Position-Direct PID/Anti-windup'
//  '<S71>'  : 'control_outer/Position loop/Position-Direct PID/D Gain'
//  '<S72>'  : 'control_outer/Position loop/Position-Direct PID/Filter'
//  '<S73>'  : 'control_outer/Position loop/Position-Direct PID/Filter ICs'
//  '<S74>'  : 'control_outer/Position loop/Position-Direct PID/I Gain'
//  '<S75>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain'
//  '<S76>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S77>'  : 'control_outer/Position loop/Position-Direct PID/Integrator'
//  '<S78>'  : 'control_outer/Position loop/Position-Direct PID/Integrator ICs'
//  '<S79>'  : 'control_outer/Position loop/Position-Direct PID/N Copy'
//  '<S80>'  : 'control_outer/Position loop/Position-Direct PID/N Gain'
//  '<S81>'  : 'control_outer/Position loop/Position-Direct PID/P Copy'
//  '<S82>'  : 'control_outer/Position loop/Position-Direct PID/Parallel P Gain'
//  '<S83>'  : 'control_outer/Position loop/Position-Direct PID/Reset Signal'
//  '<S84>'  : 'control_outer/Position loop/Position-Direct PID/Saturation'
//  '<S85>'  : 'control_outer/Position loop/Position-Direct PID/Saturation Fdbk'
//  '<S86>'  : 'control_outer/Position loop/Position-Direct PID/Sum'
//  '<S87>'  : 'control_outer/Position loop/Position-Direct PID/Sum Fdbk'
//  '<S88>'  : 'control_outer/Position loop/Position-Direct PID/Tracking Mode'
//  '<S89>'  : 'control_outer/Position loop/Position-Direct PID/Tracking Mode Sum'
//  '<S90>'  : 'control_outer/Position loop/Position-Direct PID/Tsamp - Integral'
//  '<S91>'  : 'control_outer/Position loop/Position-Direct PID/Tsamp - Ngain'
//  '<S92>'  : 'control_outer/Position loop/Position-Direct PID/postSat Signal'
//  '<S93>'  : 'control_outer/Position loop/Position-Direct PID/preSat Signal'
//  '<S94>'  : 'control_outer/Position loop/Position-Direct PID/Anti-windup/Passthrough'
//  '<S95>'  : 'control_outer/Position loop/Position-Direct PID/D Gain/External Parameters'
//  '<S96>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S97>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S98>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S99>'  : 'control_outer/Position loop/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S100>' : 'control_outer/Position loop/Position-Direct PID/I Gain/External Parameters'
//  '<S101>' : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S102>' : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S103>' : 'control_outer/Position loop/Position-Direct PID/Integrator/Discrete'
//  '<S104>' : 'control_outer/Position loop/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S105>' : 'control_outer/Position loop/Position-Direct PID/N Copy/External Parameters'
//  '<S106>' : 'control_outer/Position loop/Position-Direct PID/N Gain/External Parameters'
//  '<S107>' : 'control_outer/Position loop/Position-Direct PID/P Copy/Disabled'
//  '<S108>' : 'control_outer/Position loop/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S109>' : 'control_outer/Position loop/Position-Direct PID/Reset Signal/External Reset'
//  '<S110>' : 'control_outer/Position loop/Position-Direct PID/Saturation/Passthrough'
//  '<S111>' : 'control_outer/Position loop/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S112>' : 'control_outer/Position loop/Position-Direct PID/Sum/Sum_PID'
//  '<S113>' : 'control_outer/Position loop/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S114>' : 'control_outer/Position loop/Position-Direct PID/Tracking Mode/Enabled'
//  '<S115>' : 'control_outer/Position loop/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S116>' : 'control_outer/Position loop/Position-Direct PID/Tsamp - Integral/Passthrough'
//  '<S117>' : 'control_outer/Position loop/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S118>' : 'control_outer/Position loop/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S119>' : 'control_outer/Position loop/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S120>' : 'control_outer/Vel PID control/Anti-windup'
//  '<S121>' : 'control_outer/Vel PID control/D Gain'
//  '<S122>' : 'control_outer/Vel PID control/Filter'
//  '<S123>' : 'control_outer/Vel PID control/Filter ICs'
//  '<S124>' : 'control_outer/Vel PID control/I Gain'
//  '<S125>' : 'control_outer/Vel PID control/Ideal P Gain'
//  '<S126>' : 'control_outer/Vel PID control/Ideal P Gain Fdbk'
//  '<S127>' : 'control_outer/Vel PID control/Integrator'
//  '<S128>' : 'control_outer/Vel PID control/Integrator ICs'
//  '<S129>' : 'control_outer/Vel PID control/N Copy'
//  '<S130>' : 'control_outer/Vel PID control/N Gain'
//  '<S131>' : 'control_outer/Vel PID control/P Copy'
//  '<S132>' : 'control_outer/Vel PID control/Parallel P Gain'
//  '<S133>' : 'control_outer/Vel PID control/Reset Signal'
//  '<S134>' : 'control_outer/Vel PID control/Saturation'
//  '<S135>' : 'control_outer/Vel PID control/Saturation Fdbk'
//  '<S136>' : 'control_outer/Vel PID control/Sum'
//  '<S137>' : 'control_outer/Vel PID control/Sum Fdbk'
//  '<S138>' : 'control_outer/Vel PID control/Tracking Mode'
//  '<S139>' : 'control_outer/Vel PID control/Tracking Mode Sum'
//  '<S140>' : 'control_outer/Vel PID control/Tsamp - Integral'
//  '<S141>' : 'control_outer/Vel PID control/Tsamp - Ngain'
//  '<S142>' : 'control_outer/Vel PID control/postSat Signal'
//  '<S143>' : 'control_outer/Vel PID control/preSat Signal'
//  '<S144>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S145>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S146>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
//  '<S147>' : 'control_outer/Vel PID control/D Gain/External Parameters'
//  '<S148>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter'
//  '<S149>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S150>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S151>' : 'control_outer/Vel PID control/Filter ICs/Internal IC - Filter'
//  '<S152>' : 'control_outer/Vel PID control/I Gain/External Parameters'
//  '<S153>' : 'control_outer/Vel PID control/Ideal P Gain/Passthrough'
//  '<S154>' : 'control_outer/Vel PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S155>' : 'control_outer/Vel PID control/Integrator/Discrete'
//  '<S156>' : 'control_outer/Vel PID control/Integrator ICs/Internal IC'
//  '<S157>' : 'control_outer/Vel PID control/N Copy/External Parameters'
//  '<S158>' : 'control_outer/Vel PID control/N Gain/External Parameters'
//  '<S159>' : 'control_outer/Vel PID control/P Copy/Disabled'
//  '<S160>' : 'control_outer/Vel PID control/Parallel P Gain/External Parameters'
//  '<S161>' : 'control_outer/Vel PID control/Reset Signal/External Reset'
//  '<S162>' : 'control_outer/Vel PID control/Saturation/Enabled'
//  '<S163>' : 'control_outer/Vel PID control/Saturation Fdbk/Enabled'
//  '<S164>' : 'control_outer/Vel PID control/Sum/Sum_PID'
//  '<S165>' : 'control_outer/Vel PID control/Sum Fdbk/Enabled'
//  '<S166>' : 'control_outer/Vel PID control/Tracking Mode/Enabled'
//  '<S167>' : 'control_outer/Vel PID control/Tracking Mode Sum/Tracking Mode'
//  '<S168>' : 'control_outer/Vel PID control/Tsamp - Integral/Passthrough'
//  '<S169>' : 'control_outer/Vel PID control/Tsamp - Ngain/Passthrough'
//  '<S170>' : 'control_outer/Vel PID control/postSat Signal/Feedback_Path'
//  '<S171>' : 'control_outer/Vel PID control/preSat Signal/Feedback_Path'

#endif                                 // RTW_HEADER_control_outer_h_

//
// File trailer for generated code.
//
// [EOF]
//
