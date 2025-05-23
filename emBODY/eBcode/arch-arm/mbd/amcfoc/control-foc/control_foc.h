//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 8.9
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Fri May 23 10:48:16 2025
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
//  Block '<S14>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S65>/Data Type Duplicate' : Unused code path elimination
//  Block '<S65>/Data Type Propagation' : Unused code path elimination
//  Block '<S76>/Data Type Duplicate' : Unused code path elimination
//  Block '<S77>/Data Type Duplicate' : Unused code path elimination
//  Block '<S77>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S133>/Data Type Duplicate' : Unused code path elimination
//  Block '<S133>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S12>/Data Type Duplicate' : Unused code path elimination
//  Block '<S12>/Data Type Propagation' : Unused code path elimination
//  Block '<S13>/Kalpha' : Eliminated nontunable gain of 1
//  Block '<S13>/Kbeta' : Eliminated nontunable gain of 1
//  Block '<S50>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S76>/Ka' : Eliminated nontunable gain of 1
//  Block '<S76>/Kb' : Eliminated nontunable gain of 1
//  Block '<S76>/Kc' : Eliminated nontunable gain of 1
//  Block '<S108>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S126>/Kt' : Eliminated nontunable gain of 1
//  Block '<S78>/Offset' : Unused code path elimination
//  Block '<S78>/Unary_Minus' : Unused code path elimination
//  Block '<S134>/Offset' : Unused code path elimination
//  Block '<S134>/Unary_Minus' : Unused code path elimination


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
//  '<S13>'  : 'control_foc/FOC inner loop/Clarke Transform/Two phase input'
//  '<S14>'  : 'control_foc/FOC inner loop/Clarke Transform/Two phase input/Two phase CRL wrap'
//  '<S15>'  : 'control_foc/FOC inner loop/Compute RMS/Disabled'
//  '<S16>'  : 'control_foc/FOC inner loop/Compute Sin//Cos/STD'
//  '<S17>'  : 'control_foc/FOC inner loop/Feedforward/FF disabled'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S19>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S20>'  : 'control_foc/FOC inner loop/Id PID control/External Derivative'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/preInt Signal'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/External Parameters'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/External Derivative/Error'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/External IC'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/External IC'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S63>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S64>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S65>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S66>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S67>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S68>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S69>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S70>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S71>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S72>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S73>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S74>'  : 'control_foc/FOC inner loop/Id PID control/preInt Signal/Internal PreInt'
//  '<S75>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S76>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Two phase input'
//  '<S77>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL'
//  '<S78>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL/Switch_Axis'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/External Derivative'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/preInt Signal'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/External Derivative/Error'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/External IC'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S114>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S115>' : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S116>' : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/External IC'
//  '<S117>' : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S118>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S119>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S120>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S121>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S122>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S123>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S124>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S125>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S126>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S127>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S128>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S129>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S130>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S131>' : 'control_foc/FOC inner loop/Iq PID control/preInt Signal/Internal PreInt'
//  '<S132>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S133>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL'
//  '<S134>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL/Switch_Axis'

#endif                                 // control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
