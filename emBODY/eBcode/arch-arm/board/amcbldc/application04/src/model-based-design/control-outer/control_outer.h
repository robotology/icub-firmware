//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: control_outer.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 1.100
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:44:08 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_outer_h_
#define RTW_HEADER_control_outer_h_
#include <cmath>
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "control_outer_types.h"
#include <stddef.h>
#include "rtGetNaN.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"

// Class declaration for model control_outer
class control_outerModelClass {
  // public data and function members
 public:
  // Block states (default storage) for model 'control_outer'
  struct DW_control_outer_T {
    real32_T UD_DSTATE;                // '<S1>/UD'
    real32_T FilterDifferentiatorTF_states;// '<S42>/Filter Differentiator TF'
    real32_T UnitDelay_DSTATE;         // '<Root>/Unit Delay'
    real32_T Integrator_DSTATE;        // '<S49>/Integrator'
    real32_T FilterDifferentiatorTF_states_i;// '<S92>/Filter Differentiator TF' 
    real32_T Integrator_DSTATE_i;      // '<S99>/Integrator'
    real32_T FilterDifferentiatorTF_states_c;// '<S144>/Filter Differentiator TF' 
    real32_T Integrator_DSTATE_b;      // '<S151>/Integrator'
    real32_T FilterDifferentiatorTF_tmp;// '<S42>/Filter Differentiator TF'
    real32_T FilterDifferentiatorTF_tmp_m;// '<S92>/Filter Differentiator TF'
    real32_T FilterDifferentiatorTF_tmp_p;// '<S144>/Filter Differentiator TF'
    int8_T Integrator_PrevResetState;  // '<S49>/Integrator'
    int8_T Integrator_PrevResetState_n;// '<S99>/Integrator'
    int8_T Integrator_PrevResetState_c;// '<S151>/Integrator'
  };

  // Zero-crossing (trigger) state for model 'control_outer'
  struct PrevZCX_control_outer_T {
    ZCSigState FilterDifferentiatorTF_Reset_ZC;// '<S42>/Filter Differentiator TF' 
    ZCSigState FilterDifferentiatorTF_Reset__m;// '<S92>/Filter Differentiator TF' 
    ZCSigState FilterDifferentiatorTF_Reset__e;// '<S144>/Filter Differentiator TF' 
  };

  // Real-time Model Data Structure
  struct RT_MODEL_control_outer_T {
    const char_T **errorStatus;
  };

  // model initialize function
  void initialize();

  // model step function
  void step(const ControlModes *rtu_Flags_control_mode, const boolean_T
            *rtu_Flags_PID_reset, const real32_T
            rtu_Config_velocitylimits_limit[2], const real32_T
            *rtu_Config_motorconfig_reductio, const boolean_T
            *rtu_Config_motorconfig_has_spee, const real32_T
            *rtu_Config_PosLoopPID_P, const real32_T *rtu_Config_PosLoopPID_I,
            const real32_T *rtu_Config_PosLoopPID_D, const real32_T
            *rtu_Config_PosLoopPID_N, const real32_T *rtu_Config_VelLoopPID_P,
            const real32_T *rtu_Config_VelLoopPID_I, const real32_T
            *rtu_Config_VelLoopPID_D, const real32_T *rtu_Config_VelLoopPID_N,
            const real32_T *rtu_Config_DirLoopPID_P, const real32_T
            *rtu_Config_DirLoopPID_I, const real32_T *rtu_Config_DirLoopPID_D,
            const real32_T *rtu_Config_DirLoopPID_N, const real32_T
            *rtu_Sensors_jointpositions_posi, const real32_T
            *rtu_Sensors_motorsensors_omega, const real32_T
            *rtu_Targets_jointpositions_posi, const real32_T
            *rtu_Targets_jointvelocities_vel, boolean_T *rty_OuterOutputs_vel_en,
            boolean_T *rty_OuterOutputs_cur_en, boolean_T
            *rty_OuterOutputs_out_en, real32_T *rty_OuterOutputs_motorcurrent_c);

  // Constructor
  control_outerModelClass();

  // Destructor
  ~control_outerModelClass();

  // Real-Time Model get method
  control_outerModelClass::RT_MODEL_control_outer_T * getRTM();

  //member function to setup error status pointer
  void setErrorStatusPointer(const char_T **rt_errorStatus);

  // private data and function members
 private:
  // Block states
  DW_control_outer_T control_outer_DW;
  PrevZCX_control_outer_T control_outer_PrevZCX;// Triggered events

  // Real-Time Model
  RT_MODEL_control_outer_T control_outer_M;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S1>/Data Type Duplicate' : Unused code path elimination
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S4>/Data Type Propagation' : Unused code path elimination
//  Block '<S42>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S60>/Kt' : Eliminated nontunable gain of 1
//  Block '<S92>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S110>/Kt' : Eliminated nontunable gain of 1
//  Block '<S144>/Passthrough for tuning' : Eliminate redundant data type conversion


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
//  '<S1>'   : 'control_outer/Discrete Derivative'
//  '<S2>'   : 'control_outer/Enabling Logic'
//  '<S3>'   : 'control_outer/Position loop'
//  '<S4>'   : 'control_outer/Saturation Dynamic'
//  '<S5>'   : 'control_outer/Vel PID control'
//  '<S6>'   : 'control_outer/Enabling Logic/Compare To Constant'
//  '<S7>'   : 'control_outer/Enabling Logic/Compare To Constant1'
//  '<S8>'   : 'control_outer/Enabling Logic/Compare To Constant2'
//  '<S9>'   : 'control_outer/Enabling Logic/Compare To Constant3'
//  '<S10>'  : 'control_outer/Enabling Logic/Compare To Constant4'
//  '<S11>'  : 'control_outer/Enabling Logic/Compare To Constant5'
//  '<S12>'  : 'control_outer/Enabling Logic/Compare To Constant6'
//  '<S13>'  : 'control_outer/Enabling Logic/Compare To Constant7'
//  '<S14>'  : 'control_outer/Position loop/Position PID'
//  '<S15>'  : 'control_outer/Position loop/Position-Direct PID'
//  '<S16>'  : 'control_outer/Position loop/Position PID/Anti-windup'
//  '<S17>'  : 'control_outer/Position loop/Position PID/D Gain'
//  '<S18>'  : 'control_outer/Position loop/Position PID/Filter'
//  '<S19>'  : 'control_outer/Position loop/Position PID/Filter ICs'
//  '<S20>'  : 'control_outer/Position loop/Position PID/I Gain'
//  '<S21>'  : 'control_outer/Position loop/Position PID/Ideal P Gain'
//  '<S22>'  : 'control_outer/Position loop/Position PID/Ideal P Gain Fdbk'
//  '<S23>'  : 'control_outer/Position loop/Position PID/Integrator'
//  '<S24>'  : 'control_outer/Position loop/Position PID/Integrator ICs'
//  '<S25>'  : 'control_outer/Position loop/Position PID/N Copy'
//  '<S26>'  : 'control_outer/Position loop/Position PID/N Gain'
//  '<S27>'  : 'control_outer/Position loop/Position PID/P Copy'
//  '<S28>'  : 'control_outer/Position loop/Position PID/Parallel P Gain'
//  '<S29>'  : 'control_outer/Position loop/Position PID/Reset Signal'
//  '<S30>'  : 'control_outer/Position loop/Position PID/Saturation'
//  '<S31>'  : 'control_outer/Position loop/Position PID/Saturation Fdbk'
//  '<S32>'  : 'control_outer/Position loop/Position PID/Sum'
//  '<S33>'  : 'control_outer/Position loop/Position PID/Sum Fdbk'
//  '<S34>'  : 'control_outer/Position loop/Position PID/Tracking Mode'
//  '<S35>'  : 'control_outer/Position loop/Position PID/Tracking Mode Sum'
//  '<S36>'  : 'control_outer/Position loop/Position PID/Tsamp - Integral'
//  '<S37>'  : 'control_outer/Position loop/Position PID/Tsamp - Ngain'
//  '<S38>'  : 'control_outer/Position loop/Position PID/postSat Signal'
//  '<S39>'  : 'control_outer/Position loop/Position PID/preSat Signal'
//  '<S40>'  : 'control_outer/Position loop/Position PID/Anti-windup/Passthrough'
//  '<S41>'  : 'control_outer/Position loop/Position PID/D Gain/External Parameters'
//  '<S42>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S43>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S44>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S45>'  : 'control_outer/Position loop/Position PID/Filter ICs/Internal IC - Filter'
//  '<S46>'  : 'control_outer/Position loop/Position PID/I Gain/External Parameters'
//  '<S47>'  : 'control_outer/Position loop/Position PID/Ideal P Gain/Passthrough'
//  '<S48>'  : 'control_outer/Position loop/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S49>'  : 'control_outer/Position loop/Position PID/Integrator/Discrete'
//  '<S50>'  : 'control_outer/Position loop/Position PID/Integrator ICs/Internal IC'
//  '<S51>'  : 'control_outer/Position loop/Position PID/N Copy/External Parameters'
//  '<S52>'  : 'control_outer/Position loop/Position PID/N Gain/External Parameters'
//  '<S53>'  : 'control_outer/Position loop/Position PID/P Copy/Disabled'
//  '<S54>'  : 'control_outer/Position loop/Position PID/Parallel P Gain/External Parameters'
//  '<S55>'  : 'control_outer/Position loop/Position PID/Reset Signal/External Reset'
//  '<S56>'  : 'control_outer/Position loop/Position PID/Saturation/Passthrough'
//  '<S57>'  : 'control_outer/Position loop/Position PID/Saturation Fdbk/Passthrough'
//  '<S58>'  : 'control_outer/Position loop/Position PID/Sum/Sum_PID'
//  '<S59>'  : 'control_outer/Position loop/Position PID/Sum Fdbk/Enabled'
//  '<S60>'  : 'control_outer/Position loop/Position PID/Tracking Mode/Enabled'
//  '<S61>'  : 'control_outer/Position loop/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S62>'  : 'control_outer/Position loop/Position PID/Tsamp - Integral/Passthrough'
//  '<S63>'  : 'control_outer/Position loop/Position PID/Tsamp - Ngain/Passthrough'
//  '<S64>'  : 'control_outer/Position loop/Position PID/postSat Signal/Feedback_Path'
//  '<S65>'  : 'control_outer/Position loop/Position PID/preSat Signal/Feedback_Path'
//  '<S66>'  : 'control_outer/Position loop/Position-Direct PID/Anti-windup'
//  '<S67>'  : 'control_outer/Position loop/Position-Direct PID/D Gain'
//  '<S68>'  : 'control_outer/Position loop/Position-Direct PID/Filter'
//  '<S69>'  : 'control_outer/Position loop/Position-Direct PID/Filter ICs'
//  '<S70>'  : 'control_outer/Position loop/Position-Direct PID/I Gain'
//  '<S71>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain'
//  '<S72>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S73>'  : 'control_outer/Position loop/Position-Direct PID/Integrator'
//  '<S74>'  : 'control_outer/Position loop/Position-Direct PID/Integrator ICs'
//  '<S75>'  : 'control_outer/Position loop/Position-Direct PID/N Copy'
//  '<S76>'  : 'control_outer/Position loop/Position-Direct PID/N Gain'
//  '<S77>'  : 'control_outer/Position loop/Position-Direct PID/P Copy'
//  '<S78>'  : 'control_outer/Position loop/Position-Direct PID/Parallel P Gain'
//  '<S79>'  : 'control_outer/Position loop/Position-Direct PID/Reset Signal'
//  '<S80>'  : 'control_outer/Position loop/Position-Direct PID/Saturation'
//  '<S81>'  : 'control_outer/Position loop/Position-Direct PID/Saturation Fdbk'
//  '<S82>'  : 'control_outer/Position loop/Position-Direct PID/Sum'
//  '<S83>'  : 'control_outer/Position loop/Position-Direct PID/Sum Fdbk'
//  '<S84>'  : 'control_outer/Position loop/Position-Direct PID/Tracking Mode'
//  '<S85>'  : 'control_outer/Position loop/Position-Direct PID/Tracking Mode Sum'
//  '<S86>'  : 'control_outer/Position loop/Position-Direct PID/Tsamp - Integral'
//  '<S87>'  : 'control_outer/Position loop/Position-Direct PID/Tsamp - Ngain'
//  '<S88>'  : 'control_outer/Position loop/Position-Direct PID/postSat Signal'
//  '<S89>'  : 'control_outer/Position loop/Position-Direct PID/preSat Signal'
//  '<S90>'  : 'control_outer/Position loop/Position-Direct PID/Anti-windup/Passthrough'
//  '<S91>'  : 'control_outer/Position loop/Position-Direct PID/D Gain/External Parameters'
//  '<S92>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S93>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S94>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S95>'  : 'control_outer/Position loop/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S96>'  : 'control_outer/Position loop/Position-Direct PID/I Gain/External Parameters'
//  '<S97>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S98>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S99>'  : 'control_outer/Position loop/Position-Direct PID/Integrator/Discrete'
//  '<S100>' : 'control_outer/Position loop/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S101>' : 'control_outer/Position loop/Position-Direct PID/N Copy/External Parameters'
//  '<S102>' : 'control_outer/Position loop/Position-Direct PID/N Gain/External Parameters'
//  '<S103>' : 'control_outer/Position loop/Position-Direct PID/P Copy/Disabled'
//  '<S104>' : 'control_outer/Position loop/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S105>' : 'control_outer/Position loop/Position-Direct PID/Reset Signal/External Reset'
//  '<S106>' : 'control_outer/Position loop/Position-Direct PID/Saturation/Passthrough'
//  '<S107>' : 'control_outer/Position loop/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S108>' : 'control_outer/Position loop/Position-Direct PID/Sum/Sum_PID'
//  '<S109>' : 'control_outer/Position loop/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S110>' : 'control_outer/Position loop/Position-Direct PID/Tracking Mode/Enabled'
//  '<S111>' : 'control_outer/Position loop/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S112>' : 'control_outer/Position loop/Position-Direct PID/Tsamp - Integral/Passthrough'
//  '<S113>' : 'control_outer/Position loop/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S114>' : 'control_outer/Position loop/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S115>' : 'control_outer/Position loop/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S116>' : 'control_outer/Vel PID control/Anti-windup'
//  '<S117>' : 'control_outer/Vel PID control/D Gain'
//  '<S118>' : 'control_outer/Vel PID control/Filter'
//  '<S119>' : 'control_outer/Vel PID control/Filter ICs'
//  '<S120>' : 'control_outer/Vel PID control/I Gain'
//  '<S121>' : 'control_outer/Vel PID control/Ideal P Gain'
//  '<S122>' : 'control_outer/Vel PID control/Ideal P Gain Fdbk'
//  '<S123>' : 'control_outer/Vel PID control/Integrator'
//  '<S124>' : 'control_outer/Vel PID control/Integrator ICs'
//  '<S125>' : 'control_outer/Vel PID control/N Copy'
//  '<S126>' : 'control_outer/Vel PID control/N Gain'
//  '<S127>' : 'control_outer/Vel PID control/P Copy'
//  '<S128>' : 'control_outer/Vel PID control/Parallel P Gain'
//  '<S129>' : 'control_outer/Vel PID control/Reset Signal'
//  '<S130>' : 'control_outer/Vel PID control/Saturation'
//  '<S131>' : 'control_outer/Vel PID control/Saturation Fdbk'
//  '<S132>' : 'control_outer/Vel PID control/Sum'
//  '<S133>' : 'control_outer/Vel PID control/Sum Fdbk'
//  '<S134>' : 'control_outer/Vel PID control/Tracking Mode'
//  '<S135>' : 'control_outer/Vel PID control/Tracking Mode Sum'
//  '<S136>' : 'control_outer/Vel PID control/Tsamp - Integral'
//  '<S137>' : 'control_outer/Vel PID control/Tsamp - Ngain'
//  '<S138>' : 'control_outer/Vel PID control/postSat Signal'
//  '<S139>' : 'control_outer/Vel PID control/preSat Signal'
//  '<S140>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S141>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S142>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
//  '<S143>' : 'control_outer/Vel PID control/D Gain/External Parameters'
//  '<S144>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter'
//  '<S145>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S146>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S147>' : 'control_outer/Vel PID control/Filter ICs/Internal IC - Filter'
//  '<S148>' : 'control_outer/Vel PID control/I Gain/External Parameters'
//  '<S149>' : 'control_outer/Vel PID control/Ideal P Gain/Passthrough'
//  '<S150>' : 'control_outer/Vel PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S151>' : 'control_outer/Vel PID control/Integrator/Discrete'
//  '<S152>' : 'control_outer/Vel PID control/Integrator ICs/Internal IC'
//  '<S153>' : 'control_outer/Vel PID control/N Copy/External Parameters'
//  '<S154>' : 'control_outer/Vel PID control/N Gain/External Parameters'
//  '<S155>' : 'control_outer/Vel PID control/P Copy/Disabled'
//  '<S156>' : 'control_outer/Vel PID control/Parallel P Gain/External Parameters'
//  '<S157>' : 'control_outer/Vel PID control/Reset Signal/External Reset'
//  '<S158>' : 'control_outer/Vel PID control/Saturation/Enabled'
//  '<S159>' : 'control_outer/Vel PID control/Saturation Fdbk/Passthrough'
//  '<S160>' : 'control_outer/Vel PID control/Sum/Sum_PID'
//  '<S161>' : 'control_outer/Vel PID control/Sum Fdbk/Enabled'
//  '<S162>' : 'control_outer/Vel PID control/Tracking Mode/Disabled'
//  '<S163>' : 'control_outer/Vel PID control/Tracking Mode Sum/Passthrough'
//  '<S164>' : 'control_outer/Vel PID control/Tsamp - Integral/Passthrough'
//  '<S165>' : 'control_outer/Vel PID control/Tsamp - Ngain/Passthrough'
//  '<S166>' : 'control_outer/Vel PID control/postSat Signal/Feedback_Path'
//  '<S167>' : 'control_outer/Vel PID control/preSat Signal/Feedback_Path'

#endif                                 // RTW_HEADER_control_outer_h_

//
// File trailer for generated code.
//
// [EOF]
//
