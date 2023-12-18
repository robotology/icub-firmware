//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.4
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Thu Dec 14 10:43:43 2023
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
#include "FOCInnerLoop.h"
#include <cstring>

extern "C"
{

#include "rt_nonfinite.h"

}

// Block signals for model 'control_foc'
struct B_control_foc_c_T {
  B_FOCInnerLoop_T FOCinnerloop;       // '<Root>/FOC inner loop'
};

// Block states (default storage) for model 'control_foc'
struct DW_control_foc_f_T {
  DW_FOCInnerLoop_T FOCinnerloop;      // '<Root>/FOC inner loop'
};

// Zero-crossing (trigger) state for model 'control_foc'
struct ZCE_control_foc_T {
  ZCE_FOCInnerLoop_T FOCinnerloop;     // '<Root>/FOC inner loop'
};

// Invariant block signals for model 'control_foc'
struct ConstB_control_foc_h_T {
  ConstB_FOCInnerLoop_T FOCinnerloop;  // '<Root>/FOC inner loop'
};

// Real-time Model Data Structure
struct tag_RTM_control_foc_T {
  const char_T **errorStatus;
};

struct MdlrefDW_control_foc_T {
  B_control_foc_c_T rtb;
  DW_control_foc_f_T rtdw;
  RT_MODEL_control_foc_T rtm;
  ZCE_control_foc_T rtzce;
};

// Model reference registration function
extern void control_foc_initialize(const char_T **rt_errorStatus,
  RT_MODEL_control_foc_T *const control_foc_M, B_control_foc_c_T *localB,
  DW_control_foc_f_T *localDW, ZCE_control_foc_T *localZCE);
extern void control_foc_Init(DW_control_foc_f_T *localDW);
extern void control_foc(const SensorsData *rtu_Sensors, const FOCSlowInputs
  *rtu_FOCSlowInputs, ControlOutputs *rty_FOCOutputs, B_control_foc_c_T *localB,
  DW_control_foc_f_T *localDW, ZCE_control_foc_T *localZCE);
extern void control_foc_Term(DW_control_foc_f_T *localDW);

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S11>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S56>/Data Type Duplicate' : Unused code path elimination
//  Block '<S56>/Data Type Propagation' : Unused code path elimination
//  Block '<S66>/Data Type Duplicate' : Unused code path elimination
//  Block '<S67>/Data Type Duplicate' : Unused code path elimination
//  Block '<S67>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S119>/Data Type Duplicate' : Unused code path elimination
//  Block '<S119>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S9>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Propagation' : Unused code path elimination
//  Block '<S10>/Kalpha' : Eliminated nontunable gain of 1
//  Block '<S10>/Kbeta' : Eliminated nontunable gain of 1
//  Block '<S41>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S66>/Ka' : Eliminated nontunable gain of 1
//  Block '<S66>/Kb' : Eliminated nontunable gain of 1
//  Block '<S66>/Kc' : Eliminated nontunable gain of 1
//  Block '<S95>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S113>/Kt' : Eliminated nontunable gain of 1
//  Block '<S68>/Offset' : Unused code path elimination
//  Block '<S68>/Unary_Minus' : Unused code path elimination
//  Block '<S120>/Offset' : Unused code path elimination
//  Block '<S120>/Unary_Minus' : Unused code path elimination


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
//  '<S3>'   : 'control_foc/FOC inner loop/Degrees to Radians'
//  '<S4>'   : 'control_foc/FOC inner loop/Id PID control'
//  '<S5>'   : 'control_foc/FOC inner loop/Inverse Clarke Transform'
//  '<S6>'   : 'control_foc/FOC inner loop/Inverse Park Transform'
//  '<S7>'   : 'control_foc/FOC inner loop/Iq PID control'
//  '<S8>'   : 'control_foc/FOC inner loop/Park Transform'
//  '<S9>'   : 'control_foc/FOC inner loop/Saturation Dynamic'
//  '<S10>'  : 'control_foc/FOC inner loop/Clarke Transform/Two phase input'
//  '<S11>'  : 'control_foc/FOC inner loop/Clarke Transform/Two phase input/Two phase CRL wrap'
//  '<S12>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S13>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S14>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S15>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S16>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S17>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S19>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S20>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/External Parameters'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/Internal IC - Filter'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/Internal IC'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S63>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S64>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S65>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S66>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Two phase input'
//  '<S67>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL'
//  '<S68>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL/Switch_Axis'
//  '<S69>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S70>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S71>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S72>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S73>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S74>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/Internal IC - Filter'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/Internal IC'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S114>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S115>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S116>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S117>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S118>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S119>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL'
//  '<S120>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL/Switch_Axis'

#endif                                 // RTW_HEADER_control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
