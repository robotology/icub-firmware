//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 9.11
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Tue Jul  8 13:04:41 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef control_foc_h_
#define control_foc_h_
#include "rtwtypes.h"
#include "control_foc_types.h"
#include "FOCInnerLoop.h"

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

struct MdlrefDW_control_foc_T {
  B_control_foc_c_T rtb;
  DW_control_foc_f_T rtdw;
  ZCE_control_foc_T rtzce;
};

// Model reference registration function
extern void control_foc_initialize(ZCE_control_foc_T *localZCE);
extern void control_foc_Init(DW_control_foc_f_T *localDW);
extern void control_foc(const SensorsData *rtu_Sensors, const FOCSlowInputs
  *rtu_FocSlowInputs, FOCOutputs *rty_FOCOutputs, B_control_foc_c_T *localB,
  DW_control_foc_f_T *localDW, ZCE_control_foc_T *localZCE);

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S17>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S68>/Data Type Duplicate' : Unused code path elimination
//  Block '<S68>/Data Type Propagation' : Unused code path elimination
//  Block '<S82>/Data Type Duplicate' : Unused code path elimination
//  Block '<S86>/Data Type Duplicate' : Unused code path elimination
//  Block '<S86>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S145>/Data Type Duplicate' : Unused code path elimination
//  Block '<S145>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S12>/Data Type Duplicate' : Unused code path elimination
//  Block '<S12>/Data Type Propagation' : Unused code path elimination
//  Block '<S16>/Kalpha' : Eliminated nontunable gain of 1
//  Block '<S16>/Kbeta' : Eliminated nontunable gain of 1
//  Block '<S53>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S82>/Ka' : Eliminated nontunable gain of 1
//  Block '<S82>/Kb' : Eliminated nontunable gain of 1
//  Block '<S82>/Kc' : Eliminated nontunable gain of 1
//  Block '<S117>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S135>/Kt' : Eliminated nontunable gain of 1
//  Block '<S87>/Offset' : Unused code path elimination
//  Block '<S87>/Unary_Minus' : Unused code path elimination
//  Block '<S146>/Offset' : Unused code path elimination
//  Block '<S146>/Unary_Minus' : Unused code path elimination


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
//  '<S6>'   : 'control_foc/FOC inner loop/Feedforward'
//  '<S7>'   : 'control_foc/FOC inner loop/Id PID control'
//  '<S8>'   : 'control_foc/FOC inner loop/Inverse Clarke Transform'
//  '<S9>'   : 'control_foc/FOC inner loop/Inverse Park Transform'
//  '<S10>'  : 'control_foc/FOC inner loop/Iq PID control'
//  '<S11>'  : 'control_foc/FOC inner loop/Park Transform'
//  '<S12>'  : 'control_foc/FOC inner loop/Saturation Dynamic'
//  '<S13>'  : 'control_foc/FOC inner loop/Clarke Transform/Variant'
//  '<S14>'  : 'control_foc/FOC inner loop/Clarke Transform/Variant/mcb'
//  '<S15>'  : 'control_foc/FOC inner loop/Clarke Transform/Variant/mcb/Clarke Transform'
//  '<S16>'  : 'control_foc/FOC inner loop/Clarke Transform/Variant/mcb/Clarke Transform/Two phase input'
//  '<S17>'  : 'control_foc/FOC inner loop/Clarke Transform/Variant/mcb/Clarke Transform/Two phase input/Two phase CRL wrap'
//  '<S18>'  : 'control_foc/FOC inner loop/Compute RMS/Disabled'
//  '<S19>'  : 'control_foc/FOC inner loop/Compute Sin//Cos/STD'
//  '<S20>'  : 'control_foc/FOC inner loop/Feedforward/FF disabled'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/External Derivative'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/preInt Signal'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/External Parameters'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/External Derivative/Error'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/External IC'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/External IC'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S63>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S64>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S65>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S66>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S67>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S68>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S69>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S70>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S71>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S72>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S73>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S74>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S75>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S76>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S77>'  : 'control_foc/FOC inner loop/Id PID control/preInt Signal/Internal PreInt'
//  '<S78>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S79>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Variant'
//  '<S80>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Variant/mcb'
//  '<S81>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Variant/mcb/Inverse Clarke Transform'
//  '<S82>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Variant/mcb/Inverse Clarke Transform/Two phase input'
//  '<S83>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Variant'
//  '<S84>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Variant/mcb'
//  '<S85>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Variant/mcb/Inverse Park Transform'
//  '<S86>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Variant/mcb/Inverse Park Transform/Two inputs CRL'
//  '<S87>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Variant/mcb/Inverse Park Transform/Two inputs CRL/Switch_Axis'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/External Derivative'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/preInt Signal'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S114>' : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S115>' : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S116>' : 'control_foc/FOC inner loop/Iq PID control/External Derivative/Error'
//  '<S117>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S118>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S119>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S120>' : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/External IC'
//  '<S121>' : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S122>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S123>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S124>' : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S125>' : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/External IC'
//  '<S126>' : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S127>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S128>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S129>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S130>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S131>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S132>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S133>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S134>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S135>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S136>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S137>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S138>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S139>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S140>' : 'control_foc/FOC inner loop/Iq PID control/preInt Signal/Internal PreInt'
//  '<S141>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S142>' : 'control_foc/FOC inner loop/Park Transform/Variant'
//  '<S143>' : 'control_foc/FOC inner loop/Park Transform/Variant/mcb'
//  '<S144>' : 'control_foc/FOC inner loop/Park Transform/Variant/mcb/Park Transform'
//  '<S145>' : 'control_foc/FOC inner loop/Park Transform/Variant/mcb/Park Transform/Two inputs CRL'
//  '<S146>' : 'control_foc/FOC inner loop/Park Transform/Variant/mcb/Park Transform/Two inputs CRL/Switch_Axis'

#endif                                 // control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
