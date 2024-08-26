//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 7.4
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Aug 26 12:35:20 2024
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
//  Block '<S13>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S62>/Data Type Duplicate' : Unused code path elimination
//  Block '<S62>/Data Type Propagation' : Unused code path elimination
//  Block '<S72>/Data Type Duplicate' : Unused code path elimination
//  Block '<S73>/Data Type Duplicate' : Unused code path elimination
//  Block '<S73>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S127>/Data Type Duplicate' : Unused code path elimination
//  Block '<S127>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S11>/Data Type Duplicate' : Unused code path elimination
//  Block '<S11>/Data Type Propagation' : Unused code path elimination
//  Block '<S12>/Kalpha' : Eliminated nontunable gain of 1
//  Block '<S12>/Kbeta' : Eliminated nontunable gain of 1
//  Block '<S47>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S72>/Ka' : Eliminated nontunable gain of 1
//  Block '<S72>/Kb' : Eliminated nontunable gain of 1
//  Block '<S72>/Kc' : Eliminated nontunable gain of 1
//  Block '<S103>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S121>/Kt' : Eliminated nontunable gain of 1
//  Block '<S74>/Offset' : Unused code path elimination
//  Block '<S74>/Unary_Minus' : Unused code path elimination
//  Block '<S128>/Offset' : Unused code path elimination
//  Block '<S128>/Unary_Minus' : Unused code path elimination


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
//  '<S15>'  : 'control_foc/FOC inner loop/Compute Sin//Cos/STD'
//  '<S16>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S17>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/External Derivative'
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
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/External Derivative/Error'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/External IC'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/External IC'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S63>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S64>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S65>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S66>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S67>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S68>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S69>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S70>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S71>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S72>'  : 'control_foc/FOC inner loop/Inverse Clarke Transform/Two phase input'
//  '<S73>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL'
//  '<S74>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Two inputs CRL/Switch_Axis'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/External Derivative'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/External Derivative/Error'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/External IC'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/External IC'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S114>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S115>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S116>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S117>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S118>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S119>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S120>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S121>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S122>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S123>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S124>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S125>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S126>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S127>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL'
//  '<S128>' : 'control_foc/FOC inner loop/Park Transform/Two inputs CRL/Switch_Axis'

#endif                                 // control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
