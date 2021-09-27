//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 1.128
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:59 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_foc_h_
#define RTW_HEADER_control_foc_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "control_foc_types.h"

// Child system includes
#include "FOCInnerLoop.h"
#include <stddef.h>
#include "rtGetInf.h"
#include "rt_nonfinite.h"

// Class declaration for model control_foc
class control_focModelClass {
  // public data and function members
 public:
  // Block states (default storage) for model 'control_foc'
  struct DW_control_foc_T {
    real32_T Integrator_DSTATE;        // '<S43>/Integrator'
    real32_T UnitDelay_DSTATE;         // '<S1>/Unit Delay'
    real32_T Integrator_DSTATE_m;      // '<S93>/Integrator'
    int8_T Integrator_PrevResetState;  // '<S43>/Integrator'
    int8_T Integrator_PrevResetState_k;// '<S93>/Integrator'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_control_foc_T {
    const char_T **errorStatus;
  };

  // model initialize function
  void initialize();

  // model step function
  void control_foc_ISR(const boolean_T *rtu_Flags_PID_reset, const real32_T
                       *rtu_Config_motorconfig_Kp, const real32_T
                       *rtu_Config_motorconfig_Ki, const real32_T
                       *rtu_Config_motorconfig_Kbemf, const real32_T
                       *rtu_Config_motorconfig_Rphase, const real32_T
                       *rtu_Config_motorconfig_Vmax, const real32_T
                       *rtu_Config_motorconfig_Vcc, const real32_T
                       rtu_Sensors_motorsensors_Iabc[3], const real32_T
                       *rtu_Sensors_motorsensors_angl_k, const real32_T
                       *rtu_Sensors_motorsensors_omeg_k, const real32_T
                       *rtu_Targets_motorcurrent_curr_k, const real32_T
                       *rtu_Targets_motorvoltage_volt_e, const boolean_T
                       *rtu_OuterOutputs_vel_en, const boolean_T
                       *rtu_OuterOutputs_cur_en, const boolean_T
                       *rtu_OuterOutputs_out_en, const real32_T
                       *rtu_OuterOutputs_motorcurrent_p, uint16_T
                       rty_Vabc_PWM_ticks[3], real32_T *rty_Iq_fbk_current);

  // Constructor
  control_focModelClass();

  // Destructor
  ~control_focModelClass();

  // Real-Time Model get method
  control_focModelClass::RT_MODEL_control_foc_T * getRTM();

  //member function to setup error status pointer
  void setErrorStatusPointer(const char_T **rt_errorStatus);

  // private data and function members
 private:
  // Block states
  DW_control_foc_T control_foc_DW;

  // Real-Time Model
  RT_MODEL_control_foc_T control_foc_M;

  // private member function(s) for subsystem '<Root>/FOC inner loop'
  void FOCInnerLoop(const real32_T *rtu_Iref, const real32_T *rtu_TargetCurrent,
                    const real32_T *rtu_Vref, const boolean_T *rtu_Reset, const
                    boolean_T *rtu_Currentctrlen, const boolean_T
                    *rtu_Velocityctrlen, const boolean_T *rtu_Outputen, const
                    real32_T *rtu_RotorAngle, const real32_T rtu_Iabc_fbk[3],
                    const real32_T *rtu_MotorConfig, const real32_T
                    *rtu_MotorConfig_e, const real32_T *rtu_MotorConfig_i, const
                    real32_T *rtu_MotorConfig_ig, const real32_T
                    *rtu_MotorConfig_a, const real32_T *rtu_MotorConfig_c, const
                    real32_T *rtu_RotorSpeed, uint16_T rty_Vabc[3], real32_T
                    *rty_Iq_fbk);
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Data Type Duplicate' : Unused code path elimination
//  Block '<S1>/Direct Lookup Table (n-D)1' : Unused code path elimination
//  Block '<S51>/Data Type Duplicate' : Unused code path elimination
//  Block '<S51>/Data Type Propagation' : Unused code path elimination
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S7>/Data Type Duplicate' : Unused code path elimination
//  Block '<S8>/Data Type Duplicate' : Unused code path elimination
//  Block '<S8>/Data Type Propagation' : Unused code path elimination
//  Block '<S104>/Kt' : Eliminated nontunable gain of 1
//  Block '<S61>/Offset' : Unused code path elimination
//  Block '<S110>/Offset' : Unused code path elimination


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
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/Disabled'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disabled'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/Disabled'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/Internal IC'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/Disabled wSignal Specification'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/Disabled'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PI'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/Passthrough'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S61>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Switch_Axis'
//  '<S62>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S63>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S64>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S65>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S66>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S67>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S68>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S69>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S70>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S71>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S72>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S73>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S74>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain/Disabled'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disabled'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/Disabled'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/Internal IC'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy/Disabled wSignal Specification'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain/Disabled'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PI'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/Passthrough'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S110>' : 'control_foc/FOC inner loop/Park Transform/Switch_Axis'

#endif                                 // RTW_HEADER_control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
