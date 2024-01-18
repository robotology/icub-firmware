//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.16
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Mon Jan 15 18:21:34 2024
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

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S13>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S61>/Data Type Duplicate' : Unused code path elimination
//  Block '<S61>/Data Type Propagation' : Unused code path elimination
//  Block '<S71>/Data Type Duplicate' : Unused code path elimination
//  Block '<S72>/Data Type Duplicate' : Unused code path elimination
//  Block '<S72>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S124>/Data Type Duplicate' : Unused code path elimination
//  Block '<S124>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S11>/Data Type Duplicate' : Unused code path elimination
//  Block '<S11>/Data Type Propagation' : Unused code path elimination
//  Block '<S12>/Kalpha' : Eliminated nontunable gain of 1
//  Block '<S12>/Kbeta' : Eliminated nontunable gain of 1
//  Block '<S46>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S71>/Ka' : Eliminated nontunable gain of 1
//  Block '<S71>/Kb' : Eliminated nontunable gain of 1
//  Block '<S71>/Kc' : Eliminated nontunable gain of 1
//  Block '<S100>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S118>/Kt' : Eliminated nontunable gain of 1
//  Block '<S73>/Offset' : Unused code path elimination
//  Block '<S73>/Unary_Minus' : Unused code path elimination
//  Block '<S125>/Offset' : Unused code path elimination
//  Block '<S125>/Unary_Minus' : Unused code path elimination


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
//  '<S3>'   : 'control_foc/FOC inner loop/Compute RMS'
//  '<S4>'   : 'control_foc/FOC inner loop/Compute Sin//Cos'
//  '<S5>'   : 'control_foc/FOC inner loop/Degrees to Radians'
//  '<S6>'   : 'control_foc/FOC inner loop/Id PID control'
//  '<S7>'   : 'control_foc/FOC inner loop/Inverse Clarke Transform'
//  '<S8>'   : 'control_foc/FOC inner loop/Inverse Park Transform'
//  '<S9>'   : 'control_foc/FOC inner loop/Iq PID control'
//  '<S10>'  : 'control_foc/FOC inner loop/Park Transform'
//  '<S11>'  : 'control_foc/FOC inner loop/Saturation Dynamic'
//  '<S12>'  : 'control_foc/FOC inner loop/Clarke Transform/Two phase input'
//  '<S13>'  : 'control_foc/FOC inner loop/Clarke Transform/Two phase input/Two phase CRL wrap'
//  '<S14>'  : 'control_foc/FOC inner loop/Compute RMS/Disabled'
//  '<S15>'  : 'control_foc/FOC inner loop/Compute Sin//Cos/CMSIS'
//  '<S16>'  : 'control_foc/FOC inner loop/Compute Sin//Cos/CMSIS/Wrap Angle'
//  '<S17>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S19>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S20>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/External Parameters'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/Internal IC - Filter'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/Internal IC'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S63>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S64>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S65>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S66>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S67>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S68>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S69>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S70>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S71>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Two phase input'
//  '<S72>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL'
//  '<S73>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL/Switch_Axis'
//  '<S74>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/Internal IC - Filter'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/Internal IC'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S114>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S115>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S116>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S117>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S118>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S119>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S120>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S121>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S122>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S123>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S124>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL'
//  '<S125>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL/Switch_Axis'

#endif                                 // RTW_HEADER_control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
