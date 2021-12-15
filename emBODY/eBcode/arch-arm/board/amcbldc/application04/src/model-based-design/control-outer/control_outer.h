//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 2.24
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec 14 19:25:49 2021
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
namespace amc_bldc_codegen
{
  class control_outer
  {
    // public data and function members
   public:
    // Block states (default storage) for model 'control_outer'
    struct DW_control_outer_T {
      real32_T UD_DSTATE;              // '<S2>/UD'
      real32_T FilterDifferentiatorTF_states;// '<S43>/Filter Differentiator TF' 
      real32_T UnitDelay_DSTATE;       // '<Root>/Unit Delay'
      real32_T Integrator_DSTATE;      // '<S50>/Integrator'
      real32_T FilterDifferentiatorTF_states_i;// '<S93>/Filter Differentiator TF' 
      real32_T Integrator_DSTATE_i;    // '<S100>/Integrator'
      real32_T FilterDifferentiatorTF_states_c;// '<S145>/Filter Differentiator TF' 
      real32_T Integrator_DSTATE_b;    // '<S152>/Integrator'
      ControlModes DelayInput1_DSTATE; // '<S1>/Delay Input1'
      real32_T FilterDifferentiatorTF_tmp;// '<S43>/Filter Differentiator TF'
      real32_T FilterDifferentiatorTF_tmp_m;// '<S93>/Filter Differentiator TF'
      real32_T FilterDifferentiatorTF_tmp_p;// '<S145>/Filter Differentiator TF' 
      int8_T Integrator_PrevResetState;// '<S50>/Integrator'
      int8_T Integrator_PrevResetState_n;// '<S100>/Integrator'
      int8_T Integrator_PrevResetState_c;// '<S152>/Integrator'
    };

    // Zero-crossing (trigger) state for model 'control_outer'
    struct PrevZCX_control_outer_T {
      ZCSigState FilterDifferentiatorTF_Reset_ZC;// '<S43>/Filter Differentiator TF' 
      ZCSigState FilterDifferentiatorTF_Reset__m;// '<S93>/Filter Differentiator TF' 
      ZCSigState FilterDifferentiatorTF_Reset__e;// '<S145>/Filter Differentiator TF' 
    };

    // Real-time Model Data Structure
    struct RT_MODEL_control_outer_T {
      const char_T **errorStatus;
    };

    // model initialize function
    void initialize();

    // Initial conditions function
    void init();

    // model step function
    void step(const Flags &arg_Flags, const ConfigurationParameters &
              arg_ConfigurationParameters, const Targets &arg_Targets, const
              SensorsData &arg_Sensors, const EstimatedData &arg_Estimates,
              ControlOuterOutputs &arg_OuterOutputs);

    // Real-Time Model get method
    amc_bldc_codegen::control_outer::RT_MODEL_control_outer_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // Block states
    DW_control_outer_T control_outer_DW;

    // Triggered events
    PrevZCX_control_outer_T control_outer_PrevZCX;
    void control_outer_PrevZCStateInit();

    // Constructor
    control_outer();

    // Destructor
    ~control_outer();

    // private data and function members
   private:
    // Real-Time Model
    RT_MODEL_control_outer_T control_outer_M;
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Propagation' : Unused code path elimination
//  Block '<S43>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S61>/Kt' : Eliminated nontunable gain of 1
//  Block '<S93>/Passthrough for tuning' : Eliminate redundant data type conversion
//  Block '<S111>/Kt' : Eliminated nontunable gain of 1
//  Block '<S145>/Passthrough for tuning' : Eliminate redundant data type conversion


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
//  '<S1>'   : 'control_outer/Detect Change'
//  '<S2>'   : 'control_outer/Discrete Derivative'
//  '<S3>'   : 'control_outer/Enabling Logic'
//  '<S4>'   : 'control_outer/Position loop'
//  '<S5>'   : 'control_outer/Saturation Dynamic'
//  '<S6>'   : 'control_outer/Vel PID control'
//  '<S7>'   : 'control_outer/Enabling Logic/Compare To Constant'
//  '<S8>'   : 'control_outer/Enabling Logic/Compare To Constant1'
//  '<S9>'   : 'control_outer/Enabling Logic/Compare To Constant2'
//  '<S10>'  : 'control_outer/Enabling Logic/Compare To Constant3'
//  '<S11>'  : 'control_outer/Enabling Logic/Compare To Constant4'
//  '<S12>'  : 'control_outer/Enabling Logic/Compare To Constant5'
//  '<S13>'  : 'control_outer/Enabling Logic/Compare To Constant6'
//  '<S14>'  : 'control_outer/Enabling Logic/Compare To Constant7'
//  '<S15>'  : 'control_outer/Position loop/Position PID'
//  '<S16>'  : 'control_outer/Position loop/Position-Direct PID'
//  '<S17>'  : 'control_outer/Position loop/Position PID/Anti-windup'
//  '<S18>'  : 'control_outer/Position loop/Position PID/D Gain'
//  '<S19>'  : 'control_outer/Position loop/Position PID/Filter'
//  '<S20>'  : 'control_outer/Position loop/Position PID/Filter ICs'
//  '<S21>'  : 'control_outer/Position loop/Position PID/I Gain'
//  '<S22>'  : 'control_outer/Position loop/Position PID/Ideal P Gain'
//  '<S23>'  : 'control_outer/Position loop/Position PID/Ideal P Gain Fdbk'
//  '<S24>'  : 'control_outer/Position loop/Position PID/Integrator'
//  '<S25>'  : 'control_outer/Position loop/Position PID/Integrator ICs'
//  '<S26>'  : 'control_outer/Position loop/Position PID/N Copy'
//  '<S27>'  : 'control_outer/Position loop/Position PID/N Gain'
//  '<S28>'  : 'control_outer/Position loop/Position PID/P Copy'
//  '<S29>'  : 'control_outer/Position loop/Position PID/Parallel P Gain'
//  '<S30>'  : 'control_outer/Position loop/Position PID/Reset Signal'
//  '<S31>'  : 'control_outer/Position loop/Position PID/Saturation'
//  '<S32>'  : 'control_outer/Position loop/Position PID/Saturation Fdbk'
//  '<S33>'  : 'control_outer/Position loop/Position PID/Sum'
//  '<S34>'  : 'control_outer/Position loop/Position PID/Sum Fdbk'
//  '<S35>'  : 'control_outer/Position loop/Position PID/Tracking Mode'
//  '<S36>'  : 'control_outer/Position loop/Position PID/Tracking Mode Sum'
//  '<S37>'  : 'control_outer/Position loop/Position PID/Tsamp - Integral'
//  '<S38>'  : 'control_outer/Position loop/Position PID/Tsamp - Ngain'
//  '<S39>'  : 'control_outer/Position loop/Position PID/postSat Signal'
//  '<S40>'  : 'control_outer/Position loop/Position PID/preSat Signal'
//  '<S41>'  : 'control_outer/Position loop/Position PID/Anti-windup/Passthrough'
//  '<S42>'  : 'control_outer/Position loop/Position PID/D Gain/External Parameters'
//  '<S43>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter'
//  '<S44>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S45>'  : 'control_outer/Position loop/Position PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S46>'  : 'control_outer/Position loop/Position PID/Filter ICs/Internal IC - Filter'
//  '<S47>'  : 'control_outer/Position loop/Position PID/I Gain/External Parameters'
//  '<S48>'  : 'control_outer/Position loop/Position PID/Ideal P Gain/Passthrough'
//  '<S49>'  : 'control_outer/Position loop/Position PID/Ideal P Gain Fdbk/Passthrough'
//  '<S50>'  : 'control_outer/Position loop/Position PID/Integrator/Discrete'
//  '<S51>'  : 'control_outer/Position loop/Position PID/Integrator ICs/Internal IC'
//  '<S52>'  : 'control_outer/Position loop/Position PID/N Copy/External Parameters'
//  '<S53>'  : 'control_outer/Position loop/Position PID/N Gain/External Parameters'
//  '<S54>'  : 'control_outer/Position loop/Position PID/P Copy/Disabled'
//  '<S55>'  : 'control_outer/Position loop/Position PID/Parallel P Gain/External Parameters'
//  '<S56>'  : 'control_outer/Position loop/Position PID/Reset Signal/External Reset'
//  '<S57>'  : 'control_outer/Position loop/Position PID/Saturation/Passthrough'
//  '<S58>'  : 'control_outer/Position loop/Position PID/Saturation Fdbk/Passthrough'
//  '<S59>'  : 'control_outer/Position loop/Position PID/Sum/Sum_PID'
//  '<S60>'  : 'control_outer/Position loop/Position PID/Sum Fdbk/Enabled'
//  '<S61>'  : 'control_outer/Position loop/Position PID/Tracking Mode/Enabled'
//  '<S62>'  : 'control_outer/Position loop/Position PID/Tracking Mode Sum/Tracking Mode'
//  '<S63>'  : 'control_outer/Position loop/Position PID/Tsamp - Integral/Passthrough'
//  '<S64>'  : 'control_outer/Position loop/Position PID/Tsamp - Ngain/Passthrough'
//  '<S65>'  : 'control_outer/Position loop/Position PID/postSat Signal/Feedback_Path'
//  '<S66>'  : 'control_outer/Position loop/Position PID/preSat Signal/Feedback_Path'
//  '<S67>'  : 'control_outer/Position loop/Position-Direct PID/Anti-windup'
//  '<S68>'  : 'control_outer/Position loop/Position-Direct PID/D Gain'
//  '<S69>'  : 'control_outer/Position loop/Position-Direct PID/Filter'
//  '<S70>'  : 'control_outer/Position loop/Position-Direct PID/Filter ICs'
//  '<S71>'  : 'control_outer/Position loop/Position-Direct PID/I Gain'
//  '<S72>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain'
//  '<S73>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain Fdbk'
//  '<S74>'  : 'control_outer/Position loop/Position-Direct PID/Integrator'
//  '<S75>'  : 'control_outer/Position loop/Position-Direct PID/Integrator ICs'
//  '<S76>'  : 'control_outer/Position loop/Position-Direct PID/N Copy'
//  '<S77>'  : 'control_outer/Position loop/Position-Direct PID/N Gain'
//  '<S78>'  : 'control_outer/Position loop/Position-Direct PID/P Copy'
//  '<S79>'  : 'control_outer/Position loop/Position-Direct PID/Parallel P Gain'
//  '<S80>'  : 'control_outer/Position loop/Position-Direct PID/Reset Signal'
//  '<S81>'  : 'control_outer/Position loop/Position-Direct PID/Saturation'
//  '<S82>'  : 'control_outer/Position loop/Position-Direct PID/Saturation Fdbk'
//  '<S83>'  : 'control_outer/Position loop/Position-Direct PID/Sum'
//  '<S84>'  : 'control_outer/Position loop/Position-Direct PID/Sum Fdbk'
//  '<S85>'  : 'control_outer/Position loop/Position-Direct PID/Tracking Mode'
//  '<S86>'  : 'control_outer/Position loop/Position-Direct PID/Tracking Mode Sum'
//  '<S87>'  : 'control_outer/Position loop/Position-Direct PID/Tsamp - Integral'
//  '<S88>'  : 'control_outer/Position loop/Position-Direct PID/Tsamp - Ngain'
//  '<S89>'  : 'control_outer/Position loop/Position-Direct PID/postSat Signal'
//  '<S90>'  : 'control_outer/Position loop/Position-Direct PID/preSat Signal'
//  '<S91>'  : 'control_outer/Position loop/Position-Direct PID/Anti-windup/Passthrough'
//  '<S92>'  : 'control_outer/Position loop/Position-Direct PID/D Gain/External Parameters'
//  '<S93>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter'
//  '<S94>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S95>'  : 'control_outer/Position loop/Position-Direct PID/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S96>'  : 'control_outer/Position loop/Position-Direct PID/Filter ICs/Internal IC - Filter'
//  '<S97>'  : 'control_outer/Position loop/Position-Direct PID/I Gain/External Parameters'
//  '<S98>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain/Passthrough'
//  '<S99>'  : 'control_outer/Position loop/Position-Direct PID/Ideal P Gain Fdbk/Passthrough'
//  '<S100>' : 'control_outer/Position loop/Position-Direct PID/Integrator/Discrete'
//  '<S101>' : 'control_outer/Position loop/Position-Direct PID/Integrator ICs/Internal IC'
//  '<S102>' : 'control_outer/Position loop/Position-Direct PID/N Copy/External Parameters'
//  '<S103>' : 'control_outer/Position loop/Position-Direct PID/N Gain/External Parameters'
//  '<S104>' : 'control_outer/Position loop/Position-Direct PID/P Copy/Disabled'
//  '<S105>' : 'control_outer/Position loop/Position-Direct PID/Parallel P Gain/External Parameters'
//  '<S106>' : 'control_outer/Position loop/Position-Direct PID/Reset Signal/External Reset'
//  '<S107>' : 'control_outer/Position loop/Position-Direct PID/Saturation/Passthrough'
//  '<S108>' : 'control_outer/Position loop/Position-Direct PID/Saturation Fdbk/Passthrough'
//  '<S109>' : 'control_outer/Position loop/Position-Direct PID/Sum/Sum_PID'
//  '<S110>' : 'control_outer/Position loop/Position-Direct PID/Sum Fdbk/Enabled'
//  '<S111>' : 'control_outer/Position loop/Position-Direct PID/Tracking Mode/Enabled'
//  '<S112>' : 'control_outer/Position loop/Position-Direct PID/Tracking Mode Sum/Tracking Mode'
//  '<S113>' : 'control_outer/Position loop/Position-Direct PID/Tsamp - Integral/Passthrough'
//  '<S114>' : 'control_outer/Position loop/Position-Direct PID/Tsamp - Ngain/Passthrough'
//  '<S115>' : 'control_outer/Position loop/Position-Direct PID/postSat Signal/Feedback_Path'
//  '<S116>' : 'control_outer/Position loop/Position-Direct PID/preSat Signal/Feedback_Path'
//  '<S117>' : 'control_outer/Vel PID control/Anti-windup'
//  '<S118>' : 'control_outer/Vel PID control/D Gain'
//  '<S119>' : 'control_outer/Vel PID control/Filter'
//  '<S120>' : 'control_outer/Vel PID control/Filter ICs'
//  '<S121>' : 'control_outer/Vel PID control/I Gain'
//  '<S122>' : 'control_outer/Vel PID control/Ideal P Gain'
//  '<S123>' : 'control_outer/Vel PID control/Ideal P Gain Fdbk'
//  '<S124>' : 'control_outer/Vel PID control/Integrator'
//  '<S125>' : 'control_outer/Vel PID control/Integrator ICs'
//  '<S126>' : 'control_outer/Vel PID control/N Copy'
//  '<S127>' : 'control_outer/Vel PID control/N Gain'
//  '<S128>' : 'control_outer/Vel PID control/P Copy'
//  '<S129>' : 'control_outer/Vel PID control/Parallel P Gain'
//  '<S130>' : 'control_outer/Vel PID control/Reset Signal'
//  '<S131>' : 'control_outer/Vel PID control/Saturation'
//  '<S132>' : 'control_outer/Vel PID control/Saturation Fdbk'
//  '<S133>' : 'control_outer/Vel PID control/Sum'
//  '<S134>' : 'control_outer/Vel PID control/Sum Fdbk'
//  '<S135>' : 'control_outer/Vel PID control/Tracking Mode'
//  '<S136>' : 'control_outer/Vel PID control/Tracking Mode Sum'
//  '<S137>' : 'control_outer/Vel PID control/Tsamp - Integral'
//  '<S138>' : 'control_outer/Vel PID control/Tsamp - Ngain'
//  '<S139>' : 'control_outer/Vel PID control/postSat Signal'
//  '<S140>' : 'control_outer/Vel PID control/preSat Signal'
//  '<S141>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S142>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S143>' : 'control_outer/Vel PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
//  '<S144>' : 'control_outer/Vel PID control/D Gain/External Parameters'
//  '<S145>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter'
//  '<S146>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter/Tsamp'
//  '<S147>' : 'control_outer/Vel PID control/Filter/Disc. Trapezoidal Filter/Tsamp/Internal Ts'
//  '<S148>' : 'control_outer/Vel PID control/Filter ICs/Internal IC - Filter'
//  '<S149>' : 'control_outer/Vel PID control/I Gain/External Parameters'
//  '<S150>' : 'control_outer/Vel PID control/Ideal P Gain/Passthrough'
//  '<S151>' : 'control_outer/Vel PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S152>' : 'control_outer/Vel PID control/Integrator/Discrete'
//  '<S153>' : 'control_outer/Vel PID control/Integrator ICs/Internal IC'
//  '<S154>' : 'control_outer/Vel PID control/N Copy/External Parameters'
//  '<S155>' : 'control_outer/Vel PID control/N Gain/External Parameters'
//  '<S156>' : 'control_outer/Vel PID control/P Copy/Disabled'
//  '<S157>' : 'control_outer/Vel PID control/Parallel P Gain/External Parameters'
//  '<S158>' : 'control_outer/Vel PID control/Reset Signal/External Reset'
//  '<S159>' : 'control_outer/Vel PID control/Saturation/Enabled'
//  '<S160>' : 'control_outer/Vel PID control/Saturation Fdbk/Passthrough'
//  '<S161>' : 'control_outer/Vel PID control/Sum/Sum_PID'
//  '<S162>' : 'control_outer/Vel PID control/Sum Fdbk/Enabled'
//  '<S163>' : 'control_outer/Vel PID control/Tracking Mode/Disabled'
//  '<S164>' : 'control_outer/Vel PID control/Tracking Mode Sum/Passthrough'
//  '<S165>' : 'control_outer/Vel PID control/Tsamp - Integral/Passthrough'
//  '<S166>' : 'control_outer/Vel PID control/Tsamp - Ngain/Passthrough'
//  '<S167>' : 'control_outer/Vel PID control/postSat Signal/Feedback_Path'
//  '<S168>' : 'control_outer/Vel PID control/preSat Signal/Feedback_Path'

#endif                                 // RTW_HEADER_control_outer_h_

//
// File trailer for generated code.
//
// [EOF]
//
