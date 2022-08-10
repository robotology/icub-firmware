//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 3.10
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:35 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_foc_h_
#define RTW_HEADER_control_foc_h_
#include "rtwtypes.h"
#include "control_foc_types.h"
#include "rtGetInf.h"

extern "C" {

#include "rt_nonfinite.h"

}
#include "zero_crossing_types.h"

// Block states (default storage) for model 'control_foc'
#ifndef control_foc_MDLREF_HIDE_CHILD_

struct DW_control_foc_f_T {
  real32_T FilterDifferentiatorTF_states;// '<S91>/Filter Differentiator TF'
  real32_T UnitDelay_DSTATE;           // '<S1>/Unit Delay'
  real32_T Integrator_DSTATE;          // '<S98>/Integrator'
  real32_T FilterDifferentiatorTF_states_k;// '<S39>/Filter Differentiator TF'
  real32_T Integrator_DSTATE_o;        // '<S46>/Integrator'
  ControlModes DelayInput1_DSTATE;     // '<S3>/Delay Input1'
  real32_T FilterDifferentiatorTF_tmp; // '<S91>/Filter Differentiator TF'
  real32_T FilterDifferentiatorTF_tmp_c;// '<S39>/Filter Differentiator TF'
  int8_T Integrator_PrevResetState;    // '<S98>/Integrator'
  int8_T Integrator_PrevResetState_k;  // '<S46>/Integrator'
};

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

// Zero-crossing (trigger) state for model 'control_foc'
#ifndef control_foc_MDLREF_HIDE_CHILD_

struct ZCE_control_foc_T {
  ZCSigState FilterDifferentiatorTF_Reset_ZC;// '<S91>/Filter Differentiator TF' 
  ZCSigState FilterDifferentiatorTF_Reset__o;// '<S39>/Filter Differentiator TF' 
};

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

// Invariant block signals for model 'control_foc'
#ifndef control_foc_MDLREF_HIDE_CHILD_

struct ConstB_control_foc_h_T {
  real32_T Gain5;                      // '<S1>/Gain5'
  real32_T Sum5;                       // '<S1>/Sum5'
};

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

#ifndef control_foc_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_control_foc_T {
  const char_T **errorStatus;
};

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

#ifndef control_foc_MDLREF_HIDE_CHILD_

struct MdlrefDW_control_foc_T {
  RT_MODEL_control_foc_T rtm;
};

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

extern void control_foc_Init(void);
extern void control_foc(const SensorsData *rtu_Sensors, const FOCSlowInputs
  *rtu_FOCSlowInputs, ControlOutputs *rty_FOCOutputs);

// Model reference registration function
extern void control_foc_initialize(const char_T **rt_errorStatus);

#ifndef control_foc_MDLREF_HIDE_CHILD_

extern MdlrefDW_control_foc_T control_foc_MdlrefDW;

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

#ifndef control_foc_MDLREF_HIDE_CHILD_

// Block states (default storage)
extern DW_control_foc_f_T control_foc_DW;

// Previous zero-crossings (trigger) states
extern ZCE_control_foc_T control_foc_PrevZCX;

#endif                                 //control_foc_MDLREF_HIDE_CHILD_

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S54>/Data Type Duplicate' : Unused code path elimination
//  Block '<S54>/Data Type Propagation' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S6>/Data Type Duplicate' : Unused code path elimination
//  Block '<S8>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Propagation' : Unused code path elimination
//  Block '<S39>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S91>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S109>/Kt' : Eliminated nontunable gain of 1
//  Block '<S64>/Offset' : Unused code path elimination
//  Block '<S115>/Offset' : Unused code path elimination


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
//  '<Root>' : 'control_foc'
//  '<S1>'   : 'control_foc/FOC inner loop'
//  '<S2>'   : 'control_foc/FOC inner loop/Clarke Transform'
//  '<S3>'   : 'control_foc/FOC inner loop/Detect Change'
//  '<S4>'   : 'control_foc/FOC inner loop/Id PID control'
//  '<S5>'   : 'control_foc/FOC inner loop/Inverse Clarke Transform'
//  '<S6>'   : 'control_foc/FOC inner loop/Inverse Park Transform'
//  '<S7>'   : 'control_foc/FOC inner loop/Iq PID control'
//  '<S8>'   : 'control_foc/FOC inner loop/Park Transform'
//  '<S9>'   : 'control_foc/FOC inner loop/Saturation Dynamic'
//  '<S10>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S11>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S12>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S13>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S14>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S15>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S16>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S17>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S19>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S20>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/External Parameters'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/Internal IC - Filter'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/Internal IC'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/Passthrough'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S63>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S64>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Switch_Axis'
//  '<S65>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S66>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S67>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S68>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S69>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S70>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S71>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S72>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S73>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S74>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/Internal IC - Filter'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/Internal IC'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/Passthrough'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S114>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S115>' : 'control_foc/FOC inner loop/Park Transform/Switch_Axis'

#endif                                 // RTW_HEADER_control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
