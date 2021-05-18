//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: Current_FOC.h
//
// Code generated for Simulink model 'Current_FOC'.
//
// Model version                  : 2.15
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Sun May 16 18:18:18 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_Current_FOC_h_
#define RTW_HEADER_Current_FOC_h_
#include "rtwtypes.h"
#include "Current_FOC_types.h"
#include "arm_math.h"
#include "mw_cmsis.h"

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

// Class declaration for model Current_FOC
class Current_FOCModelClass {
  // public data and function members
 public:
  // Block states (default storage) for system '<Root>'
  struct DW_Current_FOC_T {
    real32_T Integrator_DSTATE;        // '<S39>/Integrator'
    real32_T Integrator_DSTATE_o;      // '<S88>/Integrator'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU_Current_FOC_T {
    real32_T I_ref;                    // '<Root>/I_ref'
    real32_T Iabc_fbk[3];              // '<Root>/Iabc_fbk'
    real32_T Speed_fbk;                // '<Root>/Speed_fbk'
    real32_T Angle_fbk;                // '<Root>/Angle_fbk'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY_Current_FOC_T {
    real32_T PWMabc_out[3];            // '<Root>/PWMabc_out'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_Current_FOC_T {
    const char_T * volatile errorStatus;
  };

  // model initialize function
  void initialize();

  // model step function
  void step();

  // model terminate function
  void terminate();

  // Constructor
  Current_FOCModelClass();

  // Destructor
  ~Current_FOCModelClass();

  // Root-level structure-based inputs set method

  // Root inports set method
  void setExternalInputs(const ExtU_Current_FOC_T* pExtU_Current_FOC_T)
  {
    Current_FOC_U = *pExtU_Current_FOC_T;
  }

  // Root-level structure-based outputs get method

  // Root outports get method
  const Current_FOCModelClass::ExtY_Current_FOC_T & getExternalOutputs() const
  {
    return Current_FOC_Y;
  }

  // Real-Time Model get method
  Current_FOCModelClass::RT_MODEL_Current_FOC_T * getRTM();

  // private data and function members
 private:
  // Block states
  DW_Current_FOC_T Current_FOC_DW;

  // External inputs
  ExtU_Current_FOC_T Current_FOC_U;

  // External outputs
  ExtY_Current_FOC_T Current_FOC_Y;

  // Real-Time Model
  RT_MODEL_Current_FOC_T Current_FOC_M;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Data Type Duplicate' : Unused code path elimination
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S7>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Scope' : Unused code path elimination
//  Block '<S1>/Gain7' : Eliminated nontunable gain of 1
//  Block '<S56>/Offset' : Unused code path elimination
//  Block '<S56>/Unary_Minus' : Unused code path elimination
//  Block '<S105>/Offset' : Unused code path elimination
//  Block '<S105>/Unary_Minus' : Unused code path elimination


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Note that this particular code originates from a subsystem build,
//  and has its own system numbers different from the parent model.
//  Refer to the system hierarchy for this subsystem below, and use the
//  MATLAB hilite_system command to trace the generated code back
//  to the parent model.  For example,
//
//  hilite_system('FOC_for_AMC_BLDC_code_generation/Current_FOC')    - opens subsystem FOC_for_AMC_BLDC_code_generation/Current_FOC
//  hilite_system('FOC_for_AMC_BLDC_code_generation/Current_FOC/Kp') - opens and selects block Kp
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'FOC_for_AMC_BLDC_code_generation'
//  '<S1>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC'
//  '<S2>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Clarke Transform'
//  '<S3>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID'
//  '<S4>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Inverse Clarke Transform'
//  '<S5>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Inverse Park Transform'
//  '<S6>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID'
//  '<S7>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Park Transform'
//  '<S8>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Anti-windup'
//  '<S9>'   : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/D Gain'
//  '<S10>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Filter'
//  '<S11>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Filter ICs'
//  '<S12>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/I Gain'
//  '<S13>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Ideal P Gain'
//  '<S14>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Ideal P Gain Fdbk'
//  '<S15>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Integrator'
//  '<S16>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Integrator ICs'
//  '<S17>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/N Copy'
//  '<S18>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/N Gain'
//  '<S19>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/P Copy'
//  '<S20>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Parallel P Gain'
//  '<S21>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Reset Signal'
//  '<S22>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Saturation'
//  '<S23>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Saturation Fdbk'
//  '<S24>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Sum'
//  '<S25>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Sum Fdbk'
//  '<S26>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tracking Mode'
//  '<S27>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tracking Mode Sum'
//  '<S28>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tsamp - Integral'
//  '<S29>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tsamp - Ngain'
//  '<S30>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/postSat Signal'
//  '<S31>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/preSat Signal'
//  '<S32>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Anti-windup/Passthrough'
//  '<S33>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/D Gain/Disabled'
//  '<S34>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Filter/Disabled'
//  '<S35>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Filter ICs/Disabled'
//  '<S36>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/I Gain/External Parameters'
//  '<S37>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Ideal P Gain/Passthrough'
//  '<S38>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Ideal P Gain Fdbk/Disabled'
//  '<S39>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Integrator/Discrete'
//  '<S40>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Integrator ICs/Internal IC'
//  '<S41>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/N Copy/Disabled wSignal Specification'
//  '<S42>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/N Gain/Disabled'
//  '<S43>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/P Copy/Disabled'
//  '<S44>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Parallel P Gain/External Parameters'
//  '<S45>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Reset Signal/Disabled'
//  '<S46>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Saturation/Enabled'
//  '<S47>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Saturation Fdbk/Disabled'
//  '<S48>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Sum/Sum_PI'
//  '<S49>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Sum Fdbk/Disabled'
//  '<S50>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tracking Mode/Disabled'
//  '<S51>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tracking Mode Sum/Passthrough'
//  '<S52>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tsamp - Integral/Passthrough'
//  '<S53>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/Tsamp - Ngain/Passthrough'
//  '<S54>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/postSat Signal/Forward_Path'
//  '<S55>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Id control PID/preSat Signal/Forward_Path'
//  '<S56>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Inverse Park Transform/Switch_Axis'
//  '<S57>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Anti-windup'
//  '<S58>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/D Gain'
//  '<S59>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Filter'
//  '<S60>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Filter ICs'
//  '<S61>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/I Gain'
//  '<S62>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Ideal P Gain'
//  '<S63>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Ideal P Gain Fdbk'
//  '<S64>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Integrator'
//  '<S65>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Integrator ICs'
//  '<S66>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/N Copy'
//  '<S67>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/N Gain'
//  '<S68>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/P Copy'
//  '<S69>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Parallel P Gain'
//  '<S70>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Reset Signal'
//  '<S71>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Saturation'
//  '<S72>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Saturation Fdbk'
//  '<S73>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Sum'
//  '<S74>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Sum Fdbk'
//  '<S75>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tracking Mode'
//  '<S76>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tracking Mode Sum'
//  '<S77>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tsamp - Integral'
//  '<S78>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tsamp - Ngain'
//  '<S79>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/postSat Signal'
//  '<S80>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/preSat Signal'
//  '<S81>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Anti-windup/Passthrough'
//  '<S82>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/D Gain/Disabled'
//  '<S83>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Filter/Disabled'
//  '<S84>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Filter ICs/Disabled'
//  '<S85>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/I Gain/External Parameters'
//  '<S86>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Ideal P Gain/Passthrough'
//  '<S87>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Ideal P Gain Fdbk/Disabled'
//  '<S88>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Integrator/Discrete'
//  '<S89>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Integrator ICs/Internal IC'
//  '<S90>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/N Copy/Disabled wSignal Specification'
//  '<S91>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/N Gain/Disabled'
//  '<S92>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/P Copy/Disabled'
//  '<S93>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Parallel P Gain/External Parameters'
//  '<S94>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Reset Signal/Disabled'
//  '<S95>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Saturation/Enabled'
//  '<S96>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Saturation Fdbk/Disabled'
//  '<S97>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Sum/Sum_PI'
//  '<S98>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Sum Fdbk/Disabled'
//  '<S99>'  : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tracking Mode/Disabled'
//  '<S100>' : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tracking Mode Sum/Passthrough'
//  '<S101>' : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tsamp - Integral/Passthrough'
//  '<S102>' : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/Tsamp - Ngain/Passthrough'
//  '<S103>' : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/postSat Signal/Forward_Path'
//  '<S104>' : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Iq control PID/preSat Signal/Forward_Path'
//  '<S105>' : 'FOC_for_AMC_BLDC_code_generation/Current_FOC/Park Transform/Switch_Axis'

#endif                                 // RTW_HEADER_Current_FOC_h_

//
// File trailer for generated code.
//
// [EOF]
//
