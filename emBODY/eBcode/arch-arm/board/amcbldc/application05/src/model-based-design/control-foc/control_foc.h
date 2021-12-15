//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 2.75
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec 14 19:25:42 2021
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
namespace amc_bldc_codegen
{
  class control_foc
  {
    // public data and function members
   public:
    // Block states (default storage) for model 'control_foc'
    struct DW_control_foc_T {
      real32_T Integrator_DSTATE;      // '<S44>/Integrator'
      real32_T UnitDelay_DSTATE;       // '<S1>/Unit Delay'
      real32_T Integrator_DSTATE_m;    // '<S94>/Integrator'
      ControlModes DelayInput1_DSTATE; // '<S3>/Delay Input1'
      int8_T Integrator_PrevResetState;// '<S44>/Integrator'
      int8_T Integrator_PrevResetState_k;// '<S94>/Integrator'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_control_foc_T {
      const char_T **errorStatus;
    };

    // model initialize function
    void initialize();

    // Initial conditions function
    void init();

    // model step function
    void step(const Flags *rtu_Flags, const ConfigurationParameters
              *rtu_ConfigurationParameters, const SensorsData *rtu_Sensors,
              const EstimatedData *rtu_Estimates, const Targets *rtu_Targets,
              const ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs
              *rty_FOCOutputs);

    // Real-Time Model get method
    amc_bldc_codegen::control_foc::RT_MODEL_control_foc_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // Block states
    DW_control_foc_T control_foc_DW;

    // Constructor
    control_foc();

    // Destructor
    ~control_foc();

    // private data and function members
   private:
    // private member function(s) for subsystem '<Root>/FOC inner loop'
    void FOCInnerLoop_Init();
    void FOCInnerLoop(const Flags *rtu_Flags, const ConfigurationParameters
                      *rtu_ConfigurationParameters, const SensorsData
                      *rtu_Sensors, const EstimatedData *rtu_Estimates, const
                      Targets *rtu_Targets, const ControlOuterOutputs
                      *rtu_OuterOutputs, ControlOutputs *rty_FOCOutputs);

    // Real-Time Model
    RT_MODEL_control_foc_T control_foc_M;
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Data Type Duplicate' : Unused code path elimination
//  Block '<S52>/Data Type Duplicate' : Unused code path elimination
//  Block '<S52>/Data Type Propagation' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S6>/Data Type Duplicate' : Unused code path elimination
//  Block '<S8>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Propagation' : Unused code path elimination
//  Block '<S105>/Kt' : Eliminated nontunable gain of 1
//  Block '<S62>/Offset' : Unused code path elimination
//  Block '<S111>/Offset' : Unused code path elimination


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
//  '<S3>'   : 'control_foc/FOC inner loop/Detect Change'
//  '<S4>'   : 'control_foc/FOC inner loop/Id PID control'
//  '<S5>'   : 'control_foc/FOC inner loop/Inverse Clarke Transform'
//  '<S6>'   : 'control_foc/FOC inner loop/Inverse Park Transform'
//  '<S7>'   : 'control_foc/FOC inner loop/Iq PID control'
//  '<S8>'   : 'control_foc/FOC inner loop/Park Transform'
//  '<S9>'   : 'control_foc/FOC inner loop/Saturation Dynamic'
//  '<S10>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup'
//  '<S11>'  : 'control_foc/FOC inner loop/Id PID control/D Gain'
//  '<S12>'  : 'control_foc/FOC inner loop/Id PID control/Filter'
//  '<S13>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs'
//  '<S14>'  : 'control_foc/FOC inner loop/Id PID control/I Gain'
//  '<S15>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain'
//  '<S16>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk'
//  '<S17>'  : 'control_foc/FOC inner loop/Id PID control/Integrator'
//  '<S18>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs'
//  '<S19>'  : 'control_foc/FOC inner loop/Id PID control/N Copy'
//  '<S20>'  : 'control_foc/FOC inner loop/Id PID control/N Gain'
//  '<S21>'  : 'control_foc/FOC inner loop/Id PID control/P Copy'
//  '<S22>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain'
//  '<S23>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal'
//  '<S24>'  : 'control_foc/FOC inner loop/Id PID control/Saturation'
//  '<S25>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk'
//  '<S26>'  : 'control_foc/FOC inner loop/Id PID control/Sum'
//  '<S27>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk'
//  '<S28>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode'
//  '<S29>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum'
//  '<S30>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral'
//  '<S31>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain'
//  '<S32>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal'
//  '<S33>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal'
//  '<S34>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel'
//  '<S35>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S36>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S37>'  : 'control_foc/FOC inner loop/Id PID control/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S38>'  : 'control_foc/FOC inner loop/Id PID control/D Gain/Disabled'
//  '<S39>'  : 'control_foc/FOC inner loop/Id PID control/Filter/Disabled'
//  '<S40>'  : 'control_foc/FOC inner loop/Id PID control/Filter ICs/Disabled'
//  '<S41>'  : 'control_foc/FOC inner loop/Id PID control/I Gain/External Parameters'
//  '<S42>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain/Passthrough'
//  '<S43>'  : 'control_foc/FOC inner loop/Id PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S44>'  : 'control_foc/FOC inner loop/Id PID control/Integrator/Discrete'
//  '<S45>'  : 'control_foc/FOC inner loop/Id PID control/Integrator ICs/Internal IC'
//  '<S46>'  : 'control_foc/FOC inner loop/Id PID control/N Copy/Disabled wSignal Specification'
//  '<S47>'  : 'control_foc/FOC inner loop/Id PID control/N Gain/Disabled'
//  '<S48>'  : 'control_foc/FOC inner loop/Id PID control/P Copy/Disabled'
//  '<S49>'  : 'control_foc/FOC inner loop/Id PID control/Parallel P Gain/External Parameters'
//  '<S50>'  : 'control_foc/FOC inner loop/Id PID control/Reset Signal/External Reset'
//  '<S51>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External'
//  '<S52>'  : 'control_foc/FOC inner loop/Id PID control/Saturation/External/Saturation Dynamic'
//  '<S53>'  : 'control_foc/FOC inner loop/Id PID control/Saturation Fdbk/Passthrough'
//  '<S54>'  : 'control_foc/FOC inner loop/Id PID control/Sum/Sum_PI'
//  '<S55>'  : 'control_foc/FOC inner loop/Id PID control/Sum Fdbk/Enabled'
//  '<S56>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode/Disabled'
//  '<S57>'  : 'control_foc/FOC inner loop/Id PID control/Tracking Mode Sum/Passthrough'
//  '<S58>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Integral/Passthrough'
//  '<S59>'  : 'control_foc/FOC inner loop/Id PID control/Tsamp - Ngain/Passthrough'
//  '<S60>'  : 'control_foc/FOC inner loop/Id PID control/postSat Signal/Feedback_Path'
//  '<S61>'  : 'control_foc/FOC inner loop/Id PID control/preSat Signal/Feedback_Path'
//  '<S62>'  : 'control_foc/FOC inner loop/Inverse Park Transform/Switch_Axis'
//  '<S63>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup'
//  '<S64>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain'
//  '<S65>'  : 'control_foc/FOC inner loop/Iq PID control/Filter'
//  '<S66>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs'
//  '<S67>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain'
//  '<S68>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain'
//  '<S69>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk'
//  '<S70>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator'
//  '<S71>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs'
//  '<S72>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy'
//  '<S73>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain'
//  '<S74>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy'
//  '<S75>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain'
//  '<S76>'  : 'control_foc/FOC inner loop/Iq PID control/Reset Signal'
//  '<S77>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation'
//  '<S78>'  : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk'
//  '<S79>'  : 'control_foc/FOC inner loop/Iq PID control/Sum'
//  '<S80>'  : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk'
//  '<S81>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode'
//  '<S82>'  : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum'
//  '<S83>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral'
//  '<S84>'  : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain'
//  '<S85>'  : 'control_foc/FOC inner loop/Iq PID control/postSat Signal'
//  '<S86>'  : 'control_foc/FOC inner loop/Iq PID control/preSat Signal'
//  '<S87>'  : 'control_foc/FOC inner loop/Iq PID control/Anti-windup/Passthrough'
//  '<S88>'  : 'control_foc/FOC inner loop/Iq PID control/D Gain/Disabled'
//  '<S89>'  : 'control_foc/FOC inner loop/Iq PID control/Filter/Disabled'
//  '<S90>'  : 'control_foc/FOC inner loop/Iq PID control/Filter ICs/Disabled'
//  '<S91>'  : 'control_foc/FOC inner loop/Iq PID control/I Gain/External Parameters'
//  '<S92>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain/Passthrough'
//  '<S93>'  : 'control_foc/FOC inner loop/Iq PID control/Ideal P Gain Fdbk/Passthrough'
//  '<S94>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator/Discrete'
//  '<S95>'  : 'control_foc/FOC inner loop/Iq PID control/Integrator ICs/Internal IC'
//  '<S96>'  : 'control_foc/FOC inner loop/Iq PID control/N Copy/Disabled wSignal Specification'
//  '<S97>'  : 'control_foc/FOC inner loop/Iq PID control/N Gain/Disabled'
//  '<S98>'  : 'control_foc/FOC inner loop/Iq PID control/P Copy/Disabled'
//  '<S99>'  : 'control_foc/FOC inner loop/Iq PID control/Parallel P Gain/External Parameters'
//  '<S100>' : 'control_foc/FOC inner loop/Iq PID control/Reset Signal/External Reset'
//  '<S101>' : 'control_foc/FOC inner loop/Iq PID control/Saturation/Passthrough'
//  '<S102>' : 'control_foc/FOC inner loop/Iq PID control/Saturation Fdbk/Passthrough'
//  '<S103>' : 'control_foc/FOC inner loop/Iq PID control/Sum/Sum_PI'
//  '<S104>' : 'control_foc/FOC inner loop/Iq PID control/Sum Fdbk/Enabled'
//  '<S105>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode/Enabled'
//  '<S106>' : 'control_foc/FOC inner loop/Iq PID control/Tracking Mode Sum/Tracking Mode'
//  '<S107>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Integral/Passthrough'
//  '<S108>' : 'control_foc/FOC inner loop/Iq PID control/Tsamp - Ngain/Passthrough'
//  '<S109>' : 'control_foc/FOC inner loop/Iq PID control/postSat Signal/Feedback_Path'
//  '<S110>' : 'control_foc/FOC inner loop/Iq PID control/preSat Signal/Feedback_Path'
//  '<S111>' : 'control_foc/FOC inner loop/Park Transform/Switch_Axis'

#endif                                 // RTW_HEADER_control_foc_h_

//
// File trailer for generated code.
//
// [EOF]
//
