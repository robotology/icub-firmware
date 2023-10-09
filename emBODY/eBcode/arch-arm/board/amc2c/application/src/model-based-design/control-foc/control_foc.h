//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 5.12
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Tue Jun 27 10:18:32 2023
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

#include "zero_crossing_types.h"

// Block signals for model 'control_foc'
struct B_control_foc_c_T {
  B_FOCInnerLoop_T FOCinnerloop;       // '<Root>/FOC inner loop'
};

// Block states (default storage) for model 'control_foc'
struct DW_control_foc_f_T {
  DW_FOCInnerLoop_T FOCinnerloop;      // '<Root>/FOC inner loop'
};

// Zero-crossing (trigger) state for model 'control_foc'
struct ZCV_control_foc_g_T {
  ZCV_FOCInnerLoop_T FOCinnerloop;     // '<Root>/FOC inner loop'
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
//  Block '<S2>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S53>/Data Type Duplicate' : Unused code path elimination
//  Block '<S53>/Data Type Propagation' : Unused code path elimination
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S7>/Data Type Duplicate' : Unused code path elimination
//  Block '<S7>/Data Type Duplicate1' : Unused code path elimination
//  Block '<S8>/Data Type Duplicate' : Unused code path elimination
//  Block '<S8>/Data Type Propagation' : Unused code path elimination
//  Block '<S38>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S90>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S108>/Kt' : Eliminated nontunable gain of 1
//  Block '<S63>/Offset' : Unused code path elimination
//  Block '<S114>/Offset' : Unused code path elimination


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
//  '<S3>'   : 'control_foc/FOC inner loop/Id PID control'
//  '<S4>'   : 'control_foc/FOC inner loop/Inverse Clarke Transform'
//  '<S5>'   : 'control_foc/FOC inner loop/Inverse Park Transform'
//  '<S6>'   : 'control_foc/FOC inner loop/Iq PID control'
//  '<S7>'   : 'control_foc/FOC inner loop/Park Transform'
//  '<S8>'   : 'control_foc/FOC inner loop/Saturation Dynamic'
//  '<S9>'   : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S10>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S11>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S12>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S13>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S14>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S15>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S16>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S17>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S19>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S20>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/External Parameters'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/Internal IC - Filter'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/Internal IC'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/External Parameters'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/External Parameters'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PID'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S62>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S63>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Switch_Axis'
//  '<S64>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S65>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S66>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S67>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S68>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S69>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S70>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S71>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S72>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S73>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S74>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain/External Parameters'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/Internal IC - Filter'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/Internal IC'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy/External Parameters'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/N Gain/External Parameters'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PID'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/TsSignalSpecification'
//  '<S111>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S112>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S113>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S114>' : 'control_foc/FOC inner loop/Park Transform/Switch_Axis'

#endif                                 // RTW_HEADER_control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
